// ad_mipmap.h - version 1.0 - A mipmap generator.
//
// Written in 2018 by Andrew Dawson
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.
//
// Purpose
//
// This is intended for creating mipmaps as part of an pre-run asset build or
// as a no-fuss way to generate them that avoids involving any particular
// graphics API. However, it will not run as quickly as when using a graphics
// API. So, if updating mipmaps during a frame, or generating a substantial
// volume at load time, consider another approach.
//
// For reference, a GPU-based approach usually involves setting up a source and
// target image and using blit commands to copy and downscale from one to the
// other.
//
// Building
//
// In exactly one C or C++ file that includes this file, add
//
//      #define AD_MIPMAP_IMPLEMENTATION
//
// before the #include for this file. This will put the implementation into that
// file.
//
// Basic Usage
//
// Fill out a AdmBitmap struct with a bitmap and pass it to
// adm_generate_mipmaps.
//
//     AdmBitmap bitmap;
//     bitmap.width = …;
//     bitmap.height = …;
//     bitmap.bytes_per_pixel = …;
//     bitmap.pixels = …;
//
//     int levels;
//     AdmBitmap* mipmaps = adm_generate_mipmaps(&levels, &bitmap);
//
// mipmaps is an array which includes bitmap as the first element. Then, once
// you're done, adm_free_mipmaps deallocates the array (including the original).
//
//     adm_free_mipmaps(mipmaps, levels);
//
// If you don't want the original to be in the returned array, use
// adm_generate_mipmaps_no_base.
//
// Memory Allocation
//
// To use another memory allocator instead of calloc and free, define both of
// the following before the #include that introduces the implementation.
//
//     #define ADM_ALLOCATE(bytes, allocator) …
//     #define ADM_DEALLOCATE(memory, allocator) …

#ifndef AD_MIPMAP_H_
#define AD_MIPMAP_H_

typedef struct AdmBitmap
{
    void* pixels;
    int width;
    int height;
    int bytes_per_pixel;
} AdmBitmap;

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(AD_MIPMAP_STATIC)
#define ADM_CALL static
#else
#define ADM_CALL extern
#endif

ADM_CALL int adm_count_mip_levels(int width, int height);
ADM_CALL int adm_get_next_mipmap_size(int width, int height, int bytes_per_pixel);

ADM_CALL void adm_generate_mipmap(AdmBitmap* mipmap, AdmBitmap* bitmap);
ADM_CALL AdmBitmap* adm_generate_mipmaps(int* levels, AdmBitmap* bitmap);
ADM_CALL AdmBitmap* adm_generate_mipmaps_no_base(int* levels, AdmBitmap* bitmap);
ADM_CALL void adm_free_mipmaps(AdmBitmap* mipmaps, int levels);

#if defined(__cplusplus)
} // extern "C"
#endif

#endif // AD_MIPMAP_H_

#if defined(AD_MIPMAP_IMPLEMENTATION)

#if !defined(ADM_ALLOCATE)

#include <stdlib.h>

#define ADM_ALLOCATE(bytes, allocator) ((void) (allocator), calloc(1, bytes))
#define ADM_DEALLOCATE(memory, allocator) ((void) (allocator), free(memory))

#endif // !defined(ADM_ALLOCATE)

#if !defined(ADM_ALLOCATE) || !defined(ADM_DEALLOCATE)
#error ADM_ALLOCATE and ADM_DEALLOCATE must both be defined, or neither defined.
#endif

#include <stdint.h>

typedef struct AdmPixel8
{
    uint8_t r;
} AdmPixel8;

typedef struct AdmPixel16
{
    uint8_t r, g;
} AdmPixel16;

typedef struct AdmPixel24
{
    uint8_t r, g, b;
} AdmPixel24;

typedef struct AdmPixel32
{
    uint8_t a, r, g, b;
} AdmPixel32;

