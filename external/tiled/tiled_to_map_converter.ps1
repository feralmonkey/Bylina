param (
    [string]$InputFile,
    [string]$OutputFile
)

# Flipping bit masks
$FLIPPED_HORIZONTALLY_FLAG = 0x80000000
$FLIPPED_VERTICALLY_FLAG   = 0x40000000
$FLIPPED_DIAGONALLY_FLAG   = 0x20000000

$InputFile = "village1.tmx"
$TileSet = "town_set.tsx"
$OutputFile = "village.map"

[xml]$tmx = Get-Content $InputFile
[xml]$tsx = Get-Content "D:\source\repos\Bylina\tiled\$TileSet"

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

    if ($gid -eq 0) {
        # Empty tile → encode as 00
        $output += "00"
        continue
    }

    # Remove flipping bits
    $gid = $gid -band -bnot ($FLIPPED_HORIZONTALLY_FLAG -bor $FLIPPED_VERTICALLY_FLAG -bor $FLIPPED_DIAGONALLY_FLAG)

    $localId = $gid - $firstGid
    $xIndex = $localId % $numCols
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
