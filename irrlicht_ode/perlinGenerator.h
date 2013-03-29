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
void clearPixmap(const pixmap *p);
void savePixmap(const pixmap *p, const char *fileName);
void putpixel(pixmap *p,
				const unsigned int x,             // pozice pixelu v pixmape
				const unsigned int y,
				const unsigned char r,            // barva pixelu
				const unsigned char g,
				const unsigned char b);

void mapPalette(int palette, int iter, char *r, char *g, char *b);
void recalcPerlinNoise2D(pixmap *pix,           // pracovni pixmapa
							int    palette,             // cislo barvove palety
							double xmin,                // meze fraktalu v rovine
							double ymin,
							double xmax,
							double ymax,
							double alpha,               // koeficient alpha
							double beta,                // koeficient beta
							int    n)  ;

pixmap * GetHeightMap(int width, int height, int palette);

#endif