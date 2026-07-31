#include <canvas.h>
#include <Info_Panel.h>
#include <string.h>
#include <tinyfiledialogs.h>
#include <data.h>
#include <nob.h>

#if defined(__has_include) && __has_include(<unistd.h>)
    #include <unistd.h>
#endif

void ED_Dialog(Canvas *c, Ui_State *ui)
{
    char const * filter_params[] = { "*.json" };
    ui->output_path = tinyfd_saveFileDialog("Path to Genrated JSON Data",
                                          "./", NOB_ARRAY_LEN(filter_params),
                                          filter_params, "JSON file");
    if (ui->output_path == NULL) return;
    Export_Data(c, ui);
}

void Export_Button(Canvas *c, Rectangle bondry, Ui_State *ui)
{
    Rectangle Box = {
        .x = bondry.width * (1 - 0.125f) - 250*0.25f,
        .y = bondry.height * (1 - 0.45f),
        .width = 250,
        .height = 30
    };
    float thick = 2.5f;
    Color color;
    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, Box)) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ED_Dialog(c, ui);
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
    DrawTextEx(ui->font, text, (Vector2) {
                    .x = Box.x + Box.width/2 - MeasureTextEx(ui->font, text, font_size, 0).x/2,
                    .y = Box.y + Box.height/2 - ((float)font_size/2)
                }, font_size, 0, WHITE);

}

void ID_Dialog(Canvas *c, Ui_State *ui)
{
    char const * filter_params[] = { "*.json" };
    char const * path = tinyfd_openFileDialog("Import JSON Data File", "./",
                                           ARRAY_LEN(filter_params), filter_params,
                                           "JSON File", 0);
    ui->import_Path = strdup(path);
    if (ui->import_Path == NULL) {
        nob_log(NOB_WARNING, "Output Path is invalide");
        return;
    }
    Import_Data(c, ui);
}

void Import_Button(Canvas *c, Rectangle bondry, Ui_State *ui)
{
    Rectangle Box = {
        .x = bondry.width * (1 - 0.125f) - 250*0.25f,
        .y = bondry.height * (0.1f),
        .width = 250,
        .height = 30
    };
    float thick = 2.5f;
    Color color;
    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, Box)) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ID_Dialog(c, ui);
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
    DrawTextEx(ui->font, text, (Vector2) {
                    .x = Box.x + Box.width/2 - MeasureTextEx(ui->font, text, font_size, 0).x/2,
                    .y = Box.y + Box.height/2 - ((float)font_size/2)
                }, font_size, 0, WHITE);
}

void Info_Section(Canvas *c, Rectangle bondry, Ui_State *ui)       // Print the Selction Box Info
{
    String_Builder sb = {0};
    String_View cstr_Info = {0};
    if (c->Select_Mode) {
        sb_appendf(&sb, "Edit Frame: X = %.2f, Y : %.2f | Frame Size: %.2fx%.2f (PX)",
                      c->edited_frame.x, c->edited_frame.y, c->edited_frame.width, c->edited_frame.height);
        sb_append_null(&sb);
        cstr_Info = sb_to_sv(sb);
    }
    else if (ui->IsFrameSelected) {
        sb_appendf(&sb, "Selected Frame: X = %.2f, Y : %.2f | Frame Size: %.2fx%.2f (PX) | Texture_Index: %zu",
                      c->selected_frame.x, c->selected_frame.y, c->selected_frame.width, c->selected_frame.height, ui->current_texture_index);
        sb_append_null(&sb);
        cstr_Info = sb_to_sv(sb);
    }

    if(cstr_Info.count != 0) {
        const char *Info = nob_temp_sv_to_cstr(cstr_Info);
        DrawTextEx(ui->font, Info, (Vector2) {bondry.x + 300, bondry.height - 50}, 24, 0, RAYWHITE);
    }
    else {
        DrawText(" - ", bondry.x + 300, bondry.height - 50, 24, RAYWHITE);
    }
}

void II_Dialog(Canvas *c, Ui_State *ui)
{
    char const * filter_params[] = { "*.png", "*.jpg", "*.jpeg" };
    const char* path = tinyfd_openFileDialog("Import Image File", "./",
                                            ARRAY_LEN(filter_params), filter_params,
                                            "Image File", 0);
    if (path == NULL) return;
    da_append(&ui->texture_paths, strdup(path));
    ui->NeedsLoading = true;
    load_textures(c, ui);
}

void Import_Image_Button(Rectangle bondry, Canvas *c, Ui_State *ui)
{
    static float box_height = 50;
    static float box_width = 260;

    Rectangle Box = {
        .x = bondry.x + box_width*0.075f,
        .y = bondry.y + bondry.height*0.5f - box_height*0.5f,
        .width = box_width,
        .height = box_height
    };
    float thick = 2.5f;
    Color color;
    Vector2 MousePos = GetMousePosition();
    if (CheckCollisionPointRec(MousePos, Box)) {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) II_Dialog(c, ui);
        color = ColorBrightness(GetColor(0x181818ff), 0.2f);
        thick = 1.5f;
    }
    else {
        color = GetColor(0x181818ff);
    }

    const char *text = "Import Image";
    const int font_size = 24;
    DrawRectangleRec(Box, color);
    DrawRectangleLinesEx((Rectangle) {
            .x = Box.x - thick,
            .y = Box.y - thick,
            .width = Box.width + 2*thick,
            .height = Box.height + 2*thick
        }, thick, WHITE);
    DrawTextEx(ui->font, text, (Vector2) {
                    .x = Box.x + Box.width/2 - MeasureTextEx(ui->font, text, font_size, 0).x/2,
                    .y = Box.y + Box.height/2 - ((float)font_size/2)
                }, font_size, 0, WHITE);
}

void Info_Panel(Canvas *c,Rectangle bondry, Ui_State *ui) {
    Color color = GetColor(0x252525ff);
    DrawRectangleRec(bondry, color);
    Export_Button(c, bondry, ui);
    Import_Button(c, bondry, ui);
    Info_Section(c, bondry, ui);
    Import_Image_Button(bondry, c, ui);
}
