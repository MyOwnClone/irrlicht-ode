#include <irrlicht.h>
#include "terrain.h"
#include "perlinGenerator.h"
#include "perlinMath.h"

using namespace irr;
using namespace video;

pixmap *pixmapInstance;
int lineWidth;

pixmap * GetHeightMap( int width, int height, int palette )
{
	static pixmap * result = NULL;
	
	if (!result)
	{
		result = createPixmap(width, height);

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
	int result = (int) pixmapInstance->pixels[(x*3) + (y*lineWidth*3)];

	return ((result/(float)255));
}

double ode_heightfield_callback( void* pUserData, int x, int z )
{
	return GetHeightMapFunc(x, z, 0);
}

void TerrainMeshGenerator::initFromODE(int width, int depth, float wstep, float dstep, float wsamp, float dsamp, generate_func gen_func)
{
	SMeshBuffer *buf = 0;

	// create new buffer
	buf = new SMeshBuffer();
	Mesh->addMeshBuffer(buf);
	// to simplify things we drop here but continue using buf
	buf->drop();
	buf->Vertices.set_used(width * depth); 

	u32 i=0;
	for (u16 y = 0; y <= depth; ++y)
	{
		for (u16 x = 0; x < width; ++x)
		{
			const f32 z = gen_func(x, y, 1 );
			const f32 xx = (f32)x/(f32)Width;
			const f32 yy = (f32)y/(f32)Height;

			S3DVertex& v = buf->Vertices[i++];
			v.Pos.set(x, z , y);
			//v.Normal.set(hm.getnormal(x, y, Scale));
			//v.Color=cf(xx, yy, z);
			v.TCoords.set(xx, yy);
		}
	}

	buf->Indices.set_used(6 * (width - 1) * (depth));
	i=0;
	for(u16 y = 0; y < depth; ++y)
	{
		for(u16 x = 0; x < width - 1; ++x)
		{
			const u16 n = (depth) * Width + x;
			buf->Indices[i]=n;
			buf->Indices[++i]=n + Width;
			buf->Indices[++i]=n + Width + 1;
			buf->Indices[++i]=n + Width + 1;
			buf->Indices[++i]=n + 1;
			buf->Indices[++i]=n;
			++i;
		}
	}

	buf->recalculateBoundingBox();

	buf->recalculateBoundingBox();

	Mesh->setDirty();
	Mesh->recalculateBoundingBox();
}

// Unless the heightmap is small, it won't all fit into a single
// SMeshBuffer. This function chops it into pieces and generates a
// buffer from each one.

void TerrainMeshGenerator::init(pixmap *pixmapInstance, f32 scale, colour_func cf, IVideoDriver *driver)
{
	Scale = scale;

	const u32 mp = driver -> getMaximalPrimitiveCount();
	Width = pixmapInstance->width;
	Height = pixmapInstance->height;

	const u32 sw = mp / (6 * Height); // the width of each piece

	u32 i=0;
	for(u32 y0 = 0; y0 < Height; y0 += sw)
	{
		u16 y1 = y0 + sw;
		if (y1 >= Height)
			y1 = Height - 1; // the last one might be narrower
		addstrip(pixmapInstance, cf, y0, y1, i);
		++i;
	}
	if (i<Mesh->getMeshBufferCount())
	{
		// clear the rest
		for (u32 j=i; j<Mesh->getMeshBufferCount(); ++j)
		{
			Mesh->getMeshBuffer(j)->drop();
		}
		Mesh->MeshBuffers.erase(i,Mesh->getMeshBufferCount()-i);
	}
	// set dirty flag to make sure that hardware copies of this
	// buffer are also updated, see IMesh::setHardwareMappingHint
	Mesh->setDirty();
	Mesh->recalculateBoundingBox();
}

/*void TerrainMeshGenerator::init(HeightMap &hm, f32 scale, colour_func cf, IVideoDriver *driver)
{
	Scale = scale;

	const u32 mp = driver -> getMaximalPrimitiveCount();
	Width = hm.width();
	Height = hm.height();

	const u32 sw = mp / (6 * Height); // the width of each piece

	u32 i=0;
	for(u32 y0 = 0; y0 < Height; y0 += sw)
	{
		u16 y1 = y0 + sw;
		if (y1 >= Height)
			y1 = Height - 1; // the last one might be narrower
		addstrip(hm, cf, y0, y1, i);
		++i;
	}
	if (i<Mesh->getMeshBufferCount())
	{
		// clear the rest
		for (u32 j=i; j<Mesh->getMeshBufferCount(); ++j)
		{
			Mesh->getMeshBuffer(j)->drop();
		}
		Mesh->MeshBuffers.erase(i,Mesh->getMeshBufferCount()-i);
	}
	// set dirty flag to make sure that hardware copies of this
	// buffer are also updated, see IMesh::setHardwareMappingHint
	Mesh->setDirty();
	Mesh->recalculateBoundingBox();
}*/

// Generate a SMeshBuffer which represents all the vertices and
// indices for values of y between y0 and y1, and add it to the
// mesh.

/*void TerrainMeshGenerator::addstrip(const HeightMap &hm, colour_func cf, u16 y0, u16 y1, u32 bufNum)
{
	SMeshBuffer *buf = 0;
	if (bufNum<Mesh->getMeshBufferCount())
	{
		buf = (SMeshBuffer*)Mesh->getMeshBuffer(bufNum);
	}
	else
	{
		// create new buffer
		buf = new SMeshBuffer();
		Mesh->addMeshBuffer(buf);
		// to simplify things we drop here but continue using buf
		buf->drop();
	}
	buf->Vertices.set_used((1 + y1 - y0) * Width);

	u32 i=0;
	for (u16 y = y0; y <= y1; ++y)
	{
		for (u16 x = 0; x < Width; ++x)
		{
			const f32 z = hm.Get(x, y);
			const f32 xx = (f32)x/(f32)Width;
			const f32 yy = (f32)y/(f32)Height;

			S3DVertex& v = buf->Vertices[i++];
			v.Pos.set(x, Scale * z, y);
			v.Normal.set(hm.GetNormal(x, y, Scale));
			v.Color=cf(xx, yy, z);
			v.TCoords.set(xx, yy);
		}
	}

	buf->Indices.set_used(6 * (Width - 1) * (y1 - y0));
	i=0;
	for(u16 y = y0; y < y1; ++y)
	{
		for(u16 x = 0; x < Width - 1; ++x)
		{
			const u16 n = (y-y0) * Width + x;
			buf->Indices[i]=n;
			buf->Indices[++i]=n + Width;
			buf->Indices[++i]=n + Width + 1;
			buf->Indices[++i]=n + Width + 1;
			buf->Indices[++i]=n + 1;
			buf->Indices[++i]=n;
			++i;
		}
	}

	buf->recalculateBoundingBox();
}	*/

void TerrainMeshGenerator::addstrip(pixmap *pixmapInstance, colour_func cf, u16 y0, u16 y1, u32 bufNum)
{
	SMeshBuffer *buf = 0;
	if (bufNum<Mesh->getMeshBufferCount())
	{
		buf = (SMeshBuffer*)Mesh->getMeshBuffer(bufNum);
	}
	else
	{
		// create new buffer
		buf = new SMeshBuffer();
		Mesh->addMeshBuffer(buf);
		// to simplify things we drop here but continue using buf
		buf->drop();
	}
	buf->Vertices.set_used((1 + y1 - y0) * Width);

	u32 i=0;
	for (u16 y = y0; y <= y1; ++y)
	{
		for (u16 x = 0; x < Width; ++x)
		{
			const f32 z = pixmapInstance->pixels[(x*3) + (y*Width*3)]/255.0;
			const f32 xx = (f32)x/(f32)Width;
			const f32 yy = (f32)y/(f32)Height;

			S3DVertex& v = buf->Vertices[i++];
			v.Pos.set(x, Scale * z, y);

			v.Normal.set(GetNormal(x, y, Scale));

			v.Color=cf(xx, yy, z);
			v.TCoords.set(xx, yy);
		}
	}

	buf->Indices.set_used(6 * (Width - 1) * (y1 - y0));
	i=0;
	for(u16 y = y0; y < y1; ++y)
	{
		for(u16 x = 0; x < Width - 1; ++x)
		{
			const u16 n = (y-y0) * Width + x;
			buf->Indices[i]=n;
			buf->Indices[++i]=n + Width;
			buf->Indices[++i]=n + Width + 1;
			buf->Indices[++i]=n + Width + 1;
			buf->Indices[++i]=n + 1;
			buf->Indices[++i]=n;
			++i;
		}
	}

	buf->recalculateBoundingBox();
}	

core::vector3df TerrainMeshGenerator::GetNormal(u16 x, u16 y, f32 s) const
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

	return core::vector3df(s * 2 * (zl - zr), 4, s * 2 * (zd - zu)).normalize();
}