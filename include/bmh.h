/*
 * bmh.h
 *
 *  Created on: Nov 6, 2024
 *      Author: amneiht
 */

#ifndef BMH_H_
#define BMH_H_

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#define bmh_size(size) ((size + 7) >> 3 )
#define BMH_NAME(name) (strrchr(name, '/') ? strrchr(name, '/') + 1 : name ) 
struct bmh_data {
	int bitmap_top, bitmap_left;
	int width, rows;
	char name;
	char *buffer;
};

struct bmh_map {
	int width, rows;
	unsigned char **buffer;
};

void bmh_data_free(struct bmh_data *data);
struct bmh_data* bmh_fromGlyph(FT_GlyphSlot slot);
// for map
void bmh_draw(struct bmh_data *data, int baseline_pos, struct bmh_map *map);
void bmh_map_show(struct bmh_map *map);
struct bmh_map* bmh_map_create(int width, int row);
void bmh_map_free(struct bmh_map *map);
struct bmh_map* bmh_map_convertToBitMap(struct bmh_map *img, int threshold);

#endif /* BMH_H_ */
