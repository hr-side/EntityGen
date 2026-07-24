#include <data.h>
#include <unistd.h>
#define NOB_IMPLIMENTAITON
#include <nob.h>

void Export_Data(Canvas *c)
{
    Nob_String_Builder File_Data = {0};
    File_Data.count = 0;
    sb_appendf(&File_Data, "{\n");
    sb_appendf(&File_Data, "    \"texturePath\": \"%s\",\n", c->texture_Path); 
    sb_appendf(&File_Data, "        \"frames\": [\n"); 
    sb_appendf(&File_Data, "            {\"x\" : %.2f, \"y\" : %.2f,\"width\" : %.2f,\"height\" : %.2f}\n",
               c->Selecte_Box.x,
               c->Selecte_Box.y,
               c->Selecte_Box.width,
               c->Selecte_Box.height); 
    sb_appendf(&File_Data, "        ]\n"); 
    sb_appendf(&File_Data, "}\n");
    write_entire_file(c->output_path, File_Data.items, File_Data.count);
}
