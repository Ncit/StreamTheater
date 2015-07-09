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
#include "VRMenu/GuiSys.h"
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
	MoveScreenMenu( NULL ),
	MoveScreenLabel( Cinema ),
	MoveScreenAlpha(),
	PlaybackControlsMenu( NULL ),
	PlaybackControlsPosition( Cinema ),
	PlaybackControlsScale( Cinema ),
	MovieTitleLabel( Cinema ),
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
	MouseMenuButton( Cinema ),
	MouseMenu( NULL ),
	ButtonGaze( Cinema ),
	ButtonTrackpad( Cinema ),
	ButtonOff( Cinema ),
	ButtonXSensitivity( Cinema ),
	ButtonYSensitivity( Cinema ),
	StreamMenuButton( Cinema ),
	StreamMenu( NULL ),
	Button1080p60( Cinema ),
	Button1080p30( Cinema ),
	Button720p60( Cinema ),
	Button720p30( Cinema ),
	ButtonHostAudio( Cinema ),
	ScreenMenuButton( Cinema ),
	ScreenMenu( NULL ),
	ButtonSBS( Cinema ),
	ButtonChangeSeat( Cinema ),
	ButtonDistance( Cinema ),
	ButtonSize( Cinema ),
	ControllerMenuButton( Cinema ),
	ControllerMenu( NULL ),
	ButtonSpeed( Cinema ),
	ButtonComfortMode( Cinema ),
	ButtonMapKeyboard( Cinema ),
	BackgroundClicked( false ),
	UIOpened( false ),
	s00(0.0f),s01(0.0f),s10(0.0f),s11(0.0f),s20(0.0f),s21(0.0f),
	allowDrag(false),
	mouseMoving(false),
	clickStartTime(0.0),
	lastScroll(0),
	lastMouse(0.0,0.0),
	mouseDownLeft(false),
	mouseDownRight(false),
	mouseDownMiddle(false),
	mouseMode(MOUSE_GAZE),
	streamWidth(1280),
	streamHeight(720),
	streamFPS(60),
	streamHostAudio(true)
{
}

MoviePlayerView::~MoviePlayerView()
{
}

//=========================================================================================

void MoviePlayerView::OneTimeInit( const char * launchIntent )
{
	LOG( "MoviePlayerView::OneTimeInit" );

	const double start = vrapi_GetTimeInSeconds();

	GazeUserId = Cinema.GetGuiSys().GetGazeCursor().GenerateUserId();

	CreateMenu( Cinema.GetGuiSys() );

	LOG( "MoviePlayerView::OneTimeInit: %3.1f seconds", vrapi_GetTimeInSeconds() - start );
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


void MouseMenuButtonCallback( UIButton *button, void *object )
{
	( ( MoviePlayerView * )object )->MouseMenuButtonPressed();
}
void StreamMenuButtonCallback( UIButton *button, void *object )
{
	( ( MoviePlayerView * )object )->StreamMenuButtonPressed();
}
void ScreenMenuButtonCallback( UIButton *button, void *object )
{
	( ( MoviePlayerView * )object )->ScreenMenuButtonPressed();
}
void ControllerMenuButtonCallback( UIButton *button, void *object )
{
	( ( MoviePlayerView * )object )->ControllerMenuButtonPressed();
}

void GazeCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->GazePressed();
}
void TrackpadCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->TrackpadPressed();
}
void OffCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->OffPressed();
}
void XSensitivityCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->XSensitivityPressed();
}
void YSensitivityCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->YSensitivityPressed();
}
void Button1080p60Callback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->Button1080p60Pressed();
}
void Button1080p30Callback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->Button1080p30Pressed();
}
void Button720p60Callback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->Button720p60Pressed();
}
void Button720p30Callback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->Button720p30Pressed();
}
void HostAudioCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->HostAudioPressed();
}
void SBSCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->SBSPressed();
}
void ChangeSeatCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->ChangeSeatPressed();
}
void DistanceCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->DistancePressed();
}
void SizeCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->SizePressed();
}
void SpeedCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->SpeedPressed();
}
void ComfortModeCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->ComfortModePressed();
}
void MapKeyboardCallback( UITextButton *button, void *object )
{
	( ( MoviePlayerView * )object )->MapKeyboardPressed();
}



bool GazeActiveCallback( UITextButton *button, void *object )
{
	return ( ( MoviePlayerView * )object )->GazeActive();
}
bool TrackpadActiveCallback( UITextButton *button, void *object )
{
	return ( ( MoviePlayerView * )object )->TrackpadActive();
}
bool OffActiveCallback( UITextButton *button, void *object )
{
	return ( ( MoviePlayerView * )object )->OffActive();
}

bool Button1080p60IsSelectedCallback( UITextButton *button, void *object )
{
	return ( ( MoviePlayerView * )object )->Button1080p60IsSelected();
}
bool Button1080p30IsSelectedCallback( UITextButton *button, void *object )
{
	return ( ( MoviePlayerView * )object )->Button1080p30IsSelected();
}
bool Button720p60IsSelectedCallback( UITextButton *button, void *object )
{
	return ( ( MoviePlayerView * )object )->Button720p60IsSelected();
}
bool Button720p30IsSelectedCallback( UITextButton *button, void *object )
{
	return ( ( MoviePlayerView * )object )->Button720p30IsSelected();
}
bool HostAudioIsSelectedCallback( UITextButton *button, void *object )
{
	return ( ( MoviePlayerView * )object )->HostAudioIsSelected();
}


bool DisableButton( UITextButton *button, void *object )
{
	return false;
}


