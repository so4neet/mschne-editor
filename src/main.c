#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string.h>
#include "brush.h"
#include "bsp.h"
#include "wrld.h"

// Editor State

static Brush brushes[MAX_BRUSHES];
static int brush_count = 0;
static int selected = -1;

// Convert brush to BoundingBox

static BoundingBox brush_to_bb(const Brush *b) {
    return (BoundingBox) {
        .min = { b->min[0], b->min[1], b->min[2] },
        .max = { b->max[0], b->max[1], b->max[2] }
    };
}

static void add_brush(void) {
    if (brush_count >= MAX_BRUSHES) return;
    Brush *b = &brushes[brush_count++];
    b->min[0] = -0.5; b->min[1] = 0.0f; b->min[2] = -0.5f;
    b->max[0] = 0.5f; b->max[1] = 1.0f; b->max[2] = 0.5f;
    b->selected = 0;
}

// Compile
static void compile(void) {
    if (brush_count == 0) return;
    
    Tree tree = bsp_compile(brushes, brush_count);
    int result = wrld_write("test.wrld", &tree);
    bsp_free(tree.root);
    
    if (result == 0) {
        TraceLog(LOG_INFO, "Exported (%d nodes)", tree.node_count);
    } else {
        TraceLog(LOG_ERROR, "Export failed");
    }
}

int main(void) {
    const int DEF_WIDTH = 1280;
    const int DEF_HEIGHT = 720;
    
    InitWindow(DEF_WIDTH, DEF_HEIGHT, "MSCHNE Editor");
    SetTargetFPS(165);
    
    Camera3D cam = {
        .position = { 5.0f, 5.0f, 5.0f },
        .target = { 0.0f, 0.0f, 0.0f },
        .up = {0.0f, 1.0f, 0.0f },
        .fovy = 60.0f,
        .projection = CAMERA_PERSPECTIVE
    };

    while (!WindowShouldClose()) {
        UpdateCamera(&cam, CAMERA_FREE);
        if (IsKeyPressed(KEY_B)) add_brush();
        if (IsKeyPressed(KEY_C)) compile();
        if (IsKeyPressed(KEY_DELETE) && selected >= 0) {
            for (int i=selected; i<brush_count - 1; i++) {
                brushes[i] = brushes[i+1];
            }
            brush_count--;
            selected = -1;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetScreenToWorldRay(GetMousePosition(), cam);
            selected = -1;
            float closest = 1e9f;
            
            for (int i=0; i<brush_count; i++) {
                RayCollision hit = GetRayCollisionBox(ray, brush_to_bb(&brushes[i]));
                if (hit.hit && hit.distance < closest) {
                    closest = hit.distance;
                    selected = i;
                }
            }
        }
        
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode3D(cam);
                DrawGrid(20, 1.0f);
                
                for (int i=0; i<brush_count; i++) {
                    BoundingBox bb = brush_to_bb(&brushes[i]);
                    Vector3 size = Vector3Subtract(bb.max, bb.min);
                    Vector3 center = {
                        (bb.min.x + bb.max.x) * 0.5f,
                        (bb.min.y + bb.max.y) * 0.5f,
                        (bb.min.z + bb.max.z) * 0.5f
                    };
                    
                    Color fill = (i == selected)
                        ? (Color){255,200,50,80} : (Color){80,140,220,225};
                    DrawCube(center, size.x, size.y, size.z, fill);
                    
                    Color wire = (i == selected)
                        ? (Color){255,220,80,255} : (Color){80,140,220,255};
                    DrawCubeWires(center, size.x, size.y, size.z, wire);
                }
            EndMode3D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}