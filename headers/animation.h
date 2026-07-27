#ifndef __ANIMATION_H_
#define __ANIMATION_H_
#include <raylib.h>
#include <stddef.h>

typedef struct {
    Rectangle cords;
    size_t Texture_index;
    size_t Hitbox_index;
} Frame;

typedef struct {
    Frame *items;
    size_t count;
    size_t capacity;
} Frames;

typedef struct {
    const char* name;
    float duration;
    bool loops;
    Frames frames;
} Animation;

typedef struct {
    Animation *items;
    size_t count;
    size_t capacity;
} Animations;


#endif//__ANIMATION_H_
