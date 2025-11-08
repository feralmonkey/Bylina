local map_texture_asset_id = "outdoor-tiles"

-- Define a table with the start values of the outdoor map
Map = {
	    ----------------------------------------------------
    -- Table to define the list of assets
    ----------------------------------------------------
    assets = {
        [0] =
        { type = "texture", id = "outdoor-tiles",               file = "./assets/images/outdoor_tiles.png" },
        { type = "texture", id = "hero-tiles",                  file = "./assets/images/heroes.png" },  
        { type = "texture", id = "character-tiles",             file = "./assets/images/character_tiles.png" },
        { type = "texture", id = "npc-tiles",                   file = "./assets/images/npc-1.png" },  
    },

    ----------------------------------------------------
    -- table to define the map config variables
    ----------------------------------------------------
    tilemap = {
        map_file = "./assets/tilemaps/overworld.map",
        texture_asset_id = map_texture_asset_id,
        num_rows = 512,
        num_cols = 512,
        tile_size = 8,
        scale = 1.0
    },

        ----------------------------------------------------
    -- table to define entities and their components
    ----------------------------------------------------
    entities = {
        [0] =
        {
            -- Player
            tag = "hero",
            components = {
                player = {},
                transform = {
                    position = { x = 320, y = 320 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                stats = {},
                rigidbody = {
                    velocity = { x = 0.0, y = 0.0 }
                },
                sprite = {
                    texture_asset_id = "hero-tiles",
                    width = 16,
                    height = 16,
                    z_index = 2,
                    fixed = false,
                    anchor_x = 0,
                    src_rect_x = 0,
                    src_rect_y = 0
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 4 -- fps
                },
                boxcollider = {
                    width = 16,
                    height = 16,
                    offset = { x = 0, y = 0 }
                },
                camera_follow = {
                    follow = true
                }
            }
        }
    }
}

-- Define some useful global variables
--map_width = Level.tilemap.num_cols * Level.tilemap.tile_size * Level.tilemap.scale
--map_height = Level.tilemap.num_rows * Level.tilemap.tile_size * Level.tilemap.scale