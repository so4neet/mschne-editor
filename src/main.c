#include <raylib.h>

int main() {
        InitWindow(1280, 720, "MSCHNE Wrld Editor");

        while(!WindowShouldClose()) {
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
        }
        CloseWindow();
        return 0;
}