#include <irrlicht.h>

#include "controllableCamera.h"

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

	if (!device)
		return 1;

	device->setWindowCaption(L"Irrlicht Engine");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	auto infotext = guienv->addStaticText(L"Irrlicht",	rect<s32>(10, 10, 200, 40), true);	

	irr::scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(0, irr::core::vector3df(0, 50, 50), irr::core::vector3df(0, 0, 0));

	if (camera) {
		ControllableCamera *snaca = new ControllableCamera(device->getCursorControl(), 300, 10, 100, camera, 100);

		irr::scene::ISceneNodeAnimator* anim = dynamic_cast<irr::scene::ISceneNodeAnimator*>(snaca);
		camera->addAnimator(anim);

		anim->drop();
	}

	irr::gui::IGUISkin* skin = guienv->getSkin();

	irr::gui::IGUIFont* font = guienv->getFont("fonthaettenschweiler.bmp");
	if (font)
	{
		skin->setFont(font);
	}
	else
		skin->setFont(guienv->getBuiltInFont(), irr::gui::EGDF_TOOLTIP);

	skin->setColor(EGUI_DEFAULT_COLOR::EGDC_BUTTON_TEXT , SColor(255, 255, 255, 255));

	scene::ISceneNode* cubeSceneNode = smgr->addMeshSceneNode(smgr->getGeometryCreator()->createCubeMesh());
	
	if (cubeSceneNode)
	{
		cubeSceneNode->setMaterialTexture(0, driver->getTexture("t351sml.jpg"));
		cubeSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);
		scene::ISceneNodeAnimator* anim = smgr->createFlyCircleAnimator(core::vector3df(0,30,0), 5.0f);
		if (anim)
		{
			cubeSceneNode->addAnimator(anim);
			anim->drop();
		}

		cubeSceneNode->setScale(vector3df(5, 5, 5));
		((IMeshSceneNode*)cubeSceneNode)->addShadowVolumeSceneNode();
	}

	scene::ISceneNode* groundSceneNode = smgr->addCubeSceneNode();

	auto textureScale = 4;

	if (groundSceneNode)
	{
		groundSceneNode->setMaterialTexture(0, driver->getTexture("sand.jpg"));
		groundSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);

		groundSceneNode->setMaterialFlag(video::EMF_TEXTURE_WRAP, false);
		groundSceneNode->getMaterial(0).getTextureMatrix(0).setScale(vector3df((irr::f32) textureScale, (irr::f32) textureScale, (irr::f32) textureScale));

		groundSceneNode->setScale(core::vector3df(100, 0, 100));
	}

	irr::f32 lightColorKoef = 1.0f;

	scene::ISceneNode*  lightNode = smgr->addLightSceneNode(0, core::vector3df(0,10,0), video::SColorf(lightColorKoef, lightColorKoef, lightColorKoef, 1.0f), 
		800.0f); // radius

	scene::ISceneNodeAnimator* animNode = NULL;
	animNode = smgr->createFlyCircleAnimator (core::vector3df(0,300,0), 200.0f);
	lightNode->addAnimator(animNode);
	animNode->drop();

	// attach billboard to light

	lightNode = smgr->addBillboardSceneNode(lightNode, core::dimension2d<f32>(50, 50));
	lightNode->setMaterialFlag(video::EMF_LIGHTING, false);
	lightNode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

	lightNode->setMaterialTexture(0, driver->getTexture("particlewhite.bmp"));

	smgr->setShadowColor(video::SColor(150,0,0,0));

	while(device->run())
	{
		driver->beginScene(true, true, SColor(255, 0, 0, 0));

		smgr->drawAll();
		guienv->drawAll();
		driver->endScene();

		core::stringw strFps = L"FPS: ";
		strFps += (s32)driver->getFPS();       
		infotext->setText(strFps.c_str());

		auto oldRotation = cubeSceneNode->getRotation();
		cubeSceneNode->setRotation(vector3df(0.1, 0.1, 0.1)+oldRotation);
	}

	device->drop();

	return 0;
}
