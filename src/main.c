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
#define ORC_TEXTURE_X 11
#define ORC_TEXTURE_Y 0
#define CHEST_TEXTURE_X 9
#define CHEST_TEXTURE_Y 3

#define PLAYER_START_X 3
#define PLAYER_START_Y 3
#define ORC_START_X 5
#define ORC_START_Y 5

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

typedef enum {
    ZONE_ALL = 0,
    ZONE_WORLD,
    ZONE_DUN
} zone;

typedef struct {
    int x;
    int y;
    int type;
} sTile;

typedef struct {
    int x;
    int y;
    zone zone;
    int health;
    int damage;
    bool isAlive;
    int money;
    int xp;
} sEntity;

Texture2D textures[MAX_TEXTURES];
sTile world[WORLD_WIDTH][WORLD_HEIGHT];
sTile dungeon[WORLD_WIDTH][WORLD_HEIGHT];

sEntity player;
sEntity dun_gate;
sEntity orc;
sEntity chest = { 0 };

Camera2D camera = {};

void GameStartup();
void GameUpdate();
void GameRender();
void GameShutdown();
void DrawTile(int x_pos, int y_pos, int texture_tile_x, int texture_tile_y);
bool IsInBounds(int x, int y);

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
        .y = TILE_HEIGHT * PLAYER_START_Y,
        .zone = ZONE_WORLD,
        .health = 100,
        .damage = 0,
        .isAlive = true,
        .money = 1000,
        .xp = 0
    };

    dun_gate = (sEntity) {
        .x = TILE_WIDTH * DUNGEON_GATE_X,
        .y = TILE_HEIGHT * DUNGEON_GATE_Y,
        .zone = ZONE_ALL
    };

    orc = (sEntity) {
        .x = TILE_WIDTH * ORC_START_X,
        .y = TILE_HEIGHT * ORC_START_Y,
        .zone = ZONE_DUN,
        .health = 100,
        .damage = 0,
        .isAlive = true,
        .xp = GetRandomValue(10, 100)
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
        if (IsInBounds(x - (1 * TILE_WIDTH), y)) x -= 1 * TILE_WIDTH;
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        if (IsInBounds(x + (1 * TILE_WIDTH), y)) x += 1 * TILE_WIDTH;
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (IsInBounds(x, y - (1 * TILE_HEIGHT))) y -= 1 * TILE_HEIGHT;
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (IsInBounds(x, y + (1 * TILE_HEIGHT))) y += 1 * TILE_HEIGHT;
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        const float zoomIncrement = 0.125f;
        camera.zoom += (wheel * zoomIncrement);
        if (camera.zoom < 3.0f) camera.zoom = 3.0f;
        if (camera.zoom > 8.0f) camera.zoom = 8.0f;
    }  

    if (IsKeyPressed(KEY_E)) {
        if (player.x == dun_gate.x && player.y == dun_gate.y) {
            if (player.zone == ZONE_WORLD) {
                player.zone = ZONE_DUN;
            } else if (player.zone == ZONE_DUN) {
                player.zone = ZONE_WORLD;
            }
        }
    }

    if (player.zone == orc.zone && orc.x == x && orc.y == y && orc.isAlive) {
        int damage = GetRandomValue(2, 20); //2d10 dice
        orc.health -= damage;
        orc.damage = damage;

        if (orc.health <= 0) {
            orc.isAlive = false;
            player.xp += orc.xp;

            chest.isAlive = true;
            chest.x = orc.x;
            chest.y = orc.y;
            chest.zone = orc.zone;
            chest.money = GetRandomValue(10, 100);
        }
    } else {
        player.x = x;
        player.y = y;
        camera.target = (Vector2) { player.x, player.y };
    }

    if (IsKeyPressed(KEY_G)) {
        if (player.x == chest.x && player.y == chest.y && !orc.isAlive) {
            chest.isAlive = false;
            player.money += chest.money;
            
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
            if (player.zone == ZONE_WORLD) {
                tile = world[i][j];
            } else if (player.zone == ZONE_DUN) {
                tile = dungeon[i][j];
            }             

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
    //render orc
    if (orc.zone == player.zone) {
        if (orc.isAlive) DrawTile(orc.x, orc.y, ORC_TEXTURE_X, ORC_TEXTURE_Y);
        if (chest.isAlive) DrawTile(chest.x, chest.y, CHEST_TEXTURE_X, CHEST_TEXTURE_Y);
    }
    //render player
    DrawTile(camera.target.x, camera.target.y, PLAYER_TEXTURE_X, PLAYER_TEXTURE_Y);

    EndMode2D();

    DrawRectangle(5, 5, 330, 120, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 120, BLUE);

    DrawText(TextFormat("Camera Target: (%06.2f, %06.2f)", camera.target.x, camera.target.y), 15, 10, 14, YELLOW);
    DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 30, 14, YELLOW);
    DrawText(TextFormat("Player Position: %d, %d", player.x, player.y), 15, 50, 14, YELLOW);
    DrawText(TextFormat("Player Money: %d", player.money), 15, 70, 14, YELLOW);
    DrawText(TextFormat("Player XP: %d", player.xp), 15, 90, 14, YELLOW);

    if (orc.isAlive) {
        DrawText(TextFormat("Orc Health: %d", orc.health), 15, 110, 14, YELLOW);
    }
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

bool IsInBounds(int x, int y) {
    return x >= 0 
        && x <= (int) (WORLD_WIDTH - 1) * TILE_WIDTH 
        && y >= 0 
        && y <= (int) (WORLD_HEIGHT - 1) * TILE_HEIGHT;
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
