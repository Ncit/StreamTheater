/************************************************************************************

Filename    :   SwipeHintComponent.h
Content     :   Menu component for the swipe hints.
Created     :   October 6, 2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "VRMenu/VRMenuComponent.h"
#include "Lerp.h"

#if !defined( SwipeHintComponent_h )
#define SwipeHintComponent_h

using namespace OVR;

namespace OculusCinema {

class CarouselBrowserComponent;

//==============================================================
// SwipeHintComponent
class SwipeHintComponent : public VRMenuComponent
{
public:
	static const char *			TYPE_NAME;

	static bool					ShowSwipeHints;

	SwipeHintComponent( CarouselBrowserComponent *carousel, const bool isRightSwipe, const float totalTime, const float timeOffset, const float delay );

	virtual const char *		GetTypeName( ) const { return TYPE_NAME; }

	void						Reset( VRMenuObject * self );

private:
    CarouselBrowserComponent *	Carousel;
    bool 						IsRightSwipe;
    float 						TotalTime;
    float						TimeOffset;
    float 						Delay;
    double 						StartTime;
    bool						ShouldShow;
    bool						IgnoreDelay;
    Lerp						TotalAlpha;

private:
    bool 						CanSwipe() const;
    void 						Show( const double now );
    void 						Hide( const double now );
    virtual eMsgStatus      	OnEvent_Impl( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );
    eMsgStatus              	Opening( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );
    eMsgStatus              	Frame( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
                                    VRMenuObject * self, VRMenuEvent const & event );
};

} // namespace OculusCinema

#endif // SwipeHintComponent_h
