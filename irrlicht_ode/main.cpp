#include <irrlicht.h>
#include <vector>

#include "controllableCamera.h"
#include "demo.h"
#include "terrain.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

int main()
{
	IrrlichtDevice *device = createDevice( video::EDT_OPENGL, dimension2d<u32>(1200, 800), 32, false, true, false, 0);
	PhysicsContext odeContext;
	std::vector<PlaceableObject> objects;

	odeContext.sceneWidth = 100;
	odeContext.sceneHeight = 100;

	lineWidth = odeContext.sceneWidth;

	SetupOde(odeContext);

	if (!device)
		return 1;

	device->setWindowCaption(L"Irrlicht Engine");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();
		
	SetupCamera(smgr, device);
	
	SetupGui(guienv);

	AddActors(smgr, driver, odeContext, objects );
	//AddGround(smgr, driver, odeContext.sceneWidth, odeContext.sceneHeight);
	SetupLightsAndShadows(smgr, driver);

	//----------------
	auto scale = 1;

	TerrainMeshGenerator terrainMesh;

	auto heightMap = new HeightMap(odeContext.sceneWidth, odeContext.sceneHeight);
	pixmapInstance = GetHeightMap(odeContext.sceneWidth, odeContext.sceneHeight, 8);
	heightMap->generate(GetHeightMapFunc);
	terrainMesh.init(*heightMap, scale, white_colour_func, driver);

	auto terrainMeshnode = smgr->addMeshSceneNode(terrainMesh.Mesh);
	terrainMeshnode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);    
	terrainMeshnode->setPosition(vector3df(-odeContext.sceneWidth/2, 0, -odeContext.sceneHeight/2));
	//terrainMeshnode->setScale(vector3df(10, 1, 10));
	terrainMeshnode->setMaterialTexture(0, driver->getTexture("sand.jpg"));

	auto textureScale = 4;
	terrainMeshnode->setMaterialFlag(video::EMF_TEXTURE_WRAP, false);
	terrainMeshnode->getMaterial(0).getTextureMatrix(0).setScale(vector3df((irr::f32) textureScale, (irr::f32) textureScale, (irr::f32) textureScale));
	

	dHeightfieldDataID heightid = dGeomHeightfieldDataCreate();
	
	const u32 widthSamples = 2; // the width of each piece
	const u32 depthSamples = 2; // the width of each piece

	// Our heightfield geom

	dGeomHeightfieldDataBuildCallback( heightid, NULL, ode_heightfield_callback, odeContext.sceneWidth, odeContext.sceneHeight, widthSamples, depthSamples, 
		REAL( scale), 
		REAL( 0.0 ), // offset
		REAL( 0.0 ), // thickness
		0 ); // wrap?

	// Give some very bounds which, while conservative,
	// makes AABB computation more accurate than +/-INF.
	dGeomHeightfieldDataSetBounds( heightid, REAL( -300.0 ), REAL( +300.0 ) );
	dGeomID gheight = dCreateHeightfield( odeContext.space, heightid, 1 );

	dVector3 pos;
	pos[ 0 ] = 0;
	pos[ 1 ] = 0;
	pos[ 2 ] = 0;			

	dGeomSetPosition( gheight, pos[0], pos[1], pos[2] );

	//----------------

	auto infotext = guienv->addStaticText(L"Irrlicht",	rect<s32>(10, 10, 200, 40), true);	

	auto frameCounter = 0;

	while(device->run())
	{
		driver->beginScene(true, true, SColor(255, 0, 0, 0));

		smgr->drawAll();
		guienv->drawAll();
		driver->endScene();

		core::stringw strFps = L"FPS: ";
		strFps += (s32)driver->getFPS();       
		infotext->setText(strFps.c_str());

		if (frameCounter % 5 == 0)
		{
			SimulationStep(odeContext);
			UpdateActors(objects);
		}

		frameCounter++;
	}

	device->drop();

	return 0;
}