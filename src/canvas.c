#include <animation.h>
#include <canvas.h>
#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <nob.h>

#define SELECT_BOX_SIZE     4 // px
#define COURSURE_BOX_SIZE   4
#define PREVIEW_FRAME_SIZE 32 // px

static float Padding = 7.5f;
static int font_size = 24;

void load_textures(Canvas *c, Ui_State *ui)
{   
    if (!ui->NeedsLoading) return;
    if (ui->texture_paths.count == 0) return;
    if (ui->texture_paths.count == c->textures_visited.count) return;

    if (ui->texture_paths.count == 1) {
        if (c->textures_visited.count == 1) {
            nob_log(NOB_INFO, "Default Texture already loaded!.");
        } else if (c->textures_visited.count < 1) {
            nob_log(NOB_INFO, "     Path|Index : %s|%d", ui->texture_paths.items[0], 0);
            da_append(&c->textures_visited, ui->texture_paths.items[0]);

            Image tmpI = LoadImage(ui->texture_paths.items[0]);
            Texture2D tmpT = LoadTextureFromImage(tmpI);
            da_append(&c->textures, tmpT);
            goto clean;
        }
    }

    nob_log(NOB_INFO, "Loading Textures: ");
    for (size_t i = c->textures_visited.count; i < ui->texture_paths.count; ++i) {
        const char *target_path = ui->texture_paths.items[i];

        nob_log(NOB_INFO, "     Path|Index: %s|%zu", target_path, i);
        da_append(&c->textures_visited, target_path);

        Image tmpI = LoadImage(target_path);
        Texture2D tmpT = LoadTextureFromImage(tmpI);
        da_append(&c->textures, tmpT);
    }

    nob_log(NOB_INFO, " Loaded Textures:");
    for (size_t i = 0; i < c->textures_visited.count; ++i) {
        nob_log(NOB_INFO, "     Loaded Texture Path|Index: '%s'|%zu", c->textures_visited.items[i], i);
    }

clean:
    c->texture = &c->textures.items[c->textures_visited.count - 1];
    ui->current_texture_index = c->textures_visited.count - 1;
    ui->NeedsLoading = false;
}

void Init_Canvas(Canvas *c, Ui_State *ui)
{
    memset(c, 0, sizeof(Canvas));
    c->textures = (Textures) {0};
    c->textures_visited = (Textures_Visited) {0};
    load_textures(c, ui);
    c->texture = &c->textures.items[ui->current_texture_index];
    c->state = EDIT_FRAMES;
    c->backgroundShader = LoadShader( NULL, "res/Shaders/Canvas_Background.fs");
    c->gridSizeLocation = GetShaderLocation(c->backgroundShader, "Grid_size_");
    c->canvasSizeLocation = GetShaderLocation(c->backgroundShader, "Canvas_size");
    c->zoom = 1.0f;
    c->canvasPos = (Vector2) {0, 0};
    c->edited_frame = (Rectangle) {0};
    c->selected_frame = (Rectangle) {0};
    c->firstclick = true;
    c->Select_Mode = false;
    c->drawSelectedFrameMode = false;
    c->next_frame_id = 1;
}

void canvas_background(Rectangle bondry, float *GridSize, Canvas *c)
{
    ClearBackground(GetColor(0x655561ff));

    SetShaderValue(c->backgroundShader, c->gridSizeLocation, GridSize, SHADER_UNIFORM_FLOAT);

    Vector2 canvasSize = { bondry.width, bondry.height };
    SetShaderValue(c->backgroundShader, c->canvasSizeLocation, &canvasSize, SHADER_UNIFORM_VEC2);

    BeginShaderMode(c->backgroundShader);

    Texture2D defaultTex = GetShapesTexture();
    Rectangle sourceRec = { 0.0f, 0.0f, (float)defaultTex.width, (float)defaultTex.height };

    DrawTexturePro(defaultTex, sourceRec, bondry, (Vector2){ 0, 0 }, 0.0f, WHITE);
    EndShaderMode();

    DrawRectangleLinesEx(bondry, 1.8f, GetColor(0x181818ff));
}

