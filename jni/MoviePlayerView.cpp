/************************************************************************************

Filename    :   MoviePlayerView.cpp
Content     :
Created     :	6/17/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include <android/keycodes.h>
#include "CinemaApp.h"
#include "Native.h"
#include "VRMenu/VRMenuMgr.h"
#include "Kernel/OVR_String_Utils.h"

#include "CinemaStrings.h"

namespace VRMatterStreamTheater
{

const int MoviePlayerView::MaxSeekSpeed = 5;
const int MoviePlayerView::ScrubBarWidth = 516;

const double MoviePlayerView::GazeTimeTimeout = 4;

MoviePlayerView::MoviePlayerView( CinemaApp &cinema ) :
	View( "MoviePlayerView" ),
	Cinema( cinema ),
	uiActive( false ),
	RepositionScreen( false ),
	BackgroundTintTexture(),
	RWTexture(),
	RWHoverTexture(),
	RWPressedTexture(),
	FFTexture(),
	FFHoverTexture(),
	FFPressedTexture(),
	PlayTexture(),
	PlayHoverTexture(),
	PlayPressedTexture(),
	PauseTexture(),
	PauseHoverTexture(),
	PausePressedTexture(),
	CarouselTexture(),
	CarouselHoverTexture(),
	CarouselPressedTexture(),
	SeekbarBackgroundTexture(),
	SeekbarProgressTexture(),
	SeekPosition(),
	SeekFF2x(),
	SeekFF4x(),
	SeekFF8x(),
	SeekFF16x(),
	SeekFF32x(),
	SeekRW2x(),
	SeekRW4x(),
	SeekRW8x(),
	SeekRW16x(),
	SeekRW32x(),
	MoveScreenMenu( NULL ),
	MoveScreenLabel( Cinema ),
	MoveScreenAlpha(),
	PlaybackControlsMenu( NULL ),
	PlaybackControlsPosition( Cinema ),
	PlaybackControlsScale( Cinema ),
	MovieTitleLabel( Cinema ),
	SeekIcon( Cinema ),
	ControlsBackground( Cinema ),
	GazeTimer(),
	RewindButton( Cinema ),
	PlayButton( Cinema ),
	FastForwardButton( Cinema ),
	CarouselButton( Cinema ),
	SeekbarBackground( Cinema ),
	SeekbarProgress( Cinema ),
	ScrubBar(),
	CurrentTime( Cinema ),
	SeekTime( Cinema ),
	BackgroundClicked( false ),
	UIOpened( false ),
	s00(0.0f),s01(0.0f),s10(0.0f),s11(0.0f),
	allowDrag(false),
	clickStartTime(0.0),
	lastScroll(0),
	lastMouse()
{
}

MoviePlayerView::~MoviePlayerView()
{
}

//=========================================================================================

void MoviePlayerView::OneTimeInit( const char * launchIntent )
{
	LOG( "MoviePlayerView::OneTimeInit" );

	const double start = ovr_GetTimeInSeconds();

	GazeUserId = Cinema.app->GetGazeCursor().GenerateUserId();

	CreateMenu( Cinema.app, Cinema.app->GetVRMenuMgr(), Cinema.app->GetDefaultFont() );

	LOG( "MoviePlayerView::OneTimeInit: %3.1f seconds", ovr_GetTimeInSeconds() - start );
}

void MoviePlayerView::OneTimeShutdown()
{
	LOG( "MoviePlayerView::OneTimeShutdown" );
}

float PixelScale( const float x )
{
	return x * VRMenuObject::DEFAULT_TEXEL_SCALE;
}

Vector3f PixelPos( const float x, const float y, const float z )
{
	return Vector3f( PixelScale( x ), PixelScale( y ), PixelScale( z ) );
}

void PlayPressedCallback( UIButton *button, void *object )
{

}

void RewindPressedCallback( UIButton *button, void *object )
{
	( ( MoviePlayerView * )object )->RewindPressed();
}

void FastForwardPressedCallback( UIButton *button, void *object )
{
	( ( MoviePlayerView * )object )->FastForwardPressed();
}

void CarouselPressedCallback( UIButton *button, void *object )
{
	( ( MoviePlayerView * )object )->CarouselPressed();
}

void ScrubBarCallback( ScrubBarComponent *scrubbar, void *object, const float progress )
{
	( ( MoviePlayerView * )object )->ScrubBarClicked( progress );
}

void MoviePlayerView::CreateMenu( App * app, OvrVRMenuMgr & menuMgr, BitmapFont const & font )
{
	BackgroundTintTexture.LoadTextureFromApplicationPackage( "assets/backgroundTint.png" );

	RWTexture.LoadTextureFromApplicationPackage( "assets/img_btn_rw.png" );
	RWHoverTexture.LoadTextureFromApplicationPackage( "assets/img_btn_rw_hover.png" );
	RWPressedTexture.LoadTextureFromApplicationPackage( "assets/img_btn_rw_pressed.png" );

	FFTexture.LoadTextureFromApplicationPackage( "assets/img_btn_ff.png" );
	FFHoverTexture.LoadTextureFromApplicationPackage( "assets/img_btn_ff_hover.png" );
	FFPressedTexture.LoadTextureFromApplicationPackage( "assets/img_btn_ff_pressed.png" );

	PlayTexture.LoadTextureFromApplicationPackage( "assets/img_btn_play.png" );
	PlayHoverTexture.LoadTextureFromApplicationPackage( "assets/img_btn_play_hover.png" );
	PlayPressedTexture.LoadTextureFromApplicationPackage( "assets/img_btn_play_pressed.png" );

	PauseTexture.LoadTextureFromApplicationPackage( "assets/img_btn_pause.png" );
	PauseHoverTexture.LoadTextureFromApplicationPackage( "assets/img_btn_pause_hover.png" );
	PausePressedTexture.LoadTextureFromApplicationPackage( "assets/img_btn_pause_pressed.png" );

	CarouselTexture.LoadTextureFromApplicationPackage( "assets/img_btn_carousel.png" );
	CarouselHoverTexture.LoadTextureFromApplicationPackage( "assets/img_btn_carousel_hover.png" );
	CarouselPressedTexture.LoadTextureFromApplicationPackage( "assets/img_btn_carousel_pressed.png" );

	SeekbarBackgroundTexture.LoadTextureFromApplicationPackage( "assets/img_seekbar_background.png" );
	SeekbarProgressTexture.LoadTextureFromApplicationPackage( "assets/img_seekbar_progress_blue.png" );

	SeekPosition.LoadTextureFromApplicationPackage( "assets/img_seek_position.png" );

	SeekFF2x.LoadTextureFromApplicationPackage( "assets/img_seek_ff2x.png" );
	SeekFF4x.LoadTextureFromApplicationPackage( "assets/img_seek_ff4x.png" );
	SeekFF8x.LoadTextureFromApplicationPackage( "assets/img_seek_ff8x.png" );
	SeekFF16x.LoadTextureFromApplicationPackage( "assets/img_seek_ff16x.png" );
	SeekFF32x.LoadTextureFromApplicationPackage( "assets/img_seek_ff32x.png" );

	SeekRW2x.LoadTextureFromApplicationPackage( "assets/img_seek_rw2x.png" );
	SeekRW4x.LoadTextureFromApplicationPackage( "assets/img_seek_rw4x.png" );
	SeekRW8x.LoadTextureFromApplicationPackage( "assets/img_seek_rw8x.png" );
	SeekRW16x.LoadTextureFromApplicationPackage( "assets/img_seek_rw16x.png" );
	SeekRW32x.LoadTextureFromApplicationPackage( "assets/img_seek_rw32x.png" );

    // ==============================================================================
    //
    // reorient message
    //
	MoveScreenMenu = new UIMenu( Cinema );
	MoveScreenMenu->Create( "MoviePlayerMenu" );
	MoveScreenMenu->SetFlags( VRMenuFlags_t( VRMENU_FLAG_TRACK_GAZE ) | VRMenuFlags_t( VRMENU_FLAG_BACK_KEY_DOESNT_EXIT ) );

	MoveScreenLabel.AddToMenu( MoveScreenMenu, NULL );
    MoveScreenLabel.SetLocalPose( Quatf( Vector3f( 0.0f, 1.0f, 0.0f ), 0.0f ), Vector3f( 0.0f, 0.0f, -1.8f ) );
    MoveScreenLabel.GetMenuObject()->AddFlags( VRMenuObjectFlags_t( VRMENUOBJECT_DONT_HIT_ALL ) );
    MoveScreenLabel.SetFontScale( 0.5f );
    MoveScreenLabel.SetText( CinemaStrings::MoviePlayer_Reorient );
    MoveScreenLabel.SetTextOffset( Vector3f( 0.0f, -24 * VRMenuObject::DEFAULT_TEXEL_SCALE, 0.0f ) );  // offset to be below gaze cursor
    MoveScreenLabel.SetVisible( false );

    // ==============================================================================
    //
    // Playback controls
    //
    PlaybackControlsMenu = new UIMenu( Cinema );
    PlaybackControlsMenu->Create( "PlaybackControlsMenu" );
    PlaybackControlsMenu->SetFlags( VRMenuFlags_t( VRMENU_FLAG_BACK_KEY_DOESNT_EXIT ) );

    PlaybackControlsPosition.AddToMenu( PlaybackControlsMenu );
    PlaybackControlsScale.AddToMenu( PlaybackControlsMenu, &PlaybackControlsPosition );
    PlaybackControlsScale.SetLocalPosition( Vector3f( 0.0f, 0.0f, 0.05f ) );
    PlaybackControlsScale.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 1080, 1080 );

	// ==============================================================================
    //
    // movie title
    //
    MovieTitleLabel.AddToMenu( PlaybackControlsMenu, &PlaybackControlsScale );
    MovieTitleLabel.SetLocalPosition( PixelPos( 0, 266, 0 ) );
    MovieTitleLabel.SetFontScale( 1.4f );
    MovieTitleLabel.SetText( "" );
    MovieTitleLabel.SetTextOffset( Vector3f( 0.0f, 0.0f, 0.01f ) );
    MovieTitleLabel.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 320, 120 );

	// ==============================================================================
    //
    // seek icon
    //
    SeekIcon.AddToMenu( PlaybackControlsMenu, &PlaybackControlsScale );
    SeekIcon.SetLocalPosition( PixelPos( 0, 0, 0 ) );
    SeekIcon.SetLocalScale( Vector3f( 2.0f ) );
    SetSeekIcon( 0 );

    // ==============================================================================
    //
    // controls
    //
    ControlsBackground.AddToMenuFlags( PlaybackControlsMenu, &PlaybackControlsScale, VRMenuObjectFlags_t( VRMENUOBJECT_RENDER_HIERARCHY_ORDER ) );
    ControlsBackground.SetLocalPosition( PixelPos( 0, -288, 0 ) );
    ControlsBackground.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 1004, 168 );
    ControlsBackground.AddComponent( &GazeTimer );

    RewindButton.AddToMenu( PlaybackControlsMenu, &ControlsBackground );
    RewindButton.SetLocalPosition( PixelPos( -448, 0, 1 ) );
    RewindButton.SetLocalScale( Vector3f( 2.0f ) );
    RewindButton.SetButtonImages( RWTexture, RWHoverTexture, RWPressedTexture );
    RewindButton.SetOnClick( RewindPressedCallback, this );

	FastForwardButton.AddToMenu( PlaybackControlsMenu, &ControlsBackground );
	FastForwardButton.SetLocalPosition( PixelPos( -234, 0, 1 ) );
	FastForwardButton.SetLocalScale( Vector3f( 2.0f ) );
	FastForwardButton.SetButtonImages( FFTexture, FFHoverTexture, FFPressedTexture );
	FastForwardButton.SetOnClick( FastForwardPressedCallback, this );
	FastForwardButton.GetMenuObject()->SetLocalBoundsExpand( Vector3f::ZERO, PixelPos( -20, 0, 0 ) );

	// playbutton created after fast forward button to fix z issues
    PlayButton.AddToMenu( PlaybackControlsMenu, &ControlsBackground );
    PlayButton.SetLocalPosition( PixelPos( -341, 0, 2 ) );
    PlayButton.SetLocalScale( Vector3f( 2.0f ) );
    PlayButton.SetButtonImages( PauseTexture, PauseHoverTexture, PausePressedTexture );
    PlayButton.SetOnClick( PlayPressedCallback, this );

	CarouselButton.AddToMenu( PlaybackControlsMenu, &ControlsBackground );
	CarouselButton.SetLocalPosition( PixelPos( 418, 0, 1 ) );
	CarouselButton.SetLocalScale( Vector3f( 2.0f ) );
	CarouselButton.SetButtonImages( CarouselTexture, CarouselHoverTexture, CarouselPressedTexture );
	CarouselButton.SetOnClick( CarouselPressedCallback, this );
	CarouselButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

	SeekbarBackground.AddToMenu( PlaybackControlsMenu, &ControlsBackground );
	SeekbarBackground.SetLocalPosition( PixelPos( 78, 0, 2 ) );
	SeekbarBackground.SetColor( Vector4f( 0.5333f, 0.5333f, 0.5333f, 1.0f ) );
	SeekbarBackground.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekbarBackgroundTexture, ScrubBarWidth + 6, 46 );
	SeekbarBackground.AddComponent( &ScrubBar );

	SeekbarProgress.AddToMenu( PlaybackControlsMenu, &SeekbarBackground );
	SeekbarProgress.SetLocalPosition( PixelPos( 0, 0, 1 ) );
	SeekbarProgress.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekbarProgressTexture, ScrubBarWidth, 40 );
	SeekbarProgress.GetMenuObject()->AddFlags( VRMenuObjectFlags_t( VRMENUOBJECT_DONT_HIT_ALL ) );

	CurrentTime.AddToMenu( PlaybackControlsMenu, &SeekbarBackground );
	CurrentTime.SetLocalPosition( PixelPos( -234, 52, 2 ) );
	CurrentTime.SetLocalScale( Vector3f( 1.0f ) );
	CurrentTime.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekPosition );
	CurrentTime.SetText( "2:33:33" );
	CurrentTime.SetTextOffset( PixelPos( 0, 6, 1 ) );
	CurrentTime.SetFontScale( 0.71f );
	CurrentTime.SetColor( Vector4f( 0 / 255.0f, 93 / 255.0f, 219 / 255.0f, 1.0f ) );
	CurrentTime.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	CurrentTime.GetMenuObject()->AddFlags( VRMenuObjectFlags_t( VRMENUOBJECT_DONT_HIT_ALL ) );

	SeekTime.AddToMenu( PlaybackControlsMenu, &SeekbarBackground );
	SeekTime.SetLocalPosition( PixelPos( -34, 52, 4 ) );
	SeekTime.SetLocalScale( Vector3f( 1.0f ) );
	SeekTime.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekPosition );
	SeekTime.SetText( "2:33:33" );
	SeekTime.SetTextOffset( PixelPos( 0, 6, 1 ) );
	SeekTime.SetFontScale( 0.71f );
	SeekTime.SetColor( Vector4f( 47.0f / 255.0f, 70 / 255.0f, 89 / 255.0f, 1.0f ) );
	SeekTime.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	SeekTime.GetMenuObject()->AddFlags( VRMenuObjectFlags_t( VRMENUOBJECT_DONT_HIT_ALL ) );

	ScrubBar.SetWidgets( &SeekbarBackground, &SeekbarProgress, &CurrentTime, &SeekTime, ScrubBarWidth );
	ScrubBar.SetOnClick( ScrubBarCallback, this );
}

void MoviePlayerView::SetSeekIcon( const int seekSpeed )
{
	const UITexture * texture = NULL;

	switch( seekSpeed )
	{
		case -5 : texture = &SeekRW32x; break;
		case -4 : texture = &SeekRW16x; break;
		case -3 : texture = &SeekRW8x; break;
		case -2 : texture = &SeekRW4x; break;
		case -1 : texture = &SeekRW2x; break;

		default:
		case 0 : texture = NULL; break;

		case 1 : texture = &SeekFF2x; break;
		case 2 : texture = &SeekFF4x; break;
		case 3 : texture = &SeekFF8x; break;
		case 4 : texture = &SeekFF16x; break;
		case 5 : texture = &SeekFF32x; break;
	}

	if ( texture == NULL )
	{
	    SeekIcon.SetVisible( false );
	}
	else
	{
	    SeekIcon.SetVisible( true );
	    SeekIcon.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, *texture );
	}
}

void MoviePlayerView::OnOpen()
{
	LOG( "OnOpen" );
	CurViewState = VIEWSTATE_OPEN;

	Cinema.SceneMgr.ClearMovie();

	SetSeekIcon( 1 );

	ScrubBar.SetProgress( 0.0f );

	RepositionScreen = false;
	MoveScreenAlpha.Set( 0, 0, 0, 0.0f );

	HideUI();
	Cinema.SceneMgr.LightsOff( 1.5f );

	Cinema.StartMoviePlayback();

	MovieTitleLabel.SetText( Cinema.GetCurrentMovie()->Name );
	Bounds3f titleBounds = MovieTitleLabel.GetTextLocalBounds( Cinema.app->GetDefaultFont() ) * VRMenuObject::TEXELS_PER_METER;
	MovieTitleLabel.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, titleBounds.GetSize().x + 88, titleBounds.GetSize().y + 32 );

	PlayButton.SetButtonImages( PauseTexture, PauseHoverTexture, PausePressedTexture );
}

void MoviePlayerView::OnClose()
{
	LOG( "OnClose" );
	CurViewState = VIEWSTATE_CLOSED;
	HideUI();
	Cinema.app->GetGazeCursor().ShowCursor();

	if ( MoveScreenMenu->IsOpen() )
	{
		MoveScreenLabel.SetVisible( false );
		MoveScreenMenu->Close();
	}

	Cinema.SceneMgr.ClearMovie();

	if ( Cinema.SceneMgr.VoidedScene )
	{
		Cinema.SceneMgr.SetSceneModel( Cinema.GetCurrentTheater() );
	}
}

/*
 * Command
 *
 * Actions that need to be performed on the render thread.
 */
