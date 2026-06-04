#pragma once
#include "brush.h"

// Planes are defined by a normal vector and distance from origin. Equation is dot(normal, point) = distance
// If a point is dot(normal, point) is GREATER than the distance, it's in front. LESS than, its behind. 

typedef struct Plane {
    float normal[3];
    float distance;
} Plane;

// BSP Nodes. 

typedef struct Node {
    Plane splitter;        // Splitting plane
    int brush_index;        // Which brush generated this node
    struct Node *front;     // Sub-tree in front of the plane
    struct Node *back;      // Sub-tree behind the plane
    int is_leaf;
    int is_solid;            // 1 = Solid, 0 = Empty
} Node;

// Full BSP Tree

typedef struct Tree {
    Node *root;
    int node_count;
} Tree;

Tree bsp_compile(Brush *brushes, int count);

void bsp_free(Node *node);