Rectangle Edit_mode(Canvas *c, Rectangle bondry, Rectangle *Frame)
{
    if (c->zoom == 1.0f && c->canvasPos.x == 0.0f && c->canvasPos.y == 0.0f) {
        float scaleX = bondry.width / (float)(*c->texture).width;
        float scaleY = bondry.height / (float)c->texture->height;

        c->zoom = (scaleX < scaleY) ? scaleX : scaleY;

        c->canvasPos.x = bondry.x + (bondry.width - ((float)c->texture->width * c->zoom)) / 2.0f;
        c->canvasPos.y = bondry.y + (bondry.height - ((float)c->texture->height * c->zoom)) / 2.0f;
    }

    Vector2 mousePos = GetMousePosition();


    if (CheckCollisionPointRec(mousePos, bondry)) {

        float oldZoom = c->zoom;
        float wheelMove = GetMouseWheelMove();
        float zoomDelta = 0.0f;

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            Vector2 delta_positon = GetMouseDelta();
            c->canvasPos.x += delta_positon.x;
            c->canvasPos.y += delta_positon.y;
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_EQUAL)) {
            zoomDelta = 0.5f;
        }
        else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_MINUS)) {
            zoomDelta = -0.5f;
        }
        else if (wheelMove != 0.0f) {
            zoomDelta = wheelMove * 0.5f;
        }

        if (zoomDelta != 0.0f) {
            c->zoom += zoomDelta;

            if (c->zoom <= 0.4f) c->zoom = 0.5f;
        }

        if (c->zoom != oldZoom)
        {
            float mouseRelativeToCanvasX = mousePos.x - c->canvasPos.x;
            float mouseRelativeToCanvasY = mousePos.y - c->canvasPos.y;

            c->canvasPos.x = mousePos.x - (mouseRelativeToCanvasX * (c->zoom / oldZoom));
            c->canvasPos.y = mousePos.y - (mouseRelativeToCanvasY * (c->zoom / oldZoom));
        }

    }
    float textureZoomedWidth = (float)(*c->texture).width * c->zoom;
    float textureZoomedHeight = (float)(*c->texture).height * c->zoom;

    Frame->x = 0;
    Frame->y = 0;
    Frame->width = (*c->texture).width;
    Frame->height = (*c->texture).height;

    return (Rectangle) {
        .x = c->canvasPos.x,
        .y = c->canvasPos.y,
        .width = textureZoomedWidth,
        .height = textureZoomedHeight
    };

}

Rectangle Preview_mode(Canvas *c, Rectangle bondry, Rectangle *Frame )
{
    float scaleX = bondry.width / (float)(*c->texture).width;
    float scaleY = bondry.height / (float)(*c->texture).height;

    c->zoom = (scaleX < scaleY) ? scaleX : scaleY;

    Frame->x = 0;
    Frame->y = 0;
    Frame->width = PREVIEW_FRAME_SIZE;
    Frame->height = PREVIEW_FRAME_SIZE;

    return bondry;
}


void Draw_Selected_frame(Canvas *c, Rectangle bondry, float thick)
{
    DrawRectangleLinesEx(
        (Rectangle) {
            .x = c->selected_frame.x * c->zoom + bondry.x - thick,
            .y = c->selected_frame.y * c->zoom + bondry.y - thick,
            .width = c->selected_frame.width * c->zoom + thick*2,
            .height = c->selected_frame.height * c->zoom + thick*2
        }
        , thick ,BLUE);
    DrawRectangleLinesEx(
        (Rectangle) {
            .x = c->selected_frame.x * c->zoom + bondry.x,
            .y = c->selected_frame.y * c->zoom + bondry.y,
            .width = c->selected_frame.width * c->zoom,
            .height = c->selected_frame.height * c->zoom
        }
        , 1.0f , BLACK);
    DrawRectangleRec(
        (Rectangle) {
            .x = c->selected_frame.x * c->zoom + bondry.x,
            .y = c->selected_frame.y * c->zoom + bondry.y,
            .width = c->selected_frame.width * c->zoom,
            .height = c->selected_frame.height * c->zoom
        }
        ,ColorAlpha(BLUE, 0.25f));
}

