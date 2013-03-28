#include "controllableCamera.h"


//! constructor
ControllableCamera::ControllableCamera( gui::ICursorControl* cursor, f32 rotateSpeed, f32 zoomSpeed, f32 translationSpeed, ICameraSceneNode *oldcam, double currentZoom ) : CursorControl(cursor), Zooming(false), Rotating(false), Moving(false),
	Translating(false), ZoomSpeed(zoomSpeed), RotateSpeed(rotateSpeed), TranslateSpeed(translationSpeed),
	CurrentZoom(currentZoom), RotX(0.0f), RotY(45.0f), OldCamera(oldcam), MousePos(0.5f, 0.5f), MouseWheel(0)
{
	if (CursorControl)
	{
		CursorControl->grab();
		MousePos = CursorControl->getRelativePosition();
	}

	allKeysUp();
}


//! destructor
ControllableCamera::~ControllableCamera()
{
	if (CursorControl)
		CursorControl->drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
bool ControllableCamera::OnEvent(const SEvent& event)
{
	if(event.EventType == EET_KEY_INPUT_EVENT)
	{
		KeyboardKeys[event.KeyInput.Key] = event.KeyInput.PressedDown;
	}
	else if(event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		MouseWheel = event.MouseInput.Wheel;
		if(KeyboardKeys[KEY_RSHIFT]||KeyboardKeys[KEY_LSHIFT])
			CurrentZoom += (-MouseWheel)*ZoomSpeed/10.0f;
		else
			CurrentZoom += (-MouseWheel)*ZoomSpeed;

		if(CurrentZoom<0)CurrentZoom=0;
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
			MouseKeys[0] = true;
			break;
		case EMIE_RMOUSE_PRESSED_DOWN:
			MouseKeys[2] = true;
			break;
		case EMIE_MMOUSE_PRESSED_DOWN:
			MouseKeys[1] = true;
			break;
		case EMIE_LMOUSE_LEFT_UP:
			MouseKeys[0] = false;
			break;
		case EMIE_RMOUSE_LEFT_UP:
			MouseKeys[2] = false;
			break;
		case EMIE_MMOUSE_LEFT_UP:
			MouseKeys[1] = false;
			break;
		case EMIE_MOUSE_MOVED:
			MousePos = CursorControl->getRelativePosition();
			break;
		case EMIE_MOUSE_WHEEL:
		case EMIE_LMOUSE_DOUBLE_CLICK:
		case EMIE_RMOUSE_DOUBLE_CLICK:
		case EMIE_MMOUSE_DOUBLE_CLICK:
		case EMIE_LMOUSE_TRIPLE_CLICK:
		case EMIE_RMOUSE_TRIPLE_CLICK:
		case EMIE_MMOUSE_TRIPLE_CLICK:
		case EMIE_COUNT:
			return false;
		}
	}
	return true;
}


