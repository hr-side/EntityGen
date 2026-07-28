#include <animation.h>
#include <canvas.h>
#include <raylib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <nob.h>

#define SELECT_BOX_SIZE     4 // px
#define COURSURE_BOX_SIZE   4
#define PREVIEW_FRAME_SIZE 32 // px

void Init_Canvas(Canvas *c, Ui_State *ui)
{
    memset(c, 0, sizeof(Canvas));
    Image image = LoadImage(ui->texture_paths.items[0]);
    c->texture = LoadTextureFromImage(image);
    UnloadImage(image);
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
        float scaleX = bondry.width / (float)c->texture.width;
        float scaleY = bondry.height / (float)c->texture.height;

        c->zoom = (scaleX < scaleY) ? scaleX : scaleY;

        c->canvasPos.x = bondry.x + (bondry.width - ((float)c->texture.width * c->zoom)) / 2.0f;
        c->canvasPos.y = bondry.y + (bondry.height - ((float)c->texture.height * c->zoom)) / 2.0f;
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
    float textureZoomedWidth = (float)c->texture.width * c->zoom;
    float textureZoomedHeight = (float)c->texture.height * c->zoom;

    Frame->x = 0;
    Frame->y = 0;
    Frame->width = c->texture.width;
    Frame->height = c->texture.height;

    return (Rectangle) {
        .x = c->canvasPos.x,
        .y = c->canvasPos.y,
        .width = textureZoomedWidth,
        .height = textureZoomedHeight
    };

}

Rectangle Preview_mode(Canvas *c, Rectangle bondry, Rectangle *Frame )
{
    float scaleX = bondry.width / (float)c->texture.width;
    float scaleY = bondry.height / (float)c->texture.height;

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

void Draw_Canvas(Ui_State *ui, Rectangle bondry, Canvas *c)
{
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
    static int next_frame_id = 1;
    if (c->Select_Mode && IsKeyPressed(KEY_F) && ui->IsAnimationSelected) {
        Frame tmp = (Frame) {
            .cords = c->edited_frame,
            .hitbox_index = 0,
            .texture_index = 0,
            .ID = next_frame_id++
        };
        da_append(&ui->animations.items[ui->currentAnimationIndex].frames, tmp);
        c->Select_Mode = false;
    }

    DrawTexturePro(c->texture, Frame_v, zoomedBondry, (Vector2) {0, 0}, 0.0f, WHITE);
    Select_box(ui, c, zoomedBondry); // Create And Draw The Select Box
    Draw_Box_Around_courser(c, zoomedBondry);
    if (c->drawSelectedFrameMode && ui->IsFrameSelected) {
        Draw_Selected_frame(c, zoomedBondry, 6.0f);
    }
    EndScissorMode();

    DrawRectangleLinesEx(bondry, 1.8f, WHITE);
}