void Draw_edited_frame(Canvas *c, Rectangle bondry, float thick)
{
    DrawRectangleLinesEx(
        (Rectangle) {
            .x = c->edited_frame.x * c->zoom + bondry.x - thick,
            .y = c->edited_frame.y * c->zoom + bondry.y - thick,
            .width = c->edited_frame.width * c->zoom + thick*2,
            .height = c->edited_frame.height * c->zoom + thick*2
        }
        , thick ,BLUE);
    DrawRectangleLinesEx(
        (Rectangle) {
            .x = c->edited_frame.x * c->zoom + bondry.x,
            .y = c->edited_frame.y * c->zoom + bondry.y,
            .width = c->edited_frame.width * c->zoom,
            .height = c->edited_frame.height * c->zoom
        }
        , 1.0f , BLACK);
    DrawRectangleRec(
        (Rectangle) {
            .x = c->edited_frame.x * c->zoom + bondry.x,
            .y = c->edited_frame.y * c->zoom + bondry.y,
            .width = c->edited_frame.width * c->zoom,
            .height = c->edited_frame.height * c->zoom
        }
        ,ColorAlpha(BLUE, 0.25f));
}

void Select_box(Ui_State *ui, Canvas *c, Rectangle bondry)
{
    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, bondry)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 MouseOffset = {
                .x = (MousePos.x - bondry.x) / c->zoom,
                .y = (MousePos.y - bondry.y) / c->zoom
            };

            int current_cell_x = (int)floorf(MouseOffset.x / SELECT_BOX_SIZE);
            int current_cell_y = (int)floorf(MouseOffset.y / SELECT_BOX_SIZE);

            if (c->firstclick) {
                c->FirstClickPos = (Vector2) {
                    .x = current_cell_x,
                    .y = current_cell_y
                };
                c->firstclick = false;
                c->Select_Mode = false;
            } else {
                c->SecondClickPos = (Vector2) {
                    .x = current_cell_x,
                    .y = current_cell_y
                };
                c->firstclick = true;
                c->Select_Mode = true;
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            c->Select_Mode = false;
            ui->IsFrameSelected = false;
            c->drawSelectedFrameMode = false;
        }
    }
    if (c->Select_Mode) {
        int min_x = fminf(c->FirstClickPos.x, c->SecondClickPos.x);
        int max_x = fmaxf(c->FirstClickPos.x, c->SecondClickPos.x);
        int min_y = fminf(c->FirstClickPos.y, c->SecondClickPos.y);
        int max_y = fmaxf(c->FirstClickPos.y, c->SecondClickPos.y);

        c->edited_frame.x = min_x * SELECT_BOX_SIZE;
        c->edited_frame.y = min_y * SELECT_BOX_SIZE;
        c->edited_frame.width  = (max_x - min_x + 1) * SELECT_BOX_SIZE;
        c->edited_frame.height = (max_y - min_y + 1) * SELECT_BOX_SIZE;

        Draw_edited_frame(c, bondry, 6.0f);
    }
}

void Draw_Box_Around_courser(Canvas *c, Rectangle bondry)
{
    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, bondry)) {
        Vector2 MouseOffset = {
            .x = (MousePos.x - bondry.x) / c->zoom,
            .y = (MousePos.y - bondry.y) / c->zoom
        };

        int cell_x = (int) floor(MouseOffset.x / COURSURE_BOX_SIZE);
        int cell_y = (int) floor(MouseOffset.y / COURSURE_BOX_SIZE);

        DrawRectangleLinesEx((Rectangle) {
                .x = (cell_x * COURSURE_BOX_SIZE) * c->zoom + bondry.x,
                .y = (cell_y * COURSURE_BOX_SIZE) * c->zoom + bondry.y,
                .width =  ((float) COURSURE_BOX_SIZE) * c->zoom,
                .height = ((float) COURSURE_BOX_SIZE) * c->zoom
            }, 4.0f, GREEN);
    }
}

