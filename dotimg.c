#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "png_in.h"

#define DOT_COUNT 30000

/* print SVG code to `stdout` */
void print_svg(int width, int height, int dot_count, float (*dots)[2], float r)
{
	/* print `<svg>` root element */
	printf("<svg version=\"1.1\" width=\"%d\" height=\"%d\" "
	       "xmlns=\"http://www.w3.org/2000/svg\">\n",
	       width, height);

	/* print the SVG code of dots */
	for (int i = 0; i < dot_count; ++i) {
		printf("<circle cx=\"%0.2f\" cy=\"%0.2f\" r=\"%0.2f\" "
		       "fill=\"black\"/>\n",
		       dots[i][0], dots[i][1], r);
	}

	/* print closing `</svg>` tag */
	fputs("</svg>\n", stdout);
}

/* return if dot should persist
 * based on a logistic curve */
bool persists(int value, int max)
{
	int r = rand() % max;
	return max / (1 + exp(-0.03 * (r - max / 2.0))) > value;
}

/* generate list of dot coordinates */
float (*gen_dots(struct gray_png *png, float dot_count))[2]
{
	/* save dot coordinates in VLA */
	float(*dots)[2] = malloc(dot_count * sizeof *dots);
	if (!dots) {
		fprintf(stderr, "Error: memory allocation failed\n");
		return NULL;
	}

	/* fill `dots` with coordinates */
	int i = 0;
	while (i < dot_count) {
		float y = rand() / ((float)RAND_MAX / png->height);
		float x = rand() / ((float)RAND_MAX / png->width);

		/* decide if dot should persist */
		int value = png->rows[(int)y][(int)x];
		bool persist = persists(value, (1 << png->depth));
		if (persist) {
			dots[i][0] = x;
			dots[i][1] = y;
			++i;
		}
	}

	return dots;
}

/* handle command-line arguments */
int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <png_in>\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* seed `rand()` */
	srand(time(NULL));

	/* read png file and convert it to grayscale */
	struct gray_png *png = read_png(argv[1]);
	if (!png)
		return EXIT_FAILURE;

	float r = 2;
	float(*dots)[2] = gen_dots(png, DOT_COUNT);
	print_svg(png->width, png->height, DOT_COUNT, dots, r);

	/* free allocated memory */
	free(dots);
	free_gray_png(png);
	return EXIT_SUCCESS;
}
