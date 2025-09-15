local map_texture_asset_id = "outdoor-tiles"

-- Define a table with the start values of the outdoor map
Map = {
	    ----------------------------------------------------
    -- Table to define the list of assets
    ----------------------------------------------------
    assets = {
        [0] =
        { type = "texture", id = "outdoor-tiles",               file = "./assets/images/town_tiles.png" },
        { type = "texture", id = "hero-tiles",                  file = "./assets/images/heroes.png" },  
        { type = "texture", id = "character-tiles",             file = "./assets/images/character_tiles.png" }
    },

    ----------------------------------------------------
    -- table to define the map config variables
    ----------------------------------------------------
    tilemap = {
        map_file = "./assets/tilemaps/village.map",
        texture_asset_id = map_texture_asset_id,
        num_rows = 64,
        num_cols = 64,
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
                player_tag = {},
                transform = {
                    position = { x = 32, y = 32 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = 0.0, y = 0.0 }
                },
                sprite = {
                    texture_asset_id = "hero-tiles",
                    width = 16,
                    height = 16,
                    z_index = 2,
                    fixed = false,
                    src_rect_x = 0,
                    src_rect_y = 0
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 4 -- fps
                },
                --boxcollider = {
                    --width = 32,
                    --height = 25,
                    --offset = { x = 0, y = 5 }
                --},
                --keyboard_controller = {
                    --up_velocity = { x = 0, y = -50 },
                    --right_velocity = { x = 50, y = 0 },
                    --down_velocity = { x = 0, y = 50 },
                    --left_velocity = { x = -50, y = 0 }
                --},
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

-- this is a lua function that calculates the factorial of a given number
function factorial(n)
	if n == 1 then
		return 1
	end
	return n * factorial(n - 1)
end