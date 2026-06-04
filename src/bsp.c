#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bsp.h"

// This returns a plane from one face of a brush 

static Plane plane_from_face(const Brush *b, int axis, int side) {
    Plane p;
    memset(p.normal, 0, sizeof(p.normal));
    
    if (side == 0) {
        // Min face = normal points inward
        p.normal[axis] = -1.0f;
        p.distance = -b->min[axis];
    } else {
        // The normal points outward
        p.normal[axis] = 1.0f;
        p.distance = b->max[axis];
    }
    return p;
}

// Classify if the brush is in front, behind, or straddling a plane

static int classify_brush(const Brush *b, const Plane *plane) {
    int front = 0, behind = 0;
    
    for (int x=0; x<2; x++) {
        for (int y=0; y<2; y++) {
            for (int z=0; z<2; z++) {
                float corner[3] = {
                    x ? b->max[0] : b->min[0],
                    y ? b->max[1] : b->min[1],
                    z ? b->max[2] : b->min[2]
                };
                
                // dot(normal, corner) vs distance
                float d = corner[0]*plane->normal[0]
                        + corner[1]*plane->normal[1]
                        + corner[2]*plane->normal[2]
                        - plane->distance;
                if (d > 0.001f)    front = 1;
                if (d < -0.001f)   behind = 1;
            }
        }
    }
    
    if (front && !behind) return 1;
    if (behind && !front) return -1;
    return 0;
}

// BSP Compiler, partitions brushes into a tree

static Node *build_node(Brush *brushes, int count, int *total_nodes) {
    if (count == 0) {
        // Empty
        Node *leaf = calloc(1, sizeof(Node));
        leaf->is_leaf = 1;
        leaf->is_solid = 0;
        leaf->brush_index = -1;
        (*total_nodes)++;
        return leaf;
    }
    // First X face as splitter
    Plane splitter = plane_from_face(&brushes[0], 0, 1);
    
    Node *node = calloc(1, sizeof(Node));
    node->splitter = splitter;
    node->brush_index = 0;
    node->is_leaf = 0;
    (*total_nodes)++;
    
    Brush *front_list = malloc(count * sizeof(Brush));
    Brush *back_list = malloc(count * sizeof(Brush));
    int front_count = 0, back_count = 0;
    
    // Skip the splitter node 
    for (int i=1; i<count; i++) {
        int side = classify_brush(&brushes[i], &splitter);
        if (side >= 0) front_list[front_count++] = brushes[i];
        if (side <= 0) back_list[back_count++] = brushes[i];
        // Duplicating straddling nodes currently
    }
    
    node->front = build_node(front_list, front_count, total_nodes);
    node->back = build_node(back_list, back_count, total_nodes);
    
    free(front_list);
    free(back_list);
    return node;
}

Tree bsp_compile(Brush *brushes, int count) {
    Tree tree = {0};
    tree.root = build_node(brushes, count, &tree.node_count);
    return tree;
}

void bsp_free(Node *node) {
    if (!node) return;
    bsp_free(node->front);
    bsp_free(node->back);
    free(node);
}