param (
    [string]$InputFile = "",
    [string]$OutputFile = ""
)

# --- Configuration / defaults (change if you like) ---
if ([string]::IsNullOrEmpty($InputFile)) {
    $InputFile = "D:\source\repos\Bylina\Bylina\external\tiled\village-collisio.tmx"
}
if ([string]::IsNullOrEmpty($OutputFile)) {
    $OutputFile = "D:\source\repos\Bylina\Bylina\external\tiled\villagec.map"
}

# Flipping bit masks (uint32)
$FLIPPED_HORIZONTALLY_FLAG = 0x80000000
$FLIPPED_VERTICALLY_FLAG   = 0x40000000
$FLIPPED_DIAGONALLY_FLAG   = 0x20000000

# Mask for valid tile ID bits (lowest 29 bits)
$GID_MASK = 0x1FFFFFFF

# Read TMX
[xml]$tmx = Get-Content $InputFile

$mapNode = $tmx.map
if (-not $mapNode) {
    Write-Error "Failed to parse TMX map from $InputFile"
    exit 1
}

# Base directory to resolve external .tsx paths
$tmxDir = Split-Path -Parent $InputFile

# Build tileset metadata list and collision lookup keyed by full GID
$tilesetEntries = @()
$collisionLookup = @{}   # keys are stringified full-gid -> boolean

# Iterate tilesets declared in the TMX (they may be external with source attribute)
foreach ($tsNode in $mapNode.tileset) {
    $firstgid = [int]$tsNode.firstgid
    $source = $tsNode.source

    # resolve tsx path: source could be relative to the tmx file
    if ([string]::IsNullOrEmpty($source)) {
        # inline tileset in TMX (rare) -> use tsNode directly
        $tsxPath = $null
        $tsxXml = $tsNode
    } else {
        $tsxPath = Join-Path $tmxDir $source
        if (-not (Test-Path $tsxPath)) {
            Write-Warning "Tileset source not found: $tsxPath (trying relative)"
            $tsxPath = (Resolve-Path $tsxPath -ErrorAction SilentlyContinue).Path
            if (-not $tsxPath) {
                Write-Error "Cannot locate tileset source: $source"
                continue
            }
        }
        [xml]$tsxXml = Get-Content $tsxPath
    }

    # fetch tile size and image width/height (if present)
    $tilewidth = if ($tsxXml.tileset.tilewidth) { [int]$tsxXml.tileset.tilewidth } else { [int]$mapNode.tilewidth }
    $tileheight = if ($tsxXml.tileset.tileheight) { [int]$tsxXml.tileset.tileheight } else { [int]$mapNode.tileheight }

    # If image info is present, compute columns; otherwise we may compute columns from tilecount
    $imageWidth = 0
    $imageHeight = 0
    $columns = 0
    if ($tsxXml.tileset.image -and $tsxXml.tileset.image.width) {
        $imageWidth = [int]$tsxXml.tileset.image.width
        $imageHeight = [int]$tsxXml.tileset.image.height
        if ($tilewidth -ne 0) { $columns = [int]($imageWidth / $tilewidth) }
    } elseif ($tsxXml.tileset.tilecount) {
        $tilecount = [int]$tsxXml.tileset.tilecount
        # fallback: assume square-ish sheet: compute columns = ceil(sqrt(tilecount))
        $columns = [int]([math]::Ceiling([math]::Sqrt($tilecount)))
    } else {
        $columns = 16 # reasonable default
    }

    # Build collision entries for this tileset by scanning <tile> entries in the tsx
    if ($tsxXml.tileset.tile) {
        foreach ($tileDef in $tsxXml.tileset.tile) {
            $localId = [int]$tileDef.id          # 0-based tileset-local id
            $fullGid = $firstgid + $localId      # actual GID used in TMX layer data
            $hasCollision = $false

            if ($tileDef.properties -and $tileDef.properties.property) {
                foreach ($prop in $tileDef.properties.property) {
                    $pname = $prop.name
                    $pval  = $prop.value
                    if ($pname -eq "collision" -and ($pval -eq "true" -or $pval -eq "1" -or $pval -eq "True")) {
                        $hasCollision = $true
                        break
                    }
                }
            }

            # store keyed by full GID as string (avoid numeric key typing issues)
            $collisionLookup[[string]$fullGid] = $hasCollision
        }
    }

    # Also record tileset entry metadata for later source-rect calculations
    $entry = [PSCustomObject]@{
        firstgid = $firstgid
        columns  = $columns
        tilewidth = $tilewidth
        tileheight = $tileheight
    }
    $tilesetEntries += $entry
}

# Sort tileset entries by firstgid ascending (so we can find owner by searching descending)
$tilesetEntries = $tilesetEntries | Sort-Object -Property firstgid

# Get layer CSV (first tile layer)
# Choose the first <layer> that has data encoding="csv"
$layerNode = $mapNode.layer | Where-Object { $_.data -and $_.data.encoding -eq "csv" } | Select-Object -First 1
if (-not $layerNode) {
    Write-Error "No CSV-encoded layer found in TMX."
    exit 1
}
$rawCsv = $layerNode.data.'#text'.Trim()
$width = [int]$layerNode.width
$height = [int]$layerNode.height

# flatten CSV into gids array
$gids = $rawCsv -split ',' | ForEach-Object { $_.Trim() } | Where-Object { $_ -ne "" }

# Process each gid
$output = @()

foreach ($gidStr in $gids) {
    # parse as unsigned 32-bit, strip flip bits
    $gid = [uint32]$gidStr
    $gid = $gid -band $GID_MASK

    if ($gid -eq 0) {
        $output += "000"    # empty tile: Y=0 X=0 coll=0 (you can change empty encoding if desired)
        continue
    }

    # Determine which tileset owns this gid:
    # find the tileset with largest firstgid <= gid
    $owner = $tilesetEntries | Where-Object { $_.firstgid -le $gid } | Sort-Object -Property firstgid -Descending | Select-Object -First 1
    if (-not $owner) {
        Write-Warning "No tileset found owning gid $gid. Skipping."
        $output += "000"
        continue
    }

    $localId = [int]($gid - $owner.firstgid)   # tileset-local id (0-based)
    $cols = [int]$owner.columns
    if ($cols -eq 0) { $cols = 16 } # fallback

    $xIndex = $localId % $cols
    $yIndex = [math]::Floor($localId / $cols)

    # collision lookup: keyed by full gid (string)
    $collisionFlag = 0
    $key = [string]$gid
    if ($collisionLookup.ContainsKey($key) -and $collisionLookup[$key]) { $collisionFlag = 1 }

    # format hex pieces; note: using single-hex for each index like before -
    # if you expect indices > 0xF, you'll get multi-digit hex here (preserve that)
    $hexX = "{0:X}" -f ([int]$xIndex)
    $hexY = "{0:X}" -f ([int]$yIndex)
    $hexC = "{0:X}" -f $collisionFlag

    $output += ("{0}{1}{2}" -f $hexY, $hexX, $hexC)

    # debug (optional)
    # Write-Host ("gid={0} localId={1} x={2} y={3} coll={4}" -f $gid, $localId, $xIndex, $yIndex, $collisionFlag)
}

# Write rows
$rows = @()
for ($i = 0; $i -lt $output.Count; $i += $width) {
    $rows += ($output[$i..([math]::Min($i+$width-1, $output.Count-1))] -join ",")
}

$rows | Out-File -FilePath $OutputFile -Encoding ascii

Write-Host "Converted $InputFile -> $OutputFile (`$width x $height` tiles)."
