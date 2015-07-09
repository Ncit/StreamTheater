/************************************************************************************

Filename    :   UITextButton.cpp
Content     :
Created     :	1/23/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "UI/UITextButton.h"
#include "UI/UIMenu.h"
#include "VRMenu/VRMenuMgr.h"
#include "CinemaApp.h"

namespace VRMatterStreamTheater {

UITextButton::UITextButton( CinemaApp &cinema ) :
	UIWidget( cinema ),
	ButtonComponent( *this ),
	SelectedColor(1.0f,1.0f,1.0f,1.0f),
	HoverColor(1.0f, 1.0f, 0.5f, 1.0f),
	EnabledColor(0.6f,0.6f,0.6f,1.0f),
	DisabledColor(0.4f,0.4f,0.4f,0.4f),
	OnClickFunction( NULL ),
	OnClickObject( NULL ),
	IsSelectedFunction( NULL ),
	IsSelectedObject( NULL ),
	IsEnabledFunction( NULL ),
	IsEnabledObject( NULL )

{
}

UITextButton::~UITextButton()
{
}

void UITextButton::AddToMenu( OvrGuiSys & guiSys, UIMenu *menu, UIWidget *parent )
{
	const Posef pose( Quatf( Vector3f( 0.0f, 1.0f, 0.0f ), 0.0f ), Vector3f( 0.0f, 0.0f, 0.0f ) );

	Vector3f defaultScale( 1.0f );
	VRMenuFontParms fontParms( true, true, false, false, false, 1.0f );
	
	VRMenuObjectParms parms( VRMENU_BUTTON, Array< VRMenuComponent* >(), VRMenuSurfaceParms(),
			"", pose, defaultScale, fontParms, menu->AllocId(),
			VRMenuObjectFlags_t(), VRMenuObjectInitFlags_t( VRMENUOBJECT_INIT_FORCE_POSITION ) );

	AddToMenuWithParms( guiSys, menu, parent, parms );

	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	object->AddComponent( &ButtonComponent );
}

void UITextButton::SetOnClick( void ( *callback )( UITextButton *, void * ), void *object )
{
	OnClickFunction = callback;
	OnClickObject = object;
}

void UITextButton::SetIsSelected( bool ( *callback )( UITextButton *, void * ), void *object )
{
	IsSelectedFunction = callback;
	IsSelectedObject = object;
}

void UITextButton::SetIsEnabled( bool ( *callback )( UITextButton *, void * ), void *object )
{
	IsEnabledFunction = callback;
	IsEnabledObject = object;
}

void UITextButton::OnClick()
{
	if ( OnClickFunction != NULL )
	{
		( *OnClickFunction )( this, OnClickObject );
	}
}

bool UITextButton::IsSelected()
{
	if ( IsSelectedFunction != NULL )
	{
		return ( *IsSelectedFunction )( this, OnClickObject );
	}
	return false;
}

bool UITextButton::IsEnabled()
{
	if ( IsEnabledFunction != NULL )
	{
		return ( *IsEnabledFunction )( this, OnClickObject );
	}
	return true;
}



//==============================
//  UITextButton::UpdateButtonState
void UITextButton::UpdateButtonState()
{
	if( !IsEnabled() )
	{
		SetTextColor(GetDisabledColor());
	}
	else if ( IsSelected() || ButtonComponent.IsPressed() )
	{
		SetTextColor(GetSelectedColor());
	}
	else if ( GetMenuObject()->IsHilighted() )
	{
		SetTextColor(GetHoverColor());
	}
	else
	{
		SetTextColor(GetEnabledColor());
	}
}

void UITextButton::SetText( const char *text )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetText( text );
}

void UITextButton::SetText( const String &text )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetText( text.ToCStr() );
}

void UITextButton::SetTextWordWrapped( char const * text, class BitmapFont const & font, float const widthInMeters )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetTextWordWrapped( text, font, widthInMeters );
}

const String & UITextButton::GetText() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetText();
}

void UITextButton::SetFontScale( float const scale )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );

	VRMenuFontParms parms = object->GetFontParms();
	parms.Scale = scale;
	object->SetFontParms( parms );

}

float UITextButton::GetFontScale() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	VRMenuFontParms parms = object->GetFontParms();
	return parms.Scale;
}

void UITextButton::SetTextOffset( Vector3f const & pos )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetTextLocalPosition( pos );
}

Vector3f const & UITextButton::GetTextOffset() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetTextLocalPosition();
}

void UITextButton::SetTextColor( Vector4f const & c )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetTextColor( c );
}

Vector4f const & UITextButton::GetTextColor() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetTextColor();
}

Bounds3f UITextButton::GetTextLocalBounds( BitmapFont const & font ) const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetTextLocalBounds( font );
}

Vector4f const & UITextButton::GetSelectedColor() const
{
	return SelectedColor;
}
void UITextButton::SetSelectedColor( Vector4f const & c )
{
	SelectedColor = c;
}
Vector4f const & UITextButton::GetHoverColor() const
{
	return HoverColor;
}
void UITextButton::SetHoverColor( Vector4f const & c )
{
	HoverColor = c;
}
Vector4f const & UITextButton::GetEnabledColor() const
{
	return EnabledColor;
}
void UITextButton::SetEnabledColor( Vector4f const & c )
{
	EnabledColor = c;
}
Vector4f const & UITextButton::GetDisabledColor() const
{
	return DisabledColor;
}
void UITextButton::SetDisabledColor( Vector4f const & c )
{
	DisabledColor = c;
}


/*************************************************************************************/