void MoviePlayerView::TextButtonHelper(UITextButton& button)
{
	button.SetLocalScale( Vector3f( 1.0f ) );
	button.SetFontScale( 1.0f );
	button.SetColor( Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	button.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	button.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 320, 120 );
	button.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

}
void MoviePlayerView::CreateMenu( OvrGuiSys & guiSys )
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

	MouseTexture.LoadTextureFromApplicationPackage( "assets/mousebutton.png" );
	MouseHoverTexture.LoadTextureFromApplicationPackage( "assets/mousebutton.png" );
	MousePressedTexture.LoadTextureFromApplicationPackage( "assets/mousebutton.png" );

	StreamTexture.LoadTextureFromApplicationPackage( "assets/streambutton.png" );
	StreamHoverTexture.LoadTextureFromApplicationPackage( "assets/streambutton.png" );
	StreamPressedTexture.LoadTextureFromApplicationPackage( "assets/streambutton.png" );

	ScreenTexture.LoadTextureFromApplicationPackage( "assets/screenbutton.png" );
	ScreenHoverTexture.LoadTextureFromApplicationPackage( "assets/screenbutton.png" );
	ScreenPressedTexture.LoadTextureFromApplicationPackage( "assets/screenbutton.png" );

	ControllerTexture.LoadTextureFromApplicationPackage( "assets/controllerbutton.png" );
	ControllerHoverTexture.LoadTextureFromApplicationPackage( "assets/controllerbutton.png" );
	ControllerPressedTexture.LoadTextureFromApplicationPackage( "assets/controllerbutton.png" );

    // ==============================================================================
    //
    // reorient message
    //
	MoveScreenMenu = new UIMenu( Cinema );
	MoveScreenMenu->Create( "MoviePlayerMenu" );
	MoveScreenMenu->SetFlags( VRMenuFlags_t( VRMENU_FLAG_TRACK_GAZE ) | VRMenuFlags_t( VRMENU_FLAG_BACK_KEY_DOESNT_EXIT ) );

	MoveScreenLabel.AddToMenu( guiSys, MoveScreenMenu, NULL );
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

    PlaybackControlsPosition.AddToMenu( guiSys, PlaybackControlsMenu );
    PlaybackControlsScale.AddToMenu( guiSys, PlaybackControlsMenu, &PlaybackControlsPosition );
    PlaybackControlsScale.SetLocalPosition( Vector3f( 0.0f, 0.0f, 0.05f ) );
    PlaybackControlsScale.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 1080, 1080 );

	// ==============================================================================
    //
    // movie title
    //
    MovieTitleLabel.AddToMenu( guiSys, PlaybackControlsMenu, &PlaybackControlsScale );
    MovieTitleLabel.SetLocalPosition( PixelPos( 0, 266, 0 ) );
    MovieTitleLabel.SetFontScale( 1.4f );
    MovieTitleLabel.SetText( "" );
    MovieTitleLabel.SetTextOffset( Vector3f( 0.0f, 0.0f, 0.01f ) );
    MovieTitleLabel.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 320, 120 );

    // ==============================================================================
    //
    // controls
    //
    ControlsBackground.AddToMenuFlags( guiSys, PlaybackControlsMenu, &PlaybackControlsScale, VRMenuObjectFlags_t( VRMENUOBJECT_RENDER_HIERARCHY_ORDER ) );
    ControlsBackground.SetLocalPosition( PixelPos( 0, 550, 0 ) );
    ControlsBackground.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 1004, 168 );
    ControlsBackground.AddComponent( &GazeTimer );

	SeekbarBackground.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
	SeekbarBackground.SetLocalPosition( PixelPos( 78, 0, 2 ) );
	SeekbarBackground.SetColor( Vector4f( 0.5333f, 0.5333f, 0.5333f, 1.0f ) );
	SeekbarBackground.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekbarBackgroundTexture, ScrubBarWidth + 6, 46 );
	SeekbarBackground.AddComponent( &ScrubBar );

	SeekbarProgress.AddToMenu( guiSys, PlaybackControlsMenu, &SeekbarBackground );
	SeekbarProgress.SetLocalPosition( PixelPos( 0, 0, 1 ) );
	SeekbarProgress.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekbarProgressTexture, ScrubBarWidth, 40 );
	SeekbarProgress.GetMenuObject()->AddFlags( VRMenuObjectFlags_t( VRMENUOBJECT_DONT_HIT_ALL ) );

	CurrentTime.AddToMenu( guiSys, PlaybackControlsMenu, &SeekbarBackground );
	CurrentTime.SetLocalPosition( PixelPos( -234, 52, 2 ) );
	CurrentTime.SetLocalScale( Vector3f( 1.0f ) );
	CurrentTime.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekPosition );
	CurrentTime.SetText( "2:33:33" );
	CurrentTime.SetTextOffset( PixelPos( 0, 6, 1 ) );
	CurrentTime.SetFontScale( 0.71f );
	CurrentTime.SetColor( Vector4f( 0 / 255.0f, 93 / 255.0f, 219 / 255.0f, 1.0f ) );
	CurrentTime.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	CurrentTime.GetMenuObject()->AddFlags( VRMenuObjectFlags_t( VRMENUOBJECT_DONT_HIT_ALL ) );

	SeekTime.AddToMenu( guiSys, PlaybackControlsMenu, &SeekbarBackground );
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

	SeekbarBackground.SetVisible(false);
