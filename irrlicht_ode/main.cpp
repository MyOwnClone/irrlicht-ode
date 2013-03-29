#include <irrlicht.h>
#include <ode/ode.h>

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

#define MAX_CONTACTS 16

struct PhysicsContext
{
	dWorldID world;
	dSpaceID space;
	dJointGroupID contactgroup;
	int sceneWidth;
	int sceneHeight;
};

struct PhysicalObject
{
	dBodyID body;
	dGeomID geometry;
};

struct GenericObject
{
	irr::core::vector3d<double> position;
	irr::core::vector3d<double> size;
};

struct VisibleObject
{
	irr::scene::ISceneNode *node;
};

struct PlaceableObject
{
	PhysicalObject physicalObject;
	GenericObject genericObject;
	VisibleObject visibleObject;
};

void SetupLightsAndShadows( ISceneManager* smgr, IVideoDriver* driver );
void AddActors( ISceneManager* smgr, IVideoDriver* driver, PhysicsContext &odeContext );
void AddGround( ISceneManager* smgr, IVideoDriver* driver, int width, int height );
void SetupCamera( ISceneManager* smgr, IrrlichtDevice * device ) ;
void SetupGui( IGUIEnvironment* guienv ) ;
void SetupOde(PhysicsContext &odeContext);
void SimulationStep(PhysicsContext &odeContext);
void nearCollisionCallback(void* data, dGeomID o1, dGeomID o2);
void QuaternionToEuler(const dQuaternion quaternion, vector3df &euler);
void AddOdeActor(PhysicsContext &context, PhysicalObject &physicalObject, core::vector3d<double> position, core::vector3d<double> size, dMatrix3 rotation, double density);
void UpdateActor(PhysicsContext &context, PlaceableObject &placeableObject);


int main()
{
	IrrlichtDevice *device = createDevice( video::EDT_OPENGL, dimension2d<u32>(1200, 800), 32, false, true, false, 0);
	PhysicsContext odeContext;

	odeContext.sceneWidth = 100;
	odeContext.sceneHeight = 100;
	SetupOde(odeContext);

	if (!device)
		return 1;

	device->setWindowCaption(L"Irrlicht Engine");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();
		
	SetupCamera(smgr, device);
	
	SetupGui(guienv);

	AddActors(smgr, driver, odeContext);
	AddGround(smgr, driver, odeContext.sceneWidth, odeContext.sceneHeight);
	SetupLightsAndShadows(smgr, driver);

	auto infotext = guienv->addStaticText(L"Irrlicht",	rect<s32>(10, 10, 200, 40), true);	

	while(device->run())
	{
		driver->beginScene(true, true, SColor(255, 0, 0, 0));

		smgr->drawAll();
		guienv->drawAll();
		driver->endScene();

		core::stringw strFps = L"FPS: ";
		strFps += (s32)driver->getFPS();       
		infotext->setText(strFps.c_str());

		SimulationStep(odeContext);

		/*auto oldRotation = cubeSceneNode->getRotation();
		cubeSceneNode->setRotation(vector3df(0.1, 0.1, 0.1)+oldRotation);*/
	}

	device->drop();

	return 0;
}

void SetupLightsAndShadows( ISceneManager* smgr, IVideoDriver* driver ) 
{
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
}

void AddActors( ISceneManager* smgr, IVideoDriver* driver, PhysicsContext &odeContext )
{
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
}

void AddGround( ISceneManager* smgr, IVideoDriver* driver, int width, int height )
{
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
}

void SetupCamera( ISceneManager* smgr, IrrlichtDevice * device ) 
{
	irr::scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(0, irr::core::vector3df(0, 50, 50), irr::core::vector3df(0, 0, 0));

	if (camera) {
		ControllableCamera *snaca = new ControllableCamera(device->getCursorControl(), 300, 10, 100, camera, 100);

		irr::scene::ISceneNodeAnimator* anim = dynamic_cast<irr::scene::ISceneNodeAnimator*>(snaca);
		camera->addAnimator(anim);

		anim->drop();
	}
}

void SetupGui( IGUIEnvironment* guienv ) 
{
	irr::gui::IGUISkin* skin = guienv->getSkin();

	irr::gui::IGUIFont* font = guienv->getFont("fonthaettenschweiler.bmp");
	if (font)
	{
		skin->setFont(font);
	}
	else
		skin->setFont(guienv->getBuiltInFont(), irr::gui::EGDF_TOOLTIP);

	skin->setColor(EGUI_DEFAULT_COLOR::EGDC_BUTTON_TEXT , SColor(255, 255, 255, 255));
}