void Draw_Tabs(Rectangle bondry, Canvas *c, Ui_State *ui)
{
    static float thick = 1.5f;
    static float factor = 5.0f;
    static float field_width = 120.0f;
    // GetFileNameWithoutExt();
    UNUSED(ui);
    Rectangle Tabs_Panel_box = {
        .x = bondry.x + factor * thick,
        .y = bondry.y + factor * thick,
        .width = bondry.width - (factor * 2 * thick),
        .height = bondry.height*0.1f - (factor * 2* thick)
    };

    Rectangle Tabs_Panel_box_outline = {
        .x = bondry.x + factor * thick,
        .y = bondry.y + factor * thick,
        .width = bondry.width - (2 * thick * factor),
        .height = bondry.height*0.1f - (2 * thick * factor)
    };

    DrawRectangleRec(Tabs_Panel_box, GetColor(0x181818ff));
    DrawRectangleLinesEx(Tabs_Panel_box_outline, thick, WHITE);

    for (size_t i = 1; i < c->textures_visited.count; ++i)
    {
        Vector2 MousePos = GetMousePosition();
        const char *text = GetFileNameWithoutExt(c->textures_visited.items[i]);
        Rectangle tiny_box = {
            .x = Tabs_Panel_box.x + ((i - 1)*field_width) + (Padding * (i - 1) * 2) + (2*Padding),
            .y = Tabs_Panel_box.y + Tabs_Panel_box.height*0.5f - (font_size),
            .width = field_width,
            .height = 2*font_size
        };
        
        Rectangle tiny_box_outline = {
            .x = tiny_box.x - thick,
            .y = tiny_box.y - thick,
            .width = tiny_box.width + (2 * thick),
            .height = tiny_box.height + (2 * thick)
        };

        Color color = GetColor(0x252525ff);

        if (CheckCollisionPointRec(MousePos, tiny_box)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ui->current_texture_index = i;
                ui->IsFrameSelected = false;
                c->drawSelectedFrameMode = false;
            }
            color = ColorBrightness(color, 0.3f);
        }
        
        if (i == ui->current_texture_index) {
            color = ColorBrightness(color, 0.15f);
        }

        DrawRectangleRec(tiny_box, color);
        DrawRectangleLinesEx(tiny_box_outline, thick, WHITE);
        BeginScissorMode(tiny_box.x, tiny_box.y, tiny_box.width, tiny_box.height);
        DrawTextEx(ui->font, text,
                   (Vector2) {
                    .x = tiny_box.x + tiny_box.width*0.1f,
                    .y = tiny_box.y + tiny_box.height*0.5f - font_size*0.5f
                   },
                   font_size, 0, WHITE);
        EndScissorMode();
    }
}

void Draw_Canvas(Ui_State *ui, Rectangle bondry, Canvas *c)
{
    if (ui->ShowMenu) return;
    if (ui->current_texture_index == 0) {
        DrawRectangleRec(bondry, GetColor(0x655561ff));
        DrawRectangleLinesEx(bondry, 1.8f, WHITE);
        return;
    }
    c->texture = &c->textures.items[ui->current_texture_index];

    Rectangle Frame_v = {0};
    Rectangle zoomedBondry = {0};
    switch (c->state) {
        case PREVIEW: zoomedBondry = Preview_mode(c, bondry, &Frame_v); break;
        case EDIT_FRAMES: zoomedBondry = Edit_mode(c, bondry, &Frame_v); break;
    }

    BeginScissorMode(bondry.x, bondry.y, bondry.width, bondry.height);
    // Background Pattren:
    float GridSize = 16.0f * c->zoom;
    canvas_background(zoomedBondry, &GridSize, c);

    if (c->Select_Mode && IsKeyPressed(KEY_F) && ui->IsAnimationSelected) {
        Frame tmp = (Frame) {
            .cords = c->edited_frame,
            .hitbox_index = 0,
            .texture_index = ui->current_texture_index,
            .ID = c->next_frame_id++
        };
        da_append(&ui->animations.items[ui->currentAnimationIndex].frames, tmp);
        c->Select_Mode = false;
    }

    DrawTexturePro(*c->texture, Frame_v, zoomedBondry, (Vector2) {0, 0}, 0.0f, WHITE);
    Select_box(ui, c, zoomedBondry); // Create And Draw The Select Box
    Draw_Box_Around_courser(c, zoomedBondry);

    if (c->drawSelectedFrameMode && ui->IsFrameSelected) {
        ui->current_texture_index = ui->animations.items[ui->currentAnimationIndex].frames.items[ui->currentVisiableFrameIndex].texture_index;
        Draw_Selected_frame(c, zoomedBondry, 6.0f);
    }

    Draw_Tabs(bondry, c, ui);
    EndScissorMode();

    DrawRectangleLinesEx(bondry, 1.8f, WHITE);
}

void Unload_Textures(Canvas *c)
{
    for (size_t i = 0; i < c->textures.count; ++i) {
        UnloadTexture(c->textures.items[i]);
    }
    free(c->textures.items);
    free(c->textures_visited.items);
}
