#pragma once
#include <stdint.h>
#include "bsp.h"

/* WRLD Binary Format: 
    [ HEADER ]
    identifier : char[4] = "WRLD" 
    version : uint32     = 1
    node_count : uint32 
    
    [ NODES ]
    normal : float[3]
    distance : float
    front_idx : int32
    back_idx : int32
    is_leaf : uint8
    is_solid : uint8
    _pad : uint8[2]
    
*/

#pragma pack(push, 1)

typedef struct WrldHeader {
    char    identifier[4];
    uint32_t      version;
    uint32_t   node_count;
} WrldHeader;

typedef struct WrldNode {
    float   normal[3];
    float   distance;
    int32_t front_idx;
    int32_t back_idx;
    uint8_t is_leaf;
    uint8_t is_solid;
    uint8_t _pad[2];
} WrldNode;

#pragma pack(pop)

int wrld_write(const char *path, const Tree *tree);

WrldNode *wrld_read(const char *path, uint32_t *out_count);