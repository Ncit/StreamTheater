/************************************************************************************

Filename    :   TheaterSelectionView.h
Content     :
Created     :	6/19/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( TheaterSelectionView_h )
#define TheaterSelectionView_h

#include "Kernel/OVR_Array.h"
#include "View.h"
#include "VRMenu/GuiSys.h"

namespace VRMatterStreamTheater {

class CinemaApp;
class CarouselBrowser;
class CarouselItem;

class TheaterSelectionView : public View
{
public:
								TheaterSelectionView( CinemaApp &cinema );
	virtual 					~TheaterSelectionView();

	virtual void 				OneTimeInit( const char * launchIntent );
	virtual void				OneTimeShutdown();

	virtual void 				OnOpen();
	virtual void 				OnClose();

	virtual bool 				Command( const char * msg );
	virtual bool 				OnKeyEvent( const int keyCode, const int repeatCount, const KeyEventType eventType );

	virtual Matrix4f 			DrawEyeView( const int eye, const float fovDegrees );
	virtual Matrix4f 			Frame( const VrFrame & vrFrame );

	void 						SelectPressed( void );
	void 						SelectTheater( int theater );

	int							GetSelectedTheater() const { return SelectedTheater; }

private:
	CinemaApp &					Cinema;

	static VRMenuId_t  			ID_CENTER_ROOT;
	static VRMenuId_t			ID_ICONS;
	static VRMenuId_t  			ID_TITLE_ROOT;
	static VRMenuId_t 			ID_SWIPE_ICON_LEFT;
	static VRMenuId_t 			ID_SWIPE_ICON_RIGHT;

	VRMenu *					Menu;
	VRMenuObject * 				CenterRoot;
	VRMenuObject * 				SelectionObject;

	CarouselBrowserComponent *	TheaterBrowser;
	Array<CarouselItem *> 		Theaters;

	int							SelectedTheater;

	double						IgnoreSelectTime;

private:
	void						SetPosition( OvrVRMenuMgr & menuMgr, const Vector3f &pos );
	void 						CreateMenu( OvrGuiSys & guiSys );
};

} // namespace VRMatterStreamTheater

#endif // TheaterSelectionView_h
