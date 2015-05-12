/************************************************************************************

Filename    :   UIButton.cpp
Content     :
Created     :	1/23/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "UI/UIButton.h"
#include "UI/UIMenu.h"
#include "VRMenu/VRMenuMgr.h"
#include "CinemaApp.h"

namespace VRMatterStreamTheater {

UIButton::UIButton( CinemaApp &cinema ) :
	UIWidget( cinema ),
	ButtonComponent( *this ),
	Normal(),
	Hover(),
	Pressed(),
	OnClickFunction( NULL ),
	OnClickObject( NULL )

{
}

UIButton::~UIButton()
{
}

void UIButton::AddToMenu( UIMenu *menu, UIWidget *parent )
{
	const Posef pose( Quatf( Vector3f( 0.0f, 1.0f, 0.0f ), 0.0f ), Vector3f( 0.0f, 0.0f, 0.0f ) );

	Vector3f defaultScale( 1.0f );
	VRMenuFontParms fontParms( true, true, false, false, false, 1.0f );
	
	VRMenuObjectParms parms( VRMENU_BUTTON, Array< VRMenuComponent* >(), VRMenuSurfaceParms(),
			"", pose, defaultScale, fontParms, menu->AllocId(),
			VRMenuObjectFlags_t(), VRMenuObjectInitFlags_t( VRMENUOBJECT_INIT_FORCE_POSITION ) );

	AddToMenuWithParms( menu, parent, parms );

	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	object->AddComponent( &ButtonComponent );
}

void UIButton::SetButtonImages( const UITexture &normal, const UITexture &hover, const UITexture &pressed )
{
	Normal 	= normal;
	Hover 	= hover;
	Pressed = pressed;

	UpdateButtonState();
}

void UIButton::SetOnClick( void ( *callback )( UIButton *, void * ), void *object )
{
	OnClickFunction = callback;
	OnClickObject = object;
}

void UIButton::OnClick()
{
	if ( OnClickFunction != NULL )
	{
		( *OnClickFunction )( this, OnClickObject );
	}
}

//==============================
//  UIButton::UpdateButtonState
void UIButton::UpdateButtonState()
{
	if ( ButtonComponent.IsPressed() )
	{
		SetImage( 0, SURFACE_TEXTURE_DIFFUSE, Pressed );
	}
	else if ( GetMenuObject()->IsHilighted() )
	{
		SetImage( 0, SURFACE_TEXTURE_DIFFUSE, Hover );
	}
	else
	{
		SetImage( 0, SURFACE_TEXTURE_DIFFUSE, Normal );
	}
}

/*************************************************************************************/

//==============================
//  UIButtonComponent::
UIButtonComponent::UIButtonComponent( UIButton &button ) :
    VRMenuComponent( VRMenuEventFlags_t( VRMENU_EVENT_TOUCH_DOWN ) |
            VRMENU_EVENT_TOUCH_UP |
            VRMENU_EVENT_FOCUS_GAINED |
            VRMENU_EVENT_FOCUS_LOST ),
    Button( button ),
	TouchDown( false )

{
}

//==============================
//  UIButtonComponent::OnEvent_Impl
eMsgStatus UIButtonComponent::OnEvent_Impl( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
    switch( event.EventType )
    {
        case VRMENU_EVENT_FOCUS_GAINED:
            return FocusGained( app, vrFrame, menuMgr, self, event );
        case VRMENU_EVENT_FOCUS_LOST:
            return FocusLost( app, vrFrame, menuMgr, self, event );
        case VRMENU_EVENT_TOUCH_DOWN:
        	TouchDown = true;
        	Button.UpdateButtonState();
            DownSoundLimiter.PlaySound( app, "touch_down", 0.1 );
            return MSG_STATUS_ALIVE;
        case VRMENU_EVENT_TOUCH_UP:
        	TouchDown = false;
        	Button.UpdateButtonState();
        	Button.OnClick();
            UpSoundLimiter.PlaySound( app, "touch_up", 0.1 );
            return MSG_STATUS_ALIVE;
        default:
            OVR_ASSERT( !"Event flags mismatch!" );
            return MSG_STATUS_ALIVE;
    }
}

//==============================
//  UIButtonComponent::FocusGained
eMsgStatus UIButtonComponent::FocusGained( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
    // set the hilight flag
    self->SetHilighted( true );
    Button.UpdateButtonState();
	GazeOverSoundLimiter.PlaySound( app, "gaze_on", 0.1 );
    return MSG_STATUS_ALIVE;
}

//==============================
//  UIButtonComponent::FocusLost
eMsgStatus UIButtonComponent::FocusLost( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
    // clear the hilight flag
    self->SetHilighted( false );
    TouchDown = false;
    Button.UpdateButtonState();
    GazeOverSoundLimiter.PlaySound( app, "gaze_off", 0.1 );
    return MSG_STATUS_ALIVE;
}

} // namespace VRMatterStreamTheater
