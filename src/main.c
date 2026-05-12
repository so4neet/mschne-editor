#include <raylib.h>
#include <raymath.h>
#include "globals.h"
#include "camera.h"

int main() {
        InitWindow(1280, 720, "MSCHNE Wrld Editor");
        SetExitKey(KEY_NULL);
        SetTargetFPS(165);
        Camera cam;
        float camAngleX = 0; float camAngleY = 0;
        bool isTyping = false;
        bool isMouseOverUI = false;
        InitEditorCamera(&cam, &camAngleX, &camAngleY);
        while(!WindowShouldClose()) {
                UpdateEditorCamera(&cam, &camAngleX, &camAngleY, isTyping, isMouseOverUI);                
                BeginDrawing();
                ClearBackground(BLACK);
                BeginMode3D(cam);
                DrawCube((Vector3){0.0f, 0.0f, 0.0f}, 10.0f, 10.0f, 10.0f, RED);
                DrawGrid(100, 1.0f);
                EndMode3D();
                EndDrawing();
        }
        
        CloseWindow();
        return 0;
}