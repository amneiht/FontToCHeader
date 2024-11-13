/*
 * ft_face.c
 *
 *  Created on: Nov 11, 2024
 *      Author: amneiht
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <freetype2/ft2build.h>
#include "bmh.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BBOX_H

static int gen_header(const char *filename, const char *font_name,
		struct bmh_data *data[], int size, int width, int height, int threshold) {

	// calcuate base line position
	int bottom_pos = 0;
	int i, j, k, f = 0;
	int cwidth = 0;
	for (i = 0; i < size; i++) {
		int sb = data[i]->rows - data[i]->bitmap_top;
		if (sb > bottom_pos)
			bottom_pos = sb;
		if (data[i]->width +data[i]->bitmap_left > cwidth) {
			cwidth = data[i]->width + data[i]->bitmap_left;
		}
		// todo handle size
	}

	int baseline_pos = height - bottom_pos;
	width = (cwidth + 7) & 0xFFF8;
	printf("create font with baseline_pos %d and max width %d\n", baseline_pos,
			width);

	struct bmh_map *map = bmh_map_create(width, height);
	FILE *header = fopen(filename, "w");
	if (header == NULL) {
		printf("Cant not open %s", filename);
		return -1;
	}
	// header declear
	
	
	fprintf(header, "#ifndef _FONT_HEADER_H_\n");
	fprintf(header, "#define _FONT_HEADER_H_\n");
	fprintf(header, "\n");
	fprintf(header, "/*\n\n");
	fprintf(header, " bitmap font generate , author:amneiht \n");
	fprintf(header, " font name:             %s\n", BMH_NAME(font_name));
	fprintf(header, " threshold level:       %d\n", threshold);
	fprintf(header, " box size:              %d\n", height);
	fprintf(header, "\n*/");
	fprintf(header, "\n");
	fprintf(header, "#define font_w  %d  // size in byte\n", bmh_size(width));
	fprintf(header, "#define font_h  %d  \n", height);

	fprintf(header, "unsigned char char_map[%d][%d][%d] = {   \n", size, height,
			width / 8);
	for (i = 0; i < size; i++) {
		fprintf(header, "\t//charactor \"%c\"\n", data[i]->name);
		// draw to image
		bmh_draw(data[i], baseline_pos, map);
		/// convert to bitmap mode
		struct bmh_map *bitmap = bmh_map_convertToBitMap(map, threshold);
		fprintf(header, "{\n");
		for (j = 0; j < bitmap->rows; j++) {
			fprintf(header, "\t{ ");

			for (k = 0; k < bitmap->width - 1; k++) {
				fprintf(header, "%3d ,", bitmap->buffer[j][k]);
			}
			fprintf(header, "%3d,", bitmap->buffer[j][k]);
			fprintf(header, " },\t//");
			// for data show
			for (f = 0; f < map->width; f++) {
				if (map->buffer[j][f] == 0) {
					fprintf(header, ".");
				} else {
					fprintf(header, "*");
				}
			}
			fprintf(header, "\n");
		}
		fprintf(header, "},\n");
		bmh_map_free(bitmap);
	}
	fprintf(header, "};\n");
	// end decleare
	fprintf(header, "#endif //_FONT_HEADER_H_\n");
	fclose(header);
	bmh_map_free(map);
	printf("complete create header\n");
	return baseline_pos;
}

void font_load(const char *fontname, const char *headername, int height,
		int width, int threshold) {
	FT_Library library;
	FT_Face face;

	FT_GlyphSlot slot;
	FT_Error error;
	FT_UInt glyph_index;

	error = FT_Init_FreeType(&library); /* initialize library */
	/* error handling omitted */
	error = FT_New_Face(library, fontname, 0, &face);/* create face object */
	/* error handling omitted */

	if (error) {
		printf("can not load font:%s\n", fontname);
		return;
	}
	error = FT_Set_Pixel_Sizes(face, width, height);
	//	error = FT_Set_Char_Size(face, width * 64, height * 64, 96, 96);
	struct bmh_map *map = bmh_map_create(width, height);
	// accsii
	// all display accsii size
	int bsize = '~' - ' ' + 1;
	struct bmh_data *data[bsize];

	for (int n = ' '; n <= '~'; n++) {
		/* load glyph image into the slot (erase previous one) */
		glyph_index = FT_Get_Char_Index(face, n);
		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

		/* convert to an anti-aliased bitmap */
		error |= FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		slot = face->glyph;

		if (error) {
			printf("cant not decode char %c\n", n);
			exit(1);
		}

		struct bmh_data *img = bmh_fromGlyph(slot);
		img->name = n;
		data[n - ' '] = img;

	}

	gen_header(headername, fontname, data, bsize, width, height, threshold);
	// free data
	bmh_map_free(map);
	for (int i = 0; i < bsize; i++) {
		bmh_data_free(data[i]);
	}
	FT_Done_Face(face);
	FT_Done_FreeType(library);

}
