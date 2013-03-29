#ifndef _PERLIN_GENERATOR
#define _PERLIN_GENERATOR

#pragma once


struct pixmap {                                // struktura popisujici pixmapu
	unsigned int width;
	unsigned int height;
	unsigned char *pixels;
};

pixmap * createPixmap(const unsigned int width, const unsigned int height);
void destroyPixmap(pixmap *p);
void putpixel(pixmap *p,
				const unsigned int x,             // pozice pixelu v pixmape
				const unsigned int y,
				const unsigned char r,            // barva pixelu
				const unsigned char g,
				const unsigned char b);

void mapPalette(int palette, int iter, char *r, char *g, char *b);

pixmap * GetHeightMap(int width, int height, int palette);

#endif