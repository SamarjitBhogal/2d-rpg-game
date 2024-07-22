#include "raylib.h"
#include "raymath.h"

#define TILE_WIDTH 8
#define TILE_HEIGHT 8
#define MAX_TEXTURES 1
#define WORLD_WIDTH 20
#define WORLD_HEIGHT 20

#define PLAYER_TEXTURE_X 4
#define PLAYER_TEXTURE_Y 0
#define DUNGEON_TEXTURE_X 8
#define DUNGEON_TEXTURE_Y 9

#define PLAYER_START_X 3
#define PLAYER_START_Y 3

#define DUNGEON_GATE_X 10
#define DUNGEON_GATE_Y 10

const int screenWidth = 800;
const int screenHeight = 600;

typedef enum {
    TEXTURE_TILEMAP = 0
} texture_asset;

typedef enum {
    TILE_DIRT = 0,
    TILE_GRASS,
    TILE_TREE,
    TILE_STONE,
} tile_type;

typedef struct {
    int x;
    int y;
    int type;
} sTile;

typedef struct {
    int x;
    int y;
} sEntity;

Texture2D textures[MAX_TEXTURES];
sTile world[WORLD_WIDTH][WORLD_HEIGHT];
sTile dungeon[WORLD_WIDTH][WORLD_HEIGHT];

sEntity player;
sEntity dun_gate;

Camera2D camera = {};

void GameStartup();
void GameUpdate();
void GameRender();
void GameShutdown();
void DrawTile(int x_pos, int y_pos, int texture_tile_x, int texture_tile_y);

void GameStartup() {
    InitAudioDevice();

    Image img = LoadImage("resources/colored_tilemap_packed.png");
    textures[TEXTURE_TILEMAP] = LoadTextureFromImage(img);
    UnloadImage(img);

    for (int i = 0; i < WORLD_WIDTH; i++) {
        for (int j = 0; j < WORLD_HEIGHT; j++) {
            world[i][j] = (sTile) {
                .x = i,
                .y = j,
                .type = GetRandomValue(TILE_DIRT, TILE_TREE)
            };

            dungeon[i][j] = (sTile) {
                .x = i,
                .y = j,
                .type = TILE_DIRT
            };
        }
    }

    player = (sEntity) {
        .x = TILE_WIDTH * PLAYER_START_X,
        .y = TILE_HEIGHT * PLAYER_START_Y
    };

    dun_gate = (sEntity) {
        .x = TILE_WIDTH * DUNGEON_GATE_X,
        .y = TILE_HEIGHT * DUNGEON_GATE_Y
    };

    camera.target = (Vector2) { player.x, player.y };
    camera.offset = (Vector2) { (float) screenWidth / 2, (float) screenHeight / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 3.0f;
}

void GameUpdate() {

    float x = player.x;
    float y = player.y;
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        x -= 1 * TILE_WIDTH;
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        x += 1 * TILE_WIDTH;
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        y -= 1 * TILE_HEIGHT;
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        y += 1 * TILE_HEIGHT;
    }

    player.x = x;
    player.y = y;

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        const float zoomIncrement = 0.125f;
        camera.zoom += (wheel * zoomIncrement);
        if (camera.zoom < 3.0f) camera.zoom = 3.0f;
        if (camera.zoom > 8.0f) camera.zoom = 8.0f;
    }  

    camera.target = (Vector2) { player.x, player.y };

    if (IsKeyPressed(KEY_E)) {
        if (player.x == dun_gate.x && player.y == dun_gate.y) {
            
        }
    }
}

void GameRender() {
    //Enables game to use camera settings defined
    BeginMode2D(camera);
    sTile tile;
    sTile texture_tile;

    for (int i = 0; i < WORLD_WIDTH; i++) {
        for (int j = 0; j < WORLD_HEIGHT; j++) {
            tile = world[i][j];

            switch (tile.type) {
                case TILE_DIRT:
                    texture_tile = (sTile) {
                        .x = 4,
                        .y = 4
                    };
                    break;
                case TILE_GRASS:
                    texture_tile = (sTile) {
                        .x = 5,
                        .y = 4
                    };
                    break;
                case TILE_TREE: 
                    texture_tile = (sTile) {
                        .x = 5,
                        .y = 5
                    };
                    break;
            }

            DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, texture_tile.x, texture_tile.y);
        }
    }    
    //render dungeon gate
    DrawTile(dun_gate.x, dun_gate.y, DUNGEON_TEXTURE_X, DUNGEON_TEXTURE_Y);
    //render player
    DrawTile(camera.target.x, camera.target.y, PLAYER_TEXTURE_X, PLAYER_TEXTURE_Y);

    EndMode2D();

    DrawRectangle(5, 5, 330, 120, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 120, BLUE);

    DrawText(TextFormat("Camera Target: (%06.2f, %06.2f)", camera.target.x, camera.target.y), 15, 10, 14, YELLOW);
    DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 30, 14, YELLOW);
}

void GameShutdown() {
    CloseAudioDevice();

    for (int i = 0; i < MAX_TEXTURES; i++) {
        UnloadTexture(textures[i]);
    }
}

void DrawTile(int x_pos, int y_pos, int texture_tile_x, int texture_tile_y) {
    Rectangle source = { 
        (float) (texture_tile_x * TILE_WIDTH), 
        (float) (texture_tile_y * TILE_HEIGHT), 
        (float) TILE_WIDTH, 
        (float) TILE_HEIGHT 
        };
    Rectangle dest = { 
        (float) x_pos, 
        (float) y_pos, 
        (float) TILE_WIDTH, 
        (float) TILE_HEIGHT
        };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(
        textures[TEXTURE_TILEMAP],
        source, 
        dest, 
        origin,
        0.0f, 
        WHITE);
}

int main() {
    InitWindow(screenWidth, screenHeight, "Raylib 2D RPG");
    SetTargetFPS(60);

    GameStartup();

    while (!WindowShouldClose()) {
        GameUpdate();

        BeginDrawing();
            ClearBackground(GRAY);
            GameRender();
        EndDrawing();
    }

    GameShutdown();

    CloseWindow();

    return 0;    
}
