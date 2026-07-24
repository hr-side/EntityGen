#include "raylib.h"
#include <Info_Panel.h>
#include <tinyfiledialogs.h>
#include <data.h>
#include <nob.h>
#include <unistd.h>

void ED_Dialog(Canvas *c)
{
    char const * filter_params[] = { "*.json" };
    c->output_path = tinyfd_saveFileDialog("Path to Genrated JSON Data",
                                          "./", NOB_ARRAY_LEN(filter_params),
                                          filter_params, "JSON file");
    if (c->output_path == NULL) return;
    Export_Data(c);
}

void Export_Button(Canvas *c, Rectangle bondry, Ui_State ui)
{
    Rectangle Box = {
        .x = bondry.width * (1 - 0.125f),
        .y = bondry.height * (1 - 0.45f),
        .width = 180,
        .height = 30
    };
    float thick = 2.5f;
    Color color;
    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, Box)) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ED_Dialog(c);
        color = ColorBrightness(GetColor(0x181818ff), 0.2f);
        thick = 1.5f;
    }
    else {
        color = GetColor(0x181818ff);
    }

    const char *text = "Export";
    const int font_size = 24;
    DrawRectangleRec(Box, color);
    DrawRectangleLinesEx((Rectangle) {
            .x = Box.x - thick,
            .y = Box.y - thick,
            .width = Box.width + 2*thick,
            .height = Box.height + 2*thick
        }, thick, WHITE);
    DrawTextEx(ui.font, text, (Vector2) {
                    .x = Box.x + Box.width/2 - MeasureTextEx(ui.font, text, font_size, 0).x/2,
                    .y = Box.y + Box.height/2 - ((float)font_size/2)
                }, font_size, 0, WHITE);

}

void ID_Dialog(Canvas *c)
{
    char const * filter_params[] = { "*.json" };
    c->import_Path = tinyfd_openFileDialog("Import JSON Data File", "./",
                                           ARRAY_LEN(filter_params), filter_params,
                                           "JSON File", 0);
    if (c->output_path == NULL) return;
    Import_Data(c);
}

void Import_Button(Canvas *c, Rectangle bondry, Ui_State ui)
{
    UNUSED(ui);
    Rectangle Box = {
        .x = bondry.width * (1 - 0.125f),
        .y = bondry.height * (0.1f),
        .width = 180,
        .height = 30
    };
    float thick = 2.5f;
    Color color;
    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, Box)) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ID_Dialog(c);
        color = ColorBrightness(GetColor(0x181818ff), 0.2f);
        thick = 1.5f;
    }
    else {
        color = GetColor(0x181818ff);
    }

    const char *text = "Import";
    const int font_size = 24;
    DrawRectangleRec(Box, color);
    DrawRectangleLinesEx((Rectangle) {
            .x = Box.x - thick,
            .y = Box.y - thick,
            .width = Box.width + 2*thick,
            .height = Box.height + 2*thick
        }, thick, WHITE);
    DrawTextEx(ui.font, text, (Vector2) {
                    .x = Box.x + Box.width/2 - MeasureTextEx(ui.font, text, font_size, 0).x/2,
                    .y = Box.y + Box.height/2 - ((float)font_size/2)
                }, font_size, 0, WHITE);
}

void Info_Section(Canvas *c, Rectangle bondry, Ui_State ui)       // Print the Selction Box Info
{
    String_Builder sb = {0};
    String_View cstr_Info = {0};
    if (c->Select_Mode) {
        sb_appendf(&sb, "Slection Box: X = %.2f, Y : %.2f | Frame Size: %.2fx%.2f (PX)",
                      c->Selecte_Box.x, c->Selecte_Box.y, c->Selecte_Box.width, c->Selecte_Box.height);
        sb_append_null(&sb);
        cstr_Info = sb_to_sv(sb);
    }
    if(cstr_Info.count != 0) {
        const char *Info = nob_temp_sv_to_cstr(cstr_Info);
        DrawTextEx(ui.font, Info, (Vector2) {bondry.x + 300, bondry.height - 50}, 24, 0, RAYWHITE);
    }
    else {
        DrawText(" - ", bondry.x + 300, bondry.height - 50, 24, RAYWHITE);
    }
}

void Info_Panel(Canvas *c,Rectangle bondry, Ui_State ui) {
    Color color = GetColor(0x252525ff);
    DrawRectangleRec(bondry, color);
    Export_Button(c, bondry, ui);
    Import_Button(c, bondry, ui);
    Info_Section(c, bondry, ui);
}
