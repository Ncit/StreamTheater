/************************************************************************************

Filename    :   SwipeHintComponent.cpp
Content     :   Menu component for the swipe hints.
Created     :   October 6, 2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "SwipeHintComponent.h"
#include "CarouselBrowserComponent.h"
#include "VrApi/Vsync.h"
#include "Input.h"

namespace VRMatterStreamTheater {

const char * SwipeHintComponent::TYPE_NAME = "SwipeHintComponent";

bool SwipeHintComponent::ShowSwipeHints = true;

//==============================
//  SwipeHintComponent::SwipeHintComponent()
SwipeHintComponent::SwipeHintComponent( CarouselBrowserComponent *carousel, const bool isRightSwipe, const float totalTime, const float timeOffset, const float delay ) :
	VRMenuComponent( VRMenuEventFlags_t( VRMENU_EVENT_FRAME_UPDATE ) | VRMENU_EVENT_OPENING ),
	Carousel( carousel ),
	IsRightSwipe( isRightSwipe ),
    TotalTime( totalTime ),
    TimeOffset( timeOffset ),
    Delay( delay ),
    StartTime( 0 ),
    ShouldShow( false ),
    IgnoreDelay( false ),
    TotalAlpha()

{
}

//==============================
//  SwipeHintComponent::Reset
void SwipeHintComponent::Reset( VRMenuObject * self )
{
	IgnoreDelay = true;
	ShouldShow = false;
	const double now = ovr_GetTimeInSeconds();
	TotalAlpha.Set( now, TotalAlpha.Value( now ), now, 0.0f );
	self->SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 0.0f ) );
}

//==============================
//  SwipeHintComponent::CanSwipe
bool SwipeHintComponent::CanSwipe() const
{
	return IsRightSwipe ? Carousel->CanSwipeForward() : Carousel->CanSwipeBack();
}

//==============================
//  SwipeHintComponent::Show
void SwipeHintComponent::Show( const double now )
{
	if ( !ShouldShow )
	{
		ShouldShow = true;
		StartTime = now + TimeOffset + ( IgnoreDelay ? 0.0f : Delay );
		TotalAlpha.Set( now, TotalAlpha.Value( now ), now + 0.5f, 1.0f );
	}
}

//==============================
//  SwipeHintComponent::Hide
void SwipeHintComponent::Hide( const double now )
{
	if ( ShouldShow )
	{
		TotalAlpha.Set( now, TotalAlpha.Value( now ), now + 0.5f, 0.0f );
		ShouldShow = false;
	}
}

//==============================
//  SwipeHintComponent::OnEvent_Impl
eMsgStatus SwipeHintComponent::OnEvent_Impl( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
    switch( event.EventType )
    {
    	case VRMENU_EVENT_OPENING :
    		return Opening( app, vrFrame, menuMgr, self, event );
        case VRMENU_EVENT_FRAME_UPDATE :
        	return Frame( app, vrFrame, menuMgr, self, event );
        default:
            OVR_ASSERT( !"Event flags mismatch!" );
            return MSG_STATUS_ALIVE;
    }
}

//==============================
//  SwipeHintComponent::Opening
eMsgStatus SwipeHintComponent::Opening( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr, VRMenuObject * self, VRMenuEvent const & event )
{
	Reset( self );
	return MSG_STATUS_ALIVE;
}

//==============================
//  SwipeHintComponent::Frame
eMsgStatus SwipeHintComponent::Frame( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr, VRMenuObject * self, VRMenuEvent const & event )
{
	if ( ShowSwipeHints && Carousel->HasSelection() && CanSwipe() )
	{
		Show( vrFrame.PoseState.TimeInSeconds );
	}
	else
	{
		Hide( vrFrame.PoseState.TimeInSeconds );
	}

	IgnoreDelay = false;

	float alpha = TotalAlpha.Value( vrFrame.PoseState.TimeInSeconds );
	if ( alpha > 0.0f )
	{
		double time = vrFrame.PoseState.TimeInSeconds - StartTime;
		if ( time < 0.0f )
		{
			alpha = 0.0f;
		}
		else
		{
			float normTime = time / TotalTime;
			alpha *= sin( M_PI * 2.0f * normTime );
			alpha = OVR::Alg::Max( alpha, 0.0f );
		}
	}

	self->SetColor( Vector4f( 1.0f, 1.0f, 1.0f, alpha ) );

	return MSG_STATUS_ALIVE;
}

} // namespace VRMatterStreamTheater
