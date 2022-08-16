/* See LICENSE file for copyright and license details. */

#ifndef PNG_IO_H 
#define PNG_IO_H

#include <png.h>

/* grayscale png data */
struct gray_png {
	png_uint_32 width;
	png_uint_32 height;
	png_byte depth;
	png_byte **rows;
};

/* read png file and convert it to grayscale */
struct gray_png *read_png(const char *filename);

/* free allocated memory of `gray_png` struct */
void free_gray_png(struct gray_png *png_data);

#endif /* PNG_IO_H */
