#include <stdbool.h>
#define  NOB_IMPLEMENTATION
#include <nob.h>
#include <ui.h>
#include <canvas.h>
#include <raylib.h>
#include <stddef.h>
#include <Info_Panel.h>
#include <Animations.h>

Ui_State UI;
Canvas   c;

void ui_init()
{
    UI.font = LoadFont("res/Fonts/0xProtoNerdFont-Regular.ttf");
    UI.currentAnimationIndex = 0;
    UI.IsAnimationSelected = false;
    UI.ShowMenu = false;
    UI.Win_state = WELLCOME_WINDOW;

    UI.texture_paths = (Texture_Paths) {0};
    da_append(&UI.texture_paths, "res/Default.png");
    UI.NeedsLoading = true;
    Init_Canvas(&c, &UI);
}

void ui_update()
{
    float InfoPanelHeight = 80.0f;
    float sidePanelWidth = 300.0f;
    float padding = 30.0f;
    float font_size = 24.0f;
    float w = GetScreenWidth();
    float h = GetScreenHeight();

    Rectangle InfoPanelBondry = {
        .x = 0,
        .y = 0,
        .width = w,
        .height = InfoPanelHeight
    };

    Rectangle leftPanelBondry = {
        .x = 0,
        .y = InfoPanelHeight,
        .width = sidePanelWidth,
        .height = h - InfoPanelHeight
    };

    Rectangle rightPanelBondry = {
        .x = w - sidePanelWidth,
        .y = InfoPanelHeight,
        .width = sidePanelWidth,
        .height = h - InfoPanelHeight
    };

    Rectangle canvasBondry = {
        .x = sidePanelWidth + padding,
        .y = InfoPanelHeight + padding,
        .width = w - (2 * (sidePanelWidth + padding)),
        .height = h - (InfoPanelHeight + 2 * padding)
    };

    switch (UI.Win_state) {
        case WELLCOME_WINDOW: break;
        case ANIMATION_WINDOW: {
            Draw_Canvas(&UI, canvasBondry, &c);
            Info_Panel(&c, InfoPanelBondry, &UI);
            Animation_Panel(&UI, &c,rightPanelBondry);
            Animations_Panel(&UI, leftPanelBondry);
        } break;
        case COLLISION_WINDOW: break;
        case WINDOW_COUNT: UNREACHABLE("WINDOW_COUNT"); break;
    }


    if (IsKeyPressed(KEY_TAB)) {
        UI.ShowMenu = !UI.ShowMenu;
    }

    if (UI.ShowMenu) {
        float thick = 5.0f;
        // Clear the background with some transparinccy.
        DrawRectangle(0, 0, w, h, GetColor(0x181818AA));

        Rectangle Menu = {
            .x = sidePanelWidth,
            .y = InfoPanelHeight,
            .width = w - (2 * sidePanelWidth),
            .height = h - (InfoPanelHeight + padding)
        };

        Rectangle Menu_outline = {
            .x = Menu.x - thick,
            .y = Menu.y - thick,
            .width = Menu.width + 2*thick,
            .height = Menu.height + 2*thick
        };

        DrawRectangleRec(Menu, GetColor(0x181818ff));
        DrawRectangleLinesEx(Menu_outline, thick, WHITE);

        for (size_t i = 0; i < WINDOW_COUNT; ++i) {
            Color color = GetColor(0x252525ff);

            Rectangle tiny_box = {
                .x = Menu.x + Menu.width*0.25,
                .y = Menu.y + font_size + (padding * (i) * 2) + (i*padding),
                .width = Menu.width*0.5,
                .height = font_size + padding
            };

            Rectangle tiny_box_outline = {
                .x = tiny_box.x - thick,
                .y = tiny_box.y - thick,
                .width = tiny_box.width + 2*thick,
                .height = tiny_box.height + 2*thick
            };


            const char* text;
            switch ((WINDOW_State) i) {
                case WELLCOME_WINDOW: {
                    text = "Welcome Win";
                    if (UI.Win_state == WELLCOME_WINDOW) {
                        color = ColorBrightness(color, 0.2f);
                    }
                }; break;
                case ANIMATION_WINDOW: {
                    text = "Animation Win";
                    if (UI.Win_state == ANIMATION_WINDOW) {
                        color = ColorBrightness(color, 0.2f);
                    }
                }; break;
                case COLLISION_WINDOW: {
                    text = "Collision Win";
                    if (UI.Win_state == COLLISION_WINDOW) {
                        color = ColorBrightness(color, 0.2f);
                    }
                }; break;
                case WINDOW_COUNT: UNREACHABLE("WINDOW_COUNT");
            }

            Vector2 MousePos = GetMousePosition();
            if (CheckCollisionPointRec(MousePos, tiny_box)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    UI.Win_state = (WINDOW_State) i;
                    UI.ShowMenu = false;
                }
                color = ColorBrightness(color, 0.3f);
            }
            
            DrawRectangleRec(tiny_box, color);
            DrawTextEx(UI.font, text, (Vector2) {
                .x = tiny_box.x + tiny_box.width/2 - MeasureTextEx(UI.font, text, font_size, 0).x/2,
                .y = tiny_box.y + tiny_box.height/2 - ((float)font_size/2)
            }, font_size, 0, WHITE);
            DrawRectangleLinesEx(tiny_box_outline, thick, WHITE);
        }
    }
}

void ui_unload()
{
    Unload_Textures(&c);
    UnloadShader(c.backgroundShader);
    UnloadFont(UI.font);
}
