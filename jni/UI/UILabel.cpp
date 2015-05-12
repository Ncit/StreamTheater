/************************************************************************************

Filename    :   UILabel.cpp
Content     :
Created     :	1/5/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "UI/UILabel.h"
#include "UI/UIMenu.h"
#include "VRMenu/VRMenuMgr.h"
#include "CinemaApp.h"

namespace VRMatterStreamTheater {

UILabel::UILabel( CinemaApp &cinema ) :
	UIWidget( cinema )

{
}

UILabel::~UILabel()
{
}

void UILabel::AddToMenu( UIMenu *menu, UIWidget *parent )
{
	const Posef pose( Quatf( Vector3f( 0.0f, 1.0f, 0.0f ), 0.0f ), Vector3f( 0.0f, 0.0f, 0.0f ) );

	Vector3f defaultScale( 1.0f );
	VRMenuFontParms fontParms( true, true, false, false, false, 1.0f );
	
	VRMenuObjectParms parms( VRMENU_STATIC, Array< VRMenuComponent* >(), VRMenuSurfaceParms(),
			"", pose, defaultScale, fontParms, menu->AllocId(),
			VRMenuObjectFlags_t(), VRMenuObjectInitFlags_t( VRMENUOBJECT_INIT_FORCE_POSITION ) );

	AddToMenuWithParms( menu, parent, parms );
}

void UILabel::SetText( const char *text )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetText( text );
}

void UILabel::SetText( const String &text )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetText( text.ToCStr() );
}

void UILabel::SetTextWordWrapped( char const * text, class BitmapFont const & font, float const widthInMeters )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetTextWordWrapped( text, font, widthInMeters );
}

const String & UILabel::GetText() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetText();
}

void UILabel::SetFontScale( float const scale )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );

	VRMenuFontParms parms = object->GetFontParms();
	parms.Scale = scale;
	object->SetFontParms( parms );

}

float UILabel::GetFontScale() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	VRMenuFontParms parms = object->GetFontParms();
	return parms.Scale;
}

void UILabel::SetTextOffset( Vector3f const & pos )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetTextLocalPosition( pos );
}

Vector3f const & UILabel::GetTextOffset() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetTextLocalPosition();
}

void UILabel::SetTextColor( Vector4f const & c )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetTextColor( c );
}

Vector4f const & UILabel::GetTextColor() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetTextColor();
}

Bounds3f UILabel::GetTextLocalBounds( BitmapFont const & font ) const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetTextLocalBounds( font );
}

} // namespace VRMatterStreamTheater
