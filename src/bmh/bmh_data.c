/*
 * bmh_data.c
 *
 *  Created on: Nov 6, 2024
 *      Author: amneiht
 */

#include "bmh.h"
#include <stdlib.h>

void bmh_data_free(struct bmh_data *data) {
	free(data->buffer);
	free(data);
}
struct bmh_data* bmh_fromGlyph(FT_GlyphSlot slot) {

	struct bmh_data *data = malloc(sizeof(struct bmh_data));
	data->bitmap_left = slot->bitmap_left;
	data->bitmap_top = slot->bitmap_top;
	data->rows = slot->bitmap.rows;
	data->width = slot->bitmap.width;
	data->buffer = malloc(data->rows * data->width);
	memcpy(data->buffer, slot->bitmap.buffer, data->rows * data->width);
	return data;
}

void bmh_draw(struct bmh_data *data, int baseline_pos, struct bmh_map *map) {
	// clear map
	for (int i = 0; i < map->rows; i++) {
		for (int j = 0; j < map->width; j++) {
			map->buffer[i][j] = 0;
		}
	}
	int posy = baseline_pos - data->bitmap_top;
	int posx = data->bitmap_left;
	if (posx < 0)
		posx = 0;
	if (posy < 0)
		posy = 0;
	if (data->rows > map->rows) {
		printf("canot draw for %c, it over height\n", data->name);
		exit(-1);
	}
	if (data->width > map->width) {
		printf("canot draw for %c, it over widht\n", data->name);
		exit(-1);
	}
	for (int i = 0; i < data->rows; i++) {
		for (int j = 0; j < data->width; j++) {
			map->buffer[i + posy][j + posx] = data->buffer[i * data->width + j];
		}
	}
}

void bmh_map_show(struct bmh_map *map) {
	for (int i = 0; i < map->rows; i++) {
		for (int j = 0; j < map->width; j++) {
			if (map->buffer[i][j])
				printf("*");
			else
				printf(".");
		}
		printf("\n");
	}

}
struct bmh_map* bmh_map_create(int width, int row) {
	struct bmh_map *map = malloc(sizeof(struct bmh_map));
	map->rows = row;
	map->width = width;
	int psize = sizeof(char[row]);
	(void) psize;
	map->buffer = (unsigned char**) malloc(sizeof(char*) * row);
	for (int i = 0; i < row; i++) {
		map->buffer[i] = calloc(width, sizeof(char));
	}
	return map;
}

struct bmh_map* bmh_map_convertToBitMap(struct bmh_map *img, int threshold) {
	// little edian
	int size = bmh_size(img->width);
	unsigned int data = 0;
	unsigned short lwith;

	struct bmh_map *map = bmh_map_create(size, img->rows);
	for (int i = 0; i < map->rows; i++) {
		for (int j = 0; j < map->width; j++) {
			lwith = 0;
			for (int k = 0; k < 8; k++) {
				if (j * 8 + k < img->width) {
					data = img->buffer[i][j * 8 + k];
				} else {
					data = 0;
				}
				if (data >= threshold)
					data = 1;
				else
					data = 0;

				lwith = lwith * 2 + data;
			}
			map->buffer[i][j] = lwith;
		}
	}
	return map;
}

void bmh_map_free(struct bmh_map *map) {
	for (int i = 0; i < map->rows; i++) {
		char *lmap = (char*) map->buffer[i];
		free(lmap);
	}
	free(map->buffer);
	free(map);
}

