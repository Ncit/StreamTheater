/************************************************************************************

Filename    :   MovieSelectionComponent.cpp
Content     :   Menu component for the movie selection menu.
Created     :   October 8, 2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "MovieSelectionComponent.h"
#include "Input.h"
#include "SelectionView.h"

namespace VRMatterStreamTheater {

//==============================
//  MovieSelectionComponent::
MovieSelectionComponent::MovieSelectionComponent( SelectionView *view ) :
	VRMenuComponent( VRMenuEventFlags_t( VRMENU_EVENT_FRAME_UPDATE ) |
		VRMENU_EVENT_TOUCH_DOWN |
		VRMENU_EVENT_TOUCH_UP |
        VRMENU_EVENT_FOCUS_GAINED |
        VRMENU_EVENT_FOCUS_LOST ),
    CallbackView( view )

{
}

//==============================
//  MovieSelectionComponent::OnEvent_Impl
eMsgStatus MovieSelectionComponent::OnEvent_Impl( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
    switch( event.EventType )
    {
		case VRMENU_EVENT_FRAME_UPDATE:
			return Frame( app, vrFrame, menuMgr, self, event );
        case VRMENU_EVENT_FOCUS_GAINED:
            return FocusGained( app, vrFrame, menuMgr, self, event );
        case VRMENU_EVENT_FOCUS_LOST:
            return FocusLost( app, vrFrame, menuMgr, self, event );
        case VRMENU_EVENT_TOUCH_DOWN:
       		Sound.PlaySound( app, "touch_down", 0.1 );
       		return MSG_STATUS_CONSUMED;
        case VRMENU_EVENT_TOUCH_UP:
        	if ( !( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) )
			{
        		Sound.PlaySound( app, "touch_up", 0.1 );
        		CallbackView->Select();
        		return MSG_STATUS_CONSUMED;
        	}
            return MSG_STATUS_ALIVE;
        default:
            OVR_ASSERT( !"Event flags mismatch!" );
            return MSG_STATUS_ALIVE;
    }
}

//==============================
//  MovieSelectionComponent::Frame
eMsgStatus MovieSelectionComponent::Frame( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
	CallbackView->SelectionHighlighted( self->IsHilighted() );

#if 0
	if ( self->IsHilighted() )
	{
		if ( vrFrame.Input.buttonPressed & BUTTON_A )
		{
			Sound.PlaySound( app, "touch_down", 0.1 );
		}
		if ( vrFrame.Input.buttonReleased & BUTTON_A )
		{
			Sound.PlaySound( app, "touch_up", 0.1 );
			CallbackView->SelectMovie();
		}
	}
#endif

    return MSG_STATUS_ALIVE;
}

//==============================
//  MovieSelectionComponent::FocusGained
eMsgStatus MovieSelectionComponent::FocusGained( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
	LOG( "FocusGained" );
    // set the hilight flag
    self->SetHilighted( true );
    CallbackView->SelectionHighlighted( true );

    Sound.PlaySound( app, "gaze_on", 0.1 );
	
    return MSG_STATUS_ALIVE;
}

//==============================
//  MovieSelectionComponent::FocusLost
eMsgStatus MovieSelectionComponent::FocusLost( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
	LOG( "FocusLost" );
    // clear the hilight flag
    self->SetHilighted( false );
    CallbackView->SelectionHighlighted( false );

   	Sound.PlaySound( app, "gaze_off", 0.1 );

    return MSG_STATUS_ALIVE;
}

} // namespace VRMatterStreamTheater
