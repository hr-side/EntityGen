#include <stdbool.h>
#include <stdio.h>
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
    UI.currentAnimationIndex = 0;
    UI.IsAnimationSelected = false;
    da_append(&UI.texture_paths, "res/Player.png");
    Init_Canvas(&c, &UI);
}


void Animation_Panel(Rectangle bondry) {
    Color color = GetColor(0x252525ff);
    DrawRectangleRec(bondry, color);
    // TODO: Cuntinue this shit
    char text[256];
    if (!UI.IsAnimationSelected) {
        sprintf(text, " NO Animation Has Been\n Selected");
    }
    else {
        snprintf(text, 256, "Selected : %s", UI.animations.items[UI.currentAnimationIndex].name);
    }

    DrawTextEx(UI.font, text, (Vector2) {
        .x = bondry.x,
        .y = bondry.y + bondry.height/2 - 12.0f
    }, 24.0f, 0, WHITE);
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
