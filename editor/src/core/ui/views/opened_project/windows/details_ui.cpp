#include "details_ui.h"
#include "animation_editor_ui.h"
#include "../../../../scene/scene_manager.h"
#include "../../../../project_properties.h"
#include "../../../../asset_browser.h"
#include "../../../../editor_callbacks.h"

// Temp
struct FuncObject {
    FuncObject(std::function<void()> func) {
        func();
    }
};

namespace ComponentDetailsDrawUtils {
void DrawTransform2D(SceneNode* node) {
    if (auto* transform2DComp = node->GetComponentSafe<Transform2DComp>()) {
        ImGui::Text("Transform 2D Component");

        ImGuiHelper::DragFloat2 positionDragFloat2("Position", (float*) &transform2DComp->transform2D.position);
        ImGuiHelper::BeginDragFloat2(positionDragFloat2);

        ImGuiHelper::DragFloat2 scaleDragFloat2("Scale", (float*) &transform2DComp->transform2D.scale);
        ImGuiHelper::BeginDragFloat2(scaleDragFloat2);

        ImGuiHelper::DragFloat rotationDragFloat("Rotation", transform2DComp->transform2D.rotation);
        ImGuiHelper::BeginDragFloat(rotationDragFloat);

        ImGuiHelper::DragInt zIndexDragInt("Z Index", transform2DComp->zIndex);
        zIndexDragInt.valueMin = -1000;
        zIndexDragInt.valueMax = 1000;
        ImGuiHelper::BeginDragInt(zIndexDragInt);

        ImGuiHelper::CheckBox zIsRelativeToParentCheckBox("Z Is Relative To Parent", transform2DComp->isZIndexRelativeToParent);
        ImGuiHelper::BeginCheckBox(zIsRelativeToParentCheckBox);

        ImGuiHelper::CheckBox ignoreCameraCheckBox("Ignore Camera", transform2DComp->ignoreCamera);
        ImGuiHelper::BeginCheckBox(ignoreCameraCheckBox);

        ImGui::Separator();
    }
}

void DrawSprite(SceneNode* node) {
    if (SpriteComp* spriteComp = node->GetComponentSafe<SpriteComp>()) {
        ImGui::Text("Sprite Component");

        // Texture Path Combo Box
        static AssetBrowser* assetBrowser = AssetBrowser::Get();

        static ImGuiHelper::AssetBrowserComboBox spriteTexturePathComboBox("Texture Path", ".png");
        spriteTexturePathComboBox.onSelectionChangeCallback = [spriteComp](const char* newItem) {
            spriteComp->texturePath = newItem;
            if (spriteComp->texturePath == ImGuiHelper::COMBO_BOX_LIST_NONE) {
                spriteComp->texturePath.clear();
            }
        };

        static ImGuiHelper::AssetBrowserComboBox shaderPathComboBox("Shader Path", ".shader");
        shaderPathComboBox.onSelectionChangeCallback = [spriteComp](const char* newItem) {
            spriteComp->shaderPath = newItem;
            if (spriteComp->shaderPath == ImGuiHelper::COMBO_BOX_LIST_NONE) {
                spriteComp->shaderPath.clear();
            }
        };

        static FuncObject initializeFunc = FuncObject([spriteComp] {
            if (spriteComp->texturePath.empty()) {
                spriteTexturePathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE);
            } else {
                spriteTexturePathComboBox.SetSelected(spriteComp->texturePath);
            }
            if (spriteComp->shaderPath.empty()) {
                shaderPathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE);
            } else {
                shaderPathComboBox.SetSelected(spriteComp->shaderPath);
            }
            EditorCallbacks::Get()->RegisterOnSceneNodeSelected([](SceneNode* sceneNode) {
                // Disable on selection call back while switching nodes to prevent previous node from being set
                spriteTexturePathComboBox.onSelectionChangeCallback = nullptr;
                shaderPathComboBox.onSelectionChangeCallback = nullptr;
                if (auto spriteC = sceneNode->GetComponentSafe<SpriteComp>()) {
                    if (spriteC->texturePath.empty()) {
                        spriteTexturePathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE);
                    } else {
                        spriteTexturePathComboBox.SetSelected(spriteC->texturePath);
                    }
                    if (spriteC->shaderPath.empty()) {
                        shaderPathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE);
                    } else {
                        shaderPathComboBox.SetSelected(spriteC->shaderPath);
                    }
                }
            });
        });

        ImGuiHelper::BeginAssetBrowserComboBox(spriteTexturePathComboBox);

        // TODO: Update draw source automatically when a texture is set (can also make it toggleable)
        ImGuiHelper::DragFloat4 drawSourceDragFloat4("Draw Source", (float*) &spriteComp->drawSource);
        ImGuiHelper::BeginDragFloat4(drawSourceDragFloat4);

        ImGuiHelper::DragFloat2 originDragFloat2("Origin", (float*) &spriteComp->origin);
        ImGuiHelper::BeginDragFloat2(originDragFloat2);

        ImGuiHelper::ColorEdit4 modulateColorEdit4("Modulate", (float*) &spriteComp->modulate);
        ImGuiHelper::BeginColorEdit4(modulateColorEdit4);

        ImGuiHelper::CheckBox flipXCheckBox("Flix X", spriteComp->flipX);
        ImGuiHelper::BeginCheckBox(flipXCheckBox);

        ImGuiHelper::CheckBox flipYCheckBox("Flix Y", spriteComp->flipY);
        ImGuiHelper::BeginCheckBox(flipYCheckBox);

        ImGuiHelper::BeginAssetBrowserComboBox(shaderPathComboBox);

        ImGui::Separator();
    }
}

