#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wrld.h"

// Flatten tree into array

static void flatten(Node *node, WrldNode *out, int *idx) {
    if (!node) return;
    
    int index = (*idx)++;
    WrldNode *w = &out[index];
    
    w->normal[0] = node->splitter.normal[0];
    w->normal[1] = node->splitter.normal[1];
    w->normal[2] = node->splitter.normal[2];
    w->distance  = node->splitter.distance;
    w->is_leaf   = node->is_leaf;
    w->is_solid  = node->is_solid;
    w->_pad[0]   = w->_pad[1] = 0;
    
    // Children
    w->front_idx = node->front ? *idx : -1;
    flatten(node->front, out, idx);
    
    out[index].back_idx = node->back ? *idx : -1;
    flatten(node->back, out, idx);
}

int wrld_write(const char *path, const Tree *tree, Brush *brushes, int brush_count) {
    // Flatten 
    WrldNode *nodes = calloc(tree->node_count, sizeof(WrldNode));
    if (!nodes) return -1;
    
    int idx = 0;
    flatten(tree->root, nodes, &idx);
    
    FILE *f = fopen(path, "wb");
    if (!f) { free(nodes); return -1; }
    
    WrldHeader head;
    memcpy(head.identifier, "WRLD", 4);
    head.version = 2;
    head.brush_count = (uint32_t)brush_count;
    head.node_count =  (uint32_t)tree->node_count;
    
    fwrite(&head, sizeof(WrldHeader), 1, f);
    
    // Brushes
    for (int i=0; i<brush_count; i++) {
        WrldBrush wb;
        memcpy(wb.min, brushes[i].min, sizeof(float)*3);
        memcpy(wb.max, brushes[i].max, sizeof(float)*3);
        fwrite(&wb, sizeof(WrldBrush), 1, f);
    }
    
    fwrite(nodes, sizeof(WrldNode), tree->node_count, f);
    
    fclose(f);
    free(nodes);
    return 0;
}

WrldNode *wrld_read(const char *path, uint32_t *out_node_count, WrldBrush **out_brushes, uint32_t *out_brush_count) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
        
    WrldHeader head;
    fread(&head, sizeof(WrldHeader), 1, f);
    
    // Validate identifier and version
    if (memcmp(head.identifier, "WRLD", 4) != 0 || head.version != 2) {
        fclose(f);
        return NULL;
    }
    
    WrldBrush *brushes = malloc(head.brush_count * sizeof(WrldBrush));
    fread(brushes, sizeof(WrldBrush), head.brush_count, f);
    
    WrldNode *nodes = malloc(head.node_count * sizeof(WrldNode));
    fread(nodes, sizeof(WrldNode), head.node_count, f);
    fclose(f);
    
    *out_brush_count = head.brush_count;
    *out_node_count = head.node_count;
    *out_brushes = brushes;
    return nodes;
}