bool MoviePlayerView::Command( const char * msg )
{
	if ( CurViewState != VIEWSTATE_OPEN )
	{
		return false;
	}

	if ( MatchesHead( "resume ", msg ) )
	{
		Cinema.AppSelection( false );
		return false;	// allow VrLib to handle it, too
	}
	else if ( MatchesHead( "pause ", msg ) )
	{
		Native::StopMovie( Cinema.app );
		return false;	// allow VrLib to handle it, too
	}

	return false;
}

void MoviePlayerView::MovieLoaded( const int width, const int height, const int duration )
{
	ScrubBar.SetDuration( duration );
}

void MoviePlayerView::BackPressed()
{
	LOG( "BackPressed" );
	HideUI();
	if ( Cinema.AllowTheaterSelection() )
	{
		LOG( "Opening TheaterSelection" );
		Cinema.TheaterSelection();
	}
	else
	{
		LOG( "Opening MovieSelection" );
		Cinema.AppSelection( true );
	}
}

bool MoviePlayerView::OnKeyEvent( const int keyCode, const KeyState::eKeyEventType eventType )
{
	switch ( keyCode )
	{
		case AKEYCODE_BACK:
		{
			switch ( eventType )
			{
				case KeyState::KEY_EVENT_SHORT_PRESS:
				LOG( "KEY_EVENT_SHORT_PRESS" );
				BackPressed();
				return true;
				break;
				default:
				//LOG( "unexpected back key state %i", eventType );
				break;
			}
		}
		break;
		case AKEYCODE_MEDIA_NEXT:
			if ( eventType == KeyState::KEY_EVENT_UP )
			{
				Cinema.SetMovie( Cinema.GetNextMovie() );
				Cinema.ResumeOrRestartMovie();
			}
			break;
		case AKEYCODE_MEDIA_PREVIOUS:
			if ( eventType == KeyState::KEY_EVENT_UP )
			{
				Cinema.SetMovie( Cinema.GetPreviousMovie() );
				Cinema.ResumeOrRestartMovie();
			}
			break;
		break;
	}
	return false;
}

