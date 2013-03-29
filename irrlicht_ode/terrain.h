#ifndef HMAPMESHGEN
#define HMAPMESHGEN

#include "perlinGenerator.h"
#include "irrlicht.h"
#include "heightMap.h"

extern pixmap *pixmapInstance;

using namespace irr;
using namespace video;
using namespace scene;

typedef irr::video::SColor colour_func(f32 x, f32 y, f32 z);


typedef f32 generate_func(s16 x, s16 y, f32 s);

class TerrainMeshGenerator
{
private:
	u16 Width;
	u16 Height;
	f32 Scale;
public:
	SMesh* Mesh;

	TerrainMeshGenerator() : Mesh(0), Width(0), Height(0), Scale(1.f)
	{
		Mesh = new SMesh();
	}

	~TerrainMeshGenerator()
	{
		Mesh->drop();
	}

	// Unless the heightmap is small, it won't all fit into a single
	// SMeshBuffer. This function chops it into pieces and generates a
	// buffer from each one.

	void init(HeightMap &hm, f32 scale, colour_func cf, IVideoDriver *driver)
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
	}

	// Generate a SMeshBuffer which represents all the vertices and
	// indices for values of y between y0 and y1, and add it to the
	// mesh.

	void addstrip(const HeightMap &hm, colour_func cf, u16 y0, u16 y1, u32 bufNum)
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
				const f32 z = hm.get(x, y);
				const f32 xx = (f32)x/(f32)Width;
				const f32 yy = (f32)y/(f32)Height;

				S3DVertex& v = buf->Vertices[i++];
				v.Pos.set(x, Scale * z, y);
				v.Normal.set(hm.getnormal(x, y, Scale));
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
};

SColor white_colour_func(f32 x, f32 y, f32 z);
irr::f32 GetHeightMapFunc(irr::s16 x, irr::s16 y, irr::f32 s);
double ode_heightfield_callback( void* pUserData, int x, int z );

extern int lineWidth;

#endif
