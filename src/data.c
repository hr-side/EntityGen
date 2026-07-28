#include <data.h>
#include <stddef.h>
#include <unistd.h>
#define NOB_IMPLIMENTAITON
#include <nob.h>

void Export_Data(Canvas *c, Ui_State *ui)
{
    UNUSED(c);
    Nob_String_Builder File_Data = {0};
    File_Data.count = 0;
    sb_appendf(&File_Data, "{\n");

    sb_appendf(&File_Data, "    \"texturePath\": [\n"); 
    for (size_t i = 0; i < ui->texture_paths.count - 1; ++i) {
        sb_appendf(&File_Data, "        \"%s\",\n", ui->texture_paths.items[i]);
    }
    sb_appendf(&File_Data, "        \"%s\"\n", ui->texture_paths.items[ui->texture_paths.count - 1]);
    sb_appendf(&File_Data, "    ],\n");

    if (ui->animations.count == 0) {
        sb_appendf(&File_Data, "    \"animations\" : {}\n");
        goto save;
    }
    sb_appendf(&File_Data, "    \"animations\" : {\n");
    for (size_t i = 0; i < ui->animations.count - 1; ++i) {
        sb_appendf(&File_Data, "        \"%s\" : {\n", ui->animations.items[i].name);
        sb_appendf(&File_Data, "            \"frameDuration\" : %.2f,\n", ui->animations.items[i].duration);
        sb_appendf(&File_Data, "            \"loops\" : %s,\n", ui->animations.items[i].loops ? "true" : "false");
        sb_appendf(&File_Data, "            \"frames\": [\n"); 
        for (size_t j = 0; j < ui->animations.items[i].frames.count - 1; ++j) {
            sb_appendf(&File_Data, "                {\"x\" : %.2f, \"y\" : %.2f,\"width\" : %.2f,\"height\" : %.2f, \"texture_index\" : %zu, \"hitbox_index\" : %zu},\n",
                       ui->animations.items[i].frames.items[j].cords.x,
                       ui->animations.items[i].frames.items[j].cords.y,
                       ui->animations.items[i].frames.items[j].cords.width,
                       ui->animations.items[i].frames.items[j].cords.height,
                       ui->animations.items[i].frames.items[j].texture_index,
                       ui->animations.items[i].frames.items[j].hitbox_index); 
        }
        sb_appendf(&File_Data, "                {\"x\" : %.2f, \"y\" : %.2f,\"width\" : %.2f,\"height\" : %.2f, \"texture_index\" : %zu, \"hitbox_index\" : %zu}\n",
                   ui->animations.items[i].frames.items[ui->animations.items[i].frames.count - 1].cords.y,
                   ui->animations.items[i].frames.items[ui->animations.items[i].frames.count - 1].cords.x,
                   ui->animations.items[i].frames.items[ui->animations.items[i].frames.count - 1].cords.width,
                   ui->animations.items[i].frames.items[ui->animations.items[i].frames.count - 1].cords.height,
                   ui->animations.items[i].frames.items[ui->animations.items[i].frames.count - 1].texture_index,
                   ui->animations.items[i].frames.items[ui->animations.items[i].frames.count - 1].hitbox_index); 
        sb_appendf(&File_Data, "            ]\n"); 
        sb_appendf(&File_Data, "        },\n");
    }
    sb_appendf(&File_Data, "        \"%s\" : {\n", ui->animations.items[ui->animations.count - 1].name);
    sb_appendf(&File_Data, "            \"frameDuration\" : %.2f,\n", ui->animations.items[ui->animations.count - 1].duration);
    sb_appendf(&File_Data, "            \"loops\" : %s,\n", ui->animations.items[ui->animations.count - 1].loops ? "true" : "false");
    sb_appendf(&File_Data, "            \"frames\": [\n"); 
    for (size_t j = 0; j < ui->animations.items[ui->animations.count - 1].frames.count - 1; ++j) {
        sb_appendf(&File_Data, "                {\"x\" : %.2f, \"y\" : %.2f,\"width\" : %.2f,\"height\" : %.2f, \"texture_index\" : %zu, \"hitbox_index\" : %zu},\n",
                   ui->animations.items[ui->animations.count - 1].frames.items[j].cords.x,
                   ui->animations.items[ui->animations.count - 1].frames.items[j].cords.y,
                   ui->animations.items[ui->animations.count - 1].frames.items[j].cords.width,
                   ui->animations.items[ui->animations.count - 1].frames.items[j].cords.height,
                   ui->animations.items[ui->animations.count - 1].frames.items[j].texture_index,
                   ui->animations.items[ui->animations.count - 1].frames.items[j].hitbox_index); 
    }
    sb_appendf(&File_Data, "                {\"x\" : %.2f, \"y\" : %.2f,\"width\" : %.2f,\"height\" : %.2f, \"texture_index\" : %zu, \"hitbox_index\" : %zu}\n",
               ui->animations.items[ui->animations.count - 1].frames.items[ui->animations.items[ui->animations.count - 1].frames.count - 1].cords.y,
               ui->animations.items[ui->animations.count - 1].frames.items[ui->animations.items[ui->animations.count - 1].frames.count - 1].cords.x,
               ui->animations.items[ui->animations.count - 1].frames.items[ui->animations.items[ui->animations.count - 1].frames.count - 1].cords.width,
               ui->animations.items[ui->animations.count - 1].frames.items[ui->animations.items[ui->animations.count - 1].frames.count - 1].cords.height,
               ui->animations.items[ui->animations.count - 1].frames.items[ui->animations.items[ui->animations.count - 1].frames.count - 1].texture_index,
               ui->animations.items[ui->animations.count - 1].frames.items[ui->animations.items[ui->animations.count - 1].frames.count - 1].hitbox_index); 
    sb_appendf(&File_Data, "            ]\n"); 
    sb_appendf(&File_Data, "        }\n");
    sb_appendf(&File_Data, "    }\n");

save:
    sb_appendf(&File_Data, "}\n");
    write_entire_file(ui->output_path, File_Data.items, File_Data.count);
}

void Import_Data(Canvas *c, Ui_State *ui)
{
    UNUSED(c);  // Import and parse the files,
                // Not yet tho until we can save a file then we try parsing it
    UNUSED(ui);
}