//=========================================================================================

static bool InsideUnit( const Vector2f v )
{
	return v.x > -1.0f && v.x < 1.0f && v.y > -1.0f && v.y < 1.0f;
}

void MoviePlayerView::ShowUI()
{
	LOG( "ShowUI" );
	Cinema.SceneMgr.ForceMono = true;
	Cinema.app->GetGazeCursor().ShowCursor();

	PlaybackControlsMenu->Open();
	GazeTimer.SetGazeTime();

	PlaybackControlsScale.SetLocalScale( Vector3f( Cinema.SceneMgr.GetScreenSize().y * ( 500.0f / 1080.0f ) ) );
	PlaybackControlsPosition.SetLocalPose( Cinema.SceneMgr.GetScreenPose() );

	uiActive = true;
}

void MoviePlayerView::HideUI()
{
	LOG( "HideUI" );
	PlaybackControlsMenu->Close();

	Cinema.app->GetGazeCursor().HideCursor();
	Cinema.SceneMgr.ForceMono = false;
	uiActive = false;

	BackgroundClicked = false;

	SetSeekIcon( 1 );
}

void MoviePlayerView::CheckDebugControls( const VrFrame & vrFrame )
{
	if ( !Cinema.AllowDebugControls )
	{
		return;
	}

#if 0
	if ( !( vrFrame.Input.buttonState & BUTTON_Y ) )
	{
		Cinema.SceneMgr.ChangeSeats( vrFrame );
	}
#endif

	if ( vrFrame.Input.buttonPressed & BUTTON_X )
	{
		Cinema.SceneMgr.ToggleLights( 0.5f );
	}

	if ( vrFrame.Input.buttonPressed & BUTTON_SELECT )
	{
		Cinema.SceneMgr.UseOverlay = !Cinema.SceneMgr.UseOverlay;
		Cinema.app->CreateToast( "Overlay: %i", Cinema.SceneMgr.UseOverlay );
	}

	// Press Y to toggle FreeScreen mode, while holding the scale and distance can be adjusted
	if ( vrFrame.Input.buttonPressed & BUTTON_Y )
	{
		Cinema.SceneMgr.FreeScreenActive = !Cinema.SceneMgr.FreeScreenActive || Cinema.SceneMgr.SceneInfo.UseFreeScreen;
		Cinema.SceneMgr.PutScreenInFront();
	}

	if ( Cinema.SceneMgr.FreeScreenActive && ( vrFrame.Input.buttonState & BUTTON_Y ) )
	{
		Cinema.SceneMgr.FreeScreenDistance -= vrFrame.Input.sticks[0][1] * vrFrame.DeltaSeconds * 3;
		Cinema.SceneMgr.FreeScreenDistance = OVR::Alg::Clamp( Cinema.SceneMgr.FreeScreenDistance, 1.0f, 4.0f );
		Cinema.SceneMgr.FreeScreenScale += vrFrame.Input.sticks[0][0] * vrFrame.DeltaSeconds * 3;
		Cinema.SceneMgr.FreeScreenScale = OVR::Alg::Clamp( Cinema.SceneMgr.FreeScreenScale, 1.0f, 4.0f );

		if ( vrFrame.Input.buttonReleased & (BUTTON_LSTICK_UP|BUTTON_LSTICK_DOWN|BUTTON_LSTICK_LEFT|BUTTON_LSTICK_RIGHT) )
		{
			Cinema.app->CreateToast( "FreeScreenDistance:%3.1f  FreeScreenScale:%3.1f", Cinema.SceneMgr.FreeScreenDistance, Cinema.SceneMgr.FreeScreenScale );
		}
	}
}

