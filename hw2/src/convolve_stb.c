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
#include <sys/time.h>
#include <pthread.h>
#ifdef _OPENMP
#include <omp.h>
#endif

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
// - ksize: odd kernel size (e.g., 3, 5, 15, ...)
static void convolve_baseline_rows(const unsigned char *in,
                                   unsigned char *out,
                                   int w, int h, int ch,
                                   const double *kernel,
                                   int ksize,
                                   int y_start, int y_end) {
    int r = ksize / 2; // radius
    if (y_start < 0) y_start = 0;
    if (y_end > h) y_end = h;

    for (int y = y_start; y < y_end; ++y) {
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

static void convolve_baseline(const unsigned char *in,
                              unsigned char *out,
                              int w, int h, int ch,
                              const double *kernel,
                              int ksize) {
    convolve_baseline_rows(in, out, w, h, ch, kernel, ksize, 0, h);
}

// Loop-order variant: currently just calls baseline.
// 'order' can be used later to switch between different loop nestings.
static void convolve_looporder(const unsigned char *in,
                               unsigned char *out,
                               int w, int h, int ch,
                               const double *kernel,
                               int ksize,
                               int order) {
    int r = ksize / 2;

    if (order == 1) {
        // Order: x, y, c, ky, kx
        for (int x = 0; x < w; ++x) {
            for (int y = 0; y < h; ++y) {
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
    } else if (order == 2) {
        // Order: c, y, x, ky, kx
        for (int c = 0; c < ch; ++c) {
            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
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
    } else {
        // Fallback to baseline order (y, x, c, ky, kx)
        convolve_baseline(in, out, w, h, ch, kernel, ksize);
    }
}

// Tiled variant: currently just calls baseline.
// 'tile_y' and 'tile_x' control tile size and can be used later.
static void convolve_tiled(const unsigned char *in,
                           unsigned char *out,
                           int w, int h, int ch,
                           const double *kernel,
                           int ksize,
                           int tile_y, int tile_x) {
    int r = ksize / 2;
    if (tile_y <= 0 || tile_x <= 0) {
        convolve_baseline(in, out, w, h, ch, kernel, ksize);
        return;
    }

    for (int by = 0; by < h; by += tile_y) {
        int y_end = by + tile_y;
        if (y_end > h) y_end = h;

        for (int bx = 0; bx < w; bx += tile_x) {
            int x_end = bx + tile_x;
            if (x_end > w) x_end = w;

            for (int y = by; y < y_end; ++y) {
                for (int x = bx; x < x_end; ++x) {
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
    }
}

// Unrolled variant: currently just calls baseline.
// 'unroll_factor' can be used later to unroll the inner x-loop.
static void convolve_unrolled(const unsigned char *in,
                              unsigned char *out,
                              int w, int h, int ch,
                              const double *kernel,
                              int ksize,
                              int unroll_factor) {
    int r = ksize / 2;
    if (unroll_factor <= 1) {
        convolve_baseline(in, out, w, h, ch, kernel, ksize);
        return;
    }

    for (int y = 0; y < h; ++y) {
        int x = 0;
        int limit = w - (w % unroll_factor);

        // Unrolled part
        for (x = 0; x < limit; x += unroll_factor) {
            for (int c = 0; c < ch; ++c) {
                double sum[32]; // supports unroll_factor up to 32 safely
                for (int i = 0; i < unroll_factor; ++i) {
                    sum[i] = 0.0;
                }

                for (int ky = -r; ky <= r; ++ky) {
                    for (int kx = -r; kx <= r; ++kx) {
                        int sx_base = x + kx;
                        int sy = y + ky;
                        double kval = kernel[(ky + r) * ksize + (kx + r)];
                        for (int i = 0; i < unroll_factor; ++i) {
                            int sx = sx_base + i;
                            unsigned char p = get_pixel(in, w, h, ch, sx, sy, c);
                            sum[i] += p * kval;
                        }
                    }
                }

                for (int i = 0; i < unroll_factor; ++i) {
                    int iv = (int)lround(sum[i]);
                    int xx = x + i;
                    out[(y * w + xx) * ch + c] = clamp_u8(iv);
                }
            }
        }

        // Remainder (tail) part
        for (; x < w; ++x) {
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

typedef struct {
    const unsigned char *in;
    unsigned char *out;
    int w, h, ch;
    const double *kernel;
    int ksize;
    int y_start, y_end;
} thread_args_t;

static void *thread_func(void *arg) {
    thread_args_t *t = (thread_args_t *)arg;
    convolve_baseline_rows(t->in, t->out, t->w, t->h, t->ch,
                           t->kernel, t->ksize, t->y_start, t->y_end);
    return NULL;
}

static void run_pthreads_baseline(const unsigned char *in,
                                  unsigned char *out,
                                  int w, int h, int ch,
                                  const double *kernel,
                                  int ksize,
                                  int threads) {
    if (threads <= 1) {
        convolve_baseline(in, out, w, h, ch, kernel, ksize);
        return;
    }
    if (threads > h) {
        threads = h;
    }

    pthread_t *tids = (pthread_t *)malloc(sizeof(pthread_t) * threads);
    thread_args_t *args = (thread_args_t *)malloc(sizeof(thread_args_t) * threads);
    if (!tids || !args) {
        fprintf(stderr, "Warning: could not allocate thread structures, falling back to single-threaded baseline.\n");
        free(tids);
        free(args);
        convolve_baseline(in, out, w, h, ch, kernel, ksize);
        return;
    }

    int rows_per_thread = h / threads;
    int remainder = h % threads;
    int y = 0;

    for (int i = 0; i < threads; ++i) {
        int extra = (i < remainder) ? 1 : 0;
        int y_start = y;
        int y_end = y_start + rows_per_thread + extra;

        args[i].in = in;
        args[i].out = out;
        args[i].w = w;
        args[i].h = h;
        args[i].ch = ch;
        args[i].kernel = kernel;
        args[i].ksize = ksize;
        args[i].y_start = y_start;
        args[i].y_end = y_end;

        if (pthread_create(&tids[i], NULL, thread_func, &args[i]) != 0) {
            fprintf(stderr, "Warning: pthread_create failed for thread %d, falling back to single-threaded baseline.\n", i);
            for (int j = 0; j < i; ++j) {
                pthread_join(tids[j], NULL);
            }
            free(tids);
            free(args);
            convolve_baseline(in, out, w, h, ch, kernel, ksize);
            return;
        }

        y = y_end;
    }

    for (int i = 0; i < threads; ++i) {
        pthread_join(tids[i], NULL);
    }

    free(tids);
    free(args);
}

#ifdef _OPENMP
static void convolve_baseline_omp(const unsigned char *in,
                                  unsigned char *out,
                                  int w, int h, int ch,
                                  const double *kernel,
                                  int ksize,
                                  int threads) {
    int r = ksize / 2;
    if (threads <= 0) {
        threads = 1;
    }

#pragma omp parallel for collapse(2) num_threads(threads)
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
#endif

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

// Generic ksize x ksize normalized box blur kernel.
// For ksize = 15, this is the "large" kernel the homework wants.
static void make_box_blur_kernel(double *kernel, int ksize) {
    double v = 1.0 / (ksize * ksize);
    int n = ksize * ksize;
    for (int i = 0; i < n; ++i) {
        kernel[i] = v;
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input_image output_image [ksize]\n", argv[0]);
        fprintf(stderr, "Example: %s input.jpg output.png 3\n", argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];

    // Parameters controlling the convolution variant.
    // ksize: kernel size (3 or 15 as required by the homework)
    // threads: number of threads (not used yet, for future pthreads work)
    // order: loop-order selection (0 = baseline)
    // tile: tile size (0 = no tiling)
    // unroll: unroll factor (0 = no unrolling)
    int ksize  = 3;
    int threads = 1;
    int order  = 0;
    int tile   = 0;
    int unroll = 0;

    // Supported argument patterns:
    //  1) prog input output
    //  2) prog input output ksize
    //  3) prog input output threads ksize order tile unroll  (used by run_exp target)
    if (argc == 3) {
        // defaults already set
    } else if (argc == 4) {
        ksize = atoi(argv[3]);
    } else if (argc == 8) {
        threads = atoi(argv[3]);
        ksize   = atoi(argv[4]);
        order   = atoi(argv[5]);
        tile    = atoi(argv[6]);
        unroll  = atoi(argv[7]);
    } else {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s input_image output_image [ksize]\n", argv[0]);
        fprintf(stderr, "  %s input_image output_image threads ksize order tile unroll\n", argv[0]);
        return 1;
    }

    if (ksize <= 0 || (ksize % 2) == 0) {
        fprintf(stderr, "Error: ksize must be a positive odd integer (e.g. 3 or 15)\n");
        return 1;
    }

    if (threads <= 0) {
        threads = 1;
    }

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

    // Build the kernel depending on ksize.
    int kernel_elems = ksize * ksize;
    double *kernel = (double *)malloc(kernel_elems * sizeof(double));
    if (!kernel) {
        fprintf(stderr, "Error: could not allocate kernel (ksize = %d)\n", ksize);
        free(out);
        stbi_image_free(img);
        return 1;
    }

    if (ksize == 3) {
        int dummy_ksize_out = 0;
        make_edge_kernel_3x3(kernel, &dummy_ksize_out);   // 3x3 edge detection
    } else if (ksize == 15) {
        make_box_blur_kernel(kernel, ksize);              // 15x15 box blur
    } else {
        fprintf(stderr, "Error: unsupported ksize = %d (use 3 or 15 for now)\n", ksize);
        free(kernel);
        free(out);
        stbi_image_free(img);
        return 1;
    }

    // Run convolution (single-threaded or multi-threaded) and measure time.
    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    if (threads > 1 && order == 0 && tile == 0 && unroll == 0) {
#ifdef _OPENMP
        // OpenMP-parallel baseline
        convolve_baseline_omp(img, out, width, height, channels, kernel, ksize, threads);
#else
        // Pthreads-parallel baseline
        run_pthreads_baseline(img, out, width, height, channels, kernel, ksize, threads);
#endif
    } else if (tile > 0) {
        // Tiled version (single-threaded)
        convolve_tiled(img, out, width, height, channels, kernel, ksize, tile, tile);
    } else if (unroll > 0) {
        // Unrolled version (single-threaded)
        convolve_unrolled(img, out, width, height, channels, kernel, ksize, unroll);
    } else if (order != 0) {
        // Loop-order variant (single-threaded)
        convolve_looporder(img, out, width, height, channels, kernel, ksize, order);
    } else {
        // Baseline version (single-threaded)
        convolve_baseline(img, out, width, height, channels, kernel, ksize);
    }

    gettimeofday(&t1, NULL);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;
    printf("CONV_TIME %f\n", elapsed);

    // Save output image as PNG. You can also use JPG if you want, but
    // PNG is lossless and avoids compression artifacts.
    int stride_in_bytes = width * channels;
    if (!stbi_write_png(output_path, width, height, channels, out, stride_in_bytes)) {
        fprintf(stderr, "Error: could not write output image '%s'\n", output_path);
        free(kernel);
        free(out);
        stbi_image_free(img);
        return 1;
    }

    printf("Wrote %s\n", output_path);

    free(kernel);
    free(out);
    stbi_image_free(img);
    return 0;
}