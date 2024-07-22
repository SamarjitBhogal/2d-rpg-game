#include "raylib.h"
#include "raymath.h"

#define TILE_WIDTH 8
#define TILE_HEIGHT 8
#define MAX_TEXTURES 1
#define WORLD_WIDTH 20
#define WORLD_HEIGHT 20

const int screenWidth = 800;
const int screenHeight = 600;

typedef enum {
    TEXTURE_TILEMAP = 0
} texture_asset;

typedef struct {
    int x;
    int y;
} sTile, sEntity;

Texture2D textures[MAX_TEXTURES];
sTile world[WORLD_WIDTH][WORLD_HEIGHT];
sEntity player;
sTile playerTexture = (sTile) {
    .x = 4,
    .y = 0
};

Camera2D camera = {};

void GameStartup();
void GameUpdate();
void GameRender();
void GameShutdown();
void DrawTile(int x_pos, int y_pos, int tile_x, int tile_y);

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
            };
        }
    }

    player = (sEntity) {
        .x = TILE_WIDTH * 3,
        .y = TILE_HEIGHT * 3
    };

    camera.target = (Vector2) { player.x, player.y };
    camera.offset = (Vector2) { (float) screenWidth / 2, (float) screenHeight / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 3.0f;
}

void GameUpdate() {
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        const float zoomIncrement = 0.125f;
        camera.zoom += (wheel * zoomIncrement);
        if (camera.zoom < 3.0f) camera.zoom = 3.0f;
        if (camera.zoom > 8.0f) camera.zoom = 8.0f;
    }
    camera.target = (Vector2) { player.x, player.y };
}

void GameRender() {
    //Enables game to use camera settings defined
    BeginMode2D(camera);
    sTile tile;
    int textureIndex_x = 0;
    int textureIndex_y = 0;

    for (int i = 0; i < WORLD_WIDTH; i++) {
        for (int j = 0; j < WORLD_HEIGHT; j++) {
            tile = world[i][j];

            textureIndex_x = 4;
            textureIndex_y = 4;

            DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, textureIndex_x, textureIndex_y);
        }
    }
    DrawTile(camera.target.x, camera.target.y, playerTexture.x, playerTexture.y);

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

void DrawTile(int x_pos, int y_pos, int tile_x, int tile_y) {
    Rectangle source = { 
        (float) (tile_x * TILE_WIDTH), 
        (float) (tile_y * TILE_HEIGHT), 
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
