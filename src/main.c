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
} sTile;

Texture2D textures[MAX_TEXTURES];
sTile world[WORLD_WIDTH][WORLD_HEIGHT];

Camera2D camera = {};

void GameStartup();
void GameUpdate();
void GameRender();
void GameShutdown();

void GameStartup() {
    InitAudioDevice();

    Image img = LoadImage('resources/colored_tilemap.png');
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

    camera.target = (Vector2) { 0, 0 };
    camera.offset = (Vector2) { (float) screenWidth / 2, (float) screenHeight / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 3.0f;
}

void GameUpdate() {

}

void GameRender() {
    sTile tile;
    int textureIndex_x = 0;
    int textureIndex_y = 0;

    for (int i = 0; i < WORLD_WIDTH; i++) {
        for (int j = 0; j < WORLD_HEIGHT; j++) {
            tile = world[i][j];

            textureIndex_x = 4;
            textureIndex_y = 4;

            Rectangle source = { (float) textureIndex_x, (float) textureIndex_y, (float) TILE_WIDTH, (float) TILE_HEIGHT };
            Rectangle dest = { (float) (tile.x * TILE_WIDTH), (float) (tile.y * TILE_HEIGHT), (float) TILE_WIDTH, (float) TILE_HEIGHT };
            Vector2 origin = { 0, 0 };
            DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f, WHITE);
        }
    }
}

void GameShutdown() {
    CloseAudioDevice();

    for (int i = 0; i < MAX_TEXTURES; i++) {
        UnloadTexture(textures[i]);
    }
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