static Vector3f	MatrixOrigin( const Matrix4f & m )
{
	return Vector3f( -m.M[0][3], -m.M[1][3], -m.M[2][3] );
}

static Vector3f	MatrixForward( const Matrix4f & m )
{
	return Vector3f( -m.M[2][0], -m.M[2][1], -m.M[2][2] );
}

// -1 to 1 range on screenMatrix, returns -2,-2 if looking away from the screen
Vector2f MoviePlayerView::GazeCoordinatesOnScreen( const Matrix4f & viewMatrix, const Matrix4f screenMatrix ) const
{
	// project along -Z in the viewMatrix onto the Z = 0 plane of screenMatrix
	const Vector3f viewForward = MatrixForward( viewMatrix ).Normalized();

	Vector3f screenForward;
	if ( Cinema.SceneMgr.FreeScreenActive )
	{
		// FIXME: free screen matrix is inverted compared to bounds screen matrix.
		screenForward = -Vector3f( screenMatrix.M[0][2], screenMatrix.M[1][2], screenMatrix.M[2][2] ).Normalized();
	}
	else
	{
		screenForward = -MatrixForward( screenMatrix ).Normalized();
	}

	const float approach = viewForward.Dot( screenForward );
	if ( approach <= 0.1f )
	{
		// looking away
		return Vector2f( -2.0f, -2.0f );
	}

	const Matrix4f panelInvert = screenMatrix.Inverted();
	const Matrix4f viewInvert = viewMatrix.Inverted();

	const Vector3f viewOrigin = viewInvert.Transform( Vector3f( 0.0f ) );
	const Vector3f panelOrigin = MatrixOrigin( screenMatrix );

	// Should we disallow using panels from behind?
	const float d = panelOrigin.Dot( screenForward );
	const float t = -( viewOrigin.Dot( screenForward ) + d ) / approach;

	const Vector3f impact = viewOrigin + viewForward * t;
	const Vector3f localCoordinate = panelInvert.Transform( impact );

	return Vector2f( localCoordinate.x, localCoordinate.y );
}

