#include "heightMap.h"
#pragma once

vector3df HeightMap::GetNormal(u16 x, u16 y, f32 s) const
{
	const f32 zc = Get(x, y);
	f32 zl, zr, zu, zd;

	if (x == 0)
	{
		zr = Get(x + 1, y);
		zl = zc + zc - zr;
	}
	else if (x == Width - 1)
	{
		zl = Get(x - 1, y);
		zr = zc + zc - zl;
	}
	else
	{
		zr = Get(x + 1, y);
		zl = Get(x - 1, y);
	}

	if (y == 0)
	{
		zd = Get(x, y + 1);
		zu = zc + zc - zd;
	}
	else if (y == Height - 1)
	{
		zu = Get(x, y - 1);
		zd = zc + zc - zu;
	}
	else
	{
		zd = Get(x, y + 1);
		zu = Get(x, y - 1);
	}

	return vector3df(s * 2 * (zl - zr), 4, s * 2 * (zd - zu)).normalize();
}