//==============================
//  UITextButtonComponent::
UITextButtonComponent::UITextButtonComponent( UITextButton &button ) :
    VRMenuComponent( VRMenuEventFlags_t( VRMENU_EVENT_TOUCH_DOWN ) |
            VRMENU_EVENT_TOUCH_UP |
            VRMENU_EVENT_FOCUS_GAINED |
            VRMENU_EVENT_FOCUS_LOST ),
    Button( button ),
	TouchDown( false )

{
}

//==============================
//  UITextButtonComponent::OnEvent_Impl
eMsgStatus UITextButtonComponent::OnEvent_Impl( OvrGuiSys & guiSys, VrFrame const & vrFrame,
        VRMenuObject * self, VRMenuEvent const & event )
{

    switch( event.EventType )
    {
        case VRMENU_EVENT_FOCUS_GAINED:
            return FocusGained( guiSys, vrFrame, self, event );
        case VRMENU_EVENT_FOCUS_LOST:
            return FocusLost( guiSys, vrFrame, self, event );
        case VRMENU_EVENT_TOUCH_DOWN:
        	TouchDown = true;
        	Button.SetSelected( true );
        	Button.UpdateButtonState();
            DownSoundLimiter.PlaySound( guiSys.GetApp(), "touch_down", 0.1 );
            return MSG_STATUS_ALIVE;
        case VRMENU_EVENT_TOUCH_UP:
        	TouchDown = false;
        	Button.UpdateButtonState();
        	Button.OnClick();
            UpSoundLimiter.PlaySound( guiSys.GetApp(), "touch_up", 0.1 );
            return MSG_STATUS_ALIVE;
        default:
            OVR_ASSERT( !"Event flags mismatch!" );
            return MSG_STATUS_ALIVE;
    }
}

//==============================
//  UITextButtonComponent::FocusGained
eMsgStatus UITextButtonComponent::FocusGained( OvrGuiSys & guiSys, VrFrame const & vrFrame,
        VRMenuObject * self, VRMenuEvent const & event )
{
    // set the hilight flag
    self->SetHilighted( true );
    Button.UpdateButtonState();
	GazeOverSoundLimiter.PlaySound( guiSys.GetApp(), "gaze_on", 0.1 );
    return MSG_STATUS_ALIVE;
}

//==============================
//  UITextButtonComponent::FocusLost
eMsgStatus UITextButtonComponent::FocusLost( OvrGuiSys & guiSys, VrFrame const & vrFrame,
        VRMenuObject * self, VRMenuEvent const & event )
{
    // clear the hilight flag
    self->SetHilighted( false );
    TouchDown = false;
    Button.UpdateButtonState();
    GazeOverSoundLimiter.PlaySound( guiSys.GetApp(), "gaze_off", 0.1 );
    return MSG_STATUS_ALIVE;
}


} // namespace VRMatterStreamTheater
