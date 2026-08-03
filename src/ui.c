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

    Draw_Canvas(&UI, canvasBondry, &c);
    Info_Panel(&c, InfoPanelBondry, &UI);
    Animation_Panel(&UI, &c,rightPanelBondry);
    Animations_Panel(&UI, leftPanelBondry);
}

void ui_unload()
{
    Unload_Textures(&c);
    UnloadShader(c.backgroundShader);
    UnloadFont(UI.font);
}
