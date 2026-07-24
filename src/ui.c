#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#define  NOB_IMPLEMENTATION
#include <nob.h>
#include <ui.h>
#include <canvas.h>
#include <raylib.h>
#include <stddef.h>
#include <Info_Panel.h>
#include <animation.h>

Ui_State UI;
Canvas c;
Animations animations;

String_Builder animation_name_field = {0};
bool Case_Mode = false;
bool add       = false;

static float Padding = 20.0f;
static float field_height = 70.0f;
static int font_size = 24;


void ui_init()
{
    UI.font = LoadFont("res/font.ttf");
    
    Init_Canvas(&c);
}

void add_Animation(Rectangle bondry)
{

    Rectangle name_field_box = {
        .x = bondry.x + Padding,
        .y = bondry.y + Padding,
        .width = bondry.width - (2 * Padding),
        .height = field_height
    };
    Rectangle add_button_box = {
        .x = name_field_box.x + Padding,
        .y = name_field_box.y + name_field_box.height + Padding,
        .width = name_field_box.width - (2 * Padding),
        .height = field_height
    };

    DrawRectangleRec(name_field_box, GetColor(0x181818ff));
    const char* text = nob_temp_sv_to_cstr(sb_to_sv(animation_name_field));
    DrawText(text,
             (int) (name_field_box.x + name_field_box.width*0.1f),
             (int) (name_field_box.y + (name_field_box.height*0.5f - font_size*0.5)),
             font_size, WHITE);

    DrawRectangleRec(add_button_box, GetColor(0x181818ff));
    DrawTextEx(UI.font ,"Add Animation", (Vector2) {
             (add_button_box.x + add_button_box.width*0.1f),
             (add_button_box.y + add_button_box.height*0.5f - font_size*0.5)
                },
             font_size, 0, WHITE);

    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, bondry)) {
        int key = GetKeyPressed();
        if (animation_name_field.count == 15) return;

        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            Case_Mode = true;
        } else {
            Case_Mode = false;
        }

        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
            const char* cliptext = GetClipboardText();
            if (!(strcmp(cliptext, "") && strcmp(cliptext, "\n") && cliptext == NULL)) {
                sb_append_cstr(&animation_name_field, cliptext);
                // TODO: It's still buggie nedds some work
            }
        }
        switch (key) {
            case KEY_BACKSPACE: if (!(animation_name_field.count == 0)) animation_name_field.count--;break;
            case 0: break;
            case KEY_ENTER: add = true; break;
            case KEY_LEFT_SHIFT: break;
            case KEY_RIGHT_SHIFT: break;
            case KEY_LEFT_CONTROL: break;
            case KEY_RIGHT_CONTROL: break;
            default: {
                if(Case_Mode) {
                    sb_append(&animation_name_field, toupper(GetKeyName(key)[0]));
                } else {
                    sb_append(&animation_name_field, GetKeyName(key)[0]);
                }
            }
            break;
        }
    }
    if (CheckCollisionPointRec(MousePos, add_button_box) || add) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || add) {
            if (animation_name_field.count == 0) return;
            sb_append_null(&animation_name_field);

            Animation animation = {
                .name = strdup(animation_name_field.items),
                .duration = 0,
                .loops = false,
                .frames = {.Frame = NULL, .count = 0}
            };
            
            da_append(&animations, animation);
            animation_name_field.count = 0;
            add = false;
        }
    }
}

void Show_Animations(Rectangle bondry)
{
    Rectangle box = {
        .x = bondry.x + Padding,
        .y = bondry.y + bondry.height*0.6,
        .width = bondry.width - (2 * Padding),
        .height = bondry.height - bondry.height*0.6 - Padding
    };

    BeginScissorMode(box.x, box.y, box.width, box.height);
    DrawRectangleRec(box, GetColor(0x181818ff));

    if (animations.items == NULL) return;
    for (size_t i = 0; i < animations.count; ++i) {
        DrawTextEx(UI.font, animations.items[i].name, (Vector2) {
                (box.x + box.width*0.1f),
                (box.y + font_size*i + 1 + Padding)
                    },
                 font_size, 0,WHITE);
    }
    EndScissorMode();
}

void Animation_Panel(Rectangle bondry) {
    Color color = GetColor(0x252525ff);
    DrawRectangleRec(bondry, color);
    add_Animation(bondry);
    Show_Animations(bondry);
}

void right_panel(Rectangle bondry) {
    Color color = GetColor(0x12fa13ff);
    DrawRectangleRec(bondry, color);
}



void ui_update()
{
    float InfoPanelHeight = 80.0f;
    float sidePanelWidth = 300.0f;
    float padding = 30.0f;
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
    Info_Panel(&c, InfoPanelBondry, UI);
    right_panel(rightPanelBondry);
    Animation_Panel(leftPanelBondry);
    Draw_Canvas(canvasBondry, &c);
}

void ui_unload()
{
    UnloadTexture(c.texture);
    UnloadShader(c.backgroundShader);
}
