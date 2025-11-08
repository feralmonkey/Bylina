#pragma once

struct Tile {
    int srcX;
    int srcY;
    bool collider;
};

struct TilemapData {
    int tileSize = 16;
    double tileScale = 1.0;
    int cols = 0;
    int rows = 0;
    std::string textureAssetId;
    std::vector<Tile> tiles; // size = rows * cols
};