//*/


 	MouseMenuButton.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
 	MouseMenuButton.SetLocalPosition( PixelPos( -450, 0, 1 ) );
 	MouseMenuButton.SetLocalScale( Vector3f( 2.0f ) );
 	MouseMenuButton.SetButtonImages( MouseTexture, MouseHoverTexture, MousePressedTexture );
 	MouseMenuButton.SetOnClick( MouseMenuButtonCallback, this );
 	MouseMenuButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

 	StreamMenuButton.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
 	StreamMenuButton.SetLocalPosition( PixelPos( -150, 0, 1 ) );
 	StreamMenuButton.SetLocalScale( Vector3f( 2.0f ) );
 	StreamMenuButton.SetButtonImages( StreamTexture, StreamHoverTexture, StreamPressedTexture );
 	StreamMenuButton.SetOnClick( StreamMenuButtonCallback, this );
 	StreamMenuButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

 	ScreenMenuButton.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
	ScreenMenuButton.SetLocalPosition( PixelPos( 150, 0, 1 ) );
 	ScreenMenuButton.SetLocalScale( Vector3f( 2.0f ) );
 	ScreenMenuButton.SetButtonImages( ScreenTexture, ScreenHoverTexture, ScreenPressedTexture );
 	ScreenMenuButton.SetOnClick( ScreenMenuButtonCallback, this );
 	ScreenMenuButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

	ControllerMenuButton.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
	ControllerMenuButton.SetLocalPosition( PixelPos( 450, 0, 1 ) );
	ControllerMenuButton.SetLocalScale( Vector3f( 2.0f ) );
	ControllerMenuButton.SetButtonImages( ControllerTexture, ControllerHoverTexture, ControllerPressedTexture );
	ControllerMenuButton.SetOnClick( ControllerMenuButtonCallback, this );
	ControllerMenuButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

	const static int MENU_X = 200;
	const static int MENU_Y = -150;
	const static int MENU_TOP = 200;

	MouseMenu = new UIContainer( Cinema );
	MouseMenu->AddToMenu( guiSys, PlaybackControlsMenu, &PlaybackControlsScale );
	MouseMenu->SetLocalPosition( PixelPos( 0, MENU_TOP, 1 ) );
	MouseMenu->SetVisible(false);

	ButtonGaze.AddToMenu( guiSys, PlaybackControlsMenu, MouseMenu );
	ButtonGaze.SetLocalPosition( PixelPos( MENU_X * -1.8, MENU_Y * 1, 1 ) );
	ButtonGaze.SetText( CinemaStrings::ButtonText_ButtonGaze );
	TextButtonHelper(ButtonGaze);
	ButtonGaze.SetOnClick( GazeCallback, this);
	ButtonGaze.SetIsSelected( GazeActiveCallback, this);

	ButtonTrackpad.AddToMenu( guiSys, PlaybackControlsMenu, MouseMenu );
	ButtonTrackpad.SetLocalPosition( PixelPos( MENU_X * 0, MENU_Y * 1, 1 ) );
	ButtonTrackpad.SetText( CinemaStrings::ButtonText_ButtonTrackpad );
	TextButtonHelper(ButtonTrackpad);
	ButtonTrackpad.SetOnClick( TrackpadCallback, this);
	ButtonTrackpad.SetIsSelected( TrackpadActiveCallback, this);

	ButtonOff.AddToMenu( guiSys, PlaybackControlsMenu, MouseMenu );
	ButtonOff.SetLocalPosition( PixelPos( MENU_X * 1.8, MENU_Y * 1, 1 ) );
	ButtonOff.SetText( CinemaStrings::ButtonText_ButtonOff );
	TextButtonHelper(ButtonOff);
	ButtonOff.SetOnClick( OffCallback, this);
	ButtonOff.SetIsSelected( OffActiveCallback, this);

	ButtonXSensitivity.AddToMenu( guiSys, PlaybackControlsMenu, MouseMenu );
	ButtonXSensitivity.SetLocalPosition( PixelPos( MENU_X * -1, MENU_Y * 2, 1 ) );
	ButtonXSensitivity.SetText( CinemaStrings::ButtonText_ButtonXSensitivity );
	TextButtonHelper(ButtonXSensitivity);
	ButtonXSensitivity.SetOnClick( XSensitivityCallback, this);

	ButtonYSensitivity.AddToMenu( guiSys, PlaybackControlsMenu, MouseMenu );
	ButtonYSensitivity.SetLocalPosition( PixelPos( MENU_X * 1, MENU_Y * 2, 1 ) );
	ButtonYSensitivity.SetText( CinemaStrings::ButtonText_ButtonYSensitivity );
	TextButtonHelper(ButtonYSensitivity);
	ButtonYSensitivity.SetOnClick( YSensitivityCallback, this);

	StreamMenu = new UIContainer( Cinema );
	StreamMenu->AddToMenu( guiSys, PlaybackControlsMenu, &PlaybackControlsScale );
	StreamMenu->SetLocalPosition( PixelPos( 0, MENU_TOP, 1 ) );
	StreamMenu->SetVisible(false);

	Button1080p60.AddToMenu( guiSys, PlaybackControlsMenu, StreamMenu );
	Button1080p60.SetLocalPosition( PixelPos( MENU_X * -1, MENU_Y * 1, 1 ) );
	Button1080p60.SetText( CinemaStrings::ButtonText_Button1080p60 );
	TextButtonHelper(Button1080p60);
	Button1080p60.SetOnClick( Button1080p60Callback, this);
	Button1080p60.SetIsSelected( Button1080p60IsSelectedCallback, this);

	Button1080p30.AddToMenu( guiSys, PlaybackControlsMenu, StreamMenu );
	Button1080p30.SetLocalPosition( PixelPos( MENU_X * 1, MENU_Y * 1, 1 ) );
	Button1080p30.SetText( CinemaStrings::ButtonText_Button1080p30 );
	TextButtonHelper(Button1080p30);
	Button1080p30.SetOnClick( Button1080p30Callback, this);
	Button1080p30.SetIsSelected( Button1080p30IsSelectedCallback, this);

	Button720p60.AddToMenu( guiSys, PlaybackControlsMenu, StreamMenu );
	Button720p60.SetLocalPosition( PixelPos( MENU_X * -1, MENU_Y * 2, 1 ) );
	Button720p60.SetText( CinemaStrings::ButtonText_Button720p60 );
	TextButtonHelper(Button720p60);
	Button720p60.SetOnClick( Button720p60Callback, this);
	Button720p60.SetIsSelected( Button720p60IsSelectedCallback, this);

	Button720p30.AddToMenu( guiSys, PlaybackControlsMenu, StreamMenu );
	Button720p30.SetLocalPosition( PixelPos( MENU_X * 1, MENU_Y * 2, 1 ) );
	Button720p30.SetText( CinemaStrings::ButtonText_Button720p30 );
	TextButtonHelper(Button720p30);
	Button720p30.SetOnClick( Button720p30Callback, this);
	Button720p30.SetIsSelected( Button720p30IsSelectedCallback, this);

	ButtonHostAudio.AddToMenu( guiSys, PlaybackControlsMenu, StreamMenu );
	ButtonHostAudio.SetLocalPosition( PixelPos( MENU_X * -1, MENU_Y * 3, 1 ) );
	ButtonHostAudio.SetText( CinemaStrings::ButtonText_ButtonHostAudio );
	TextButtonHelper(ButtonHostAudio);
	ButtonHostAudio.SetOnClick( HostAudioCallback, this);
	ButtonHostAudio.SetIsSelected( HostAudioIsSelectedCallback, this);

	ScreenMenu = new UIContainer( Cinema );
	ScreenMenu->AddToMenu( guiSys, PlaybackControlsMenu, &PlaybackControlsScale );
	ScreenMenu->SetLocalPosition( PixelPos( 0, MENU_TOP, 1 ) );
	ScreenMenu->SetVisible(false);

	ButtonSBS.AddToMenu( guiSys, PlaybackControlsMenu, ScreenMenu );
	ButtonSBS.SetLocalPosition( PixelPos( MENU_X * -1, MENU_Y * 1, 1 ) );
	ButtonSBS.SetText( CinemaStrings::ButtonText_ButtonSBS );
	TextButtonHelper(ButtonSBS);
	ButtonSBS.SetOnClick( SBSCallback, this);

	ButtonChangeSeat.AddToMenu( guiSys, PlaybackControlsMenu, ScreenMenu );
	ButtonChangeSeat.SetLocalPosition( PixelPos( MENU_X * 1, MENU_Y * 1, 1 ) );
	ButtonChangeSeat.SetText( CinemaStrings::ButtonText_ButtonChangeSeat );
	TextButtonHelper(ButtonChangeSeat);
	ButtonChangeSeat.SetOnClick( ChangeSeatCallback, this);

	ButtonDistance.AddToMenu( guiSys, PlaybackControlsMenu, ScreenMenu );
	ButtonDistance.SetLocalPosition( PixelPos( MENU_X * -1, MENU_Y * 2, 1 ) );
	ButtonDistance.SetText( CinemaStrings::ButtonText_ButtonDistance );
	TextButtonHelper(ButtonDistance);
	ButtonDistance.SetOnClick( DistanceCallback, this);

	ButtonSize.AddToMenu( guiSys, PlaybackControlsMenu, ScreenMenu );
	ButtonSize.SetLocalPosition( PixelPos( MENU_X * 1, MENU_Y * 2, 1 ) );
	ButtonSize.SetText( CinemaStrings::ButtonText_ButtonSize );
	TextButtonHelper(ButtonSize);
	ButtonSize.SetOnClick( SizeCallback, this);

	ControllerMenu = new UIContainer( Cinema );
	ControllerMenu->AddToMenu( guiSys, PlaybackControlsMenu, &PlaybackControlsScale );
	ControllerMenu->SetLocalPosition( PixelPos( 0, MENU_TOP, 1 ) );
	ControllerMenu->SetVisible(false);

	ButtonSpeed.AddToMenu( guiSys, PlaybackControlsMenu, ControllerMenu );
	ButtonSpeed.SetLocalPosition( PixelPos( MENU_X * -1, MENU_Y * 1, 1 ) );
	ButtonSpeed.SetText( CinemaStrings::ButtonText_ButtonSpeed );
	TextButtonHelper(ButtonSpeed);
	ButtonSpeed.SetOnClick( SpeedCallback, this);

	ButtonComfortMode.AddToMenu( guiSys, PlaybackControlsMenu, ControllerMenu );
	ButtonComfortMode.SetLocalPosition( PixelPos( MENU_X * 1, MENU_Y * 1, 1 ) );
	ButtonComfortMode.SetText( CinemaStrings::ButtonText_ButtonComfortMode );
	TextButtonHelper(ButtonComfortMode);
	ButtonComfortMode.SetOnClick( ComfortModeCallback, this);

	ButtonMapKeyboard.AddToMenu( guiSys, PlaybackControlsMenu, ControllerMenu );
	ButtonMapKeyboard.SetLocalPosition( PixelPos( MENU_X * -1, MENU_Y * 2, 1 ) );
	ButtonMapKeyboard.SetText( CinemaStrings::ButtonText_ButtonMapKeyboard );
	TextButtonHelper(ButtonMapKeyboard);
	ButtonMapKeyboard.SetOnClick( MapKeyboardCallback, this);

