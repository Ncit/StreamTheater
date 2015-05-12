/************************************************************************************

Filename    :   UIMenu.cpp
Content     :
Created     :	1/5/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "UI/UIMenu.h"
#include "VRMenu/VRMenuMgr.h"
#include "CinemaApp.h"

namespace VRMatterStreamTheater {

UIMenu::UIMenu( CinemaApp &cinema ) :
	Cinema( cinema ),
	MenuName(),
	Menu( NULL ),
	MenuOpen( false ),
	IdPool( 1 )

{
	// This is called at library load time, so the system is not initialized
	// properly yet.
}

UIMenu::~UIMenu()
{
	//DeletePointerArray( MovieBrowserItems );
}

VRMenuId_t UIMenu::AllocId()
{
	VRMenuId_t id = IdPool;
	IdPool = VRMenuId_t( IdPool.Get() + 1 );

	return id;
}

void UIMenu::Open()
{
	LOG( "Open" );
	Cinema.app->GetGuiSys().OpenMenu( Cinema.app, Cinema.app->GetGazeCursor(), MenuName.ToCStr() );
	MenuOpen = true;
}

void UIMenu::Close()
{
	LOG( "Close" );
	Cinema.app->GetGuiSys().CloseMenu( Cinema.app, Menu, false );
	MenuOpen = false;
}

//=======================================================================================

void UIMenu::Create( const char *menuName )
{
	MenuName = menuName;
	Menu = VRMenu::Create( menuName );
	Menu->Init( Cinema.app->GetVRMenuMgr(), Cinema.app->GetDefaultFont(), 0.0f, VRMenuFlags_t() );
    Cinema.app->GetGuiSys().AddMenu( Menu );
}

VRMenuFlags_t const & UIMenu::GetFlags() const
{
	return Menu->GetFlags();
}

void UIMenu::SetFlags( VRMenuFlags_t const & flags )
{
	Menu->SetFlags( flags );
}

} // namespace VRMatterStreamTheater