void MoviePlayerView::CheckInput( const VrFrame & vrFrame )
{
	const Vector2f screenCursor = GazeCoordinatesOnScreen( Cinema.SceneMgr.Scene.CenterViewMatrix(), Cinema.SceneMgr.ScreenMatrix() );
	bool onscreen = false;
	if ( InsideUnit( screenCursor ) )
	{
		onscreen = true;
	}
	else if ( uiActive )
	{
		onscreen = GazeTimer.IsFocused();
	}

	if(onscreen) {
		Vector2f travel = screenCursor - lastMouse;
		lastMouse = screenCursor;
		Native::MouseMove(Cinema.app, 1280 / 2 * travel.x, 720 / -2 * travel.y );
	}

	if( (vrFrame.Input.buttonPressed & 0x3FFF) || (vrFrame.Input.buttonReleased & 0x3FFF) || s00 != vrFrame.Input.sticks[0][0] ||
			 s01 != vrFrame.Input.sticks[0][1] || s10 != vrFrame.Input.sticks[1][0] || s11 != vrFrame.Input.sticks[1][1] )
	{
		s00 = vrFrame.Input.sticks[0][0];
		s01 = vrFrame.Input.sticks[0][1];
		s10 = vrFrame.Input.sticks[1][0];
		s11 = vrFrame.Input.sticks[1][1];
		//LOG("Input! %f %f %f %f %i", s00, s01, s10, s11, vrFrame.Input.buttonState );
		Native::ControllerState(Cinema.app, s00, s01, s10, s11,
										0.0, 0.0, // What?  No trigger axis?  Oculus!
										vrFrame.Input.buttonState);
	}

	if ( ( vrFrame.Input.buttonReleased & BUTTON_TOUCH ) && !( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) )
	{
		// open ui if it's not visible
		Cinema.app->PlaySound( "touch_up" );
		if(onscreen) {
			if(allowDrag) Native::MouseClick(Cinema.app,1,true); // fast click!
			Native::MouseClick(Cinema.app,1,false);
		} else
			ShowUI();

		// ignore button A or touchpad until release so we don't close the UI immediately after opening it
		UIOpened = true;
	}

	if ( onscreen && ( vrFrame.Input.buttonPressed & BUTTON_TOUCH ) ) {
		clickStartTime = ovr_GetTimeInSeconds();
		allowDrag = true;
	}

	if( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) {
		allowDrag = false;
		if(!onscreen)
			ShowUI();
	}

	if ( onscreen && allowDrag && (clickStartTime + 0.25 < ovr_GetTimeInSeconds()) &&
			( vrFrame.Input.buttonState & BUTTON_TOUCH ) && !( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) )
	{
		Native::MouseClick(Cinema.app,1,true);
		allowDrag = false; // already dragging
	}


	if ( onscreen && ( vrFrame.Input.buttonPressed & BUTTON_SWIPE_BACK ) )
	{
		Native::MouseClick(Cinema.app,3,true);
		Cinema.app->PlaySound( "touch_up" );
	}

	if ( onscreen && ( vrFrame.Input.buttonPressed & BUTTON_SWIPE_FORWARD ) )
	{
		Native::MouseClick(Cinema.app,2,true);
		Cinema.app->PlaySound( "touch_up" );
	}

	if ( onscreen && ( vrFrame.Input.buttonPressed & BUTTON_SWIPE_UP ) )
	{
		char diff = lastScroll - (char)(127 * vrFrame.Input.swipeFraction);
		lastScroll = (char)(127 * vrFrame.Input.swipeFraction);
		Native::MouseScroll(Cinema.app, diff );
	}
	if ( onscreen && ( vrFrame.Input.buttonPressed & BUTTON_SWIPE_DOWN ) )
	{
		char diff = lastScroll - (char)(-127 * vrFrame.Input.swipeFraction);
		lastScroll = (char)(-127 * vrFrame.Input.swipeFraction);
		Native::MouseScroll(Cinema.app, diff );
	}
	if ( vrFrame.Input.buttonReleased & ( BUTTON_SWIPE_DOWN | BUTTON_SWIPE_UP ))
	{
		lastScroll = 0;
	}


	if ( Cinema.SceneMgr.FreeScreenActive )
	{
		if ( !onscreen )
		{
			// outside of screen, so show reposition message
			const double now = ovr_GetTimeInSeconds();
			float alpha = MoveScreenAlpha.Value( now );
			if ( alpha > 0.0f )
			{
				MoveScreenLabel.SetVisible( true );
				MoveScreenLabel.SetTextColor( Vector4f( alpha ) );
			}

			if ( vrFrame.Input.buttonPressed & BUTTON_TOUCH )
			{
				RepositionScreen = true;
			}
		}
		else
		{
			// onscreen, so hide message
			const double now = ovr_GetTimeInSeconds();
			MoveScreenAlpha.Set( now, -1.0f, now + 1.0f, 1.0f );
			MoveScreenLabel.SetVisible( false );
		}
	}

	// while we're holding down the button or touchpad, reposition screen
	if ( RepositionScreen )
	{
		if ( vrFrame.Input.buttonState & BUTTON_TOUCH )
		{
			Cinema.SceneMgr.PutScreenInFront();
		}
		else
		{
			RepositionScreen = false;
		}
	}
}