static int adm_imax(int a, int b)
{
    if(a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

static int adm_imin(int a, int b)
{
    if(a < b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

static int adm_log2(int x)
{
    int y = 0;
    for(y = 0; x > 1; x >>= 1, y += 1);
    return y;
}

static float adm_lerp(float v0, float v1, float t)
{
    return (1.0f - t) * v0 + t * v1;
}

static AdmPixel8 adm_lerp8(AdmPixel8 p0, AdmPixel8 p1, float t)
{
    AdmPixel8 result;
    result.r = adm_lerp(p0.r, p1.r, t);
    return result;
}

static AdmPixel16 adm_lerp16(AdmPixel16 p0, AdmPixel16 p1, float t)
{
    AdmPixel16 result;
    result.r = adm_lerp(p0.r, p1.r, t);
    result.g = adm_lerp(p0.g, p1.g, t);
    return result;
}

static AdmPixel24 adm_lerp24(AdmPixel24 p0, AdmPixel24 p1, float t)
{
    AdmPixel24 result;
    result.r = adm_lerp(p0.r, p1.r, t);
    result.g = adm_lerp(p0.g, p1.g, t);
    result.b = adm_lerp(p0.b, p1.b, t);
    return result;
}

static AdmPixel32 adm_lerp32(AdmPixel32 p0, AdmPixel32 p1, float t)
{
    AdmPixel32 result;
    result.r = adm_lerp(p0.r, p1.r, t);
    result.g = adm_lerp(p0.g, p1.g, t);
    result.b = adm_lerp(p0.b, p1.b, t);
    result.a = adm_lerp(p0.a, p1.a, t);
    return result;
}

static void adm_scale_pixel8s(AdmBitmap* source, AdmPixel8* pixels, int width, int height)
{
    AdmPixel8* from = (AdmPixel8*) source->pixels;
    int source_width = source->width;
    int source_height = source->height;

    float width_divisor = adm_imax(width - 1, 1);
    float height_divisor = adm_imax(height - 1, 1);

    for(int i = 0; i < height; i += 1)
    {
        float y = i * (source_height - 1) / height_divisor;
        int y_int = y;
        float y_fraction = y - y_int;
        int m = adm_imin(y_int + 1, source_height - 1);

        for(int j = 0; j < width; j += 1)
        {
            float x = j * (source_width - 1) / width_divisor;
            int x_int = x;
            float x_fraction = x - x_int;
            int k = adm_imin(x_int + 1, source_width - 1);

            AdmPixel8 p0 = from[(source_width * y_int) + x_int];
            AdmPixel8 p1 = from[(source_width * y_int) + k];
            AdmPixel8 p2 = from[(source_width * m) + x_int];
            AdmPixel8 p3 = from[(source_width * m) + k];

            AdmPixel8 v0 = adm_lerp8(p0, p1, x_fraction);
            AdmPixel8 v1 = adm_lerp8(p2, p3, x_fraction);
            AdmPixel8 pixel = adm_lerp8(v0, v1, y_fraction);

            pixels[(width * i) + j] = pixel;
        }
    }
}

static void adm_scale_pixel16s(AdmBitmap* source, AdmPixel16* pixels, int width, int height)
{
    AdmPixel16* from = (AdmPixel16*) source->pixels;
    int source_width = source->width;
    int source_height = source->height;

    float width_divisor = adm_imax(width - 1, 1);
    float height_divisor = adm_imax(height - 1, 1);

    for(int i = 0; i < height; i += 1)
    {
        float y = i * (source_height - 1) / height_divisor;
        int y_int = y;
        float y_fraction = y - y_int;
        int m = adm_imin(y_int + 1, source_height - 1);

        for(int j = 0; j < width; j += 1)
        {
            float x = j * (source_width - 1) / width_divisor;
            int x_int = x;
            float x_fraction = x - x_int;
            int k = adm_imin(x_int + 1, source_width - 1);

            AdmPixel16 p0 = from[(source_width * y_int) + x_int];
            AdmPixel16 p1 = from[(source_width * y_int) + k];
            AdmPixel16 p2 = from[(source_width * m) + x_int];
            AdmPixel16 p3 = from[(source_width * m) + k];

            AdmPixel16 v0 = adm_lerp16(p0, p1, x_fraction);
            AdmPixel16 v1 = adm_lerp16(p2, p3, x_fraction);
            AdmPixel16 pixel = adm_lerp16(v0, v1, y_fraction);

            pixels[(width * i) + j] = pixel;
        }
    }
}

static void adm_scale_pixel24s(AdmBitmap* source, AdmPixel24* pixels, int width, int height)
{
    AdmPixel24* from = (AdmPixel24*) source->pixels;
    int source_width = source->width;
    int source_height = source->height;

    float width_divisor = adm_imax(width - 1, 1);
    float height_divisor = adm_imax(height - 1, 1);

    for(int i = 0; i < height; i += 1)
    {
        float y = i * (source_height - 1) / height_divisor;
        int y_int = y;
        float y_fraction = y - y_int;
        int m = adm_imin(y_int + 1, source_height - 1);

        for(int j = 0; j < width; j += 1)
        {
            float x = j * (source_width - 1) / width_divisor;
            int x_int = x;
            float x_fraction = x - x_int;
            int k = adm_imin(x_int + 1, source_width - 1);

            AdmPixel24 p0 = from[(source_width * y_int) + x_int];
            AdmPixel24 p1 = from[(source_width * y_int) + k];
            AdmPixel24 p2 = from[(source_width * m) + x_int];
            AdmPixel24 p3 = from[(source_width * m) + k];

            AdmPixel24 v0 = adm_lerp24(p0, p1, x_fraction);
            AdmPixel24 v1 = adm_lerp24(p2, p3, x_fraction);
            AdmPixel24 pixel = adm_lerp24(v0, v1, y_fraction);

            pixels[(width * i) + j] = pixel;
        }
    }
}

static void adm_scale_pixel32s(AdmBitmap* source, AdmPixel32* pixels, int width, int height)
{
    AdmPixel32* from = (AdmPixel32*) source->pixels;
    int source_width = source->width;
    int source_height = source->height;

    float width_divisor = adm_imax(width - 1, 1);
    float height_divisor = adm_imax(height - 1, 1);

    for(int i = 0; i < height; i += 1)
    {
        float y = i * (source_height - 1) / height_divisor;
        int y_int = y;
        float y_fraction = y - y_int;
        int m = adm_imin(y_int + 1, source_height - 1);

        for(int j = 0; j < width; j += 1)
        {
            float x = j * (source_width - 1) / width_divisor;
            int x_int = x;
            float x_fraction = x - x_int;
            int k = adm_imin(x_int + 1, source_width - 1);

            AdmPixel32 p0 = from[(source_width * y_int) + x_int];
            AdmPixel32 p1 = from[(source_width * y_int) + k];
            AdmPixel32 p2 = from[(source_width * m) + x_int];
            AdmPixel32 p3 = from[(source_width * m) + k];

            AdmPixel32 v0 = adm_lerp32(p0, p1, x_fraction);
            AdmPixel32 v1 = adm_lerp32(p2, p3, x_fraction);
            AdmPixel32 pixel = adm_lerp32(v0, v1, y_fraction);

            pixels[(width * i) + j] = pixel;
        }
    }
}

static void adm_generate_mipmap_with_allocator(AdmBitmap* mipmap, AdmBitmap* bitmap, void* allocator)
{
    int width = adm_imax(bitmap->width / 2, 1);
    int height = adm_imax(bitmap->height / 2, 1);

    AdmBitmap result;
    result.width = width;
    result.height = height;
    result.bytes_per_pixel = bitmap->bytes_per_pixel;

    switch(bitmap->bytes_per_pixel)
    {
        case 1:
        {
            AdmPixel8* pixels = (AdmPixel8*) ADM_ALLOCATE(sizeof(AdmPixel8) * width * height, allocator);
            adm_scale_pixel8s(bitmap, pixels, width, height);
            result.pixels = pixels;
            break;
        }
        case 2:
        {
            AdmPixel16* pixels = (AdmPixel16*) ADM_ALLOCATE(sizeof(AdmPixel16) * width * height, allocator);
            adm_scale_pixel16s(bitmap, pixels, width, height);
            result.pixels = pixels;
            break;
        }
        case 3:
        {
            AdmPixel24* pixels = (AdmPixel24*) ADM_ALLOCATE(sizeof(AdmPixel24) * width * height, allocator);
            adm_scale_pixel24s(bitmap, pixels, width, height);
            result.pixels = pixels;
            break;
        }
        case 4:
        {
            AdmPixel32* pixels = (AdmPixel32*) ADM_ALLOCATE(sizeof(AdmPixel32) * width * height, allocator);
            adm_scale_pixel32s(bitmap, pixels, width, height);
            result.pixels = pixels;
            break;
        }
    }

    *mipmap = result;
}

static AdmBitmap* adm_generate_mipmaps_with_allocator(int* levels, AdmBitmap* bitmap, void* allocator)
{
    int count = adm_count_mip_levels(bitmap->width, bitmap->height);
    AdmBitmap* mipmaps = ADM_ALLOCATE(sizeof(AdmBitmap) * count, allocator);
    mipmaps[0] = *bitmap;
    for(int i = 1; i < count; i += 1)
    {
        adm_generate_mipmap(&mipmaps[i], &mipmaps[i - 1]);
    }
    *levels = count;
    return mipmaps;
}

static AdmBitmap* adm_generate_mipmaps_no_base_with_allocator(int* levels, AdmBitmap* bitmap, void* allocator)
{
    int count = adm_count_mip_levels(bitmap->width, bitmap->height) - 1;
    AdmBitmap* mipmaps = ADM_ALLOCATE(sizeof(AdmBitmap) * count, allocator);
    AdmBitmap* prior = bitmap;
    for(int i = 0; i < count; i += 1)
    {
        adm_generate_mipmap(&mipmaps[i], prior);
        prior = &mipmaps[i];
    }
    *levels = count;
    return mipmaps;
}

static void adm_free_mipmaps_with_allocator(AdmBitmap* mipmaps, int levels, void* allocator)
{
    for(int i = 0; i < levels; i += 1)
    {
        ADM_DEALLOCATE(mipmaps[i].pixels, allocator);
    }
    ADM_DEALLOCATE(mipmaps, allocator);
}

int adm_count_mip_levels(int width, int height)
{
    return adm_log2(adm_imax(width, height)) + 1;
}

int adm_get_next_mipmap_size(int width, int height, int bytes_per_pixel)
{
    int w = adm_imax(width / 2, 1);
    int h = adm_imax(height / 2, 1);
    return bytes_per_pixel * w * h;
}

void adm_generate_mipmap(AdmBitmap* mipmap, AdmBitmap* bitmap)
{
    adm_generate_mipmap_with_allocator(mipmap, bitmap, NULL);
}

AdmBitmap* adm_generate_mipmaps(int* levels, AdmBitmap* bitmap)
{
    return adm_generate_mipmaps_with_allocator(levels, bitmap, NULL);
}

AdmBitmap* adm_generate_mipmaps_no_base(int* levels, AdmBitmap* bitmap)
{
    return adm_generate_mipmaps_no_base_with_allocator(levels, bitmap, NULL);
}

void adm_free_mipmaps(AdmBitmap* mipmaps, int levels)
{
    adm_free_mipmaps_with_allocator(mipmaps, levels, NULL);
}

#endif // defined(AD_MIPMAP_IMPLEMENTATION)
