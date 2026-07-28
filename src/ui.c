#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define  NOB_IMPLEMENTATION
#include <nob.h>
#include <ui.h>
#include <canvas.h>
#include <raylib.h>
#include <stddef.h>
#include <Info_Panel.h>
#include <Animations.h>

Ui_State UI;
Canvas c;

static String_Builder duration_sb = {0};
static bool select_duration_field = false;
float duration;
bool loops;
bool update_duration = false;

static float Padding = 20.0f;
static float thick = 2.5f;
static float field_height = 50.0f;
static int font_size = 24;

void ui_init()
{
    UI.font = LoadFont("res/Fonts/0xProtoNerdFont-Regular.ttf");
    UI.currentAnimationIndex = 0;
    UI.IsAnimationSelected = false;
    da_append(&UI.texture_paths, "res/Player.png");
    Init_Canvas(&c, &UI);
}

void parse_float_from_sb(String_Builder *sb)
{
    UI.animations.items[UI.currentAnimationIndex].duration = 0;
    bool after_dot = false;
    float frac = 0.1f;

    for (size_t i = 0; i < sb->count; ++i) {
        if (sb->items[i] == '.') {
            after_dot = true;
            continue;
        }
        int digit = sb->items[i] - '0';
        if (!after_dot) {
            UI.animations.items[UI.currentAnimationIndex].duration = UI.animations.items[UI.currentAnimationIndex].duration * 10.0f + digit;
        } else {
            UI.animations.items[UI.currentAnimationIndex].duration += digit * frac;
            frac *= 0.1f;
        }
    }

    free(sb->items);
    sb->items = NULL;
    sb->count = 0;
    sb->capacity = 0;
}

void Animation_Panel_Show(Rectangle bondry)
{
    Rectangle name_field_box = {
        .x = bondry.x + Padding,
        .y = bondry.y + Padding,
        .width = bondry.width - (2 * Padding),
        .height = field_height
    };

    Rectangle name_field_box_outline = {
        .x = bondry.x + Padding - thick,
        .y = bondry.y + Padding - thick,
        .width = bondry.width - (2 * Padding) + (2 * thick),
        .height = field_height + (2 * thick)
    };


    Rectangle duration_field_box = {
        .x = bondry.x + Padding,
        .y = name_field_box_outline.y + name_field_box_outline.height + Padding,
        .width = bondry.width - (2 * Padding),
        .height = field_height
    };

    Rectangle duration_field_box_outline = {
        .x = duration_field_box.x - thick,
        .y = duration_field_box.y - thick,
        .width = duration_field_box.width + (2 * thick),
        .height = field_height + (2 * thick)
    };

    const char* text = " No Animation Has\n been Selected";
    const char* rendertext = NULL;
    bool DrawBox = false;
    Color text_color = GRAY;

    if (UI.IsAnimationSelected) {
        text = UI.animations.items[UI.currentAnimationIndex].name;
        text_color = WHITE;
        DrawBox = true;

        duration = UI.animations.items[UI.currentAnimationIndex].duration;
        if (duration_sb.items == NULL) {
            if (duration == 0) {
                sb_appendf(&duration_sb, "%.2f", duration);
            } else {
                sb_appendf(&duration_sb, "%.2f", duration);
            }
        }
    }

    if (DrawBox) {
        DrawRectangleRec(name_field_box, GetColor(0x181818ff));
        DrawRectangleLinesEx(name_field_box_outline, thick, WHITE);
        DrawRectangleRec(duration_field_box, GetColor(0x181818ff));
        DrawRectangleLinesEx(duration_field_box_outline, thick, WHITE);

        Vector2 MousePos = GetMousePosition();

        if (CheckCollisionPointRec(MousePos, bondry)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {select_duration_field = false; update_duration = true;}
        }
        if (CheckCollisionPointRec(MousePos, duration_field_box)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) select_duration_field = true; 
        }
        
        if (select_duration_field) {
            int key = GetCharPressed();

            if (IsKeyPressed(KEY_BACKSPACE) && duration_sb.count != 0) {
                duration_sb.count--;
            }
            if (IsKeyPressed(KEY_ENTER)) {
                select_duration_field = false;
                update_duration = true;
            }

            if ((key >= '0' && key <= '9') || key == '.') {
                sb_appendf(&duration_sb, "%c", key);
            }
        }

        rendertext = strndup(duration_sb.items, duration_sb.count);

        if (select_duration_field) {
            DrawRectangleRec((Rectangle) {
                .x = (duration_field_box.x + duration_field_box.width*0.1f) + MeasureTextEx(UI.font , rendertext, font_size, 0).x,
                .y = (duration_field_box.y + (duration_field_box.height*0.5f - font_size*0.5)),
                .width = 12.0f,
                .height = font_size
            } , WHITE);
        }

        DrawTextEx(UI.font, rendertext, (Vector2) {
            .x = duration_field_box.x + duration_field_box.width*0.1f,
            .y = duration_field_box.y + duration_field_box.height*0.5f - font_size*0.5f
        }
        , font_size, 0, WHITE);

        if (update_duration) {
            parse_float_from_sb(&duration_sb);
            update_duration = false;
        }
    }

    DrawTextEx(UI.font, text, (Vector2) {
        (name_field_box.x + name_field_box.width*0.1f),
        (name_field_box.y + (name_field_box.height*0.5f - font_size*0.5))
    },
    font_size, 0, text_color);
    free((void *)rendertext);
}

void Animation_Panel(Rectangle bondry) {
    Color color = GetColor(0x252525ff);
    DrawRectangleRec(bondry, color);
    Animation_Panel_Show(bondry);
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

    Info_Panel(&c, InfoPanelBondry, &UI);
    Animation_Panel(rightPanelBondry);
    Animations_Panel(&UI, leftPanelBondry);
    Draw_Canvas(canvasBondry, &c);
}

void ui_unload()
{
    UnloadTexture(c.texture);
    UnloadShader(c.backgroundShader);
    UnloadFont(UI.font);
}
