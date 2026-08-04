#include <cJSON.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <nob.h>
#include <data.h>
#include <Animations.h>
#include <animation.h>
#include <canvas.h>
#include <ui.h>

bool Parse(Ui_State *ui, Canvas *c) 
{
    String_Builder sb = {0};
    if (!read_entire_file(ui->import_Path, &sb)) return false;
    sb_append_null(&sb);

    nob_log(NOB_INFO ,"Start Parsing...");
    cJSON *json = cJSON_Parse(sb.items);


    Texture_Paths texture_paths = {0};
    cJSON *texturePath = cJSON_GetObjectItemCaseSensitive(json, "texturePath");
    if (cJSON_IsArray(texturePath)) {
        size_t len = cJSON_GetArraySize(texturePath);
        for (size_t i = 0; i < len; ++i) {
            da_append(&texture_paths, cJSON_GetArrayItem(texturePath, i)->valuestring);
        }
    }

    Animations animations = {0};
    cJSON *animations_ = cJSON_GetObjectItemCaseSensitive(json, "animations");

    if (!cJSON_IsObject(animations_)) {
        nob_log(NOB_ERROR ,"There Was No Animations Object Found!!. There Should Be One Even If It's Empty\n");
        return false;
    }

    cJSON *anim_node = NULL;
    cJSON_ArrayForEach(anim_node, animations_) {
        Animation tmp = {0};
        
        if (anim_node->string) {
            tmp.name = strdup(anim_node->string);
        }

        cJSON *duration = cJSON_GetObjectItemCaseSensitive(anim_node, "frameDuration");
        if (cJSON_IsNumber(duration)) {
            tmp.duration = duration->valuedouble;
        }

        cJSON *loops = cJSON_GetObjectItemCaseSensitive(anim_node, "loops");
        tmp.loops = cJSON_IsTrue(loops);

        cJSON *frames_ = cJSON_GetObjectItemCaseSensitive(anim_node, "frames");
        if (cJSON_IsArray(frames_)) {
            Frames frames = {0};

            cJSON *frame_node = NULL;
            cJSON_ArrayForEach(frame_node, frames_) {
                Frame frame = {0};

                cJSON *x   = cJSON_GetObjectItemCaseSensitive(frame_node, "x");
                cJSON *y   = cJSON_GetObjectItemCaseSensitive(frame_node, "y");
                cJSON *w   = cJSON_GetObjectItemCaseSensitive(frame_node, "width");
                cJSON *h   = cJSON_GetObjectItemCaseSensitive(frame_node, "height");
                cJSON *tex = cJSON_GetObjectItemCaseSensitive(frame_node, "texture_index");
                cJSON *hit = cJSON_GetObjectItemCaseSensitive(frame_node, "hitbox_index");

                frame.cords.x      = cJSON_IsNumber(x)   ? x->valuedouble   : 0.0;
                frame.cords.y      = cJSON_IsNumber(y)   ? y->valuedouble   : 0.0;
                frame.cords.width  = cJSON_IsNumber(w)   ? w->valuedouble   : 0.0;
                frame.cords.height = cJSON_IsNumber(h)   ? h->valuedouble   : 0.0;
                frame.texture_index = cJSON_IsNumber(tex) ? (size_t)tex->valueint : 0;
                frame.texture_index += 1;
                frame.hitbox_index  = cJSON_IsNumber(hit) ? (size_t)hit->valueint : 0;

                frame.ID = c->next_frame_id++;

                da_append(&frames, frame);
            }
            for (size_t i = 0; i < frames.count; ++i) {
                da_append(&tmp.frames, frames.items[i]);
            }
        }

        da_append(&animations, tmp);
    }

    if (texture_paths.count > 0) {
        for (size_t i = 0; i < texture_paths.count; ++i) {
            da_append(&ui->texture_paths, texture_paths.items[i]);
        }
    }

    if (animations.count > 0) {
        for (size_t i = 0; i < animations.count; ++i) {
            da_append(&ui->animations, animations.items[i]);
        }
    }
    ui->NeedsLoading = true;
    load_textures(c, ui);
    ui->current_texture_index = 1;
    ui->IsFrameSelected = false;
    ui->IsAnimationSelected = false;
    nob_log(NOB_INFO ,"Finish Parsing...");

    return true;
}
