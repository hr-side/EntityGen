#include <nob.h>
#include <stdbool.h>
#include <stdlib.h>
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
static float font_size = 24;

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
    if (CheckCollisionPointRec(MousePos, bondry) && !ui->ShowMenu) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) select_name_field = false; 
    }
    if (CheckCollisionPointRec(MousePos, name_field_box) && !ui->ShowMenu) {
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

    Color Add_box_color = GetColor(0x181818ff);
    if (CheckCollisionPointRec(MousePos, add_button_box) && !ui->ShowMenu) {
        Add_box_color = ColorBrightness(Add_box_color, 0.3f);
    }

    DrawRectangleRec(add_button_box, Add_box_color);
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
    if ((CheckCollisionPointRec(MousePos, add_button_box) || add) && !ui->ShowMenu) {
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
    Vector2 MousePos = GetMousePosition();


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


    static float scroling_factor = 10.0f;
    static float scrole_value = 0.0f;
    if (CheckCollisionPointRec(MousePos, box) && !ui->ShowMenu) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            ui->IsAnimationSelected = false;
        }

        float contentHeight = font_size + (Padding * (ui->animations.count) * 2) + ((ui->animations.count) * Padding);
        bool needsScroll = contentHeight > box.height;

        float Wheel = GetMouseWheelMove();

        if (Wheel != 0 && needsScroll) {
            if (Wheel < 0) scrole_value -= scroling_factor;
            if (Wheel > 0) scrole_value += scroling_factor;

            float minScroll = box.height - contentHeight;
            if (scrole_value > 0.0f)      scrole_value = 0.0f;
            if (scrole_value < minScroll) scrole_value = minScroll;
        }
    }

    Color color = GetColor(0x181818ff);

    DrawRectangleLinesEx(box_outline, thick, WHITE);
    DrawRectangleRec(box, color);

    if (ui->animations.items == NULL) return;
    BeginScissorMode(box.x, box.y, box.width, box.height);
    for (size_t i = 0; i < ui->animations.count; ++i) {
        Color tiny_color = GetColor(0x181818ff);
        Rectangle tiny_box = {
            .x = box.x + Padding,
            .y = box.y + font_size + (Padding * (i) * 2) + (i*Padding) + (scrole_value),
            .width = box.width - (2 * Padding),
            .height = font_size + Padding
        };
        Rectangle Delete_box = {
            .x = tiny_box.x + tiny_box.width - tiny_box.width*0.2f,
            .y = tiny_box.y + tiny_box.height/2 -  (float)font_size/2,
            .width = font_size,
            .height = font_size
        };

        // Figure out how to check if we are in the visible block or not
        if (tiny_box.y + tiny_box.height < box.y) continue;
        if (tiny_box.y > box.y + box.height) continue;

        if (&ui->animations.items[ui->currentAnimationIndex] == &ui->animations.items[i] && ui->IsAnimationSelected) {
            tiny_color = ColorBrightness(color, 0.3f);
        }
        if (CheckCollisionPointRec(MousePos, tiny_box) && !ui->ShowMenu) {
            if (CheckCollisionPointRec(MousePos, Delete_box)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (ui->currentAnimationIndex == i && ui->IsAnimationSelected) {
                        ui->currentAnimationIndex = 0;
                        ui->IsAnimationSelected = false;
                    }
                    else if (ui->currentAnimationIndex == (ui->animations.count - 1) && ui->IsAnimationSelected) ui->currentAnimationIndex = i;

                    if (ui->animations.count > 1) {
                        free(ui->animations.items[i].frames.items);
                        ui->animations.items[i] = ui->animations.items[ui->animations.count - 1];
                    }
                    else if (ui->animations.count == 1){
                        ui->currentAnimationIndex = 0;
                        ui->IsAnimationSelected = false;
                        free(ui->animations.items[i].frames.items);
                        free(ui->animations.items);
                        ui->animations.items = NULL;
                        ui->animations.count = 0;
                        ui->animations.capacity = 0;
                        return;
                    }
                    ui->animations.count--;
                }
            } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ui->currentAnimationIndex = i;
                ui->IsAnimationSelected = true;
                ui->IsFrameSelected = false;
            }
            tiny_color = ColorBrightness(color, 0.5f);
        }

        DrawRectangleRec(tiny_box, tiny_color);
        DrawRectangleLinesEx(tiny_box, thick, WHITE);
        {
            // BeginScissorMode(tiny_box.x, tiny_box.y, (Delete_box.x - font_size) - tiny_box.x, tiny_box.height);
            DrawTextEx(ui->font, ui->animations.items[i].name, (Vector2) {
                tiny_box.x + tiny_box.width*0.1f,
                tiny_box.y + tiny_box.height/2 - (float)font_size/2
            },
                       font_size, 0,WHITE);
            // EndScissorMode();
        }
        DrawRectangleRec(Delete_box, RED); // TODO: Try rendring a Texture (Logo)
    }
    EndScissorMode();
}

void Animations_Panel(Ui_State *ui, Rectangle bondry) {
    Color color = GetColor(0x252525ff);
    DrawRectangleRec(bondry, color);
    add_Animation(ui, bondry);
    Show_Animations(ui, bondry);
}
