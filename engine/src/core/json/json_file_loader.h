#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "../seika/src/math/se_curve_float.h"

#include "../ecs/entity/entity.h"
#include "../ecs/component/component.h"
#include "../ecs/component/node_component.h"

typedef struct JsonSceneNode {
    char* name;
    NodeBaseType type;
    void* components[MAX_COMPONENTS];
    struct JsonSceneNode* parent;
    struct JsonSceneNode* children[MAX_ENTITIES / 4];
    size_t childrenCount;
    char* spriteTexturePath;
    char* fontUID;
} JsonSceneNode;

struct CREGameProperties* cre_json_load_config_file(const char* filePath);
JsonSceneNode* cre_json_load_scene_file(const char* filePath);
void cre_json_delete_json_scene_node(JsonSceneNode* node);
SECurveFloat cre_json_load_curve_float_file(const char* filePath, bool *isSuccessful);

#ifdef __cplusplus
}
#endif
