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

SColor white_colour_func(f32 x, f32 y, f32 z);
irr::f32 GetHeightMapFunc(irr::s16 x, irr::s16 y, irr::f32 s);
double ode_heightfield_callback( void* pUserData, int x, int z );

extern int lineWidth;

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

	void initFromODE(int width, int depth, float wstep, float dstep, float wsamp, float dsamp, generate_func gen_func);

	// Unless the heightmap is small, it won't all fit into a single
	// SMeshBuffer. This function chops it into pieces and generates a
	// buffer from each one.

	void init(HeightMap &hm, f32 scale, colour_func cf, IVideoDriver *driver);

	// Generate a SMeshBuffer which represents all the vertices and
	// indices for values of y between y0 and y1, and add it to the
	// mesh.

	void addstrip(const HeightMap &hm, colour_func cf, u16 y0, u16 y1, u32 bufNum);
};

#endif
