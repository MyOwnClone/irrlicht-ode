#ifndef HMAP_H
#define HMAP_H

#include "irrlicht.h"

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
public:
	HeightMap(u16 _w, u16 _h) : Width(_w), Height(_h), s(0.f), data(0)
	{
		s = sqrtf((f32)(Width * Width + Height * Height));

		data.set_used(Width * Height);
	}

	// Fill the heightmap with values generated from f.
	void generate(generate_func f)
	{
		u32 i=0;
		for(u16 y = 0; y < Height; ++y)
			for(u16 x = 0; x < Width; ++x)
				set(i++, calc(f, x, y));
	}

	u16 height() const { return Height; }
	u16 width() const { return Width; }

	f32 calc(generate_func f, u16 x, u16 y) const
	{
		const f32 xx = (f32)x;
		const f32 yy = (f32)y;
		return f((u16)xx, (u16)yy, s);
	}

	// The height at (x, y) is at position y * Width + x.

	void set(u16 x, u16 y, f32 z) { data[y * Width + x] = z; }
	void set(u32 i, f32 z) { data[i] = z; }
	f32 get(int x, int y) const { return data[y * Width + x]; }

	vector3df getnormal(u16 x, u16 y, f32 s) const
	{
		const f32 zc = get(x, y);
		f32 zl, zr, zu, zd;

		if (x == 0)
		{
			zr = get(x + 1, y);
			zl = zc + zc - zr;
		}
		else if (x == Width - 1)
		{
			zl = get(x - 1, y);
			zr = zc + zc - zl;
		}
		else
		{
			zr = get(x + 1, y);
			zl = get(x - 1, y);
		}

		if (y == 0)
		{
			zd = get(x, y + 1);
			zu = zc + zc - zd;
		}
		else if (y == Height - 1)
		{
			zu = get(x, y - 1);
			zd = zc + zc - zu;
		}
		else
		{
			zd = get(x, y + 1);
			zu = get(x, y - 1);
		}

		return vector3df(s * 2 * (zl - zr), 4, s * 2 * (zd - zu)).normalize();
	}
};

double ode_heightfield_callback( void* pUserData, int x, int z );

#endif