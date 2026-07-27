#include <data.h>
#include <stddef.h>
#include <unistd.h>
#define NOB_IMPLIMENTAITON
#include <nob.h>

void Export_Data(Canvas *c, Ui_State *ui)
{
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
        sb_appendf(&File_Data, "                {\"x\" : %.2f, \"y\" : %.2f,\"width\" : %.2f,\"height\" : %.2f}\n",
                   c->Selecte_Box.x,
                   c->Selecte_Box.y,
                   c->Selecte_Box.width,
                   c->Selecte_Box.height); 
        sb_appendf(&File_Data, "            ]\n"); 
        sb_appendf(&File_Data, "        },\n");
    }
    sb_appendf(&File_Data, "        \"%s\" : {\n", ui->animations.items[ui->animations.count - 1].name);
    sb_appendf(&File_Data, "            \"frameDuration\" : %.2f,\n", ui->animations.items[ui->animations.count - 1].duration);
    sb_appendf(&File_Data, "            \"loops\" : %s,\n", ui->animations.items[ui->animations.count - 1].loops ? "true" : "false");
    sb_appendf(&File_Data, "            \"frames\": [\n"); 
    sb_appendf(&File_Data, "                {\"x\" : %.2f, \"y\" : %.2f,\"width\" : %.2f,\"height\" : %.2f}\n",
               c->Selecte_Box.x,
               c->Selecte_Box.y,
               c->Selecte_Box.width,
               c->Selecte_Box.height); 
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