void SetupOde(PhysicsContext &odeContext)
{
	dInitODE2(dInitFlagManualThreadCleanup);  	

	odeContext.world = dWorldCreate();

	//space = dSimpleSpaceCreate(0);
	odeContext.space = dHashSpaceCreate (0);

	odeContext.contactgroup = dJointGroupCreate(0);

	dCreatePlane(odeContext.space, 0, 1, 0, 0);

	dWorldSetGravity(odeContext.world, 0, -9, 0);

	dWorldSetERP(odeContext.world, 0.7);

	dWorldSetCFM(odeContext.world, 0.001f);//1e-2);

	dWorldSetContactMaxCorrectingVel(odeContext.world, 10);//10

	dWorldSetContactSurfaceLayer(odeContext.world, 0.001);
}

void SimulationStep(PhysicsContext &odeContext)
{
	dSpaceCollide(odeContext.space, &odeContext, &nearCollisionCallback);

	// make a simulation step for 'theWorld'
	dWorldStep(odeContext.world, 0.1f); 

	//optionally use dWorldStepFast1 instead of dWorldStep
	//dWorldStepFast1(theWorld,0.1,1);
	// clean up joints from collisions
	dJointGroupEmpty(odeContext.contactgroup);
}

void nearCollisionCallback(void* data, dGeomID o1, dGeomID o2)
{
	int i=0;

	PhysicsContext* odeContext = (PhysicsContext*) data;

	dBodyID b1=dGeomGetBody(o1);
	dBodyID b2=dGeomGetBody(o2);

	if(b1 && b2 && dAreConnectedExcluding(b1,b2,dJointTypeContact))
		return;

	dContact contact[MAX_CONTACTS];

	for(i=0;i<MAX_CONTACTS;i++){
		contact[i].surface.mode=dContactBounce | dContactSoftCFM;
		contact[i].surface.mu=dInfinity;
		contact[i].surface.mu2=0;
		contact[i].surface.bounce=1e-5f;
		contact[i].surface.bounce_vel=1e-9f;
		contact[i].surface.soft_cfm=1e-6f;
	}

	int numc=dCollide(o1,o2,MAX_CONTACTS,&contact[0].geom,sizeof(dContact));

	if(numc > 0)
	{			
		for(i = 0; i < numc; i++){		
			dJointID c = dJointCreateContact(odeContext->world, odeContext->contactgroup, &contact[i]);
			dJointAttach(c, b1, b2);
		}
	}
}

void QuaternionToEuler(const dQuaternion quaternion, vector3df &euler)
{
	dReal w,x,y,z;
	w=quaternion[0];
	x=quaternion[1];
	y=quaternion[2];
	z=quaternion[3];
	double sqw = w*w;    
	double sqx = x*x;    
	double sqy = y*y;    
	double sqz = z*z; 

	euler.Z = (irr::f32) (atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw))
		*irr::core::RADTODEG);

	euler.X = (irr::f32) (atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw))
		*irr::core::RADTODEG);  

	euler.Y = (irr::f32) (asin(-2.0 * (x*z - y*w))
		*irr::core::RADTODEG);
}

void AddOdeActor(PhysicsContext &context, PhysicalObject &physicalObject, core::vector3d<double> position, core::vector3d<double> size, dMatrix3 rotation, double density) 
{
	auto body = dBodyCreate(context.world);

	/*this->size = size;
	this->volume = (size.X * size.Y * size.Z);

	this->parent = NULL;*/

	dBodySetPosition(body, position.X, position.Y, position.Z);
	dBodySetLinearVel(body, 0, 0, 0);
	dBodySetRotation(body, rotation);

	//dBodySetData(this->body, (void*)(this));

	dMass mass;
	dReal sides[3];
	sides[0] = 2 * size.X;
	sides[1] = 2 * size.Y;
	sides[2] = 2 * size.Z;
	dMassSetBox(&mass, density, sides[0], sides[1], sides[2]);
	dBodySetMass(body, &mass);

	auto geometry = dCreateBox(context.space, sides[0], sides[1], sides[2]);
	dGeomSetBody(geometry, body);

	physicalObject.body = body;
	physicalObject.geometry = geometry;
}

void UpdateActor(PhysicsContext &context, PlaceableObject &placeableObject)
{
	dGeomID geom=placeableObject.physicalObject.geometry;
	irr::core::vector3df pos;
	irr::core::vector3df rot;

	if(geom!=0){
		// get the new position of the ODE geometry
		dReal* ode_pos = (dReal*) dGeomGetPosition(geom);

		// set the position at the scenenode
		pos.set((irr::f32)ode_pos[0],(irr::f32)ode_pos[1],(irr::f32)ode_pos[2]);

		placeableObject.visibleObject.node->setPosition(pos);

		// get the rotation quaternion
		dQuaternion result;
		dGeomGetQuaternion(geom, result);

		// convert it to eulerangles
		QuaternionToEuler(result,rot);

		// set the rotation 
		placeableObject.visibleObject.node->setRotation(rot);			
	}
}