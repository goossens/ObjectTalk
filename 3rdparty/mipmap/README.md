# ad_mipmap.h

## Purpose

This is intended for creating mipmaps as part of an pre-run asset build or
as a no-fuss way to generate them that avoids involving any particular
graphics API. However, it will not run as quickly as when using a graphics
API. So, if updating mipmaps during a frame, or generating a substantial
volume at load time, consider another approach.

For reference, a GPU-based approach usually involves setting up a source and
target image and using blit commands to copy and downscale from one to the
other.

## Building

In exactly one C or C++ file that includes this file, add

     #define AD_MIPMAP_IMPLEMENTATION

before the `#include` for this file. This will put the implementation into that
file.

## Basic Usage

Fill out a `AdmBitmap` struct with a bitmap and pass it to
`adm_generate_mipmaps`.

    AdmBitmap bitmap;
    bitmap.width = …;
    bitmap.height = …;
    bitmap.bytes_per_pixel = …;
    bitmap.pixels = …;

    int levels;
    AdmBitmap* mipmaps = adm_generate_mipmaps(&levels, &bitmap);

`mipmaps` is an array which includes `bitmap` as the first element. Then, once
you're done, `adm_free_mipmaps` deallocates the array (including the original).

    adm_free_mipmaps(mipmaps, levels);

If you don't want the original to be in the returned array, use
`adm_generate_mipmaps_no_base`.

## Memory Allocation

To use another memory allocator instead of `calloc` and `free`, define both of
the following before the `#include` that introduces the implementation.

    #define ADM_ALLOCATE(bytes, allocator) …
    #define ADM_DEALLOCATE(memory, allocator) …

## License

This code is public domain under the
[CC0 license](https://creativecommons.org/publicdomain/zero/1.0/).
