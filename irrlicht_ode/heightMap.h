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
	f32 s;
	core::array<f32> data;
	pixmap *pixmapInstance;

public:
	HeightMap(u16 _w, u16 _h) : Width(_w), Height(_h), s(0.f), data(0)
	{
		s = sqrtf((f32)(Width * Width + Height * Height));

		//data.set_used(Width * Height);
		pixmapInstance = GetHeightMap(_w, _h, 8);
	}

	// Fill the heightmap with values generated from f.
	//void Generate(generate_func f);

	u16 height() const { return Height; }
	u16 width() const { return Width; }

	//f32 Calc(generate_func f, u16 x, u16 y) const;

	// The height at (x, y) is at position y * Width + x.

	//void Set(u16 x, u16 y, f32 z) { data[y * Width + x] = z; }
	//void Set(u32 i, f32 z) { data[i] = z; }
	f32 Get(int x, int y) const { return (pixmapInstance->pixels[(x*3) + (y*Width*3)]/255.0); }

	vector3df GetNormal(u16 x, u16 y, f32 s) const;
};

double ode_heightfield_callback( void* pUserData, int x, int z );

#endif