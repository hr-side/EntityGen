#include <Info_Panel.h>
#include <tinyfiledialogs.h>
#include <data.h>
#include <nob.h>

void ED_Dialog(Canvas *c)
{
    char const * filter_params[] = { "*.json" };
    c->output_path = tinyfd_saveFileDialog("Path to rendered video",
                                          "./", NOB_ARRAY_LEN(filter_params),
                                          filter_params, "JSON file");
    if (c->output_path == NULL) return;
    Export_Data(c);
}

void Export_Button(Canvas *c, Rectangle bondry, Ui_State ui)
{
    UNUSED(ui);
    Rectangle Box = {
        .x = bondry.width * (1 - 0.125f),
        .y = bondry.height * (1 - 0.45f),
        .width = 180,
        .height = 30
    };
    Color color;
    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, Box)) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ED_Dialog(c);
        color = ColorBrightness(BLUE, 0.2f);
    }
    else {
        color = BLUE;
    }
    DrawRectangleRec(Box, color);
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
    Color color;
    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, Box)) {
        // if(IsMouseButtonPressed(Right)) Import_Data(&State);
        UNUSED(c);
        color = ColorBrightness(GREEN, 0.2f);
    }
    else {
        color = GREEN;
    }
    DrawRectangleRec(Box, color);
}

void Info_Section(Canvas *c, Rectangle bondry, Ui_State ui)       // Print the Selction Box Info
{
    String_Builder sb = {0};
    String_View cstr_Info = {0};
    if (c->Select_Mode) {
        sb_appendf(&sb, "Slection Box: X = %.2f px, Y : %.2f | Frame Size: %.2fx%.2f",
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
