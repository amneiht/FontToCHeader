/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
extern void font_load(const char *fontname, const char *headername, int height,
		int width, int threshold);
int main(int argc, char **argv) {
	char *help_str =
			"usage: -f <font dir> -o <header output dir> -b <box size> -t <threshold level> \n"
					"\t-f : path to font \n"
					"\t-b: box size, font will draw in a square box \n"
					"\t-t: gray level threshold\t"
					"\t-o: output header \n"
					"\texample: ./fonttobitmap -f ubuntu_mono.tif -o /tmp/ubuntumono.h -b 24 -t 50\n";
	char *_optstr = "f:o:b:t:";
	char *fontname = NULL;
	char *headername = NULL;
	char *_tmph = "/tmp/fontheader.h";
	int threshold = 50, box;
	int f, c;
	while ((c = getopt(argc, argv, _optstr)) != -1) {
		switch (c) {
		case 'f':
			fontname = optarg;
			break;
		case 'o':
			headername = optarg;
			break;
		case 'b':
			f = atoi(optarg);
			if (f > 0)
				box = f;

			break;
			
		case 't':
			f = atoi(optarg);
			if (f > 0)
				threshold = f;
			break;
		default:
			printf("%s", help_str);
			exit(0);
		}
	}
	if (fontname == NULL) {
		printf(
				"you must decleare you font you want convert to bitmap header\n");
		exit(0);
	}
	if (headername == NULL) {
		headername = _tmph;
		printf("using default output to file %s\n", headername);

	}
	font_load(fontname, headername, box, box, threshold);
}

/* EOF */
