/************************************************************************************

Filename    :   MovieCategoryComponent.h
Content     :   Menu component for the movie category menu.
Created     :   August 13, 2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "VRMenu/VRMenuComponent.h"
#include "MovieManager.h"

#if !defined( MovieCategoryComponent_h )
#define MovieCategoryComponent_h

using namespace OVR;

namespace OculusCinema {

class MovieSelectionView;

//==============================================================
// MovieCategoryComponent
class MovieCategoryComponent : public VRMenuComponent
{
public:
							MovieCategoryComponent( MovieSelectionView *view, MovieCategory category );

	static const Vector4f	HighlightColor;
	static const Vector4f	FocusColor;
	static const Vector4f	NormalColor;

private:
    SoundLimiter			Sound;

	bool					HasFocus;

	MovieCategory			Category;

    MovieSelectionView *	CallbackView;

private:
    virtual eMsgStatus      OnEvent_Impl( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );

	void					UpdateColor( VRMenuObject * self );

    eMsgStatus              Frame( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );
    eMsgStatus              FocusGained( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );
    eMsgStatus              FocusLost( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );
};

} // namespace OculusCinema

#endif // MovieCategoryComponent_h