void MoviePlayerView::RewindPressed()
{

}

void MoviePlayerView::FastForwardPressed()
{

}

void MoviePlayerView::CarouselPressed()
{
	Cinema.AppSelection( false );
}

void MoviePlayerView::ScrubBarClicked( const float progress )
{

}

void MoviePlayerView::UpdateUI( const VrFrame & vrFrame )
{
	if ( uiActive )
	{
		double timeSinceLastGaze = ovr_GetTimeInSeconds() - GazeTimer.GetLastGazeTime();
		if ( !ScrubBar.IsScrubbing() && ( timeSinceLastGaze > GazeTimeTimeout ) )
		{
			LOG( "Gaze timeout" );
			HideUI();
		}

		// if we press the touchpad or a button outside of the playback controls, then close the UI
		if ( ( vrFrame.Input.buttonPressed & BUTTON_TOUCH ) != 0 )
		{
			// ignore touchpad until release so we don't close the UI immediately after opening it
			BackgroundClicked = !GazeTimer.IsFocused() && !UIOpened;
		}

		if ( ( ( ( vrFrame.Input.buttonReleased & BUTTON_TOUCH ) != 0 ) && ( ( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) == 0 ) )	)
		{
			if ( !GazeTimer.IsFocused() && BackgroundClicked )
			{
				LOG( "Clicked outside playback controls" );
				Cinema.app->PlaySound( "touch_up" );
				HideUI();
			}
			BackgroundClicked = false;
		}

		if ( Cinema.SceneMgr.FreeScreenActive )
		{
			// update the screen position & size;
			PlaybackControlsScale.SetLocalScale( Vector3f( Cinema.SceneMgr.GetScreenSize().y * ( 500.0f / 1080.0f ) ) );
			PlaybackControlsPosition.SetLocalPose( Cinema.SceneMgr.GetScreenPose() );
		}
	}

	// clear the flag for ignoring button A or touchpad until release
	UIOpened = false;
}

