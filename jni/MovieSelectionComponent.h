/************************************************************************************

Filename    :   MovieSelectionComponent.h
Content     :   Menu component for the movie selection menu.
Created     :   October 8, 2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "VRMenu/VRMenuComponent.h"

#if !defined( MovieSelectionComponent_h )
#define MovieSelectionComponent_h

using namespace OVR;

namespace VRMatterStreamTheater {

class MovieSelectionView;

//==============================================================
// MovieSelectionComponent
class MovieSelectionComponent : public VRMenuComponent
{
public:
							MovieSelectionComponent( MovieSelectionView *view );

private:
    SoundLimiter			Sound;
    MovieSelectionView *	CallbackView;

private:
    virtual eMsgStatus      OnEvent_Impl( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );

    eMsgStatus              Frame( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );
    eMsgStatus              FocusGained( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );
    eMsgStatus              FocusLost( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );
};

} // namespace VRMatterStreamTheater

#endif // MovieSelectionComponent_h
