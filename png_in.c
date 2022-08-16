#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#include "png_in.h"

#define PNG_BYTES_TO_CHECK 4

/* read png file and convert it to grayscale */
struct gray_png *read_png(const char *filename)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "Error: failed to open file '%s'\n", filename);
		return NULL;
	}

	/* read signature bytes */
	unsigned char sig[PNG_BYTES_TO_CHECK];
	if (fread(sig, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) {
		fclose(fp);
		fprintf(stderr, "Error: failed to read signature bytes"
				"from '%s'\n", filename);
		return NULL;
	}

	/* compare first bytes of signature */
	if (png_sig_cmp(sig, 0, PNG_BYTES_TO_CHECK)) {
		fclose(fp);
		fprintf(stderr, "Error: '%s' is not a PNG file\n", filename);
		return NULL;
	}

	/* initialize png_struct `pngp` */
	png_struct *pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
	if (!pngp) {
		fclose(fp);
		fprintf(stderr, "Error: memory allocation failed\n");
		return NULL;
	}

	/* allocate memory for image information */
	png_info *infop = png_create_info_struct(pngp);  
	if (!infop) {
		fclose(fp);
		png_destroy_read_struct(&pngp, &infop, NULL);
		return NULL;
	}

	/* set error handling */
	if (setjmp(png_jmpbuf(pngp))) {
		fclose(fp);
		png_destroy_read_struct(&pngp, &infop, NULL);
		return NULL;
	}

	/* set up output control */
	png_init_io(pngp, fp);

	/* because we read some of the signature */
	png_set_sig_bytes(pngp, PNG_BYTES_TO_CHECK);

	/*
	 * The call to png_read_info() gives us all of the information from the
	 * PNG file before the first IDAT (image data chunk).
	 */
	png_read_info(pngp, infop);

	/* replace transparency with white */
	png_color_16 white_color = {.red = 255, .green = 255, .blue = 255};
	png_set_background(pngp, &white_color,
	                   PNG_BACKGROUND_GAMMA_SCREEN, 0, 1);

	/* convert image to grayscale */
	png_byte color_type = png_get_color_type(pngp, infop);
	if (color_type != PNG_COLOR_TYPE_GRAY /* &&
	    color_type != PNG_COLOR_TYPE_GRAY_ALPHA */) {
		png_set_rgb_to_gray(pngp, 1, -1, -1);
		png_read_update_info(pngp, infop);
	}

	/* optain information */
	struct gray_png *png_data = malloc(sizeof(struct gray_png));
	if (!png_data) {
		fprintf(stderr, "Error: memory allocation failed\n");
		return NULL;
	}
	png_data->width = png_get_image_width(pngp, infop);
	png_data->height = png_get_image_height(pngp, infop);
	png_data->depth = png_get_bit_depth(pngp, infop);

	/* allocate and read pixel rows */
	png_data->rows = malloc(sizeof(png_byte *) * png_data->height);
	for(int y = 0; y < png_data->height; ++y)
		png_data->rows[y] = malloc(png_get_rowbytes(pngp, infop));
	png_read_image(pngp, png_data->rows);

	/* Read rest of file, and get additional chunks in info_ptr */
	png_read_end(pngp, infop);

	/* free allocated memory and close file */
	png_destroy_read_struct(&pngp, &infop, NULL);
	fclose(fp);
	return png_data;
}

/* free allocated memory of `gray_png` struct */
void free_gray_png(struct gray_png *png_data) {
	for (int y = 0; y < png_data->height; ++y)
		free(png_data->rows[y]);
	free(png_data->rows);
	free(png_data);
}
