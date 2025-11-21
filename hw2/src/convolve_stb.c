

// HW2 - Parallel 2D Convolution using Pthreads
// Part 1: Baseline single-threaded convolution using stb_image
// ------------------------------------------------------------
// This file currently implements only the sequential version of
// convolution. Later parts of the homework (pthreads, loop order,
// tiling, unrolling, etc.) will extend this.

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Clamp integer value to [0, 255]
static unsigned char clamp_u8(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (unsigned char)v;
}

// Safe pixel access with clamped boundary conditions.
// If (x, y) is outside the image, it is clamped to the nearest
// valid coordinate.
static unsigned char get_pixel(const unsigned char *img,
                               int w, int h, int ch,
                               int x, int y, int c) {
    if (x < 0) x = 0;
    if (x >= w) x = w - 1;
    if (y < 0) y = 0;
    if (y >= h) y = h - 1;
    if (c < 0) c = 0;
    if (c >= ch) c = ch - 1;

    int idx = (y * w + x) * ch + c;
    return img[idx];
}

// Generic 2D convolution for an image with "ch" channels. 
// - in:  input image buffer (width * height * channels)
// - out: output image buffer (same size as input)
// - w, h, ch: image width, height, and number of channels
// - kernel: ksize x ksize convolution kernel
// - ksize: odd kernel size (e.g., 3, 5, 31, ...)
static void convolve(const unsigned char *in,
                     unsigned char *out,
                     int w, int h, int ch,
                     const double *kernel,
                     int ksize) {
    int r = ksize / 2; // radius

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < ch; ++c) {
                double sum = 0.0;

                for (int ky = -r; ky <= r; ++ky) {
                    for (int kx = -r; kx <= r; ++kx) {
                        int sx = x + kx;
                        int sy = y + ky;
                        unsigned char p = get_pixel(in, w, h, ch, sx, sy, c);
                        double kval = kernel[(ky + r) * ksize + (kx + r)];
                        sum += p * kval;
                    }
                }

                int iv = (int)lround(sum);
                out[(y * w + x) * ch + c] = clamp_u8(iv);
            }
        }
    }
}

// Example 3x3 edge-detection kernel (Sobel-like, horizontal edges).
// This matches the style of the example in the homework handout.
static void make_edge_kernel_3x3(double *kernel, int *ksize_out) {
    *ksize_out = 3;
    double k[9] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };
    for (int i = 0; i < 9; ++i) {
        kernel[i] = k[i];
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input_image output_image\n", argv[0]);
        fprintf(stderr, "Example: %s input.jpg output.png\n", argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];

    int width, height, channels;
    unsigned char *img = stbi_load(input_path, &width, &height, &channels, 0);
    if (!img) {
        fprintf(stderr, "Error: could not load image '%s'\n", input_path);
        return 1;
    }

    printf("Loaded %s (%d x %d, %d channels)\n", input_path, width, height, channels);

    size_t num_pixels = (size_t)width * (size_t)height;
    size_t buf_size = num_pixels * (size_t)channels;

    unsigned char *out = (unsigned char *)malloc(buf_size);
    if (!out) {
        fprintf(stderr, "Error: could not allocate output buffer\n");
        stbi_image_free(img);
        return 1;
    }

    // Build the kernel for part 1 (3x3 edge-detection filter).
    int ksize = 0;
    double kernel[3 * 3];
    make_edge_kernel_3x3(kernel, &ksize);

    // Run baseline convolution (single-threaded).
    convolve(img, out, width, height, channels, kernel, ksize);

    // Save output image as PNG. You can also use JPG if you want, but
    // PNG is lossless and avoids compression artifacts.
    int stride_in_bytes = width * channels;
    if (!stbi_write_png(output_path, width, height, channels, out, stride_in_bytes)) {
        fprintf(stderr, "Error: could not write output image '%s'\n", output_path);
        free(out);
        stbi_image_free(img);
        return 1;
    }

    printf("Wrote %s\n", output_path);

    free(out);
    stbi_image_free(img);
    return 0;
}