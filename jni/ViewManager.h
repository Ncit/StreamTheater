/************************************************************************************

Filename    :   ViewManager.h
Content     :
Created     :	6/17/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "Kernel/OVR_Array.h"
#include "Kernel/OVR_Math.h"
#include "Input.h"
#include "View.h"

#if !defined( ViewManager_h )
#define ViewManager_h

namespace VRMatterStreamTheater {

class ViewManager
{
public:
					ViewManager();

	View *			GetCurrentView() const { return CurrentView; };

	void 			AddView( View * view );
	void 			RemoveView( View * view );

	void 			OpenView( View & view );
	void 			CloseView();

	bool 			Command( const char * msg );
	bool 			OnKeyEvent( const int keyCode, const KeyState::eKeyEventType eventType );

	Matrix4f 		DrawEyeView( const int eye, const float fovDegrees );
	Matrix4f 		Frame( const VrFrame & vrFrame );

private:
	Matrix4f		LastViewMatrix;
	Matrix4f		LastEyeMatrix[ 2 ];

	Array<View *> 	Views;

	View *			CurrentView;
	View *			NextView;

	bool			ClosedCurrent;
};

} // namespace VRMatterStreamTheater

#endif // ViewManager_h