void DrawAnimatedSprite(SceneNode* node) {
    if (auto* animatedSpriteComp = node->GetComponentSafe<AnimatedSpriteComp>()) {
        ImGui::Text("Animated Sprite Component");

        static std::vector<std::string> spriteAnimationList = {ImGuiHelper::COMBO_BOX_LIST_NONE };
        static auto UpdateSpriteAnimationList = [] (AnimatedSpriteComp* animSpriteComp) {
            spriteAnimationList.clear();
            spriteAnimationList.emplace_back(ImGuiHelper::COMBO_BOX_LIST_NONE);
            for (const auto& anim : animSpriteComp->animations) {
                spriteAnimationList.emplace_back(anim.name);
            }
        };
        UpdateSpriteAnimationList(animatedSpriteComp);
        static ImGuiHelper::ComboBox spriteAnimationSelectionComboBox("Current Animation", spriteAnimationList);
        spriteAnimationSelectionComboBox.items = spriteAnimationList;
        spriteAnimationSelectionComboBox.onSelectionChangeCallback = [animatedSpriteComp] (const char* newItem) {
            animatedSpriteComp->currentAnimationName = newItem;
            if (animatedSpriteComp->currentAnimationName == ImGuiHelper::COMBO_BOX_LIST_NONE) {
                animatedSpriteComp->currentAnimationName.clear();
            }
        };
        ImGuiHelper::BeginComboBox(spriteAnimationSelectionComboBox);

        if (ImGui::Button("Edit Animations")) {
            static OpenedProjectUI::Windows::AnimationEditor* animationEditor = OpenedProjectUI::Windows::AnimationEditor::Get();
            ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&animationEditor->GetPopup(animatedSpriteComp));
        }
        ImGui::SameLine();
        ImGui::Text("Count: %zu", animatedSpriteComp->animations.size());

        ImGuiHelper::CheckBox ignoreCameraCheckBox("Is Playing", animatedSpriteComp->isPlaying);
        ImGuiHelper::BeginCheckBox(ignoreCameraCheckBox);

        ImGuiHelper::DragFloat2 originDragFloat2("Origin", (float*) &animatedSpriteComp->origin);
        ImGuiHelper::BeginDragFloat2(originDragFloat2);

        ImGuiHelper::ColorEdit4 modulateColorEdit4("Modulate", (float*) &animatedSpriteComp->modulate);
        ImGuiHelper::BeginColorEdit4(modulateColorEdit4);

        ImGuiHelper::CheckBox flipXCheckBox("Flix X", animatedSpriteComp->flipX);
        ImGuiHelper::BeginCheckBox(flipXCheckBox);

        ImGuiHelper::CheckBox flipYCheckBox("Flix Y", animatedSpriteComp->flipY);
        ImGuiHelper::BeginCheckBox(flipYCheckBox);

        static ImGuiHelper::AssetBrowserComboBox shaderPathComboBox("Shader Path", ".shader");
        shaderPathComboBox.onSelectionChangeCallback = [animatedSpriteComp](const char* newItem) {
            animatedSpriteComp->shaderPath = newItem;
            if (animatedSpriteComp->shaderPath == ImGuiHelper::COMBO_BOX_LIST_NONE) {
                animatedSpriteComp->shaderPath.clear();
            }
        };

        static FuncObject initializeFunc = FuncObject([animatedSpriteComp] {
            if (animatedSpriteComp->shaderPath.empty()) {
                shaderPathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE);
            } else {
                shaderPathComboBox.SetSelected(animatedSpriteComp->shaderPath);
            }
            EditorCallbacks::Get()->RegisterOnSceneNodeSelected([](SceneNode* sceneNode) {
                // Disable on selection call back while switching nodes to prevent previous node from being set
                shaderPathComboBox.onSelectionChangeCallback = nullptr;
                if (auto animatedSpriteC = sceneNode->GetComponentSafe<AnimatedSpriteComp>()) {
                    if (animatedSpriteC->shaderPath.empty()) {
                        shaderPathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE);
                    } else {
                        shaderPathComboBox.SetSelected(animatedSpriteC->shaderPath);
                    }
                }
            });
        });

        ImGuiHelper::BeginAssetBrowserComboBox(shaderPathComboBox);

        ImGui::Separator();
    }
}

