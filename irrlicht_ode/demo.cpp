#include "demo.h"
#include <irrlicht.h>
#include "controllableCamera.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

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
		groundSceneNode->setPosition(core::vector3df(0, 0.0, 0));
	}
}

void SetupCamera( ISceneManager* smgr, IrrlichtDevice * device ) 
{
	irr::scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(0, irr::core::vector3df(0, 50, 50), irr::core::vector3df(0, 0, 0));

	if (camera) {
		ControllableCamera *snaca = new ControllableCamera(device->getCursorControl(), 300, 10, 100, camera, 200);

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

void SetupOde(PhysicsContext &physicsContext)
{
	dInitODE2(dInitFlagManualThreadCleanup);  	

	physicsContext.world = dWorldCreate();

	//space = dSimpleSpaceCreate(0);
	physicsContext.space = dHashSpaceCreate (0);

	physicsContext.contactgroup = dJointGroupCreate(0);

	//dCreatePlane(physicsContext.space, 0, 1, 0, 0);

	dWorldSetGravity(physicsContext.world, 0, -9, 0);

	dWorldSetERP(physicsContext.world, 0.7);

	dWorldSetCFM(physicsContext.world, 0.001f);//1e-2);

	dWorldSetContactMaxCorrectingVel(physicsContext.world, 10);//10

	dWorldSetContactSurfaceLayer(physicsContext.world, 0.001);
}

void SimulationStep(PhysicsContext &physicsContext)
{
	dSpaceCollide(physicsContext.space, &physicsContext, &nearCollisionCallback);

	// make a simulation step for 'theWorld'
	dWorldStep(physicsContext.world, 0.1f); 

	//optionally use dWorldStepFast1 instead of dWorldStep
	//dWorldStepFast1(theWorld,0.1,1);
	// clean up joints from collisions
	dJointGroupEmpty(physicsContext.contactgroup);
}

void nearCollisionCallback(void* data, dGeomID o1, dGeomID o2)
{
	int i=0;

	PhysicsContext* odeContext = (PhysicsContext*) data;

	dBodyID b1=dGeomGetBody(o1);
	dBodyID b2=dGeomGetBody(o2);

	/*if(b1 && b2 && dAreConnectedExcluding(b1,b2,dJointTypeContact))
		return;*/

	dContact contact[MAX_CONTACTS];

	for(i=0;i<MAX_CONTACTS;i++){
		contact[i].surface.mode=dContactBounce | dContactSoftCFM;
		contact[i].surface.mu=dInfinity;
		contact[i].surface.mu2=0;
		contact[i].surface.bounce=1e-5f;
		contact[i].surface.bounce_vel=1e-9f;
		contact[i].surface.soft_cfm=1e-6f;
	}

	auto class1 = dGeomGetClass(o1);
	auto class2 = dGeomGetClass(o2);

	int numc=dCollide(o1,o2,MAX_CONTACTS,&contact[0].geom,sizeof(dContact));

	if(numc > 0)
	{			
		for(i = 0; i < numc; i++){		
			dJointID c = dJointCreateContact(odeContext->world, odeContext->contactgroup, &contact[i]);
			dJointAttach(c, b1, b2);
		}
	}
}

void nearCollisionCallback2(void *data, dGeomID o1, dGeomID o2)
{
	int numc;
	dBodyID b1;
	dBodyID b2;
	int floor;

	/* Create an array of dContact objects to hold the contact joints */
	dContact contact[MAX_CONTACTS];

	PhysicsContext* odeContext = (PhysicsContext*) data;

	/* Temporary index for each contact */
	int i;

	/* Get the dynamics body for each geom */
	b1 = dGeomGetBody(o1);
	b2 = dGeomGetBody(o2);

	dReal zero[3] = {0,0,0};
	dReal* linVel1, *linVel2;
	dReal lv[3];
	if( b1 == 0 )
		linVel1 = zero;
	else
		linVel1 = (dReal*)dBodyGetLinearVel  (b1);

	if( b2 == 0 )
		linVel2 = zero;
	else
		linVel2 = (dReal*)dBodyGetLinearVel  (b2);

	lv[0] = linVel1[0]-linVel2[0];
	lv[1] = linVel1[1]-linVel2[1];
	lv[2] = linVel1[2]-linVel2[2];

	double lvf = sqrt(lv[0]*lv[0] + lv[1]*lv[1] + lv[2]*lv[2]);
	double p = pow(2,(lvf/10)) -4;

	if( p < 0 ) p = 0;
				
	floor = 0;
	if( dGeomGetClass(o1) == dPlaneClass || dGeomGetClass(o2) == dPlaneClass )
		floor = 1;

	/*if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) 
		return;*/
	
	/* Now we set the joint properties of each contact. Going into the
	full details here would require a tutorial of its own. I'll just
	say that the members of the dContact structure control the joint
	behaviour, such as friction, velocity and bounciness. See section
	7.3.7 of the ODE manual and have fun experimenting to learn
	more. */
	for (i = 0; i < MAX_CONTACTS; i++)
	{
		contact[i].surface.mode = dContactBounce | dContactSoftCFM;
		contact[i].surface.mu = dInfinity;
		contact[i].surface.mu2 = 0;
		/*contact[i].surface.bounce = 0.0001f;
		contact[i].surface.bounce_vel = 0.001f;
		contact[i].surface.soft_cfm = 0.001f;*/
		contact[i].surface.bounce=1e-5f;
		contact[i].surface.bounce_vel=1e-9f;
		contact[i].surface.soft_cfm=1e-6f;
	}

	/* Here we do the actual collision test by calling dCollide. It
	returns the number of actual contact points or zero if there were
	none. As well as the geom IDs, max number of contacts we also
	pass the address of a dContactGeom as the fourth
	parameter. dContactGeom is a substructure of a dContact object so
	we simply pass the address of the first dContactGeom from our
	array of dContact objects and then pass the offset to the next
	dContactGeom as the fifth paramater, which is the size of a
	dContact structure. That made sense didn't it? */

	auto o1geomClass = dGeomGetClass(o1);
	auto o2geomClass = dGeomGetClass(o2);

	/*if (o1geomClass == dHeightfieldClass || o2geomClass == dHeightfieldClass)
		return;*/

	if ( (numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact))) )
	{
		/* To add each contact point found to our joint group we call
		dJointCreateContact which is just one of the many different
		joint types available. */
		for (i = 0; i < numc; i++)
		{
			/* dJointCreateContact needs to know which world and joint
			group to work with as well as the dContact object
			itself. It returns a new dJointID which we then use with
			dJointAttach to finally create the temporary contact
			joint between the two geom bodies. */
			
			//if( !(dGeomGetClass(o1) == dBoxClass && dGeomGetClass(o2) == dBoxClass ))
			{
				dJointID c;
				
				c = dJointCreateContact(odeContext->world, odeContext->contactgroup, contact + i);
				dJointAttach(c, b1, b2);
			}

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

void AddOdeActor( PhysicsContext &physicsContext, PhysicalObject &physicalObject, GenericObject &genericObject )
{
	auto body = dBodyCreate(physicsContext.world);

	dBodySetPosition(body, genericObject.position.X, genericObject.position.Y, genericObject.position.Z);
	dBodySetLinearVel(body, 0, 0, 0);

	//---
	dMatrix3 A, B, C, I, Rx, Ry, Rz;

	//set up rotation matrices
	dRFromAxisAndAngle(Rz, 0, 0, 1, genericObject.rotation.Z);
	dRFromAxisAndAngle(Ry, 0, 1, 0, genericObject.rotation.Y);
	dRFromAxisAndAngle(Rx, 1, 0, 0, genericObject.rotation.X);

	// matrix for rotation around x
	dRFromAxisAndAngle(I, 1, 0, 0, 0);

	dMultiply0(A, I, Rx, 3, 3, 3);
	dMultiply0(B, A, Ry, 3, 3, 3);
	dMultiply0(C, B, Rz, 3, 3, 3);

	// set final rotation of body
	dBodySetRotation(body, C);
	//---

	//dBodySetData(this->body, (void*)(this));

	dMass mass;
	dReal sides[3];
	sides[0] = 2 * genericObject.size.X;
	sides[1] = 2 * genericObject.size.Y;
	sides[2] = 2 * genericObject.size.Z;
	dMassSetBox(&mass, genericObject.density, sides[0], sides[1], sides[2]);
	dBodySetMass(body, &mass);

	auto geometry = dCreateBox(physicsContext.space, sides[0], sides[1], sides[2]);
	dGeomSetBody(geometry, body);

	physicalObject.body = body;
	physicalObject.geometry = geometry;
}

void UpdateActor( PlaceableObject &placeableObject )
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

double randFloat(double a, double b)
{
	return ((b-a)*((double)rand()/RAND_MAX))+a;
}

void AddActor(PhysicsContext &physicsContext, ISceneManager* smgr, IVideoDriver* driver, PlaceableObject &placeableObject)
{
	AddOdeActor(physicsContext, placeableObject.physicalObject, placeableObject.genericObject);

	placeableObject.visibleObject.node = smgr->addMeshSceneNode(smgr->getGeometryCreator()->createCubeMesh());

	if (placeableObject.visibleObject.node)
	{
		placeableObject.visibleObject.node->setMaterialTexture(0, driver->getTexture("t351sml.jpg"));
		placeableObject.visibleObject.node->setMaterialFlag(video::EMF_LIGHTING, false);

		placeableObject.visibleObject.node->setPosition(placeableObject.genericObject.position);
		placeableObject.visibleObject.node->setScale(vector3df(placeableObject.genericObject.size.X/2, placeableObject.genericObject.size.Y/2, placeableObject.genericObject.size.Z/2));
		((IMeshSceneNode*)placeableObject.visibleObject.node)->addShadowVolumeSceneNode();
	}
}

void AddActors( ISceneManager* smgr, IVideoDriver* driver, PhysicsContext &physicsContext, std::vector<PlaceableObject> &objects)
{
	auto width = physicsContext.sceneWidth;
	auto height = physicsContext.sceneHeight;

	for (int i = 0; i < 200; i++)
	{
		auto y = randFloat(50, 350);
		auto x = randFloat(-width, width);
		auto z = randFloat(-height, height);

		auto sizeKoef = randFloat(1, 5);

		auto size = vector3df(sizeKoef, sizeKoef, sizeKoef);

		PlaceableObject po;
		po.genericObject.position = vector3df(x, y, z);
		po.genericObject.size = size;
		po.genericObject.density = 0.1;

		po.genericObject.rotation = vector3df(randFloat(0, 1), randFloat(0, 1), randFloat(0, 1));

		AddActor(physicsContext, smgr, driver, po);

		objects.push_back(po);
	}
}

void UpdateActors( std::vector<PlaceableObject> &objects )
{
	for (int i = 0; i < objects.size(); i++)
	{
		UpdateActor(objects[i]);
	}
}