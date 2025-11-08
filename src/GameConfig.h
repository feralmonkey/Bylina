//
// Created by travis on 11/5/25.
//

#ifndef BYLINA_GAMECONFIG_H
#define BYLINA_GAMECONFIG_H


struct GameConfig {
    // Window settings
    int windowScale = 3;
    int logicalWidth = 256;
    int logicalHeight = 240;

    // Map settings
    int mapWidth = 0;
    int mapHeight = 0;

    // Game settings
    int tileSize = 16;
    bool debugMode = false;
};


#endif //BYLINA_GAMECONFIG_H