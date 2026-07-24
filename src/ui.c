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
bool Ctrl      = false;

static float Padding = 20.0f;
static float field_height = 50.0f;
static int font_size = 24;


void ui_init()
{
    UI.font = LoadFont("res/Fonts/0xProtoNerdFont-Regular.ttf");
    
    Init_Canvas(&c);
}

char* Procces_char(String_Builder sd)
{
    String_Builder sb = {0};
    char delma = '\n';
    for (size_t i = 0; i < sd.count; ++i) {
        if (!(sd.items[i] == delma)) {
            sb_append(&sb, sd.items[i]);
        }
        else if (sb.count > 0 && sb.items[i-1] != ' ') {
            sb_append(&sb, ' ');
        }
    }
    sb_append_null(&sb);
    return sb.items;
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
    BeginScissorMode(name_field_box.x, name_field_box.y, name_field_box.width, name_field_box.height);
    const char* text = Procces_char(animation_name_field);
    DrawTextEx(UI.font, text, (Vector2) {
             (name_field_box.x + name_field_box.width*0.1f),
             (name_field_box.y + (name_field_box.height*0.5f - font_size*0.5))
                },
             font_size, 0, WHITE);
    EndScissorMode();

    DrawRectangleRec(add_button_box, GetColor(0x181818ff));
    DrawTextEx(UI.font ,"Add Animation", (Vector2) {
             (add_button_box.x + add_button_box.width*0.1f),
             (add_button_box.y + add_button_box.height*0.5f - font_size*0.5)
                },
             font_size, 0, WHITE);

    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, bondry)) {
        int key = GetCharPressed();

        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            Case_Mode = true;
        } else {
            Case_Mode = false;
        }

        if (IsKeyPressed(KEY_ENTER)) add = true;

        if (IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL)) Ctrl = true;
        if (Ctrl && IsKeyPressed(KEY_V)) {
            Ctrl = !Ctrl;
            const char* cliptext = GetClipboardText();
            if (!(cliptext == NULL)) {
                if ((animation_name_field.count + strlen(cliptext)) > 15) {
                    goto save;
                }
                sb_append_cstr(&animation_name_field, cliptext);
                goto save;
            }
        }
        if (IsKeyPressed(KEY_BACKSPACE)) if (!(animation_name_field.count == 0)) animation_name_field.count--;

        switch (key) {
            case 0: break;
            default: {
                if (animation_name_field.count >= 15) goto save;
                if(Case_Mode) {
                    sb_append(&animation_name_field, toupper((char) (key)));
                } else {
                    sb_append(&animation_name_field, (char) (key));
                }
            }
            break;
        }
    }
save:
    if (CheckCollisionPointRec(MousePos, add_button_box) || add) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || add) {
            if (animation_name_field.count == 0) goto clean;

            char* text = Procces_char(animation_name_field);
            if (strlen(text) == 0) goto clean;
            Animation animation = {
                .name = text,
                .duration = 0,
                .loops = false,
                .frames = {.Frame = NULL, .count = 0}
            };
            
            da_append(&animations, animation);
            goto clean;
        }
    }
        return;
clean:
    animation_name_field.count = 0;
    add = false;
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
    UnloadFont(UI.font);
}
