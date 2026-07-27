#include "raylib.h"
#include <nob.h>
#include <ui.h>
#include <Animations.h>

String_Builder animation_name_field = {0};
bool Case_Mode = false;
bool add       = false;
bool Ctrl      = false;
bool select_name_field = false;

static float Padding = 20.0f;
static float thick = 2.5f;
static float field_height = 50.0f;
static int font_size = 24;

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

void add_Animation(Ui_State *ui, Rectangle bondry)
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

    Rectangle add_button_box = {
        .x = name_field_box.x + Padding,
        .y = name_field_box.y + name_field_box.height + Padding,
        .width = name_field_box.width - (2 * Padding),
        .height = field_height
    };

    Rectangle add_button_box_outline = {
        .x = name_field_box.x + Padding - thick,
        .y = name_field_box.y + name_field_box.height + Padding - thick,
        .width = name_field_box.width - (2 * Padding) + (2 * thick),
        .height = field_height + (2 * thick)
    };

    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, bondry)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) select_name_field = false; 
    }
    if (CheckCollisionPointRec(MousePos, name_field_box)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) select_name_field = true; 
    }

    DrawRectangleRec(name_field_box, GetColor(0x181818ff));
    DrawRectangleLinesEx(name_field_box_outline, thick, WHITE);
    BeginScissorMode(name_field_box.x, name_field_box.y, name_field_box.width, name_field_box.height);
    const char* text = Procces_char(animation_name_field);
    if (animation_name_field.count == 0 && !select_name_field) {
        DrawTextEx(ui->font, "Name: ", (Vector2) {
            (name_field_box.x + name_field_box.width*0.1f),
            (name_field_box.y + (name_field_box.height*0.5f - font_size*0.5))
        },
                   font_size, 0, GRAY);
    }
    else {
        DrawTextEx(ui->font, text, (Vector2) {
            (name_field_box.x + name_field_box.width*0.1f),
            (name_field_box.y + (name_field_box.height*0.5f - font_size*0.5))
        },
                   font_size, 0, WHITE);
    }
    if (select_name_field) {
        DrawRectangleRec((Rectangle) {
            .x = (name_field_box.x + name_field_box.width*0.1f) + MeasureTextEx(ui->font ,text, font_size, 0).x,
            .y = (name_field_box.y + (name_field_box.height*0.5f - font_size*0.5)),
            .width = 12.0f,
            .height = font_size
        } , WHITE);
    }
    EndScissorMode();

    DrawRectangleRec(add_button_box, GetColor(0x181818ff));
    DrawRectangleLinesEx(add_button_box_outline, thick, WHITE);
    DrawTextEx(ui->font ,"Add Animation", (Vector2) {
             (add_button_box.x + add_button_box.width*0.1f),
             (add_button_box.y + add_button_box.height*0.5f - font_size*0.5)
                },
             font_size, 0, WHITE);

    if (select_name_field) {
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
                if (animation_name_field.count > 15) goto save;
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
            select_name_field = false;
            char* text = Procces_char(animation_name_field);
            if (strlen(text) == 0) goto clean;
            Animation animation = {
                .name = text,
                .duration = 0,
                .loops = false,
                .frames = {.items = NULL, .count = 0, .capacity = 0}
            };
            
            da_append(&ui->animations, animation);
            goto clean;
        }
    }
        return;
clean:
    animation_name_field.count = 0;
    add = false;
}


void Show_Animations(Ui_State *ui, Rectangle bondry)
{
    Rectangle box = {
        .x = bondry.x + Padding,
        .y = bondry.y + Padding + (3 * field_height),
        .width = bondry.width - (2 * Padding),
        .height = bondry.height - (2 * Padding) - (3 * field_height)
    };
    Rectangle box_outline = {
        .x = bondry.x + Padding - thick,
        .y = bondry.y + Padding + (3 * field_height) - thick,
        .width = bondry.width - (2 * Padding) + (2 * thick),
        .height = bondry.height - (2 * Padding) - (3 * field_height) + (2 * thick)
    };

    DrawRectangleLinesEx(box_outline, thick, WHITE);
    DrawRectangleRec(box, GetColor(0x181818ff));

    if (ui->animations.items == NULL) return;
    for (size_t i = 0; i < ui->animations.count; ++i) {
        Rectangle tiny_box = {
            .x = box.x + Padding,
            .y = box.y + font_size + (Padding * (i) * 2) + (i*Padding),
            .width = box.width - (2 * Padding),
            .height = font_size + Padding
        };
        Rectangle Delete_box = {
            .x = tiny_box.x + tiny_box.width - tiny_box.width*0.2f,
            .y = tiny_box.y + tiny_box.height/2 -  (float)font_size/2,
            .width = font_size,
            .height = font_size
        };
        DrawRectangleLinesEx(tiny_box, thick, WHITE);
        BeginScissorMode(tiny_box.x, tiny_box.y, (Delete_box.x - font_size) - tiny_box.x, tiny_box.height);
        DrawTextEx(ui->font, ui->animations.items[i].name, (Vector2) {
                tiny_box.x + tiny_box.width*0.1f,
                tiny_box.y + tiny_box.height/2 - (float)font_size/2
        },
        font_size, 0,WHITE);
        EndScissorMode();
        DrawRectangleRec(Delete_box, RED);
    }
}

void Animation_Panel(Ui_State *ui, Rectangle bondry) {
    Color color = GetColor(0x252525ff);
    DrawRectangleRec(bondry, color);
    add_Animation(ui, bondry);
    Show_Animations(ui, bondry);
}
