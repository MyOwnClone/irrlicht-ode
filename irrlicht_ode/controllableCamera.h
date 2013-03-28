#pragma once

#include <irrlicht.h>

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::gui;

/** This scene node animator can be attached to a camera to make it act
like a 3d modelling tool camera
*/
class ControllableCamera : public ISceneNodeAnimator
{
public:
	//! Constructor
	ControllableCamera(gui::ICursorControl* cursor, f32 rotateSpeed, f32 zoomSpeed, f32 translationSpeed, ICameraSceneNode *oldcam, double currentZoom);

	//! Destructor
	virtual ~ControllableCamera();

	//! Animates the scene node, currently only works on cameras
	virtual void animateNode(ISceneNode* node, u32 timeMs);

	//! Event receiver
	virtual bool OnEvent(const SEvent& event);

	//! Returns the speed of movement in units per millisecond
	virtual f32 getMoveSpeed() const;

	//! Sets the speed of movement in units per millisecond
	virtual void setMoveSpeed(f32 moveSpeed);

	//! Returns the rotation speed
	virtual f32 getRotateSpeed() const;

	//! Set the rotation speed
	virtual void setRotateSpeed(f32 rotateSpeed);

	//! Returns the zoom speed
	virtual f32 getZoomSpeed() const;

	//! Set the zoom speed
	virtual void setZoomSpeed(f32 zoomSpeed);

	//! This animator will receive events when attached to the active camera
	virtual bool isEventReceiverEnabled() const
	{
		return true;
	}

	//! Returns type of the scene node
	virtual ESCENE_NODE_ANIMATOR_TYPE getType() const 
	{
		return ESNAT_CAMERA_MAYA;
	}

	//! Creates a clone of this animator.
	/** Please note that you will have to drop
	(IReferenceCounted::drop()) the returned pointer after calling
	this. */
	virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager=0);

	//! Adds a camera similar to the Irrlicht Maya cam but with better maneuverability
	//! The returned pointer must not be dropped.
	static ICameraSceneNode* addControllableCamera(ISceneManager* smgr, ICursorControl* cursor, ISceneNode* parent = 0,
		f32 rotateSpeed = -750, f32 zoomSpeed = 5,
		f32 translationSpeed = 150, s32 id=-1,
		bool makeActive=true);

private:

	void allKeysUp();
	void animate();

	bool MouseKeys[3];
	bool KeyboardKeys[KEY_KEY_CODES_COUNT ];
	f32 MouseWheel;
	gui::ICursorControl *CursorControl;
	core::vector3df Pos;
	bool Zooming;
	bool Rotating;
	bool Moving;
	bool Translating;
	f32 ZoomSpeed;
	f32 RotateSpeed;
	f32 TranslateSpeed;
	core::position2df RotateStart;
	core::position2df TranslateStart;
	f32 CurrentZoom;
	f32 RotX, RotY;
	core::vector3df OldTarget;
	core::vector3df LastCameraTarget;	// to find out if the camera target was moved outside this animator
	scene::ICameraSceneNode* OldCamera;

	core::position2df MousePos;
};
