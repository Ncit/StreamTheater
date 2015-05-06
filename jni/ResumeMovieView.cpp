/************************************************************************************

Filename    :   ResumeMovieView.cpp
Content     :
Created     :	9/3/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "GazeCursor.h"
#include "VRMenu/VRMenuMgr.h"
#include "CinemaApp.h"
#include "ResumeMovieView.h"
#include "ResumeMovieComponent.h"
#include "PackageFiles.h"
#include "CinemaStrings.h"

namespace OculusCinema {

VRMenuId_t ResumeMovieView::ID_CENTER_ROOT( 1000 );
VRMenuId_t ResumeMovieView::ID_TITLE( 1001 );
VRMenuId_t ResumeMovieView::ID_OPTIONS( 1002 );
VRMenuId_t ResumeMovieView::ID_OPTION_ICONS( 2000 );

ResumeMovieView::ResumeMovieView( CinemaApp &cinema ) :
	View( "ResumeMovieView" ),
	Cinema( cinema ),
	Menu( NULL )
{
// This is called at library load time, so the system is not initialized
// properly yet.
}

ResumeMovieView::~ResumeMovieView()
{
}

void ResumeMovieView::OneTimeInit( const char * launchIntent )
{
	LOG( "ResumeMovieView::OneTimeInit" );

	const double start = ovr_GetTimeInSeconds();

	CreateMenu( Cinema.app, Cinema.app->GetVRMenuMgr(), Cinema.app->GetDefaultFont() );

	LOG( "ResumeMovieView::OneTimeInit: %3.1f seconds", ovr_GetTimeInSeconds() - start );
}

void ResumeMovieView::OneTimeShutdown()
{
	LOG( "ResumeMovieView::OneTimeShutdown" );
}

void ResumeMovieView::OnOpen()
{
	LOG( "OnOpen" );

	Cinema.SceneMgr.LightsOn( 0.5f );

	Cinema.app->GetSwapParms().WarpProgram = WP_CHROMATIC;

	SetPosition(Cinema.app->GetVRMenuMgr(), Cinema.SceneMgr.Scene.FootPos);

	Cinema.SceneMgr.ClearGazeCursorGhosts();
	Cinema.app->GetGuiSys().OpenMenu( Cinema.app, Cinema.app->GetGazeCursor(), "ResumeMoviePrompt" );

	CurViewState = VIEWSTATE_OPEN;
}

void ResumeMovieView::OnClose()
{
	LOG( "OnClose" );

	Cinema.app->GetGuiSys().CloseMenu( Cinema.app, Menu, false );

	CurViewState = VIEWSTATE_CLOSED;
}

bool ResumeMovieView::Command( const char * msg )
{
	return false;
}

bool ResumeMovieView::OnKeyEvent( const int keyCode, const KeyState::eKeyEventType eventType )
{
	return false;
}

Matrix4f ResumeMovieView::DrawEyeView( const int eye, const float fovDegrees )
{
	return Cinema.SceneMgr.DrawEyeView( eye, fovDegrees );
}

void ResumeMovieView::SetPosition( OvrVRMenuMgr & menuMgr, const Vector3f &pos )
{
    menuHandle_t centerRootHandle = Menu->HandleForId( menuMgr, ID_CENTER_ROOT );
    VRMenuObject * centerRoot = menuMgr.ToObject( centerRootHandle );
    OVR_ASSERT( centerRoot != NULL );

    Posef pose = centerRoot->GetLocalPose();
    pose.Position = pos;
    centerRoot->SetLocalPose( pose );
}

void ResumeMovieView::CreateMenu( App * app, OvrVRMenuMgr & menuMgr, BitmapFont const & font )
{
	Menu = VRMenu::Create( "ResumeMoviePrompt" );

    Vector3f fwd( 0.0f, 0.0f, 1.0f );
	Vector3f up( 0.0f, 1.0f, 0.0f );
	Vector3f defaultScale( 1.0f );

    Array< VRMenuObjectParms const * > parms;

	VRMenuFontParms fontParms( true, true, false, false, false, 1.3f );

	Quatf orientation( Vector3f( 0.0f, 1.0f, 0.0f ), 0.0f );
	Vector3f centerPos( 0.0f, 0.0f, 0.0f );

	VRMenuObjectParms centerRootParms( VRMENU_CONTAINER, Array< VRMenuComponent* >(), VRMenuSurfaceParms(), "CenterRoot",
			Posef( orientation, centerPos ), Vector3f( 1.0f, 1.0f, 1.0f ), fontParms,
			ID_CENTER_ROOT, VRMenuObjectFlags_t(), VRMenuObjectInitFlags_t( VRMENUOBJECT_INIT_FORCE_POSITION ) );
    parms.PushBack( &centerRootParms );

	Menu->InitWithItems( menuMgr, font, 0.0f, VRMenuFlags_t(), parms );
    parms.Clear();

    // the centerroot item will get touch relative and touch absolute events and use them to rotate the centerRoot
    menuHandle_t centerRootHandle = Menu->HandleForId( menuMgr, ID_CENTER_ROOT );
    VRMenuObject * centerRoot = menuMgr.ToObject( centerRootHandle );
    OVR_ASSERT( centerRoot != NULL );

    // ==============================================================================
    //
    // title
    //
    {
        Posef panelPose( Quatf( up, 0.0f ), Vector3f( 0.0f, 2.2f, -3.0f ) );

		VRMenuObjectParms p( VRMENU_STATIC, Array< VRMenuComponent* >(),
				VRMenuSurfaceParms(), CinemaStrings::ResumeMenu_Title.ToCStr(), panelPose, defaultScale, fontParms, VRMenuId_t( ID_TITLE.Get() ),
				VRMenuObjectFlags_t(), VRMenuObjectInitFlags_t( VRMENUOBJECT_INIT_FORCE_POSITION ) );

		parms.PushBack( &p );

		Menu->AddItems( menuMgr, font, parms, centerRootHandle, false );
		parms.Clear();
    }

    // ==============================================================================
    //
    // options
    //
    Array<const char *> options;
    options.PushBack( CinemaStrings::ResumeMenu_Resume.ToCStr() );
    options.PushBack( CinemaStrings::ResumeMenu_Restart.ToCStr() );

    Array<const char *> icons;
    icons.PushBack( "assets/resume.png" );
    icons.PushBack( "assets/restart.png" );

    Array<PanelPose> optionPositions;
    optionPositions.PushBack( PanelPose( Quatf( up, 0.0f / 180.0f * Mathf::Pi ), Vector3f( -0.5f, 1.7f, -3.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) ) );
    optionPositions.PushBack( PanelPose( Quatf( up, 0.0f / 180.0f * Mathf::Pi ), Vector3f(  0.5f, 1.7f, -3.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) ) );

    int borderWidth = 0, borderHeight = 0;
    GLuint borderTexture = LoadTextureFromApplicationPackage( "assets/resume_restart_border.png", TextureFlags_t( TEXTUREFLAG_NO_DEFAULT ), borderWidth, borderHeight );

	for ( int i = 0; i < optionPositions.GetSizeI(); ++i )
	{
		ResumeMovieComponent * resumeMovieComponent = new ResumeMovieComponent( this, i );
		Array< VRMenuComponent* > optionComps;
		optionComps.PushBack( resumeMovieComponent );

		VRMenuSurfaceParms panelSurfParms( "",
				borderTexture, borderWidth, borderHeight, SURFACE_TEXTURE_ADDITIVE,
				0, 0, 0, SURFACE_TEXTURE_MAX,
				0, 0, 0, SURFACE_TEXTURE_MAX );

		Posef panelPose( optionPositions[ i ].Orientation, optionPositions[ i ].Position );
		VRMenuObjectParms * p = new VRMenuObjectParms( VRMENU_BUTTON, optionComps,
				panelSurfParms, options[ i ], panelPose, defaultScale, fontParms, VRMenuId_t( ID_OPTIONS.Get() + i ),
				VRMenuObjectFlags_t(), VRMenuObjectInitFlags_t( VRMENUOBJECT_INIT_FORCE_POSITION ) );

		parms.PushBack( p );

		Menu->AddItems( menuMgr, font, parms, centerRootHandle, false );
		DeletePointerArray( parms );
		parms.Clear();

		// add icon
	    menuHandle_t optionHandle = centerRoot->ChildHandleForId( menuMgr, VRMenuId_t( ID_OPTIONS.Get() + i ) );
	    VRMenuObject * optionObject = menuMgr.ToObject( optionHandle );
	    OVR_ASSERT( optionObject != NULL );

	    int iconWidth = 0, iconHeight = 0;
	    GLuint iconTexture = LoadTextureFromApplicationPackage( icons[ i ], TextureFlags_t( TEXTUREFLAG_NO_DEFAULT ), iconWidth, iconHeight );

		VRMenuSurfaceParms iconSurfParms( "",
				iconTexture, iconWidth, iconHeight, SURFACE_TEXTURE_DIFFUSE,
				0, 0, 0, SURFACE_TEXTURE_MAX,
				0, 0, 0, SURFACE_TEXTURE_MAX );


		Bounds3f textBounds = optionObject->GetTextLocalBounds( font );
		optionObject->SetTextLocalPosition( Vector3f( iconWidth * VRMenuObject::DEFAULT_TEXEL_SCALE * 0.5f, 0.0f, 0.0f ) );

		Posef iconPose( optionPositions[ i ].Orientation, optionPositions[ i ].Position + Vector3f( textBounds.GetMins().x, 0.0f, 0.01f ) );
		p = new VRMenuObjectParms( VRMENU_STATIC, Array< VRMenuComponent* >(),
				iconSurfParms, NULL, iconPose, defaultScale, fontParms, VRMenuId_t( ID_OPTION_ICONS.Get() + i ),
				VRMenuObjectFlags_t( VRMENUOBJECT_DONT_HIT_ALL ), VRMenuObjectInitFlags_t( VRMENUOBJECT_INIT_FORCE_POSITION ) );

		parms.PushBack( p );

		Menu->AddItems( menuMgr, font, parms, centerRootHandle, false );
	    DeletePointerArray( parms );
	    parms.Clear();

	    menuHandle_t iconHandle = centerRoot->ChildHandleForId( menuMgr, VRMenuId_t( ID_OPTION_ICONS.Get() + i ) );
	    resumeMovieComponent->Icon = menuMgr.ToObject( iconHandle );
	}

    Cinema.app->GetGuiSys().AddMenu( Menu );
}

void ResumeMovieView::ResumeChoice( int itemNum )
{
	if ( itemNum == 0 )
	{
		Cinema.ResumeMovieFromSavedLocation();
	}
	else
	{
		Cinema.PlayMovieFromBeginning();
	}
}

Matrix4f ResumeMovieView::Frame( const VrFrame & vrFrame )
{
	// We want 4x MSAA in the selection screen
	EyeParms eyeParms = Cinema.app->GetEyeParms();
	eyeParms.multisamples = 4;
	Cinema.app->SetEyeParms( eyeParms );

	if ( Menu->IsClosedOrClosing() && !Menu->IsOpenOrOpening() )
	{
		if ( Cinema.InLobby )
		{
			Cinema.TheaterSelection();
		}
		else
		{
			Cinema.MovieSelection( false );
		}
	}

	return Cinema.SceneMgr.Frame( vrFrame );
}

} // namespace OculusCinema