/*
 * DrawEyeView
 */
Matrix4f MoviePlayerView::DrawEyeView( const int eye, const float fovDegrees )
{
	return Cinema.SceneMgr.DrawEyeView( eye, fovDegrees );
}

/*
 * Frame()
 *
 * App override
 */
Matrix4f MoviePlayerView::Frame( const VrFrame & vrFrame )
{
	// Drop to 2x MSAA during playback, people should be focused
	// on the high quality screen.
	EyeParms eyeParms = Cinema.app->GetEyeParms();
	eyeParms.multisamples = 2;
	Cinema.app->SetEyeParms( eyeParms );

	if ( Native::HadPlaybackError( Cinema.app ) )
	{
		LOG( "Playback failed" );
		Cinema.UnableToPlayMovie();
	}
	else if ( Native::IsPlaybackFinished( Cinema.app ) )
	{
		LOG( "Playback finished" );
		Cinema.MovieFinished();
	}

	CheckInput( vrFrame );
	CheckDebugControls( vrFrame );
	UpdateUI( vrFrame );

	if ( Cinema.SceneMgr.FreeScreenActive && !MoveScreenMenu->IsOpen() )
	{
		MoveScreenMenu->Open();
	}
	else if ( !Cinema.SceneMgr.FreeScreenActive && MoveScreenMenu->IsOpen() )
	{
		MoveScreenMenu->Close();
	}

	return Cinema.SceneMgr.Frame( vrFrame );
}

/*************************************************************************************/

ControlsGazeTimer::ControlsGazeTimer() :
	VRMenuComponent( VRMenuEventFlags_t( VRMENU_EVENT_FRAME_UPDATE ) |
			VRMENU_EVENT_FOCUS_GAINED |
            VRMENU_EVENT_FOCUS_LOST ),
    LastGazeTime( 0 ),
    HasFocus( false )

{
}

void ControlsGazeTimer::SetGazeTime()
{
	LastGazeTime = ovr_GetTimeInSeconds();
}

eMsgStatus ControlsGazeTimer::OnEvent_Impl( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
    switch( event.EventType )
    {
    	case VRMENU_EVENT_FRAME_UPDATE:
    		if ( HasFocus )
    		{
    			LastGazeTime = ovr_GetTimeInSeconds();
    		}
    		return MSG_STATUS_ALIVE;
        case VRMENU_EVENT_FOCUS_GAINED:
        	HasFocus = true;
        	LastGazeTime = ovr_GetTimeInSeconds();
    		return MSG_STATUS_ALIVE;
        case VRMENU_EVENT_FOCUS_LOST:
        	HasFocus = false;
    		return MSG_STATUS_ALIVE;
        default:
            OVR_ASSERT( !"Event flags mismatch!" );
            return MSG_STATUS_ALIVE;
    }
}

/*************************************************************************************/

ScrubBarComponent::ScrubBarComponent() :
	VRMenuComponent( VRMenuEventFlags_t( VRMENU_EVENT_TOUCH_DOWN ) |
		VRMENU_EVENT_TOUCH_DOWN |
		VRMENU_EVENT_FRAME_UPDATE |
		VRMENU_EVENT_FOCUS_GAINED |
        VRMENU_EVENT_FOCUS_LOST ),
	HasFocus( false ),
	TouchDown( false ),
	Progress( 0.0f ),
	Duration( 0 ),
	Background( NULL ),
	ScrubBar( NULL ),
	CurrentTime( NULL ),
	SeekTime( NULL ),
	OnClickFunction( NULL ),
	OnClickObject( NULL )

{
}

void ScrubBarComponent::SetDuration( const int duration )
{
	Duration = duration;

	SetProgress( Progress );
}

