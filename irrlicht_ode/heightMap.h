#ifndef HMAP_H
#define HMAP_H

#include "irrlicht.h"
#include "perlinGenerator.h"

using namespace irr;
using namespace irr::core;

typedef f32 generate_func(s16 x, s16 y, f32 s);

class HeightMap
{
private:
	const u16 Width;
	const u16 Height;
	pixmap *pixmapInstance;

public:
	HeightMap(u16 _w, u16 _h) : Width(_w), Height(_h)
	{
		pixmapInstance = GetHeightMap(_w, _h, 8);
	}
	u16 height() const { return Height; }
	u16 width() const { return Width; }

	f32 Get(int x, int y) const { return (pixmapInstance->pixels[(x*3) + (y*Width*3)]/255.0); }

	vector3df GetNormal(u16 x, u16 y, f32 s) const;
};

double ode_heightfield_callback( void* pUserData, int x, int z );

#endif