#ifndef __ANIMATION_H_
#define __ANIMATION_H_
#include <raylib.h>
#include <stddef.h>

typedef struct {
    Rectangle *Frame;
    size_t count;
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
