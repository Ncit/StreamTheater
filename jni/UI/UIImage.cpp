/************************************************************************************

Filename    :   UIImage.cpp
Content     :
Created     :	1/5/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "UI/UIImage.h"
#include "UI/UIMenu.h"
#include "VRMenu/VRMenuMgr.h"
#include "CinemaApp.h"

namespace OculusCinema {

UIImage::UIImage( CinemaApp &cinema ) :
	UIWidget( cinema )

{
}

UIImage::~UIImage()
{
}

void UIImage::AddToMenu( UIMenu *menu, UIWidget *parent )
{
	AddToMenuFlags( menu, parent, VRMenuObjectFlags_t() );
}

void UIImage::AddToMenuFlags( UIMenu *menu, UIWidget *parent, VRMenuObjectFlags_t const flags )
{
	const Posef pose( Quatf( Vector3f( 0.0f, 1.0f, 0.0f ), 0.0f ), Vector3f( 0.0f, 0.0f, 0.0f ) );

	Vector3f defaultScale( 1.0f );
	VRMenuFontParms fontParms( true, true, false, false, false, 1.0f );
	
	VRMenuObjectParms parms( VRMENU_BUTTON, Array< VRMenuComponent* >(), VRMenuSurfaceParms(),
			"", pose, defaultScale, fontParms, menu->AllocId(),
			flags, VRMenuObjectInitFlags_t( VRMENUOBJECT_INIT_FORCE_POSITION ) );

	AddToMenuWithParms( menu, parent, parms );
}

} // namespace OculusCinema
