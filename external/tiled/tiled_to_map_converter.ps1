param (
    [string]$InputFile,
    [string]$OutputFile
)

# Flipping bit masks
$FLIPPED_HORIZONTALLY_FLAG = 0x80000000
$FLIPPED_VERTICALLY_FLAG   = 0x40000000
$FLIPPED_DIAGONALLY_FLAG   = 0x20000000

# Mask for valid tile ID bits (lowest 29 bits)
$GID_MASK = 0x1FFFFFFF

$Path = "D:\source\repos\Bylina\Bylina\external\tiled\"

$tmx_file = "village1.tmx"
$tsx_file = "town_set.tsx"
$output_file = "village.map"


$InputFile = "D:\source\repos\Bylina\Bylina\external\tiled\$tmx_file"
$TileSet = "D:\source\repos\Bylina\Bylina\external\tiled\$tsx_file"
$OutputFile = "D:\source\repos\Bylina\Bylina\external\tiled\$output_file"

[xml]$tmx = Get-Content $InputFile
[xml]$tsx = Get-Content $TileSet

# Read map + tileset info
$map = $tmx.map
$tileset = $map.tileset
$tilesetImage = $tsx.tileset.image
$tilesetWidth = [int]$tilesetImage.width
$tilesetHeight = [int]$tilesetImage.height
$tileWidth = [int]$tsx.tileset.tilewidth
$tileHeight = [int]$tsx.tileset.tileheight
$sheetCols = $tilesetWidth / $tileWidth
$firstGid = [int]$tileset[0].firstgid
$numCols = $tilesetWidth / $tileWidth

# Grab CSV data from first layer
$data = $map.layer.data.'#text'.Trim()
$gids = $data -split ',' | ForEach-Object { $_.Trim() } | Where-Object { $_ -ne "" }

$output = @()
foreach ($gidStr in $gids) {
    $gid = [long]$gidStr

    # Strip flip bits (keep only 29 bits of tile ID)
    $gid = $gid -band $GID_MASK

    if ($gid -eq 0) {
        # Empty tile → encode as 00
        $output += "00"
        continue
    }

    # Remove flipping bits
    $gid = $gid -band -bnot ($FLIPPED_HORIZONTALLY_FLAG -bor $FLIPPED_VERTICALLY_FLAG -bor $FLIPPED_DIAGONALLY_FLAG)

    $localId = $gid - 1 #$firstGid
    $xIndex = $localId % $numCols #- 1
    $yIndex = [math]::Floor($localId / $numCols)

    Write-Host "gid=$gid localId=$localId xIndex=$xIndex yIndex=$yIndex sheetCols=$sheetCols"

    $hexX = "{0:X}" -f ([long]$xIndex)
    $hexY = "{0:X}" -f ([long]$yIndex)

    $output += "$hexY$hexX"
}

# Format output into rows
$width = [int]$map.layer.width
$rows = @()
for ($i = 0; $i -lt $output.Count; $i += $width) {
    $rows += ($output[$i..($i+$width-1)] -join ",")
}

$rows | Out-File -FilePath $OutputFile -Encoding ascii

Write-Host "Converted $InputFile → $OutputFile"
