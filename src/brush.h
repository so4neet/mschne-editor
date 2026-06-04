#pragma once

// Learning BSP-Tree style map building, starting with brush-based geometry which is defined with a world-space minimum and maximum corner

typedef struct Brush {
    float min[3];
    float max[3];
    int selected;
} Brush;

#define MAX_BRUSHES 2048