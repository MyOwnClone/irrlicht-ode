#pragma once
#include <ode/ode.h>
#include <irrlicht.h>
#include <vector>

#define MAX_CONTACTS 32

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
	irr::core::vector3df position;
	irr::core::vector3df size;
	irr::core::vector3df rotation;
	double density;
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

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

void SetupLightsAndShadows( ISceneManager* smgr, IVideoDriver* driver );
void AddGround( ISceneManager* smgr, IVideoDriver* driver, int width, int height );
void SetupCamera( ISceneManager* smgr, IrrlichtDevice * device ) ;
void SetupGui( IGUIEnvironment* guienv ) ;
void SetupOde(PhysicsContext &physicsContext);
void SimulationStep(PhysicsContext &physicsContext);
void nearCollisionCallback(void* data, dGeomID o1, dGeomID o2);
void QuaternionToEuler(const dQuaternion quaternion, vector3df &euler);
void AddOdeActor(PhysicsContext &physicsContext, PhysicalObject &physicalObject, core::vector3df position, core::vector3df size, core::vector3df rotation, double density);
void UpdateActor(PhysicsContext &physicsContext, PlaceableObject &placeableObject);
void AddActor(PhysicsContext &physicsContext, ISceneManager* smgr, IVideoDriver* driver, PlaceableObject &placeableObject);
void AddActors( ISceneManager* smgr, IVideoDriver* driver, PhysicsContext &physicsContext, std::vector<PlaceableObject> &objects);
void UpdateActors(PhysicsContext &physicsContext, std::vector<PlaceableObject> &objects);
double randFloat(double a, double b);