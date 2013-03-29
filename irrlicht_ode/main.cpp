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

	odeContext.sceneWidth = 256;
	odeContext.sceneHeight = 256;

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
	SetupLightsAndShadows(smgr, driver);

	//----------------
	auto scale = 80;

	AddTerrain(odeContext, driver, smgr, scale);

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