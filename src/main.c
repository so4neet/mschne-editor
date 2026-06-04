#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "brush.h"
#include "bsp.h"
#include "wrld.h"

// Editor State

static Brush brushes[MAX_BRUSHES];
static int brush_count = 0;
static int selected = -1;

// Movement State

static int dragging = 0;
static float drag_offset[3];
static float drag_plane_y;

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

static void brush_center(const Brush *b, float out[3]) {
    out[0] = (b->min[0] + b->max[0]) * 0.5f;
    out[1] = (b->min[1] + b->max[1]) * 0.5f;
    out[2] = (b->min[2] + b->max[2]) * 0.5f;
}

static int ray_hit_yplane(Ray ray, float y, float out[3]) {
    float denom = ray.direction.y;
    if (denom == 0.0f) return 0;
    float t = (y - ray.position.y) / denom;
    if (t < 0.0f) return 0;
    out[0] = ray.position.x + ray.direction.x * t;
    out[1] = y;
    out[2] = ray.position.z + ray.direction.z * t;
    return 1;
}

void wrld_load(const char *path) {
    uint32_t node_count = 0;
    uint32_t wbrush_count = 0;
    WrldBrush *wbrushes = NULL;
    
    WrldNode *nodes = wrld_read(path, &node_count, &wbrushes, &wbrush_count);
    if (!nodes) return;
        
    brush_count = 0;
    selected = -1;
    
    for (uint32_t i=0; i<wbrush_count && brush_count < MAX_BRUSHES; i++) {
        memcpy(brushes[brush_count].min, wbrushes[i].min, sizeof(float)*3);
        memcpy(brushes[brush_count].max, wbrushes[i].max, sizeof(float)*3);
        brushes[brush_count].selected = 0;
        brush_count++;
    }
    
    free(wbrushes);
    free(nodes);
    TraceLog(LOG_INFO, "Loaded %s (%d brushes, %d nodes)", path, brush_count, node_count);
}

// Compile
static void compile(void) {
    if (brush_count == 0) return;
    
    Tree tree = bsp_compile(brushes, brush_count);
    int result = wrld_write("test.wrld", &tree, brushes, brush_count);
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
        if (!dragging && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            UpdateCamera(&cam, CAMERA_FREE);            
        }
        if (IsKeyPressed(KEY_B)) add_brush();
        if (IsKeyPressed(KEY_C)) compile();
        if (IsKeyPressed(KEY_L)) wrld_load("test.wrld");
        if (IsKeyPressed(KEY_DELETE) && selected >= 0) {
            for (int i=selected; i<brush_count - 1; i++) {
                brushes[i] = brushes[i+1];
            }
            brush_count--;
            selected = -1;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetScreenToWorldRay(GetMousePosition(), cam);
            int hit = -1;
            float closest = 1e9f;
            
            for (int i=0; i<brush_count; i++) {
                RayCollision rc = GetRayCollisionBox(ray, brush_to_bb(&brushes[i]));
                if (rc.hit && rc.distance < closest) {
                    closest = rc.distance;
                    hit = i;
                }
            }
            
            if (hit >= 0) {
                selected = hit;
                
                float center[3];
                brush_center(&brushes[selected], center);
                drag_plane_y = center[1];
                
                float world_hit[3];
                if (ray_hit_yplane(ray, drag_plane_y, world_hit)) {
                    drag_offset[0] = world_hit[0] - center[0];
                    drag_offset[2] = world_hit[2] - center[0];
                    dragging = 1;
                }
            } else {
                selected = -1;
                dragging = 0;
            }
        }
        
        if (dragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && selected >= 0) {
            Ray ray = GetScreenToWorldRay(GetMousePosition(), cam);
            float world_hit[3];
            
            if (ray_hit_yplane(ray, drag_plane_y, world_hit)) {
                float center[3];
                brush_center(&brushes[selected], center);
                
                float new_cx = world_hit[0] - drag_offset[0];
                float new_cz = world_hit[2] - drag_offset[2];
                float hx = (brushes[selected].max[0] - brushes[selected].min[0]) * 0.5f;
                float hz = (brushes[selected].max[2] - brushes[selected].min[2]) * 0.5f;
                
                brushes[selected].min[0] = new_cx - hx;
                brushes[selected].max[0] = new_cx + hx;
                brushes[selected].min[2] = new_cz - hz;
                brushes[selected].max[2] = new_cz + hz;
            }
        }
        
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            dragging = 0;
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