//! OnAnimate() is called just before rendering the whole scene.
//! nodes may calculate or store animations here, and may do other useful things,
//! dependent on what they are.
void ControllableCamera::animateNode(ISceneNode *node, u32 timeMs)
{
	//Alt + LM = Rotate around camera pivot
	//Alt + LM + MM = Dolly forth/back in view direction (speed % distance camera pivot - max distance to pivot)
	//Alt + MM = Move on camera plane (Screen center is about the mouse pointer, depending on move speed)

	if (!node || node->getType() != ESNT_CAMERA)
		return;

	ICameraSceneNode* camera = static_cast<ICameraSceneNode*>(node);

	// If the camera isn't the active camera, and receiving input, then don't process it.
	if(!camera->isInputReceiverEnabled())
		return;

	scene::ISceneManager * smgr = camera->getSceneManager();
	if(smgr && smgr->getActiveCamera() != camera)
		return;

	if (OldCamera != camera)
	{
		OldTarget = camera->getTarget();
		OldCamera = camera;
		LastCameraTarget = OldTarget;
	}
	else
	{
		OldTarget += camera->getTarget() - LastCameraTarget;
	}

	core::vector3df target = camera->getTarget();
	f32 nRotX = RotX;
	f32 nRotY = RotY;
	f32 nZoom = CurrentZoom;

	// Translation ---------------------------------

	core::vector3df translate(OldTarget), upVector(camera->getUpVector());

	core::vector3df tvectX = Pos - target;
	tvectX = tvectX.crossProduct(upVector);
	tvectX.normalize();

	const SViewFrustum* const va = camera->getViewFrustum();
	core::vector3df tvectY = (va->getFarLeftDown() - va->getFarRightDown());
	tvectY = tvectY.crossProduct(upVector.Y > 0 ? Pos - target : target - Pos);
	tvectY.normalize();

	if (MouseKeys[2])
	{
		if (!Translating)
		{
			TranslateStart = MousePos;
			Translating = true;
		}
		else
		{
			translate +=  tvectX * (TranslateStart.X - MousePos.X)*TranslateSpeed +
				tvectY * (TranslateStart.Y - MousePos.Y)*TranslateSpeed;
		}
	}
	else if (Translating)
	{
		translate += tvectX * (TranslateStart.X - MousePos.X)*TranslateSpeed +
			tvectY * (TranslateStart.Y - MousePos.Y)*TranslateSpeed;
		OldTarget = translate;
		Translating = false;
	}

	// Rotation ------------------------------------

	if (MouseKeys[0])
	{
		if (!Rotating)
		{
			RotateStart = MousePos;
			Rotating = true;
			nRotX = RotX;
			nRotY = RotY;
		}
		else
		{
			nRotX += (RotateStart.X - MousePos.X) * RotateSpeed;
			nRotY += (RotateStart.Y - MousePos.Y) * RotateSpeed;
		}
	}
	else if (Rotating)
	{
		RotX += (RotateStart.X - MousePos.X) * RotateSpeed;
		RotY += (RotateStart.Y - MousePos.Y) * RotateSpeed;
		nRotX = RotX;
		nRotY = RotY;
		Rotating = false;
	}

	// Set Pos ------------------------------------

	target = translate;

	Pos.X = nZoom + target.X;
	Pos.Y = target.Y;
	Pos.Z = target.Z;

	Pos.rotateXYBy(nRotY, target);
	Pos.rotateXZBy(-nRotX, target);

	// Rotation Error ----------------------------

	// jox: fixed bug: jitter when rotating to the top and bottom of y
	upVector.set(0,1,0);
	upVector.rotateXYBy(-nRotY);
	upVector.rotateXZBy(-nRotX+180.f);

	camera->setPosition(Pos);
	camera->setTarget(target);
	camera->setUpVector(upVector);
	LastCameraTarget = camera->getTarget();
}

void ControllableCamera::allKeysUp()
{
	for (s32 i=0; i<3; ++i)
		MouseKeys[i] = false;
	for (s32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
		KeyboardKeys[i] = false;
}


//! Sets the rotation speed
void ControllableCamera::setRotateSpeed(f32 speed)
{
	RotateSpeed = speed;
}


//! Sets the movement speed
void ControllableCamera::setMoveSpeed(f32 speed)
{
	TranslateSpeed = speed;
}


//! Sets the zoom speed
void ControllableCamera::setZoomSpeed(f32 speed)
{
	ZoomSpeed = speed;
}


//! Gets the rotation speed
f32 ControllableCamera::getRotateSpeed() const
{
	return RotateSpeed;
}


// Gets the movement speed
f32 ControllableCamera::getMoveSpeed() const
{
	return TranslateSpeed;
}


//! Gets the zoom speed
f32 ControllableCamera::getZoomSpeed() const
{
	return ZoomSpeed;
}

ISceneNodeAnimator* ControllableCamera::createClone(ISceneNode* node, ISceneManager* newManager)
{
	ControllableCamera * newAnimator =
		new ControllableCamera(CursorControl, RotateSpeed, ZoomSpeed, TranslateSpeed, OldCamera, 100);
	return newAnimator;
}

ICameraSceneNode* ControllableCamera::addControllableCamera( ISceneManager* smgr,
															ICursorControl* cursor, ISceneNode* parent /*= 0*/, f32 rotateSpeed /*= -1500.0f*/,
															f32 zoomSpeed /*= 200.0f*/, f32 translationSpeed /*= 1500.0f*/, s32 id/*=-1*/,
															bool makeActive/*=true*/ )
{
	ICameraSceneNode* node = smgr->addCameraSceneNode(parent, core::vector3df(),
		core::vector3df(0,0,100), id, makeActive);
	if (node)
	{
		ISceneNodeAnimator* anm = new ControllableCamera(cursor,
			rotateSpeed, zoomSpeed, translationSpeed, NULL, 100);

		node->addAnimator(anm);
		anm->drop();
	}

	return node;
}
