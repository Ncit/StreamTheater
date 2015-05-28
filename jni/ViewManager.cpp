/************************************************************************************

Filename    :   ViewManager.cpp
Content     :
Created     :	6/17/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "ViewManager.h"
#include "App.h"

namespace VRMatterStreamTheater {

ViewManager::ViewManager() :
	LastViewMatrix(),
	Views(),
	CurrentView( NULL ),
	NextView( NULL ),
	ClosedCurrent( false )

{
	LastEyeMatrix[ 0 ].SetIdentity();
	LastEyeMatrix[ 1 ].SetIdentity();
}

bool ViewManager::ChangingViews()
{
	return NextView != NULL;
}

void ViewManager::AddView( View * view )
{
	LOG( "AddView: %s", view->name );
	Views.PushBack( view );
}

void ViewManager::RemoveView( View * view )
{
	for( UPInt i = 0; i < Views.GetSize(); i++ )
	{
		if ( Views[ i ] == view )
		{
			Views.RemoveAt( i );
			return;
		}
	}

	// view wasn't in the array
	assert( 1 );
	LOG( "RemoveView: view not in array" );
}

void ViewManager::OpenView( View & view )
{
	LOG( "OpenView: %s", view.name );
	NextView = &view;
	ClosedCurrent = false;
}

void ViewManager::CloseView()
{
	if ( CurrentView != NULL )
	{
		LOG( "CloseView: %s", CurrentView->name );
		CurrentView->OnClose();
	}
}

bool ViewManager::Command( const char * msg )
{
	bool result = false;
	for( UPInt i = 0; i < Views.GetSize(); i++ )
	{
		result = Views[ i ]->Command( msg );
		if ( result )
		{
			break;
		}
	}

	return result;
}

bool ViewManager::OnKeyEvent( const int keyCode, const KeyState::eKeyEventType eventType )
{
	if ( ( CurrentView != NULL ) && !CurrentView->IsClosed() )
	{
		return CurrentView->OnKeyEvent( keyCode, eventType );
	}
	else
	{
		return false;
	}
}

Matrix4f ViewManager::DrawEyeView( const int eye, const float fovDegrees )
{
	if ( CurrentView != NULL )
	{
		LastEyeMatrix[ eye ] = CurrentView->DrawEyeView( eye, fovDegrees );
	}

	return LastEyeMatrix[ eye ];
}

Matrix4f ViewManager::Frame( const VrFrame & vrFrame )
{
	if ( ( NextView != NULL ) && ( CurrentView != NULL ) && !ClosedCurrent )
	{
		LOG( "OnClose: %s", CurrentView->name );
		CurrentView->OnClose();
		ClosedCurrent = true;
	}

	if ( ( CurrentView == NULL ) || ( CurrentView->IsClosed() ) )
	{
		CurrentView = NextView;
		NextView = NULL;
		ClosedCurrent = false;

		if ( CurrentView != NULL )
		{
			LOG( "OnOpen: %s", CurrentView->name );
			CurrentView->OnOpen();
		}
	}

	if ( CurrentView != NULL )
	{
		LastViewMatrix = CurrentView->Frame( vrFrame );
	}

	return LastViewMatrix;
}

} // namespace VRMatterStreamTheater
