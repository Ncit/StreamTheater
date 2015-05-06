/************************************************************************************

Filename    :   CarouselBrowserComponent.h
Content     :   A menu for browsing a a group of items on a carousel in front of the user
Created     :   July 25, 2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "CarouselBrowserComponent.h"
#include "App.h"

using namespace OVR;

namespace OculusCinema {

//==============================================================
// CarouselBrowserComponent
CarouselBrowserComponent::CarouselBrowserComponent( const Array<CarouselItem *> &items, const Array<PanelPose> &panelPoses ) :
	VRMenuComponent( VRMenuEventFlags_t( VRMENU_EVENT_FRAME_UPDATE ) | 	VRMENU_EVENT_TOUCH_DOWN | 
		VRMENU_EVENT_SWIPE_FORWARD | VRMENU_EVENT_SWIPE_BACK | VRMENU_EVENT_TOUCH_UP | VRMENU_EVENT_OPENED | VRMENU_EVENT_CLOSED ),
		SelectPressed( false ), PositionScale( 1.0f ), Position( 0.0f ), TouchDownTime( -1.0 ),
		ItemWidth( 0 ), ItemHeight( 0 ), Items(), MenuObjs(), MenuComps(), PanelPoses( panelPoses ),
		StartTime( 0.0 ), EndTime( 0.0 ), PrevPosition( 0.0f ), NextPosition( 0.0f ), Swiping( false ), PanelsNeedUpdate( false )

{
	SetItems( items );
}

eMsgStatus CarouselBrowserComponent::OnEvent_Impl( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
	VRMenuObject * self, VRMenuEvent const & event )
{
	OVR_ASSERT( HandlesEvent( VRMenuEventFlags_t( event.EventType ) ) );

	switch( event.EventType )
	{
		case VRMENU_EVENT_FRAME_UPDATE:
			return Frame( app, vrFrame, menuMgr, self, event );
		case VRMENU_EVENT_TOUCH_DOWN:
			return TouchDown( app, vrFrame, menuMgr, self, event);
		case VRMENU_EVENT_TOUCH_UP:
			return TouchUp( app, vrFrame, menuMgr, self, event );
		case VRMENU_EVENT_OPENED:
			return Opened( app, vrFrame, menuMgr, self, event );
		case VRMENU_EVENT_CLOSED:
			return Closed( app, vrFrame, menuMgr, self, event );
		case VRMENU_EVENT_SWIPE_FORWARD:
			return SwipeForward( app, vrFrame, menuMgr, self );
		case VRMENU_EVENT_SWIPE_BACK:
			return SwipeBack( app, vrFrame, menuMgr, self );
		default:
			OVR_ASSERT( !"Event flags mismatch!" ); // the constructor is specifying a flag that's not handled
			return MSG_STATUS_ALIVE;
	}
}

void CarouselBrowserComponent::SetPanelPoses( OvrVRMenuMgr & menuMgr, VRMenuObject * self, const Array<PanelPose> &panelPoses )
{
	PanelPoses = panelPoses;
	UpdatePanels( menuMgr, self );
}

void CarouselBrowserComponent::SetMenuObjects( const Array<VRMenuObject *> &menuObjs, const Array<CarouselItemComponent *> &menuComps )
{
	MenuObjs = menuObjs;
	MenuComps = menuComps;

	assert( MenuObjs.GetSizeI() == MenuObjs.GetSizeI() );
}

PanelPose CarouselBrowserComponent::GetPosition( const float t )
{
	int index = ( int )floor( t );
	float frac = t - ( float )index;

	PanelPose pose;

	if ( index < 0 )
	{
		pose = PanelPoses[ 0 ];
	}
	else if ( ( index == PanelPoses.GetSizeI() - 1 ) && ( fabs( frac ) <= 0.00001f ) )
	{
		pose = PanelPoses[ PanelPoses.GetSizeI() - 1 ];
	}
	else if ( index >= PanelPoses.GetSizeI() - 1 )
	{
		pose.Orientation = Quatf();
		pose.Position = Vector3f( 0.0f, 0.0f, 0.0f );
		pose.Color = Vector4f( 0.0f, 0.0f, 0.0f, 0.0f );
	}
	else
	{
		pose.Orientation = PanelPoses[ index + 1 ].Orientation.Nlerp( PanelPoses[ index ].Orientation, frac ); // NLerp has the frac inverted
		pose.Position = PanelPoses[ index ].Position.Lerp( PanelPoses[ index + 1 ].Position, frac );
		pose.Color = PanelPoses[ index ].Color * ( 1.0f - frac ) + PanelPoses[ index + 1 ].Color * frac;
	}

	pose.Position = pose.Position * PositionScale;

	return pose;
}

void CarouselBrowserComponent::SetSelectionIndex( const int selectedIndex )
{
	if ( ( selectedIndex >= 0 ) && ( selectedIndex < Items.GetSizeI() ) )
	{
		Position = selectedIndex;
	}
	else
	{
		Position = 0.0f;
	}

	NextPosition = Position;
	Swiping = false;
	PanelsNeedUpdate = true;
}

int CarouselBrowserComponent::GetSelection() const
{
	int itemIndex = floor( Position + 0.5f );
	if ( ( itemIndex >= 0 ) && ( itemIndex < Items.GetSizeI() ) )
	{
		return itemIndex;
	}

	return -1;
}

bool CarouselBrowserComponent::HasSelection() const
{
	if ( Items.GetSize() == 0 )
	{
		return false;
	}

	return !Swiping;
}

bool CarouselBrowserComponent::CanSwipeBack() const
{
	float nextPos = floor( Position ) - 1.0f;
	return ( nextPos >= 0.0f );
}

bool CarouselBrowserComponent::CanSwipeForward() const
{
	float nextPos = floor( Position ) + 1.0f;
	return ( nextPos < Items.GetSizeI() );
}

void CarouselBrowserComponent::UpdatePanels( OvrVRMenuMgr & menuMgr, VRMenuObject * self )
{
	int centerIndex = floor( Position );
	float offset = centerIndex - Position;
	int leftIndex = centerIndex - PanelPoses.GetSizeI() / 2;

	int itemIndex = leftIndex;
	for( int i = 0; i < MenuObjs.GetSizeI(); i++, itemIndex++ )
	{
		PanelPose pose = GetPosition( ( float )i + offset );
		if ( ( itemIndex < 0 ) || ( itemIndex >= Items.GetSizeI() ) || ( ( offset < 0.0f ) && ( i == 0 ) ) )
		{
			MenuComps[ i ]->SetItem( MenuObjs[ i ], NULL, pose );
		}
		else
		{
			MenuComps[ i ]->SetItem( MenuObjs[ i ], Items[ itemIndex ], pose );
		}
	}

	PanelsNeedUpdate = false;
}

void CarouselBrowserComponent::CheckGamepad( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr, VRMenuObject * self )
{
	if ( Swiping )
	{
		return;
	}

	if ( CanSwipeBack() && ( ( vrFrame.Input.buttonState & BUTTON_DPAD_LEFT ) || ( vrFrame.Input.sticks[0][0] < -0.5f ) ) )
	{
		SwipeBack( app, vrFrame, menuMgr, self );
		return;
	}

	if ( CanSwipeForward() && ( ( vrFrame.Input.buttonState & BUTTON_DPAD_RIGHT ) || ( vrFrame.Input.sticks[0][0] > 0.5f ) ) )
	{
		SwipeForward( app, vrFrame, menuMgr, self );
		return;
	}
}

eMsgStatus CarouselBrowserComponent::Frame( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr, VRMenuObject * self, VRMenuEvent const & event )
{
	if ( Swiping )
	{
		float frac = ( vrFrame.PoseState.TimeInSeconds - StartTime ) / ( EndTime - StartTime );
		if ( frac >= 1.0f )
		{
			frac = 1.0f;
			Swiping = false;
		}

		float easeOutQuad = -1.0f * frac * ( frac - 2.0f );

		Position = PrevPosition * ( 1.0f - easeOutQuad ) + NextPosition * easeOutQuad;

		PanelsNeedUpdate = true;
	}

	if ( PanelsNeedUpdate )
	{
		UpdatePanels( menuMgr, self );
	}

	return MSG_STATUS_ALIVE;
}

eMsgStatus CarouselBrowserComponent::SwipeForward( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr, VRMenuObject * self )
{
	if ( !Swiping )
	{
		float nextPos = floor( Position ) + 1.0f;
		if ( nextPos < Items.GetSizeI() )
		{
			app->PlaySound( "carousel_move" );
			PrevPosition = Position;
			StartTime = vrFrame.PoseState.TimeInSeconds;
			EndTime = StartTime + 0.25;
			NextPosition = nextPos;
			Swiping = true;
		}
	}

	return MSG_STATUS_CONSUMED;
}

eMsgStatus CarouselBrowserComponent::SwipeBack( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr, VRMenuObject * self )
{
	if ( !Swiping )
	{
		float nextPos = floor( Position ) - 1.0f;
		if ( nextPos >= 0.0f )
		{
			app->PlaySound( "carousel_move" );
			PrevPosition = Position;
			StartTime = vrFrame.PoseState.TimeInSeconds;
			EndTime = StartTime + 0.25;
			NextPosition = nextPos;
			Swiping = true;
		}
	}

	return MSG_STATUS_CONSUMED;
}

eMsgStatus CarouselBrowserComponent::TouchDown( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr, VRMenuObject * self, VRMenuEvent const & event )
{
	//LOG( "TouchDown" );
	TouchDownTime = ovr_GetTimeInSeconds();

	if ( Swiping )
	{
		return MSG_STATUS_CONSUMED;
	}

	return MSG_STATUS_ALIVE;	// don't consume -- we're just listening
}

eMsgStatus CarouselBrowserComponent::TouchUp( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr, VRMenuObject * self, VRMenuEvent const & event )
{
	//LOG( "TouchUp" );

	float const timeTouchHasBeenDown = (float)( ovr_GetTimeInSeconds() - TouchDownTime );
	TouchDownTime = -1.0;

	float dist = event.FloatValue.LengthSq();
	if ( !Swiping && ( dist < 20.0f ) && ( timeTouchHasBeenDown < 1.0f ) )
	{
		LOG( "Selectmovie" );
		SelectPressed = true;
	}
	else if ( Swiping )
	{
		return MSG_STATUS_CONSUMED;
	}

	//LOG( "Ignore: %f, %f", RotationalVelocity, ( float )timeTouchHasBeenDown );
	return MSG_STATUS_ALIVE; // don't consume -- we are just listening
}

eMsgStatus CarouselBrowserComponent::Opened( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr, VRMenuObject * self, VRMenuEvent const & event )
{
	Swiping = false;
	Position = floor( Position );
	SelectPressed = false;
	return MSG_STATUS_ALIVE;
}

eMsgStatus CarouselBrowserComponent::Closed( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr, VRMenuObject * self, VRMenuEvent const & event )
{
	SelectPressed = false;
	return MSG_STATUS_ALIVE;
}

void CarouselBrowserComponent::SetItems( const Array<CarouselItem *> &items )
{
	Items = items;
	SelectPressed = false;
	Position = 0.0f;
	TouchDownTime = -1.0;
	StartTime = 0.0;
	EndTime = 0.0;
	PrevPosition = 0.0f;
	NextPosition = 0.0f;
	PanelsNeedUpdate = true;
}

} // namespace OculusCinema
