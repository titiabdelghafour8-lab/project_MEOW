#pragma once

#include <SDL3/SDL.h>
#include <cstdint>

inline Uint32 getColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (a << 24) | (b << 16) | (g << 8) | r;
}

