#include <irrlicht.h>
#include "perlinGenerator.h"
#include "perlinMath.h"

using namespace irr;
using namespace video;

pixmap *pixmapInstance;
int lineWidth;

pixmap * GetHeightMap( int width, int height, int palette )
{
	pixmap * result = createPixmap(width, height);

	int n = 30;
	int alpha = 2;
	int beta = 2;
	double mez = 3;

	init();
	double y1 = -mez;
	for (unsigned int y = 0; y < result->height; y++) {             // pro vsechny radky pixmapy
		double x1 = -mez;
		for (unsigned int x = 0; x < result->width; x++) {          // pro vsechny pixely na radku
			char r, g, b;
			x1 += ((mez)-(-mez)) / result->width;         // pocatecni hodnota x1
			auto z = PerlinNoise2D(x1, y1, alpha, beta, n); // vypocet sumove funkce
			int i = (int) (z*32.0+128);                       // aplikace zesileni a offsetu
			mapPalette(palette, i & 0xff, &r, &g, &b); // mapovani na barvovou paletu
			putpixel(result, x, y, r, g, b);       // a vytisteni pixelu
		}
		y1 += ((mez)-(-mez))/result->height;            // prechod na dalsi radek
	}

	return result;
}

int heightMapDivider = 100;
int heightMapHeightAddition = 0;

SColor white_colour_func(f32 x, f32 y, f32 z)
{
	irr::u32 grey =(irr::u32) (z*(heightMapDivider));

	return SColor(255, 255, 255, 255);
}

irr::f32 GetHeightMapFunc(irr::s16 x, irr::s16 y, irr::f32 s)
{
	//auto lineWidth = 100;
	int result = (int) pixmapInstance->pixels[(x*3) + (y*lineWidth*3)];

	return ((result/(float)heightMapDivider)+heightMapHeightAddition);
}

double ode_heightfield_callback( void* pUserData, int x, int z )
{
	return GetHeightMapFunc(x, z, 0);
}