void DrawTextLabel(SceneNode* node) {
    if (auto* textLabelComp = node->GetComponentSafe<TextLabelComp>()) {
        ImGui::Text("Text Label Component");

        ImGuiHelper::InputText textInputText("Text", textLabelComp->text);
        ImGuiHelper::BeginInputText(textInputText);

        ImGuiHelper::ColorEdit4 colorColorEdit4("Color", (float*) &textLabelComp->color);
        ImGuiHelper::BeginColorEdit4(colorColorEdit4);

        static TextLabelComp* lastTextLabelComp = nullptr;
        static ImGuiHelper::ComboBox fontUIDComboBox("FontUID", { "default", "fight-64" });
        if (textLabelComp != lastTextLabelComp) {
            lastTextLabelComp = textLabelComp;
            fontUIDComboBox.items.clear();
            fontUIDComboBox.items.emplace_back("default");
            ProjectProperties* projectProperties = ProjectProperties::Get();
            for (const auto& fontAsset : projectProperties->assets.fonts) {
                fontUIDComboBox.items.emplace_back(fontAsset.uid);
            }
            fontUIDComboBox.onSelectionChangeCallback = [textLabelComp] (const char* newItem) {
                textLabelComp->fontUID = newItem;
                if (textLabelComp->fontUID == "default") {
                    textLabelComp->fontUID.clear();
                }
            };
            const std::string selectedFontUID = !textLabelComp->fontUID.empty() ? textLabelComp->fontUID : "default";
            fontUIDComboBox.SetSelected(selectedFontUID);
        }
        ImGuiHelper::BeginComboBox(fontUIDComboBox);

        ImGui::Separator();
    }
}

void DrawScript(SceneNode* node) {
    ImGui::Text("Script Component");
    if (auto* scriptComp = node->GetComponentSafe<ScriptComp>()) {

        ImGuiHelper::InputText classPathInputText("Class Path", scriptComp->classPath);
        ImGuiHelper::BeginInputText(classPathInputText);

        ImGuiHelper::InputText classNameInputText("Class Name", scriptComp->className);
        ImGuiHelper::BeginInputText(classNameInputText);

        if (ImGui::Button("Remove Script")) {
            node->RemoveComponent<ScriptComp>();
        }
    } else if (ImGui::Button("Add Script")) {
        node->AddComponent<ScriptComp>();
    }
    ImGui::Separator();
}

void DrawCollider2D(SceneNode* node) {
    if (auto* collider2DComp = node->GetComponentSafe<Collider2DComp>()) {
        ImGui::Text("Collider2D Component");

        ImGuiHelper::DragFloat2 extentsDragFloat2("Extents", (float*) &collider2DComp->extents);
        ImGuiHelper::BeginDragFloat2(extentsDragFloat2);

        ImGuiHelper::ColorEdit4 colorColorEdit4("Color", (float*) &collider2DComp->color);
        ImGuiHelper::BeginColorEdit4(colorColorEdit4);

        ImGui::Separator();
    }
}

void DrawColorRect(SceneNode* node) {
    if (auto* colorRectComp = node->GetComponentSafe<ColorRectComp>()) {
        ImGui::Text("Color Rect Component");

        ImGuiHelper::DragFloat2 sizeDragFloat2("Size", (float*) &colorRectComp->size);
        ImGuiHelper::BeginDragFloat2(sizeDragFloat2);

        ImGuiHelper::ColorEdit4 colorColorEdit4("Color", (float*) &colorRectComp->color);
        ImGuiHelper::BeginColorEdit4(colorColorEdit4);

        ImGui::Separator();
    }
}

void DrawParallax(SceneNode* node) {
    if (auto* parallaxComp = node->GetComponentSafe<ParallaxComp>()) {
        ImGui::Text("Parallax Component");

        ImGuiHelper::DragFloat2 scrollSpeedDragFloat2("Scroll Speed", (float*) &parallaxComp->scrollSpeed);
        ImGuiHelper::BeginDragFloat2(scrollSpeedDragFloat2);

        ImGui::Separator();
    }
}
} // namespace ComponentDetailsDrawUtils

ImGuiHelper::Window OpenedProjectUI::Windows::GetDetailsWindow() {
    ImGuiHelper::Window detailsWindow = {
        .name = "Details",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static SceneManager* sceneManager = SceneManager::Get();
            if (SceneNode* selectedNode = sceneManager->selectedSceneNode) {
                ImGui::Text("Name: %s", selectedNode->name.c_str());
                ImGui::Text("Type: %s", selectedNode->GetTypeString());
                ImGui::Separator();
                ComponentDetailsDrawUtils::DrawTransform2D(selectedNode);
                ComponentDetailsDrawUtils::DrawSprite(selectedNode);
                ComponentDetailsDrawUtils::DrawAnimatedSprite(selectedNode);
                ComponentDetailsDrawUtils::DrawTextLabel(selectedNode);
                ComponentDetailsDrawUtils::DrawScript(selectedNode);
                ComponentDetailsDrawUtils::DrawCollider2D(selectedNode);
                ComponentDetailsDrawUtils::DrawColorRect(selectedNode);
                ComponentDetailsDrawUtils::DrawParallax(selectedNode);
            }
        },
        .position = ImVec2{ 400.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    return detailsWindow;
}
