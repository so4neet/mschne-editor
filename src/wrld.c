#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wrld.h"

// Flatten tree into array

static void flatten(Node *node, WrldNode *out, int *idx, int total) {
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
    int front_slot = *idx;
    w->front_idx = node->front ? front_slot : -1;
    flatten(node->front, out, idx, total);
    
    int back_slot = *idx;
    out[index].back_idx = node->back ? back_slot : -1;
    flatten(node->back, out, idx, total);
}

int wrld_write(const char *path, const Tree *tree) {
    // Flatten 
    WrldNode *nodes = calloc(tree->node_count, sizeof(WrldNode));
    if (!nodes) return -1;
    
    int idx = 0;
    flatten(tree->root, nodes, &idx, tree->node_count);
    
    FILE *f = fopen(path, "wb");
    if (!f) { free(nodes); return -1; }
    
    WrldHeader head;
    memcpy(head.identifier, "WRLD", 4);
    head.version = 1;
    head.node_count = (uint32_t)tree->node_count;
    
    fwrite(&head, sizeof(WrldHeader), 1, f);
    fwrite(nodes, sizeof(WrldNode), tree->node_count, f);
    
    fclose(f);
    free(nodes);
    return 0;
}

WrldNode *wrld_read(const char *path, uint32_t *out_count) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
        
    WrldHeader head;
    fread(&head, sizeof(WrldHeader), 1, f);
    
    // Validate identifier and version
    if (memcmp(head.identifier, "WRLD", 4) != 0 || head.version != 1) {
        fclose(f);
        return NULL;
    }
    
    WrldNode *nodes = malloc(head.node_count * sizeof(WrldNode));
    fread(nodes, sizeof(WrldNode), head.node_count, f);
    fclose(f);
    
    *out_count = head.node_count;
    return nodes;
}