void ScrubBarComponent::SetOnClick( void ( *callback )( ScrubBarComponent *, void *, float ), void *object )
{
	OnClickFunction = callback;
	OnClickObject = object;
}

void ScrubBarComponent::SetWidgets( UIWidget *background, UIWidget *scrubBar, UILabel *currentTime, UILabel *seekTime, const int scrubBarWidth )
{
	Background 		= background;
	ScrubBar 		= scrubBar;
	CurrentTime 	= currentTime;
	SeekTime 		= seekTime;
	ScrubBarWidth	= scrubBarWidth;

	SeekTime->SetVisible( false );
}

void ScrubBarComponent::SetProgress( const float progress )
{
	Progress = progress;
	const float seekwidth = ScrubBarWidth * progress;

	Vector3f pos = ScrubBar->GetLocalPosition();
	pos.x = PixelScale( ( ScrubBarWidth - seekwidth ) * -0.5f );
	ScrubBar->SetLocalPosition( pos );
	ScrubBar->SetSurfaceDims( 0, Vector2f( seekwidth, 40.0f ) );
	ScrubBar->RegenerateSurfaceGeometry( 0, false );

	pos = CurrentTime->GetLocalPosition();
	pos.x = PixelScale( ScrubBarWidth * -0.5f + seekwidth );
	CurrentTime->SetLocalPosition( pos );
	SetTimeText( CurrentTime, Duration * progress );
}

void ScrubBarComponent::SetTimeText( UILabel *label, const int time )
{
	int seconds = time / 1000;
	int minutes = seconds / 60;
	int hours = minutes / 60;
	seconds = seconds % 60;
	minutes = minutes % 60;

	if ( hours > 0 )
	{
		label->SetText( StringUtils::Va( "%d:%02d:%02d", hours, minutes, seconds ) );
	}
	else if ( minutes > 0 )
	{
		label->SetText( StringUtils::Va( "%d:%02d", minutes, seconds ) );
	}
	else
	{
		label->SetText( StringUtils::Va( "0:%02d", seconds ) );
	}
}

eMsgStatus ScrubBarComponent::OnEvent_Impl( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
    switch( event.EventType )
    {
		case VRMENU_EVENT_FOCUS_GAINED:
			HasFocus = true;
			return MSG_STATUS_ALIVE;

		case VRMENU_EVENT_FOCUS_LOST:
			HasFocus = false;
			return MSG_STATUS_ALIVE;

    	case VRMENU_EVENT_TOUCH_DOWN:
    		TouchDown = true;
    		OnClick( app, vrFrame, event );
    		return MSG_STATUS_ALIVE;

    	case VRMENU_EVENT_FRAME_UPDATE:
    		return OnFrame( app, vrFrame, menuMgr, self, event );

        default:
            OVR_ASSERT( !"Event flags mismatch!" );
            return MSG_STATUS_ALIVE;
    }
}

eMsgStatus ScrubBarComponent::OnFrame( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
	if ( TouchDown )
	{
		if ( ( vrFrame.Input.buttonState & ( BUTTON_A | BUTTON_TOUCH ) ) != 0 )
		{
			OnClick( app, vrFrame, event );
		}
		else
		{
			TouchDown = false;
		}
	}

	SeekTime->SetVisible( HasFocus );
	if ( HasFocus )
	{
		Vector3f hitPos = event.HitResult.RayStart + event.HitResult.RayDir * event.HitResult.t;

		// move hit position into local space
		const Posef modelPose = Background->GetWorldPose();
		Vector3f localHit = modelPose.Orientation.Inverted().Rotate( hitPos - modelPose.Position );

		Bounds3f bounds = Background->GetMenuObject()->GetLocalBounds( app->GetDefaultFont() ) * Background->GetParent()->GetWorldScale();
		const float progress = ( localHit.x - bounds.GetMins().x ) / bounds.GetSize().x;

		if ( ( progress >= 0.0f ) && ( progress <= 1.0f ) )
		{
			const float seekwidth = ScrubBarWidth * progress;
			Vector3f pos = SeekTime->GetLocalPosition();
			pos.x = PixelScale( ScrubBarWidth * -0.5f + seekwidth );
			SeekTime->SetLocalPosition( pos );

			SetTimeText( SeekTime, Duration * progress );
		}
	}

	return MSG_STATUS_ALIVE;
}

void ScrubBarComponent::OnClick( App * app, VrFrame const & vrFrame, VRMenuEvent const & event )
{
	if ( OnClickFunction == NULL )
	{
		return;
	}

	Vector3f hitPos = event.HitResult.RayStart + event.HitResult.RayDir * event.HitResult.t;

	// move hit position into local space
	const Posef modelPose = Background->GetWorldPose();
	Vector3f localHit = modelPose.Orientation.Inverted().Rotate( hitPos - modelPose.Position );

	Bounds3f bounds = Background->GetMenuObject()->GetLocalBounds( app->GetDefaultFont() ) * Background->GetParent()->GetWorldScale();
	const float progress = ( localHit.x - bounds.GetMins().x ) / bounds.GetSize().x;
	if ( ( progress >= 0.0f ) && ( progress <= 1.0f ) )
	{
		( *OnClickFunction )( this, OnClickObject, progress );
	}
}

} // namespace VRMatterStreamTheater