//*/
}

void MoviePlayerView::OnOpen()
{
	LOG( "OnOpen" );
	CurViewState = VIEWSTATE_OPEN;

	Cinema.SceneMgr.ClearMovie();

	ScrubBar.SetProgress( 0.0f );

	RepositionScreen = false;
	MoveScreenAlpha.Set( 0, 0, 0, 0.0f );

	HideUI();
	Cinema.SceneMgr.LightsOff( 1.5f );

	Cinema.StartMoviePlayback(streamWidth, streamHeight, streamFPS, streamHostAudio);

	MovieTitleLabel.SetText( Cinema.GetCurrentMovie()->Name );
	Bounds3f titleBounds = MovieTitleLabel.GetTextLocalBounds( Cinema.GetGuiSys().GetDefaultFont() ) * VRMenuObject::TEXELS_PER_METER;
	MovieTitleLabel.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, titleBounds.GetSize().x + 88, titleBounds.GetSize().y + 32 );
}

void MoviePlayerView::OnClose()
{
	LOG( "OnClose" );
	CurViewState = VIEWSTATE_CLOSED;
	HideUI();
	Cinema.GetGuiSys().GetGazeCursor().ShowCursor();

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

void MoviePlayerView::SetError( const char *text, bool showSDCard, bool showErrorIcon )
{
	LOG( "Error: %s", text );
	Cinema.AppSelection( true );
	Cinema.ShowError( text );
}

void MoviePlayerView::BackPressed()
{
	LOG( "BackPressed" );

	if(uiActive)
	{
		HideUI();
	}
	else
	{
		ShowUI();
	}
}

void MoviePlayerView::BackPressedDouble()
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

bool MoviePlayerView::OnKeyEvent( const int keyCode, const int repeatCount, const KeyEventType eventType )
{
	switch ( keyCode )
	{
		case AKEYCODE_BACK:
		{
			switch ( eventType )
			{
				case KEY_EVENT_SHORT_PRESS:
					LOG( "KEY_EVENT_SHORT_PRESS" );
					BackPressed();
					return true;
					break;
				case KEY_EVENT_DOUBLE_TAP:
					BackPressedDouble();
					return true;
					break;
				default:
					//LOG( "unexpected back key state %i", eventType );
					break;
			}
		}
		break;
		case AKEYCODE_MEDIA_NEXT:
			if ( eventType == KEY_EVENT_UP )
			{
				Cinema.SetMovie( Cinema.GetNextMovie() );
				Cinema.ResumeOrRestartMovie();
			}
			break;
		case AKEYCODE_MEDIA_PREVIOUS:
			if ( eventType == KEY_EVENT_UP )
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
	Cinema.GetGuiSys().GetGazeCursor().ShowCursor();

	PlaybackControlsMenu->Open();
	GazeTimer.SetGazeTime();

	PlaybackControlsScale.SetLocalScale( Vector3f( Cinema.SceneMgr.GetScreenSize().y * ( 500.0f / 1080.0f ) ) );
	PlaybackControlsPosition.SetLocalPose( Cinema.SceneMgr.GetScreenPose() );

	uiActive = true;
}

void MoviePlayerView::HideUI()
{
	LOG( "HideUI" );
	MouseMenu->SetVisible(false);
	ScreenMenu->SetVisible(false);
	StreamMenu->SetVisible(false);
	ControllerMenu->SetVisible(false);
	PlaybackControlsMenu->Close();

	Cinema.GetGuiSys().GetGazeCursor().HideCursor();
	Cinema.SceneMgr.ForceMono = false;
	uiActive = false;

	BackgroundClicked = false;
}

void MoviePlayerView::CheckDebugControls( const VrFrame & vrFrame )
{
	if ( !Cinema.AllowDebugControls )
	{
		return;
	}

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
		// FIXME: free screen matrix is inverted compared to bounds screen matrix.  (MGH: No, everything's backwards!)
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

#define SCROLL_CLICKS 8
void MoviePlayerView::HandleGazeMouse( const VrFrame & vrFrame, bool onscreen, const Vector2f screenCursor )
{
	if(onscreen) {
		Vector2f travel = screenCursor - lastMouse;
		if(travel.x != 0.0 && travel.y != 0.0)
		{
			Native::MouseMove(Cinema.app, 1280 / 2 * 1.2 * travel.x, 720 / -2 * 1.2 * travel.y );
		}
		lastMouse = screenCursor;
	}

	// Left click
	if ( ( vrFrame.Input.buttonReleased & BUTTON_TOUCH ) && !( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) )
	{
		if(onscreen) {
			if(allowDrag && !mouseDownLeft)
			{
				Native::MouseClick(Cinema.app,1,true); // fast click!
			}
			Native::MouseClick(Cinema.app,1,false);
			Cinema.app->PlaySound( "touch_up" );
			mouseDownLeft = false;
		} else		// open ui if it's not visible
		{
			ShowUI();
			// ignore button A or touchpad until release so we don't close the UI immediately after opening it
			UIOpened = true;
		}
	}

	if ( onscreen && ( vrFrame.Input.buttonPressed & BUTTON_TOUCH ) ) {
		clickStartTime = vrapi_GetTimeInSeconds();
		allowDrag = true;
	}

	if( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) {
		allowDrag = false;
	}

	if ( onscreen && allowDrag && !mouseDownLeft && (clickStartTime + 0.5 < vrapi_GetTimeInSeconds()) &&
			( vrFrame.Input.buttonState & BUTTON_TOUCH ) && !( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) )
	{
		Native::MouseClick(Cinema.app,1,true);
		Cinema.app->PlaySound( "touch_down" );
		mouseDownLeft = true;
		allowDrag = false; // already dragging
	}

	// Right click
	if ( onscreen && !mouseDownRight && ( vrFrame.Input.buttonPressed & BUTTON_SWIPE_BACK ) )
	{
		Native::MouseClick(Cinema.app,3,true);
		Cinema.app->PlaySound( "touch_down" );
		mouseDownRight = true;
	}

	// Middle click
	if ( onscreen && !mouseDownMiddle && ( vrFrame.Input.buttonPressed & BUTTON_SWIPE_FORWARD ) )
	{
		Native::MouseClick(Cinema.app,2,true);
		Cinema.app->PlaySound( "touch_down" );
		mouseDownMiddle = true;
	}

	// Mouse Scroll
	// It says it's 0.0-1.0, but it's calculated as if(d>100) swf = d/100, so it can't be less than 1.0.
	float actualSwipeFraction = vrFrame.Input.swipeFraction - 1.0;
	if ( onscreen && ( vrFrame.Input.buttonState & BUTTON_SWIPE_UP ) )
	{
		signed char diff = (signed char)(SCROLL_CLICKS * actualSwipeFraction) - lastScroll;
		//LOG("Scroll up %i %f", diff, vrFrame.Input.swipeFraction);
		lastScroll = (signed char)(SCROLL_CLICKS * actualSwipeFraction);
		if(diff)
		{
			Native::MouseScroll(Cinema.app, diff );
			Cinema.app->PlaySound( "touch_up" );
		}
	}
	if ( onscreen && ( vrFrame.Input.buttonState & BUTTON_SWIPE_DOWN ) )
	{
		signed char diff = lastScroll - (signed char)(SCROLL_CLICKS * actualSwipeFraction);
		//LOG("Scroll down %i %f", diff, vrFrame.Input.swipeFraction);
		lastScroll = (signed char)(SCROLL_CLICKS * actualSwipeFraction);
		if(diff)
		{
			Native::MouseScroll(Cinema.app, diff );
			Cinema.app->PlaySound( "touch_down" );
		}
	}

	if ( vrFrame.Input.buttonReleased & BUTTON_TOUCH )
	{
		if(mouseDownRight)
		{
			Native::MouseClick(Cinema.app,3,false);
			mouseDownRight = false;
			Cinema.app->PlaySound( "touch_up" );
		}
		if(mouseDownMiddle)
		{
			Native::MouseClick(Cinema.app,2,false);
			mouseDownMiddle = false;
			Cinema.app->PlaySound( "touch_up" );
		}
		if(mouseDownLeft)
		{
			Native::MouseClick(Cinema.app,1,false);
			mouseDownLeft = false;
			Cinema.app->PlaySound( "touch_up" );
		}

		lastScroll = 0;
	}
}

void MoviePlayerView::HandleTrackpadMouse( const VrFrame & vrFrame )
{
	if ( vrFrame.Input.buttonPressed & BUTTON_TOUCH ) {
		clickStartTime = vrapi_GetTimeInSeconds();
	}

	if( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) {
		if(mouseMoving)
		{
			Vector2f travel = vrFrame.Input.touchRelative - lastMouse;
			lastMouse = vrFrame.Input.touchRelative;
			Native::MouseMove(Cinema.app, travel.x, travel.y );
		}
		else
		{
			lastMouse = vrFrame.Input.touchRelative;
			mouseMoving = true;
		}
	}

	if ( !mouseMoving && (clickStartTime + 0.5 < vrapi_GetTimeInSeconds()) &&
			( vrFrame.Input.buttonState & BUTTON_TOUCH ) && !( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) )
	{
		Native::MouseClick(Cinema.app,3,true);
		Cinema.app->PlaySound( "touch_down" );
		mouseDownRight = true;
	}

	if ( vrFrame.Input.buttonReleased & BUTTON_TOUCH )
	{
		if( !mouseDownLeft && !mouseDownRight && !( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) )
		{
			Native::MouseClick(Cinema.app,1,true); // fast click!
			Native::MouseClick(Cinema.app,1,false);
		}
		if( mouseDownRight )
		{
			Native::MouseClick(Cinema.app,3,false);
		}
		if(mouseDownLeft)
		{
			Native::MouseClick(Cinema.app,1,false);
		}
		Cinema.app->PlaySound( "touch_up" );
		mouseDownLeft = false;
		mouseDownRight = false;
		mouseMoving = false;
	}
}

void MoviePlayerView::CheckInput( const VrFrame & vrFrame )
{
	// while we're holding down the button or touchpad, reposition screen
	if ( RepositionScreen ) {
		if ( vrFrame.Input.buttonState & BUTTON_TOUCH ) {
			Cinema.SceneMgr.PutScreenInFront();
		} else {
			RepositionScreen = false;
		}
	}

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

	if( ( !RepositionScreen ) && ( !uiActive ) && mouseMode != MOUSE_OFF )
	{
		if( mouseMode == MOUSE_GAZE)
		{
			HandleGazeMouse(vrFrame, onscreen, screenCursor);
		}
		else if( mouseMode == MOUSE_TRACKPAD)
		{
			HandleTrackpadMouse(vrFrame);
		}
	}
	else
	{
		// Left click
		if ( ( vrFrame.Input.buttonReleased & BUTTON_TOUCH ) && !( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) )
		{
			ShowUI();
			// ignore button A or touchpad until release so we don't close the UI immediately after opening it
			UIOpened = true;
		}
	}

	if ( Cinema.SceneMgr.FreeScreenActive )
	{
		if ( !onscreen )
		{
			// outside of screen, so show reposition message
			const double now = vrapi_GetTimeInSeconds();
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
			const double now = vrapi_GetTimeInSeconds();
			MoveScreenAlpha.Set( now, -1.0f, now + 1.0f, 1.0f );
			MoveScreenLabel.SetVisible( false );
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

void MoviePlayerView::MouseMenuButtonPressed()
{

	Cinema.app->PlaySound( "touch_up" );
	UpdateMenus();
	MouseMenu->SetVisible(!MouseMenu->GetVisible());
	StreamMenu->SetVisible(false);
	ScreenMenu->SetVisible(false);
	ControllerMenu->SetVisible(false);
}
void MoviePlayerView::StreamMenuButtonPressed()
{

	Cinema.app->PlaySound( "touch_up" );
	UpdateMenus();
	MouseMenu->SetVisible(false);
	StreamMenu->SetVisible(!StreamMenu->GetVisible());
	ScreenMenu->SetVisible(false);
	ControllerMenu->SetVisible(false);
}
void MoviePlayerView::ScreenMenuButtonPressed()
{

	Cinema.app->PlaySound( "touch_up" );
	UpdateMenus();
	MouseMenu->SetVisible(false);
	StreamMenu->SetVisible(false);
	ScreenMenu->SetVisible(!ScreenMenu->GetVisible());
	ControllerMenu->SetVisible(false);
}
void MoviePlayerView::ControllerMenuButtonPressed()
{

	Cinema.app->PlaySound( "touch_up" );
	UpdateMenus();
	MouseMenu->SetVisible(false);
	StreamMenu->SetVisible(false);
	ScreenMenu->SetVisible(false);
	ControllerMenu->SetVisible(!ControllerMenu->GetVisible());
}

// Mouse controls
void MoviePlayerView::GazePressed()
{
	mouseMode = MOUSE_GAZE;
	UpdateMenus();
}
void MoviePlayerView::TrackpadPressed()
{
	mouseMode = MOUSE_TRACKPAD;
	UpdateMenus();
}
void MoviePlayerView::OffPressed()
{
	mouseMode = MOUSE_OFF;
	UpdateMenus();
}
bool MoviePlayerView::GazeActive()
{
	return mouseMode == MOUSE_GAZE;
}
bool MoviePlayerView::TrackpadActive()
{
	return mouseMode == MOUSE_TRACKPAD;
}
bool MoviePlayerView::OffActive()
{
	return mouseMode == MOUSE_OFF;
}

void MoviePlayerView::XSensitivityPressed()
{}
void MoviePlayerView::YSensitivityPressed()
{}

// Stream controls
void MoviePlayerView::Button1080p60Pressed()
{
	Cinema.SceneMgr.ClearMovie();
	//TODO: This all needs to be saved in prefs
	streamWidth = 1920;
	streamHeight = 1080;
	streamFPS = 60;
	UpdateMenus();
	Cinema.StartMoviePlayback(streamWidth, streamHeight, streamFPS, streamHostAudio);
}
void MoviePlayerView::Button1080p30Pressed()
{
	Cinema.SceneMgr.ClearMovie();
	streamWidth = 1920;
	streamHeight = 1080;
	streamFPS = 30;
	UpdateMenus();
	Cinema.StartMoviePlayback(streamWidth, streamHeight, streamFPS, streamHostAudio);
}
void MoviePlayerView::Button720p60Pressed()
{
	Cinema.SceneMgr.ClearMovie();
	streamWidth = 1280;
	streamHeight = 720;
	streamFPS = 60;
	UpdateMenus();
	Cinema.StartMoviePlayback(streamWidth, streamHeight, streamFPS, streamHostAudio);
}
void MoviePlayerView::Button720p30Pressed()
{
	Cinema.SceneMgr.ClearMovie();
	streamWidth = 1280;
	streamHeight = 720;
	streamFPS = 30;
	UpdateMenus();
	Cinema.StartMoviePlayback(streamWidth, streamHeight, streamFPS, streamHostAudio);
}
void MoviePlayerView::HostAudioPressed()
{
	Cinema.SceneMgr.ClearMovie();
	streamHostAudio = !streamHostAudio;
	UpdateMenus();
	Cinema.StartMoviePlayback(streamWidth, streamHeight, streamFPS, streamHostAudio);
}
void MoviePlayerView::SBSPressed()
{}

bool MoviePlayerView::Button1080p60IsSelected()
{
	return streamWidth == 1920 && streamHeight == 1080 && streamFPS == 60;
}
bool MoviePlayerView::Button1080p30IsSelected()
{
	return streamWidth == 1920 && streamHeight == 1080 && streamFPS == 30;
}
bool MoviePlayerView::Button720p60IsSelected()
{
	return streamWidth == 1280 && streamHeight == 720 && streamFPS == 60;
}
bool MoviePlayerView::Button720p30IsSelected()
{
	return streamWidth == 1280 && streamHeight == 720 && streamFPS == 30;
}
bool MoviePlayerView::HostAudioIsSelected()
{
	return streamHostAudio;
}


// Screen controls
void MoviePlayerView::ChangeSeatPressed()
{
	Cinema.SceneMgr.NextSeat();
}
void MoviePlayerView::DistancePressed()
{}
void MoviePlayerView::SizePressed()
{}

// Controller... controls?
void MoviePlayerView::SpeedPressed()
{}
void MoviePlayerView::ComfortModePressed()
{}
void MoviePlayerView::MapKeyboardPressed()
{}

void MoviePlayerView::UpdateMenus()
{
	ButtonGaze.UpdateButtonState();
	ButtonTrackpad.UpdateButtonState();
	ButtonOff.UpdateButtonState();
	ButtonXSensitivity.UpdateButtonState();
	ButtonYSensitivity.UpdateButtonState();

	Button1080p60.UpdateButtonState();
	Button1080p30.UpdateButtonState();
	Button720p60.UpdateButtonState();
	Button720p30.UpdateButtonState();
	ButtonHostAudio.UpdateButtonState();

	ButtonSBS.UpdateButtonState();
	ButtonChangeSeat.UpdateButtonState();
	ButtonDistance.UpdateButtonState();
	ButtonSize.UpdateButtonState();

	ButtonSpeed.UpdateButtonState();
	ButtonComfortMode.UpdateButtonState();
	ButtonMapKeyboard.UpdateButtonState();
}

void MoviePlayerView::UpdateUI( const VrFrame & vrFrame )
{
	if ( uiActive )
	{
////TODO: Commenting this out for now until I have a better idea of how to handle UI interaction.
//// The GazeTimer is currently only attached to the main menu buttons, so this code breaks the submenu options
//// Either it gazetimer needs to cover everything, it needs to be refactored to a function call, or deleted
//		double timeSinceLastGaze = vrapi_GetTimeInSeconds() - GazeTimer.GetLastGazeTime();
//		if ( !ScrubBar.IsScrubbing() && ( timeSinceLastGaze > GazeTimeTimeout ) )
//		{
//			LOG( "Gaze timeout" );
//			HideUI();
//		}

		// if we press the touchpad or a button outside of the playback controls, then close the UI
//		if ( ( vrFrame.Input.buttonPressed & BUTTON_TOUCH ) != 0 )
//		{
			// ignore touchpad until release so we don't close the UI immediately after opening it
//			BackgroundClicked = !GazeTimer.IsFocused() && !UIOpened;
//		}

//		if ( ( ( ( vrFrame.Input.buttonReleased & BUTTON_TOUCH ) != 0 ) && ( ( vrFrame.Input.buttonState & BUTTON_TOUCH_WAS_SWIPE ) == 0 ) )	)
//		{
//			if ( !GazeTimer.IsFocused() && BackgroundClicked )
//			{
//				LOG( "Clicked outside playback controls" );
//				Cinema.GetGuiSys().GetApp()->PlaySound( "touch_up" );
//				HideUI();
//			}
//			BackgroundClicked = false;
//		}

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
	ovrEyeBufferParms eyeBufferParms = Cinema.app->GetEyeBufferParms();
	eyeBufferParms.multisamples = 2;
	Cinema.app->SetEyeBufferParms( eyeBufferParms );

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
	LastGazeTime = vrapi_GetTimeInSeconds();
}

eMsgStatus ControlsGazeTimer::OnEvent_Impl( OvrGuiSys & guiSys, VrFrame const & vrFrame,
        VRMenuObject * self, VRMenuEvent const & event )
{
    switch( event.EventType )
    {
    	case VRMENU_EVENT_FRAME_UPDATE:
    		if ( HasFocus )
    		{
    			LastGazeTime = vrapi_GetTimeInSeconds();
    		}
    		return MSG_STATUS_ALIVE;
        case VRMENU_EVENT_FOCUS_GAINED:
        	HasFocus = true;
        	LastGazeTime = vrapi_GetTimeInSeconds();
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

eMsgStatus ScrubBarComponent::OnEvent_Impl( OvrGuiSys & guiSys, VrFrame const & vrFrame,
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
    		OnClick( guiSys, vrFrame, event );
    		return MSG_STATUS_ALIVE;

    	case VRMENU_EVENT_FRAME_UPDATE:
    		return OnFrame( guiSys, vrFrame, self, event );

        default:
            OVR_ASSERT( !"Event flags mismatch!" );
            return MSG_STATUS_ALIVE;
    }
}

eMsgStatus ScrubBarComponent::OnFrame( OvrGuiSys & guiSys, VrFrame const & vrFrame,
        VRMenuObject * self, VRMenuEvent const & event )
{
	if ( TouchDown )
	{
		if ( ( vrFrame.Input.buttonState & ( BUTTON_A | BUTTON_TOUCH ) ) != 0 )
		{
			OnClick( guiSys, vrFrame, event );
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

		Bounds3f bounds = Background->GetMenuObject()->GetLocalBounds(guiSys.GetDefaultFont() ) * Background->GetParent()->GetWorldScale();
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

void ScrubBarComponent::OnClick( OvrGuiSys & guiSys, VrFrame const & vrFrame, VRMenuEvent const & event )
{
	if ( OnClickFunction == NULL )
	{
		return;
	}

	Vector3f hitPos = event.HitResult.RayStart + event.HitResult.RayDir * event.HitResult.t;

	// move hit position into local space
	const Posef modelPose = Background->GetWorldPose();
	Vector3f localHit = modelPose.Orientation.Inverted().Rotate( hitPos - modelPose.Position );

	Bounds3f bounds = Background->GetMenuObject()->GetLocalBounds( guiSys.GetDefaultFont() ) * Background->GetParent()->GetWorldScale();
	const float progress = ( localHit.x - bounds.GetMins().x ) / bounds.GetSize().x;
	if ( ( progress >= 0.0f ) && ( progress <= 1.0f ) )
	{
		( *OnClickFunction )( this, OnClickObject, progress );
	}
}

} // namespace VRMatterStreamTheater
