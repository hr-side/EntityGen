#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Animations.h>
#include <nob.h>

#ifdef _WIN32 // Windows shit
char *strndup(const char *s, size_t n) {
    char *new_str = malloc(n + 1);
    if (new_str) {
        strncpy(new_str, s, n);
        new_str[n] = '\0';
    }
    return new_str;
}
#endif

static String_Builder duration_sb = {0};
static bool select_duration_field = false;
static int duration_sb_anim_index = -1;
bool loops;
bool update_duration = false;

static float Padding = 20.0f;
static float thick = 2.5f;
static float field_height = 50.0f;
static int font_size = 24;

float parse_float_from_sb(String_Builder *sb)
{
    float duration = 0;
    bool after_dot = false;
    float frac = 0.1f;

    for (size_t i = 0; i < sb->count; ++i) {
        if (sb->items[i] == '.') {
            after_dot = true;
            continue;
        }
        int digit = sb->items[i] - '0';
        if (!after_dot) {
            duration = duration * 10.0f + digit;
        } else {
            duration += digit * frac;
            frac *= 0.1f;
        }
    }

    free(sb->items);
    sb->items = NULL;
    sb->count = 0;
    sb->capacity = 0;
    return duration;
}

void Animation_Panel_Show(Ui_State *ui, Canvas *c,Rectangle bondry)
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

    Rectangle loops_field_box = {
        .x = bondry.x + bondry.width*0.8f - Padding,
        .y = duration_field_box_outline.y + duration_field_box_outline.height + Padding,
        .width = field_height,
        .height = field_height
    };

    Rectangle loops_field_box_outline = {
        .x = loops_field_box.x - thick,
        .y = loops_field_box.y - thick,
        .width = loops_field_box.width + (2 * thick),
        .height = field_height + (2 * thick)
    };

    Rectangle frames_field_box = {
        .x = bondry.x + Padding,
        .y = loops_field_box_outline.y + loops_field_box_outline.height + Padding,
        .width = bondry.width - (2 * Padding),
        .height = bondry.height - ((4 * field_height) + 3 * Padding)
    };

    Rectangle frames_field_box_outline = {
        .x = frames_field_box.x - thick,
        .y = frames_field_box.y - thick,
        .width = frames_field_box.width + (2 * thick),
        .height = frames_field_box.height + (2 * thick)
    };

    const char* text = " No Animation Has\n been Selected";
    const char* rendertext = NULL;
    bool DrawBox = false;
    Color text_color = GRAY;

    if (ui->IsAnimationSelected) {
        text = ui->animations.items[ui->currentAnimationIndex].name;
        text_color = WHITE;
        DrawBox = true;
        loops = ui->animations.items[ui->currentAnimationIndex].loops;

        if (duration_sb_anim_index == -1) {
            duration_sb_anim_index = ui->currentAnimationIndex;
        }
        if (duration_sb.items == NULL || (size_t) duration_sb_anim_index != ui->currentAnimationIndex) {
            if (duration_sb.items != NULL) {
                free(duration_sb.items);
                duration_sb.items = NULL;
                duration_sb.count = 0;
                duration_sb.capacity = 0;
            }
            select_duration_field = false;
            sb_appendf(&duration_sb, "%.2f", ui->animations.items[ui->currentAnimationIndex].duration);
            duration_sb_anim_index = ui->currentAnimationIndex;
        }
    }

    if (DrawBox) {

        Color LoopsColor = GetColor(0x181818ff);
        if (loops) {
            LoopsColor = RED;
        }

        DrawRectangleRec(loops_field_box, LoopsColor);
        DrawRectangleLinesEx(loops_field_box_outline, thick, WHITE);
        DrawRectangleRec(name_field_box, GetColor(0x181818ff));
        DrawRectangleLinesEx(name_field_box_outline, thick, WHITE);
        DrawRectangleRec(duration_field_box, GetColor(0x181818ff));
        DrawRectangleLinesEx(duration_field_box_outline, thick, WHITE);
        DrawRectangleRec(frames_field_box, GetColor(0x181818ff));
        DrawRectangleLinesEx(frames_field_box_outline, thick, WHITE);

        Vector2 MousePos = GetMousePosition();

        if (CheckCollisionPointRec(MousePos, loops_field_box)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ui->animations.items[ui->currentAnimationIndex].loops = !loops;
            }
        }

        if (CheckCollisionPointRec(MousePos, bondry)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {select_duration_field = false; update_duration = true;}
        }
        if (CheckCollisionPointRec(MousePos, duration_field_box)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {select_duration_field = true; update_duration = false;}
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
                .x = (duration_field_box.x + duration_field_box.width*0.1f) + MeasureTextEx(ui->font , rendertext, font_size, 0).x,
                .y = (duration_field_box.y + (duration_field_box.height*0.5f - font_size*0.5)),
                .width = 12.0f,
                .height = font_size
            } , WHITE);
        }

        DrawTextEx(ui->font, rendertext, (Vector2) {
            .x = duration_field_box.x + duration_field_box.width*0.1f,
            .y = duration_field_box.y + duration_field_box.height*0.5f - font_size*0.5f
        }
        , font_size, 0, WHITE);
        free((void *)rendertext);

        DrawTextEx(ui->font, " Loops: ", (Vector2) {
            .x = bondry.x + Padding,
            .y = loops_field_box.y + loops_field_box.height*0.5f - font_size*0.5f
        }, font_size, 0, WHITE);

        if (update_duration) {
            ui->animations.items[ui->currentAnimationIndex].duration = parse_float_from_sb(&duration_sb);
            update_duration = false;
        }

        // Frames_Section;
        if (ui->animations.items[ui->currentAnimationIndex].frames.items != NULL)
        {
            char frame_name[256];
            for (size_t i = 0; i < ui->animations.items[ui->currentAnimationIndex].frames.count; ++i) {

                Color tiny_color = GetColor(0x181818ff);
                Rectangle tiny_box = {
                    .x = frames_field_box.x + Padding,
                    .y = frames_field_box.y + font_size + (Padding * (i) * 2) + (i*Padding),
                    .width = frames_field_box.width - (2 * Padding),
                    .height = font_size + Padding
                };
                Rectangle Delete_box = {
                    .x = tiny_box.x + tiny_box.width - tiny_box.width*0.2f,
                    .y = tiny_box.y + tiny_box.height/2 -  (float)font_size/2,
                    .width = font_size,
                    .height = font_size
                };

                if (CheckCollisionPointRec(MousePos, tiny_box)) {
                    if (CheckCollisionPointRec(MousePos, Delete_box)) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            if (ui->currentVisiableFrameIndex == i && ui->IsFrameSelected) {
                                ui->currentVisiableFrameIndex = 0;
                                ui->IsFrameSelected = false;
                            }
                            else if (ui->currentVisiableFrameIndex == (ui->animations.items[ui->currentAnimationIndex].frames.count - 1) && ui->IsFrameSelected) ui->currentVisiableFrameIndex = i;

                            if (ui->animations.items[ui->currentAnimationIndex].frames.count > 1) {
                                ui->animations.items[ui->currentAnimationIndex].frames.items[i] = ui->animations.items[ui->currentAnimationIndex].frames.items[ui->animations.items[ui->currentAnimationIndex].frames.count - 1];
                            }
                            else if (ui->animations.items[ui->currentAnimationIndex].frames.count == 1){
                                free(ui->animations.items[ui->currentAnimationIndex].frames.items);
                                ui->animations.items[ui->currentAnimationIndex].frames.items = NULL;
                                ui->animations.items[ui->currentAnimationIndex].frames.count = 0;
                                ui->animations.items[ui->currentAnimationIndex].frames.capacity = 0;
                                return;
                            }
                            ui->animations.items[ui->currentAnimationIndex].frames.count--;
                        }
                    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        ui->IsFrameSelected = true;
                        ui->currentVisiableFrameIndex = i; 
                    }
                    tiny_color = ColorBrightness(tiny_color, 0.3f);
                }

                if (ui->animations.items[ui->currentAnimationIndex].frames.items[ui->currentVisiableFrameIndex].ID == ui->animations.items[ui->currentAnimationIndex].frames.items[i].ID && ui->IsFrameSelected) {
                    c->drawSelectedFrameMode = true;
                    c->selected_frame = ui->animations.items[ui->currentAnimationIndex].frames.items[ui->currentVisiableFrameIndex].cords;
                    tiny_color = ColorBrightness(tiny_color, 0.2f);
                }
                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    ui->IsFrameSelected = false;
                    c->drawSelectedFrameMode = false;
                }

                snprintf(frame_name, 256, "Frame{%zu}", i + 1);
                DrawRectangleRec(tiny_box, tiny_color);
                DrawRectangleLinesEx(tiny_box, thick, WHITE);
                BeginScissorMode(tiny_box.x, tiny_box.y, (Delete_box.x - font_size) - tiny_box.x, tiny_box.height);
                DrawTextEx(ui->font, frame_name, (Vector2) {
                        tiny_box.x + tiny_box.width*0.1f,
                        tiny_box.y + tiny_box.height/2 - (float)font_size/2
                }, font_size, 0,WHITE);
                EndScissorMode();
                DrawRectangleRec(Delete_box, RED);
            }
        }
    }

    DrawTextEx(ui->font, text, (Vector2) {
        (name_field_box.x + name_field_box.width*0.1f),
        (name_field_box.y + (name_field_box.height*0.5f - font_size*0.5))
    },
    font_size, 0, text_color);
}

void Animation_Panel(Ui_State *ui, Canvas *c, Rectangle bondry) {
    Color color = GetColor(0x252525ff);
    DrawRectangleRec(bondry, color);
    Animation_Panel_Show(ui, c, bondry);
}
