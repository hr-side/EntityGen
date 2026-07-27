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
Canvas c;

void ui_init()
{
    UI.font = LoadFont("res/Fonts/0xProtoNerdFont-Regular.ttf");
    da_append(&UI.texture_paths, "res/Player.png");
    Init_Canvas(&c, &UI);
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
    Info_Panel(&c, InfoPanelBondry, &UI);
    right_panel(rightPanelBondry);
    Animation_Panel(&UI, leftPanelBondry);
    Draw_Canvas(canvasBondry, &c);
}

void ui_unload()
{
    UnloadTexture(c.texture);
    UnloadShader(c.backgroundShader);
    UnloadFont(UI.font);
}
