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

#include <unistd.h>
#include <stdio.h>

#include <android/keycodes.h>
#include "CinemaApp.h"
#include "Native.h"
#include "VRMenu/VRMenuMgr.h"
#include "VRMenu/GuiSys.h"
#include "Kernel/OVR_String_Utils.h"

#include "CinemaStrings.h"

namespace VRMatterStreamTheater
{

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
	SaveMenu( NULL ),
	ButtonSaveApp( Cinema ),
	ButtonSaveDefault( Cinema ),
	ButtonResetSettings( Cinema ),
	ButtonSaveSettings1( Cinema ),
	ButtonSaveSettings2( Cinema ),
	ButtonSaveSettings3( Cinema ),
	ButtonLoadSettings1( Cinema ),
	ButtonLoadSettings2( Cinema ),
	ButtonLoadSettings3( Cinema ),
	MouseMenuButton( Cinema ),
	MouseMenu( NULL ),
	ButtonGaze( Cinema ),
	ButtonTrackpad( Cinema ),
	ButtonOff( Cinema ),
	GazeScale( Cinema ),
	GazeSliderBackground( Cinema ),
	GazeSliderIndicator( Cinema ),
	GazeCurrentSetting( Cinema ),
	GazeNewSetting( Cinema ),
	GazeSlider(),
	TrackpadScale( Cinema ),
	TrackpadSliderBackground( Cinema ),
	TrackpadSliderIndicator( Cinema ),
	TrackpadCurrentSetting( Cinema ),
	TrackpadNewSetting( Cinema ),
	TrackpadSlider(),
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
	ScreenDistance( Cinema ),
	DistanceSliderBackground( Cinema ),
	DistanceSliderIndicator( Cinema ),
	DistanceCurrentSetting( Cinema ),
	DistanceNewSetting( Cinema ),
	DistanceSlider(),
	ScreenSize( Cinema ),
	SizeSliderBackground( Cinema ),
	SizeSliderIndicator( Cinema ),
	SizeCurrentSetting( Cinema ),
	SizeNewSetting( Cinema ),
	SizeSlider(),
	HelpMenuButton( Cinema ),
	HelpMenu( NULL ),
	HelpText( Cinema ),
	ExitButton( Cinema ),
	VRModeMenuButton( Cinema ),
	VRModeMenu( NULL ),
	LatencyScale( Cinema ),
	LatencySliderBackground( Cinema ),
	LatencySliderIndicator( Cinema ),
	LatencyCurrentSetting( Cinema ),
	LatencyNewSetting( Cinema ),
	LatencySlider(),
	VRXScale( Cinema ),
	VRXSliderBackground( Cinema ),
	VRXSliderIndicator( Cinema ),
	VRXCurrentSetting( Cinema ),
	VRXNewSetting( Cinema ),
	VRXSlider(),
	VRYScale( Cinema ),
	VRYSliderBackground( Cinema ),
	VRYSliderIndicator( Cinema ),
	VRYCurrentSetting( Cinema ),
	VRYNewSetting( Cinema ),
	VRYSlider(),
	settingsVersion(1.0f),
	defaultSettingsPath(""),
	settings1Path(""),
	settings2Path(""),
	settings3Path(""),
	appSettingsPath(""),
	defaultSettings( NULL ),
	settings1( NULL ),
	settings2( NULL ),
	settings3( NULL ),
	appSettings( NULL ),
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
	gazeScaleValue(1.05),
	trackpadScaleValue(2.0),
	streamWidth(1280),
	streamHeight(720),
	streamFPS(60),
	streamHostAudio(true),
	GazeMin(0.7),
	GazeMax(1.58),
	TrackpadMin(-4.0),
	TrackpadMax(4.0),
	VoidScreenDistanceMin(0.1),
	VoidScreenDistanceMax(3.0),
	VoidScreenScaleMin(-3.0),
	VoidScreenScaleMax(4.0),
	oldPoses(),
	calibrationStage(0),
	lastPose(),
	trackCalibrationYaw(500),
	trackCalibrationPitch(500),
	latencyAddition(24),
	screenMotionPaused( false ),
	vrXscale( 1.0f ),
	vrYscale( 1.0f ),
	VRLatencyMax( 60 ),
	VRLatencyMin( 0 ),
	VRXScaleMax( 6.0f ),
	VRXScaleMin( 0.0f ),
	VRYScaleMax( 6.0f ),
	VRYScaleMin( 0.0f )
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

	delete(defaultSettings);
	defaultSettings = NULL;
	delete(appSettings);
	appSettings = NULL;
	delete(settings1);
	settings1 = NULL;
	delete(settings2);
	settings2 = NULL;
	delete(settings3);
	settings3 = NULL;
}

float PixelScale( const float x )
{
	return x * VRMenuObject::DEFAULT_TEXEL_SCALE;
}

Vector3f PixelPos( const float x, const float y, const float z )
{
	return Vector3f( PixelScale( x ), PixelScale( y ), PixelScale( z ) );
}

/*
 * UI callback functions
 */
void MouseMenuButtonCallback		( UIButton *button, void *object ) { ( ( MoviePlayerView * )object )->MouseMenuButtonPressed(); }
void StreamMenuButtonCallback		( UIButton *button, void *object ) { ( ( MoviePlayerView * )object )->StreamMenuButtonPressed(); }
void ScreenMenuButtonCallback		( UIButton *button, void *object ) { ( ( MoviePlayerView * )object )->ScreenMenuButtonPressed(); }
void VRModeMenuButtonCallback		( UIButton *button, void *object ) { ( ( MoviePlayerView * )object )->VRModeMenuButtonPressed(); }
void HelpMenuButtonCallback			( UIButton *button, void *object ) { ( ( MoviePlayerView * )object )->HelpMenuButtonPressed(); }
void ExitButtonCallback				( UIButton *button, void *object ) { ( ( MoviePlayerView * )object )->ExitButtonPressed(); }

void SaveAppCallback				( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->SaveAppPressed(); }
void SaveDefaultCallback			( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->SaveDefaultPressed(); }
void ResetDefaultCallback			( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->ResetDefaultPressed(); }
void Save1Callback					( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->Save1Pressed(); }
void Save2Callback					( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->Save2Pressed(); }
void Save3Callback					( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->Save3Pressed(); }
void Load1Callback					( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->Load1Pressed(); }
void Load2Callback					( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->Load2Pressed(); }
void Load3Callback					( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->Load3Pressed(); }

void GazeCallback					( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->GazePressed(); }
void TrackpadCallback				( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->TrackpadPressed(); }
void OffCallback					( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->OffPressed(); }
void GazeScaleCallback				( SliderComponent *button, void *object, const float value ) { ( ( MoviePlayerView * )object )->GazeScalePressed( value ); }
void TrackpadScaleCallback			( SliderComponent *button, void *object, const float value ) { ( ( MoviePlayerView * )object )->TrackpadScalePressed( value ); }
bool GazeActiveCallback				( UITextButton *button, void *object ) { return ( ( MoviePlayerView * )object )->GazeActive(); }
bool TrackpadActiveCallback			( UITextButton *button, void *object ) { return ( ( MoviePlayerView * )object )->TrackpadActive(); }
bool OffActiveCallback				( UITextButton *button, void *object ) { return ( ( MoviePlayerView * )object )->OffActive(); }

void Button1080p60Callback			( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->Button1080p60Pressed(); }
void Button1080p30Callback			( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->Button1080p30Pressed(); }
void Button720p60Callback			( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->Button720p60Pressed(); }
void Button720p30Callback			( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->Button720p30Pressed(); }
void HostAudioCallback				( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->HostAudioPressed(); }
bool Button1080p60IsSelectedCallback( UITextButton *button, void *object ) { return ( ( MoviePlayerView * )object )->Button1080p60IsSelected(); }
bool Button1080p30IsSelectedCallback( UITextButton *button, void *object ) { return ( ( MoviePlayerView * )object )->Button1080p30IsSelected(); }
bool Button720p60IsSelectedCallback	( UITextButton *button, void *object ) { return ( ( MoviePlayerView * )object )->Button720p60IsSelected(); }
bool Button720p30IsSelectedCallback	( UITextButton *button, void *object ) { return ( ( MoviePlayerView * )object )->Button720p30IsSelected(); }
bool HostAudioIsSelectedCallback	( UITextButton *button, void *object ) { return ( ( MoviePlayerView * )object )->HostAudioIsSelected(); }

void SBSCallback					( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->SBSPressed(); }
void ChangeSeatCallback				( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->ChangeSeatPressed(); }
void DistanceCallback				( SliderComponent *button, void *object, const float value ) { ( ( MoviePlayerView * )object )->DistancePressed( value ); }
void SizeCallback					( SliderComponent *button, void *object, const float value ) { ( ( MoviePlayerView * )object )->SizePressed( value ); }
bool IsChangeSeatsEnabledCallback	( UITextButton *button, void *object ) { return ( ( MoviePlayerView * )object )->IsChangeSeatsEnabled(); }

void LatencyCallback				( SliderComponent *button, void *object, const float value ) { ( ( MoviePlayerView * )object )->LatencyPressed( value ); }
void VRXCallback					( SliderComponent *button, void *object, const float value ) { ( ( MoviePlayerView * )object )->VRXPressed( value ); }
void VRYCallback					( SliderComponent *button, void *object, const float value ) { ( ( MoviePlayerView * )object )->VRYPressed( value ); }

void SpeedCallback					( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->SpeedPressed(); }
void ComfortModeCallback			( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->ComfortModePressed(); }
void MapKeyboardCallback			( UITextButton *button, void *object ) { ( ( MoviePlayerView * )object )->MapKeyboardPressed(); }

bool DisableButton					( UITextButton *button, void *object ) { return false; }



void MoviePlayerView::TextButtonHelper(UITextButton& button, float scale, int w, int h)
{
	button.SetLocalScale( Vector3f( scale ) );
	button.SetFontScale( 1.0f );
	button.SetColor( Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	button.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	button.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, w, h );
	button.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

}
void MoviePlayerView::SetUpSlider(OvrGuiSys & guiSys, UIWidget *parent, SliderComponent& scrub, UIImage& bg, UIImage& ind, UILabel& cur, UILabel& set, int slideWidth, int xoff, int yoff)
{
	bg.AddToMenu( guiSys, PlaybackControlsMenu, parent );
	bg.SetLocalPosition( PixelPos( xoff, yoff, 2 ) );
	bg.SetColor( Vector4f( 0.5333f, 0.5333f, 0.5333f, 1.0f ) );
	bg.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekbarBackgroundTexture, slideWidth + 6, 46 );
	bg.AddComponent( &scrub );

	ind.AddToMenu( guiSys, PlaybackControlsMenu, &bg );
	ind.SetLocalPosition( PixelPos( 0, 0, 1 ) );
	ind.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekbarProgressTexture, slideWidth, 40 );
	ind.GetMenuObject()->AddFlags( VRMenuObjectFlags_t( VRMENUOBJECT_DONT_HIT_ALL ) );

	cur.AddToMenu( guiSys, PlaybackControlsMenu, &bg );
	cur.SetLocalPosition( PixelPos( 0, 52, 2 ) );
	cur.SetLocalScale( Vector3f( 1.0f ) );
	cur.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekPosition );
	cur.SetText( "1.0" );
	cur.SetTextOffset( PixelPos( 0, 6, 1 ) );
	cur.SetFontScale( 0.71f );
	cur.SetColor( Vector4f( 0 / 255.0f, 93 / 255.0f, 219 / 255.0f, 1.0f ) );
	cur.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	cur.GetMenuObject()->AddFlags( VRMenuObjectFlags_t( VRMENUOBJECT_DONT_HIT_ALL ) );

	set.AddToMenu( guiSys, PlaybackControlsMenu, &bg );
	set.SetLocalPosition( PixelPos( 0, 52, 4 ) );
	set.SetLocalScale( Vector3f( 1.0f ) );
	set.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, SeekPosition );
	set.SetText( "1.0" );
	set.SetTextOffset( PixelPos( 0, 6, 1 ) );
	set.SetFontScale( 0.71f );
	set.SetColor( Vector4f( 47.0f / 255.0f, 70 / 255.0f, 89 / 255.0f, 1.0f ) );
	set.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	set.GetMenuObject()->AddFlags( VRMenuObjectFlags_t( VRMENUOBJECT_DONT_HIT_ALL ) );

	scrub.SetWidgets( &bg, &ind, &cur, &set, slideWidth );

}

void MoviePlayerView::InitializeSettings()
{
	String	outPath;
	const bool validDir = Cinema.app->GetStoragePaths().GetPathIfValidPermission(
					EST_INTERNAL_STORAGE, EFT_FILES, "", W_OK | R_OK, outPath );

	if(validDir)
	{
		if(defaultSettings == NULL)
		{
			defaultSettingsPath = outPath + "settings.json";
			defaultSettings = new Settings(defaultSettingsPath);

			defaultSettings->Define("StreamTheaterSettingsVersion", &settingsVersion);
			defaultSettings->Define("MouseMode", (int*)&mouseMode);
			defaultSettings->Define("StreamWidth", &streamWidth);
			defaultSettings->Define("StreamHeight", &streamHeight);
			defaultSettings->Define("StreamFPS", &streamFPS);
			defaultSettings->Define("EnableHostAudio", &streamHostAudio);

			defaultSettings->Define("GazeScale", &gazeScaleValue);
			defaultSettings->Define("TrackpadScale", &trackpadScaleValue);

			defaultSettings->Define("VoidScreenDistance", &Cinema.SceneMgr.FreeScreenDistance);
			defaultSettings->Define("VoidScreenScale", &Cinema.SceneMgr.FreeScreenScale);

			defaultSettings->Define("GazeScaleMax", &GazeMax);
			defaultSettings->Define("GazeScaleMin", &GazeMin);
			defaultSettings->Define("TrackpadScaleMax", &TrackpadMax);
			defaultSettings->Define("TrackpadScaleMin", &TrackpadMin);
			defaultSettings->Define("VoidScreenDistanceMax", &VoidScreenDistanceMax);
			defaultSettings->Define("VoidScreenDistanceMin", &VoidScreenDistanceMin);
			defaultSettings->Define("VoidScreenScaleMax", &VoidScreenScaleMax);
			defaultSettings->Define("VoidScreenScaleMin", &VoidScreenScaleMin);

			if(Cinema.SceneMgr.SceneInfo.UseVRScreen)
			{ // Don't save or load VR screen settings if we're not using it
				defaultSettings->Define("VRScreenLatency", &latencyAddition);
				defaultSettings->Define("VRScreenXScale", &vrXscale);
				defaultSettings->Define("VRScreenYScale", &vrYscale);
				defaultSettings->Define("VRScreenLatencyMax", &VRLatencyMax);
				defaultSettings->Define("VRScreenLatencyMin", &VRLatencyMin);
				defaultSettings->Define("VRScreenXScaleMax", &VRXScaleMax);
				defaultSettings->Define("VRScreenXScaleMin", &VRXScaleMin);
				defaultSettings->Define("VRScreenYScaleMax", &VRYScaleMax);
				defaultSettings->Define("VRScreenYScaleMin", &VRYScaleMin);
			}

			defaultSettings->Load();
		}

		if(settings1 == NULL)
		{
			settings1Path = outPath + "settings.1.json";
			settings2Path = outPath + "settings.2.json";
			settings3Path = outPath + "settings.3.json";

			settings1 = new Settings(settings1Path);
			settings2 = new Settings(settings2Path);
			settings3 = new Settings(settings3Path);

			settings1->CopyDefines(*defaultSettings);
			settings1->Define("3dMode", (int*)&Cinema.SceneMgr.CurrentMovieFormat);

			settings2->CopyDefines(*settings1);
			settings3->CopyDefines(*settings1);
		}

		if(appSettings != NULL)
		{
			delete(appSettings);
			appSettings = NULL;
		}

		appSettingsPath = outPath + "settings." + Cinema.GetCurrentMovie()->Name + ".json";
		appSettings = new Settings(appSettingsPath);
		appSettings->CopyDefines(*defaultSettings);
		appSettings->Load();

		if( Cinema.SceneMgr.CurrentMovieFormat == VT_LEFT_RIGHT_3D )
		{
			Cinema.SceneMgr.CurrentMovieWidth /= 2;
		}

		UpdateMenus();
	}
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

	HelpTexture.LoadTextureFromApplicationPackage( "assets/help.png" );
	HelpHoverTexture.LoadTextureFromApplicationPackage( "assets/help.png" );
	HelpPressedTexture.LoadTextureFromApplicationPackage( "assets/help.png" );

	ExitTexture.LoadTextureFromApplicationPackage( "assets/exitbutton.png" );
	ExitHoverTexture.LoadTextureFromApplicationPackage( "assets/exitbutton.png" );
	ExitPressedTexture.LoadTextureFromApplicationPackage( "assets/exitbutton.png" );

	VRModeTexture.LoadTextureFromApplicationPackage( "assets/vrbutton.png" );
	VRModeHoverTexture.LoadTextureFromApplicationPackage( "assets/vrbutton.png" );
	VRModePressedTexture.LoadTextureFromApplicationPackage( "assets/vrbutton.png" );

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
    PlaybackControlsScale.SetLocalPosition( Vector3f( 0.0f, 0.0f, 0.01f ) );
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
    ControlsBackground.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 1200, 168 );
    ControlsBackground.AddComponent( &GazeTimer );

 	MouseMenuButton.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
 	MouseMenuButton.SetLocalScale( Vector3f( 2.0f ) );
 	MouseMenuButton.SetButtonImages( MouseTexture, MouseHoverTexture, MousePressedTexture );
 	MouseMenuButton.SetOnClick( MouseMenuButtonCallback, this );
 	MouseMenuButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

 	StreamMenuButton.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
 	StreamMenuButton.SetLocalScale( Vector3f( 2.0f ) );
 	StreamMenuButton.SetButtonImages( StreamTexture, StreamHoverTexture, StreamPressedTexture );
 	StreamMenuButton.SetOnClick( StreamMenuButtonCallback, this );
 	StreamMenuButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

 	ScreenMenuButton.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
 	ScreenMenuButton.SetLocalScale( Vector3f( 2.0f ) );
 	ScreenMenuButton.SetButtonImages( ScreenTexture, ScreenHoverTexture, ScreenPressedTexture );
 	ScreenMenuButton.SetOnClick( ScreenMenuButtonCallback, this );
 	ScreenMenuButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

 	VRModeMenuButton.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
 	VRModeMenuButton.SetLocalScale( Vector3f( 2.0f ) );
 	VRModeMenuButton.SetButtonImages( VRModeTexture, VRModeHoverTexture, VRModePressedTexture );
 	VRModeMenuButton.SetOnClick( VRModeMenuButtonCallback, this );
 	VRModeMenuButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

	HelpMenuButton.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
	HelpMenuButton.SetLocalScale( Vector3f( 2.0f ) );
	HelpMenuButton.SetButtonImages( HelpTexture, HelpTexture, HelpTexture );
	HelpMenuButton.SetOnClick( HelpMenuButtonCallback, this );
	HelpMenuButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

	ExitButton.AddToMenu( guiSys, PlaybackControlsMenu, &ControlsBackground );
	ExitButton.SetLocalScale( Vector3f( 2.0f ) );
	ExitButton.SetButtonImages( ExitTexture, ExitHoverTexture, ExitPressedTexture );
	ExitButton.SetOnClick( ExitButtonCallback, this );
	ExitButton.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );

	static const int MenuButtonsWidth = 1100;
	static const int MenuButtonsCount = 6;
	static const int MenuButtonsStep = MenuButtonsWidth / ( MenuButtonsCount - 1 );
	int menuButtonPos = 0 - MenuButtonsWidth / 2;

 	MouseMenuButton.SetLocalPosition( PixelPos( menuButtonPos, 0, 1 ) );
 	menuButtonPos += MenuButtonsStep;
 	StreamMenuButton.SetLocalPosition( PixelPos( menuButtonPos, 0, 1 ) );
 	menuButtonPos += MenuButtonsStep;
	ScreenMenuButton.SetLocalPosition( PixelPos( menuButtonPos, 0, 1 ) );
 	menuButtonPos += MenuButtonsStep;
 	VRModeMenuButton.SetLocalPosition( PixelPos( menuButtonPos, 0, 1 ) );
 	menuButtonPos += MenuButtonsStep;
	HelpMenuButton.SetLocalPosition( PixelPos( menuButtonPos, 0, 1 ) );
 	menuButtonPos += MenuButtonsStep;
	ExitButton.SetLocalPosition( PixelPos( menuButtonPos, 0, 1 ) );

	const static int MENU_X = 200;
	const static int MENU_Y = -150;
	const static int MENU_TOP = 200;

	SaveMenu = new UIContainer( Cinema );
	SaveMenu->AddToMenu( guiSys, PlaybackControlsMenu, &PlaybackControlsScale );
	SaveMenu->SetLocalPosition( PixelPos( 0, -500, 1 ) );
	SaveMenu->SetVisible(false);

	ButtonSaveApp.AddToMenu( guiSys, PlaybackControlsMenu, SaveMenu );
	ButtonSaveApp.SetLocalPosition( PixelPos( 800, 130, 1 ) );
	ButtonSaveApp.SetText( CinemaStrings::ButtonText_ButtonSaveApp );
	TextButtonHelper(ButtonSaveApp, 0.5f);
	ButtonSaveApp.SetOnClick( SaveAppCallback, this);

	ButtonSaveDefault.AddToMenu( guiSys, PlaybackControlsMenu, SaveMenu );
	ButtonSaveDefault.SetLocalPosition( PixelPos( 800, 65, 1 ) );
	ButtonSaveDefault.SetText( CinemaStrings::ButtonText_ButtonSaveDefault );
	TextButtonHelper(ButtonSaveDefault, 0.5f);
	ButtonSaveDefault.SetOnClick( SaveDefaultCallback, this);

	ButtonResetSettings.AddToMenu( guiSys, PlaybackControlsMenu, SaveMenu );
	ButtonResetSettings.SetLocalPosition( PixelPos( 800, 0, 1 ) );
	ButtonResetSettings.SetText( CinemaStrings::ButtonText_ButtonResetSettings );
	TextButtonHelper(ButtonResetSettings, 0.5f);
	ButtonResetSettings.SetOnClick( ResetDefaultCallback, this);

	ButtonSaveSettings1.AddToMenu( guiSys, PlaybackControlsMenu, SaveMenu );
	ButtonSaveSettings1.SetLocalPosition( PixelPos( 200, 65, 1 ) );
	ButtonSaveSettings1.SetText( CinemaStrings::ButtonText_ButtonSaveSettings1 );
	TextButtonHelper(ButtonSaveSettings1, 0.5f);
	ButtonSaveSettings1.SetOnClick( Save1Callback, this);

	ButtonSaveSettings2.AddToMenu( guiSys, PlaybackControlsMenu, SaveMenu );
	ButtonSaveSettings2.SetLocalPosition( PixelPos( 400, 65, 1 ) );
	ButtonSaveSettings2.SetText( CinemaStrings::ButtonText_ButtonSaveSettings2 );
	TextButtonHelper(ButtonSaveSettings2, 0.5f);
	ButtonSaveSettings2.SetOnClick( Save2Callback, this);

	ButtonSaveSettings3.AddToMenu( guiSys, PlaybackControlsMenu, SaveMenu );
	ButtonSaveSettings3.SetLocalPosition( PixelPos( 600, 65, 1 ) );
	ButtonSaveSettings3.SetText( CinemaStrings::ButtonText_ButtonSaveSettings3 );
	TextButtonHelper(ButtonSaveSettings3, 0.5f);
	ButtonSaveSettings3.SetOnClick( Save3Callback, this);

	ButtonLoadSettings1.AddToMenu( guiSys, PlaybackControlsMenu, SaveMenu );
	ButtonLoadSettings1.SetLocalPosition( PixelPos( 200, 0, 1 ) );
	ButtonLoadSettings1.SetText( CinemaStrings::ButtonText_ButtonLoadSettings1 );
	TextButtonHelper(ButtonLoadSettings1, 0.5f);
	ButtonLoadSettings1.SetOnClick( Load1Callback, this);

	ButtonLoadSettings2.AddToMenu( guiSys, PlaybackControlsMenu, SaveMenu );
	ButtonLoadSettings2.SetLocalPosition( PixelPos( 400, 0, 1 ) );
	ButtonLoadSettings2.SetText( CinemaStrings::ButtonText_ButtonLoadSettings2 );
	TextButtonHelper(ButtonLoadSettings2, 0.5f);
	ButtonLoadSettings2.SetOnClick( Load2Callback, this);

	ButtonLoadSettings3.AddToMenu( guiSys, PlaybackControlsMenu, SaveMenu );
	ButtonLoadSettings3.SetLocalPosition( PixelPos( 600, 0, 1 ) );
	ButtonLoadSettings3.SetText( CinemaStrings::ButtonText_ButtonLoadSettings3 );
	TextButtonHelper(ButtonLoadSettings3, 0.5f);
	ButtonLoadSettings3.SetOnClick( Load3Callback, this);

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

	GazeScale.AddToMenu( guiSys, PlaybackControlsMenu, MouseMenu );
	GazeScale.SetLocalPosition( PixelPos( MENU_X * -1, MENU_Y * 2, 1 ) );
	GazeScale.SetText( CinemaStrings::ButtonText_LabelGazeScale );
	GazeScale.SetLocalScale( Vector3f( 1.0f ) );
	GazeScale.SetFontScale( 1.0f );
	GazeScale.SetColor( Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	GazeScale.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	GazeScale.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 300, 80 );
	SetUpSlider(guiSys, MouseMenu, GazeSlider, GazeSliderBackground, GazeSliderIndicator, GazeCurrentSetting, GazeNewSetting, 300, MENU_X * -1, MENU_Y * 3);
	GazeSlider.SetOnClick( GazeScaleCallback, this );

	TrackpadScale.AddToMenu( guiSys, PlaybackControlsMenu, MouseMenu );
	TrackpadScale.SetLocalPosition( PixelPos( MENU_X * 1, MENU_Y * 2, 1 ) );
	TrackpadScale.SetText( CinemaStrings::ButtonText_LabelTrackpadScale );
	TrackpadScale.SetLocalScale( Vector3f( 1.0f ) );
	TrackpadScale.SetFontScale( 1.0f );
	TrackpadScale.SetColor( Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	TrackpadScale.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	TrackpadScale.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 300, 80 );
	SetUpSlider(guiSys, MouseMenu, TrackpadSlider, TrackpadSliderBackground, TrackpadSliderIndicator, TrackpadCurrentSetting, TrackpadNewSetting, 300,  MENU_X * 1, MENU_Y * 3);
	TrackpadSlider.SetOnClick( TrackpadScaleCallback, this );

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
	ButtonSBS.SetLocalPosition( PixelPos( MENU_X * -2, MENU_Y * 2.25, 1 ) );
	ButtonSBS.SetText( CinemaStrings::ButtonText_ButtonSBS );
	TextButtonHelper(ButtonSBS);
	ButtonSBS.SetOnClick( SBSCallback, this);

	ButtonChangeSeat.AddToMenu( guiSys, PlaybackControlsMenu, ScreenMenu );
	ButtonChangeSeat.SetLocalPosition( PixelPos( MENU_X * -2, MENU_Y * 1.25, 1 ) );
	ButtonChangeSeat.SetText( CinemaStrings::ButtonText_ButtonChangeSeat );
	TextButtonHelper(ButtonChangeSeat);
	ButtonChangeSeat.SetOnClick( ChangeSeatCallback, this);
	ButtonChangeSeat.SetIsEnabled( IsChangeSeatsEnabledCallback, this );

	ScreenDistance.AddToMenu( guiSys, PlaybackControlsMenu, ScreenMenu );
	ScreenDistance.SetLocalPosition( PixelPos( MENU_X * -0.5, MENU_Y * 1.25, 1 ) );
	ScreenDistance.SetText( CinemaStrings::ButtonText_ButtonDistance );
	ScreenDistance.SetLocalScale( Vector3f( 1.0f ) );
	ScreenDistance.SetFontScale( 1.0f );
	ScreenDistance.SetColor( Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	ScreenDistance.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	ScreenDistance.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 240, 80 );
	//ScreenDistance.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );
	SetUpSlider(guiSys, ScreenMenu, DistanceSlider, DistanceSliderBackground, DistanceSliderIndicator, DistanceCurrentSetting, DistanceNewSetting, 800, MENU_X * 2, MENU_Y * 1.25);
	DistanceSlider.SetOnClick( DistanceCallback, this );

	ScreenSize.AddToMenu( guiSys, PlaybackControlsMenu, ScreenMenu );
	ScreenSize.SetLocalPosition( PixelPos( MENU_X * -0.5, MENU_Y * 2.25 , 1 ) );
	ScreenSize.SetText( CinemaStrings::ButtonText_ButtonSize );
	ScreenSize.SetLocalScale( Vector3f( 1.0f ) );
	ScreenSize.SetFontScale( 1.0f );
	ScreenSize.SetColor( Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	ScreenSize.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	ScreenSize.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 240, 80 );
	//ScreenSize.GetMenuObject()->SetLocalBoundsExpand( PixelPos( 20, 0, 0 ), Vector3f::ZERO );
	SetUpSlider(guiSys, ScreenMenu, SizeSlider, SizeSliderBackground, SizeSliderIndicator, SizeCurrentSetting, SizeNewSetting, 800, MENU_X * 2, MENU_Y * 2.25);
	SizeSlider.SetOnClick( SizeCallback, this );

	VRModeMenu = new UIContainer( Cinema );
	VRModeMenu->AddToMenu( guiSys, PlaybackControlsMenu, &PlaybackControlsScale );
	VRModeMenu->SetLocalPosition( PixelPos( 0, MENU_TOP, 1 ) );
	VRModeMenu->SetVisible(false);

	LatencyScale.AddToMenu( guiSys, PlaybackControlsMenu, VRModeMenu );
	LatencyScale.SetLocalPosition( PixelPos( MENU_X * 0, MENU_Y * 1, 1 ) );
	LatencyScale.SetText( CinemaStrings::ButtonText_LabelLatency );
	LatencyScale.SetLocalScale( Vector3f( 1.0f ) );
	LatencyScale.SetFontScale( 1.0f );
	LatencyScale.SetColor( Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	LatencyScale.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	LatencyScale.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 300, 80 );
	SetUpSlider(guiSys, VRModeMenu, LatencySlider, LatencySliderBackground, LatencySliderIndicator, LatencyCurrentSetting, LatencyNewSetting, 300,  MENU_X * 0, MENU_Y * 1.75);
	LatencySlider.SetOnClick( LatencyCallback, this );

	VRXScale.AddToMenu( guiSys, PlaybackControlsMenu, VRModeMenu );
	VRXScale.SetLocalPosition( PixelPos( MENU_X * -1, MENU_Y * 2.5, 1 ) );
	VRXScale.SetText( CinemaStrings::ButtonText_LabelVRXScale );
	VRXScale.SetLocalScale( Vector3f( 1.0f ) );
	VRXScale.SetFontScale( 1.0f );
	VRXScale.SetColor( Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	VRXScale.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	VRXScale.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 300, 80 );
	SetUpSlider(guiSys, VRModeMenu, VRXSlider, VRXSliderBackground, VRXSliderIndicator, VRXCurrentSetting, VRXNewSetting, 300,  MENU_X * -1, MENU_Y * 3.25);
	VRXSlider.SetOnClick( VRXCallback, this );

	VRYScale.AddToMenu( guiSys, PlaybackControlsMenu, VRModeMenu );
	VRYScale.SetLocalPosition( PixelPos( MENU_X * 1, MENU_Y * 2.5, 1 ) );
	VRYScale.SetText( CinemaStrings::ButtonText_LabelVRYScale );
	VRYScale.SetLocalScale( Vector3f( 1.0f ) );
	VRYScale.SetFontScale( 1.0f );
	VRYScale.SetColor( Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	VRYScale.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	VRYScale.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 300, 80 );
	SetUpSlider(guiSys, VRModeMenu, VRYSlider, VRYSliderBackground, VRYSliderIndicator, VRYCurrentSetting, VRYNewSetting, 300,  MENU_X * 1, MENU_Y * 3.25);
	VRYSlider.SetOnClick( VRYCallback, this );

	HelpMenu = new UIContainer( Cinema );
	HelpMenu->AddToMenu( guiSys, PlaybackControlsMenu, &PlaybackControlsScale );
	HelpMenu->SetLocalPosition( PixelPos( 0, MENU_TOP, 1 ) );
	HelpMenu->SetVisible(false);

	HelpText.AddToMenu( guiSys, PlaybackControlsMenu, HelpMenu );
	HelpText.SetLocalPosition( PixelPos( MENU_X * 0, MENU_Y * 1, 1 ) );
	HelpText.SetLocalScale( Vector3f( 1.0f ) );
	HelpText.SetFontScale( 1.0f );
	HelpText.SetColor( Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
	HelpText.SetTextColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	HelpText.SetImage( 0, SURFACE_TEXTURE_DIFFUSE, BackgroundTintTexture, 1200, 600 );
	HelpText.SetTextWordWrapped( CinemaStrings::HelpText, Cinema.GetGuiSys().GetDefaultFont(), HelpText.GetWorldScale().x * 2);
//*/
}

void MoviePlayerView::OnOpen()
{
	LOG( "OnOpen" );
	CurViewState = VIEWSTATE_OPEN;

	Cinema.SceneMgr.ClearMovie();

	RepositionScreen = false;
	MoveScreenAlpha.Set( 0, 0, 0, 0.0f );

	InitializeSettings();

	HideUI();
	Cinema.SceneMgr.LightsOff( 1.5f );

	Cinema.StartMoviePlayback(streamWidth, streamHeight, streamFPS, streamHostAudio);

	if ( Cinema.SceneMgr.SceneInfo.UseVRScreen )
	{
		screenMotionPaused = true;
		VRModeMenuButton.SetVisible(true);
	}
	else
	{
		VRModeMenuButton.SetVisible(false);
	}

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

	if( screenMotionPaused )
	{
		screenMotionPaused = false;
		return;
	}

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

	if ( Cinema.SceneMgr.SceneInfo.UseVRScreen )
	{
		if ( calibrationStage > 0 )
		{
			calibrationStage = -1;
		}
		else
		{
			uiActive = !uiActive;
		}
	}
	else if ( Cinema.AllowTheaterSelection() )
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
	MovieTitleLabel.SetVisible(true);

	PlaybackControlsScale.SetLocalScale( Vector3f( Cinema.SceneMgr.GetScreenSize().y * ( 500.0f / 1080.0f ) ) );
	PlaybackControlsPosition.SetLocalPose( Cinema.SceneMgr.GetScreenPose() );

	uiActive = true;
}

void MoviePlayerView::HideUI()
{
	LOG( "HideUI" );
	SaveMenu->SetVisible(false);
	MouseMenu->SetVisible(false);
	ScreenMenu->SetVisible(false);
	StreamMenu->SetVisible(false);
	VRModeMenu->SetVisible(false);
	HelpMenu->SetVisible(false);
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
	if (1 || Cinema.SceneMgr.FreeScreenActive )
	{
		// MIXFE: free screen matrix is inverted compared to bounds screen matrix.  (MGH: No, everything's backwards!)
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


Matrix4f MoviePlayerView::InterpolatePoseAtTime( long time )
{
	OldScreenPose* before = oldPoses.GetFirst();
	OldScreenPose* after = oldPoses.GetFirst();
	while(after->time < time && !oldPoses.IsLast(after))
	{
		OldScreenPose* tooOld = before;
		before = after;
		after = oldPoses.GetNext(after);
		if(tooOld != before) {
			oldPoses.Remove(tooOld);
		}
	}

//	if(before->time == after->time)
	{
		return after->pose;
	}
	return ( before->pose * (time - before->time)
			+ after->pose * (after->time - time) )
			/ (after->time - before->time);
}

void MoviePlayerView::RecordPose( long time, Matrix4f pose )
{
	OldScreenPose* p = new OldScreenPose();
	p->time = time;
	p->pose = pose;
	oldPoses.PushBack(p);
}

void MoviePlayerView::HandleCalibration( const VrFrame & vrFrame )
{
	switch(calibrationStage) {
	case -1: // TODO: Calibration was canceled, display a temporary message
		break;
	case 1: // Look up
		break;
	case 2: // Look down
		break;
	case 3: // Look center
		break;
	case 4: // Look at a recognizable object
		break;
	case 5: // Spin in a complete circle and look at the object again
		break;
	default:
		calibrationStage = -1;
		break;
	}

}

// This gets called right after the screen is updated by the scene manager
// to minimize the number of times where we have the wrong timestamp
void MoviePlayerView::MovieScreenUpdated()
{
	if(Cinema.SceneMgr.SceneInfo.UseVRScreen && !uiActive && !screenMotionPaused )

	{  // Move screen around according to lag delay
		Matrix4f pose;

		//FIXME: MovieTextureTimestamp should be used here but it's broken on lollipop!
		long timestamp = Native::getLastFrameTimestamp(Cinema.app);
		//LOG("Timestamp %lu %lu !!!!!!!!!!!! %lu", newts, currentPoseTime, currentPoseTime - newts);

		if(timestamp != 0)
		{
			pose = InterpolatePoseAtTime(timestamp - latencyAddition);
		} else {
			pose = lastPose;
		}
		float y, p, r;
		pose.ToEulerAngles<Axis_Y,Axis_X,Axis_Z,Rotate_CCW, Handed_R>(&y, &p, &r);
		Matrix4f unrotRollMatrix( Quatf( Axis_Z, -r ) );
		pose = pose * unrotRollMatrix;
		Cinema.SceneMgr.FreeScreenPose = pose;
	}
}

void MoviePlayerView::CheckVRInput( const VrFrame & vrFrame )
{
	Matrix4f currentPose = Cinema.SceneMgr.Scene.CenterViewMatrix().Inverted();
	// static double timediff = Native::currentTimeMillis(Cinema.app) - (vrapi_GetTimeInSeconds() * 1000.0);
	// ((long)(vrapi_GetPredictedDisplayTime(GetOvrMobile(), 1) * 1000.0)) - timediff;
	long currentPoseTime = Native::currentTimeMillis(Cinema.app);
	RecordPose( currentPoseTime, currentPose );

	float cy, cp, cr, ly, lp, lr;
	currentPose.ToEulerAngles<Axis_Y,Axis_X,Axis_Z,Rotate_CCW, Handed_R>(&cy, &cp, &cr);
	lastPose.ToEulerAngles<Axis_Y,Axis_X,Axis_Z,Rotate_CCW, Handed_R>(&ly, &lp, &lr);

	float yaw = cy - ly;
	float pitch = cp - lp;
	if(yaw > M_PI) yaw -= 2 * M_PI;
	if(yaw < -M_PI) yaw += 2 * M_PI;

	Vector2f mouse;
	mouse.x = - trackCalibrationYaw * yaw * vrXscale;
	mouse.y = - trackCalibrationPitch * pitch * vrYscale;

	if( mouse.x != 0 || mouse.y != 0)
	{
		lastPose = currentPose;
	}

	if(calibrationStage) {
		HandleCalibration( vrFrame );
	}

	if(uiActive)
	{
	}
	else if( screenMotionPaused )
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
		if(mouse.x != 0.0 && mouse.y != 0.0)
		{
			Native::MouseMove(Cinema.app, mouse.x, mouse.y );
		}

		// Touching the trackpad will freeze the screen and activate mouse controls until back is hit.
		if ( vrFrame.Input.buttonReleased & BUTTON_TOUCH )
		{
			screenMotionPaused = true;
		}
	}
}

#define SCROLL_CLICKS 8
void MoviePlayerView::HandleGazeMouse( const VrFrame & vrFrame, bool onscreen, const Vector2f screenCursor )
{
	if(onscreen) {
		Vector2f travel = screenCursor - lastMouse;
		if(travel.x != 0.0 && travel.y != 0.0)
		{
			Native::MouseMove(Cinema.app, streamWidth / 2 * gazeScaleValue * travel.x, streamHeight / -2 * gazeScaleValue * travel.y );
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
			Native::MouseMove(Cinema.app, travel.x * trackpadScaleValue, travel.y * abs(trackpadScaleValue) );
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
	if ( Cinema.SceneMgr.SceneInfo.UseVRScreen )
	{   // Handle VR input differently
		CheckVRInput(vrFrame);
		return;
	}

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
		if ( !uiActive && !onscreen )
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

void MoviePlayerView::MouseMenuButtonPressed()
{

	Cinema.app->PlaySound( "touch_up" );
	UpdateMenus();
	MouseMenu->SetVisible(!MouseMenu->GetVisible());
	StreamMenu->SetVisible(false);
	ScreenMenu->SetVisible(false);
	VRModeMenu->SetVisible(false);
	HelpMenu->SetVisible(false);

	SaveMenu->SetVisible(MouseMenu->GetVisible());
}
void MoviePlayerView::StreamMenuButtonPressed()
{

	Cinema.app->PlaySound( "touch_up" );
	UpdateMenus();
	MouseMenu->SetVisible(false);
	StreamMenu->SetVisible(!StreamMenu->GetVisible());
	ScreenMenu->SetVisible(false);
	VRModeMenu->SetVisible(false);
	HelpMenu->SetVisible(false);

	SaveMenu->SetVisible(StreamMenu->GetVisible());
}
void MoviePlayerView::ScreenMenuButtonPressed()
{

	Cinema.app->PlaySound( "touch_up" );
	UpdateMenus();
	MouseMenu->SetVisible(false);
	StreamMenu->SetVisible(false);
	ScreenMenu->SetVisible(!ScreenMenu->GetVisible());
	VRModeMenu->SetVisible(false);
	HelpMenu->SetVisible(false);

	SaveMenu->SetVisible(ScreenMenu->GetVisible());
}
void MoviePlayerView::VRModeMenuButtonPressed()
{

	Cinema.app->PlaySound( "touch_up" );
	UpdateMenus();
	MouseMenu->SetVisible(false);
	StreamMenu->SetVisible(false);
	ScreenMenu->SetVisible(false);
	VRModeMenu->SetVisible(!VRModeMenu->GetVisible());
	HelpMenu->SetVisible(false);

	SaveMenu->SetVisible(VRModeMenu->GetVisible());
}
void MoviePlayerView::HelpMenuButtonPressed()
{

	Cinema.app->PlaySound( "touch_up" );
	UpdateMenus();
	MouseMenu->SetVisible(false);
	StreamMenu->SetVisible(false);
	ScreenMenu->SetVisible(false);
	VRModeMenu->SetVisible(false);
	HelpMenu->SetVisible(!HelpMenu->GetVisible());

	MovieTitleLabel.SetVisible(false);

	SaveMenu->SetVisible(HelpMenu->GetVisible());
}
void MoviePlayerView::ExitButtonPressed()
{
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

// Save settings
void MoviePlayerView::SaveAppPressed()
{
	if(!appSettings) return;
	appSettings->SaveChanged();
	UpdateMenus();
}
void MoviePlayerView::SaveDefaultPressed()
{
	if(!defaultSettings) return;
//	defaultSettings->SaveChanged();
//	defaultSettings->SaveVarNames();
	defaultSettings->SaveAll();
	UpdateMenus();
}
void MoviePlayerView::ResetDefaultPressed()
{
	LOG("Resetting default settings");
	remove(defaultSettingsPath);
	delete(defaultSettings);
	defaultSettings = NULL;

	LOG("Resetting settings 1");
	remove(settings1Path);
	delete(settings1);
	settings1 = NULL;

	LOG("Resetting settings 2");
	remove(settings2Path);
	delete(settings2);
	settings2 = NULL;

	LOG("Resetting settings 3");
	remove(settings3Path);
	delete(settings3);
	settings3 = NULL;

	LOG("Resetting app settings");
	remove(appSettingsPath);
	delete(appSettings);
	appSettings = NULL;

	//FIXME: Define all these defaults in a better place
	gazeScaleValue = 1.05;
	trackpadScaleValue = 2.0;
	streamWidth = 1280;
	streamHeight = 720;
	streamFPS = 60;
	streamHostAudio = true;
	GazeMin = 0.7;
	GazeMax = 1.58;
	TrackpadMin = -4.0;
	TrackpadMax = 4.0;
	VoidScreenDistanceMin = 0.1;
	VoidScreenDistanceMax = 3.0;
	VoidScreenScaleMin = -3.0;
	VoidScreenScaleMax = 4.0;
	Cinema.SceneMgr.FreeScreenDistance = 1.5;
	Cinema.SceneMgr.FreeScreenScale = 1.0;
	latencyAddition = 24;
	vrXscale = 1.0;
	vrYscale = 1.0;
	VRLatencyMax = 60;
	VRLatencyMin = 0;
	VRXScaleMax = 6.0f;
	VRXScaleMin = 0.0f;
	VRYScaleMax = 6.0f;
	VRYScaleMin = 0.0f;

	InitializeSettings();

	Cinema.SceneMgr.ClearMovie();
	UpdateMenus();
	Cinema.StartMoviePlayback(streamWidth, streamHeight, streamFPS, streamHostAudio);

}
void MoviePlayerView::Save1Pressed()
{
	if(!settings1) return;
	settings1->SaveAll();
	UpdateMenus();
}
void MoviePlayerView::Save2Pressed()
{
	if(!settings2) return;
	settings2->SaveAll();
	UpdateMenus();
}
void MoviePlayerView::Save3Pressed()
{
	if(!settings3) return;
	settings3->SaveAll();
	UpdateMenus();
}
void MoviePlayerView::Load1Pressed()
{
	LoadSettings(settings1);
}
void MoviePlayerView::Load2Pressed()
{
	LoadSettings(settings2);
}
void MoviePlayerView::Load3Pressed()
{
	LoadSettings(settings3);
}
void MoviePlayerView::LoadSettings(Settings* set)
{
	MovieFormat oldFormat = Cinema.SceneMgr.CurrentMovieFormat;
	int oldWidth = 	streamWidth;
	int oldHeight = streamHeight;
	int oldFPS = streamFPS;

	set->Load();

	if( oldWidth != streamWidth || oldHeight != streamHeight || oldFPS != streamFPS )
	{
		Cinema.SceneMgr.ClearMovie();
		Cinema.StartMoviePlayback(streamWidth, streamHeight, streamFPS, streamHostAudio);
	}

	GazeSlider.SetExtents(GazeMax,GazeMin,2);
	GazeSlider.SetValue(gazeScaleValue);
	TrackpadSlider.SetExtents(TrackpadMax,TrackpadMin,2);
	TrackpadSlider.SetValue(trackpadScaleValue);
	DistanceSlider.SetExtents(VoidScreenDistanceMax,VoidScreenDistanceMin,2);
	DistanceSlider.SetValue(Cinema.SceneMgr.FreeScreenDistance);
	SizeSlider.SetExtents(VoidScreenScaleMax,VoidScreenScaleMin,2);
	SizeSlider.SetValue(Cinema.SceneMgr.FreeScreenScale);

	if( Cinema.SceneMgr.CurrentMovieFormat == VT_LEFT_RIGHT_3D && oldFormat != VT_LEFT_RIGHT_3D )
	{
		Cinema.SceneMgr.CurrentMovieWidth /= 2;
	}

	if( oldFormat == VT_LEFT_RIGHT_3D && Cinema.SceneMgr.CurrentMovieFormat != VT_LEFT_RIGHT_3D )
	{
		Cinema.SceneMgr.CurrentMovieWidth *= 2;
	}

	UpdateMenus();
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

void MoviePlayerView::GazeScalePressed(const float value)
{
	gazeScaleValue = value;
	GazeSlider.SetValue( value );
}
void MoviePlayerView::TrackpadScalePressed(const float value)
{
	trackpadScaleValue = value;
	TrackpadSlider.SetValue( value );
}

// Stream controls
void MoviePlayerView::Button1080p60Pressed()
{
	Cinema.SceneMgr.ClearMovie();
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
void MoviePlayerView::LatencyPressed(const float value)
{
	latencyAddition = (int)value;
	LatencySlider.SetValue( value );
}
void MoviePlayerView::VRXPressed(const float value)
{
	vrXscale = value;
	VRXSlider.SetValue( value );
}
void MoviePlayerView::VRYPressed(const float value)
{
	vrYscale = value;
	VRYSlider.SetValue( value );
}

void MoviePlayerView::SBSPressed()
{
	switch(Cinema.SceneMgr.CurrentMovieFormat)
	{
	case VT_2D:
		Cinema.SceneMgr.CurrentMovieFormat = VT_LEFT_RIGHT_3D;
		Cinema.SceneMgr.CurrentMovieWidth /= 2;
		break;
	case VT_LEFT_RIGHT_3D:
		Cinema.SceneMgr.CurrentMovieFormat = VT_LEFT_RIGHT_3D_CROP;
		Cinema.SceneMgr.CurrentMovieWidth *= 2;
		break;
	case VT_LEFT_RIGHT_3D_CROP:
		Cinema.SceneMgr.CurrentMovieFormat = VT_LEFT_RIGHT_3D_FULL;
		break;
	case VT_LEFT_RIGHT_3D_FULL:
		Cinema.SceneMgr.CurrentMovieFormat = VT_2D;
		break;
	default:
		Cinema.SceneMgr.CurrentMovieFormat = VT_2D;
	}
}

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
void MoviePlayerView::DistancePressed( const float value)
{
	Cinema.SceneMgr.FreeScreenDistance =  value;
	DistanceSlider.SetValue( value );
}
void MoviePlayerView::SizePressed( const float value)
{
	Cinema.SceneMgr.FreeScreenScale = value;
	SizeSlider.SetValue( value );
}

bool MoviePlayerView::IsChangeSeatsEnabled()
{
	return Cinema.SceneMgr.SceneSeatCount > 1;
}

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

	Button1080p60.UpdateButtonState();
	Button1080p30.UpdateButtonState();
	Button720p60.UpdateButtonState();
	Button720p30.UpdateButtonState();
	ButtonHostAudio.UpdateButtonState();

	ButtonSBS.UpdateButtonState();
	ButtonChangeSeat.UpdateButtonState();

	ButtonSaveApp.UpdateButtonState();
	ButtonSaveDefault.UpdateButtonState();
	ButtonResetSettings.UpdateButtonState();
	ButtonSaveSettings1.UpdateButtonState();
	ButtonSaveSettings2.UpdateButtonState();
	ButtonSaveSettings3.UpdateButtonState();
	ButtonLoadSettings1.UpdateButtonState();
	ButtonLoadSettings2.UpdateButtonState();
	ButtonLoadSettings3.UpdateButtonState();

	GazeSlider.SetExtents(GazeMax,GazeMin,2);
	GazeSlider.SetValue(gazeScaleValue);
	TrackpadSlider.SetExtents(TrackpadMax,TrackpadMin,2);
	TrackpadSlider.SetValue(trackpadScaleValue);
	DistanceSlider.SetExtents(VoidScreenDistanceMax,VoidScreenDistanceMin,2);
	DistanceSlider.SetValue(Cinema.SceneMgr.FreeScreenDistance);
	SizeSlider.SetExtents(VoidScreenScaleMax,VoidScreenScaleMin,2);
	SizeSlider.SetValue(Cinema.SceneMgr.FreeScreenScale);
	LatencySlider.SetExtents(VRLatencyMax, VRLatencyMin, 0);
	LatencySlider.SetValue(latencyAddition);
	VRXSlider.SetExtents(VRXScaleMax, VRXScaleMin, 2);
	VRXSlider.SetValue(vrXscale);
	VRYSlider.SetExtents(VRYScaleMax, VRYScaleMin, 2);
	VRYSlider.SetValue(vrYscale);
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

SliderComponent::SliderComponent() :
	VRMenuComponent( VRMenuEventFlags_t( VRMENU_EVENT_TOUCH_DOWN ) |
		VRMENU_EVENT_TOUCH_DOWN |
		VRMENU_EVENT_FRAME_UPDATE |
		VRMENU_EVENT_FOCUS_GAINED |
        VRMENU_EVENT_FOCUS_LOST ),
	HasFocus( false ),
	TouchDown( false ),
	Progress( 0.0f ),
	Max(1.0),
	Min(0.0),
	Background( NULL ),
	ScrubBar( NULL ),
	CurrentTime( NULL ),
	SeekTime( NULL ),
	OnClickFunction( NULL ),
	OnClickObject( NULL )

{
}

void SliderComponent::SetExtents( const float max, const float min, const int sigfigs )
{
	Max = max;
	Min = min;
	SigFigs = sigfigs;

	SetProgress( Progress );
}

float SliderComponent::ScaleValue(const float value)
{
	return Min + (Max - Min) * value;
}

void SliderComponent::SetOnClick( void ( *callback )( SliderComponent *, void *, float ), void *object )
{
	OnClickFunction = callback;
	OnClickObject = object;
}

void SliderComponent::SetWidgets( UIWidget *background, UIWidget *scrubBar, UILabel *currentTime, UILabel *seekTime, const int scrubBarWidth )
{
	Background 		= background;
	ScrubBar 		= scrubBar;
	CurrentTime 	= currentTime;
	SeekTime 		= seekTime;
	ScrubBarWidth	= scrubBarWidth;

	SeekTime->SetVisible( false );
}

void SliderComponent::SetValue( const float value )
{
	float prog = (value - Min) / (Max - Min) ;
	if(prog>1.0) prog = 1.0;
	if(prog<0.0) prog = 0.0;
	SetProgress( prog );
}

void SliderComponent::SetProgress( const float progress )
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
	SetText( CurrentTime, ScaleValue(progress) );
}

void SliderComponent::SetText( UILabel *label, const float value )
{
	if( SigFigs == 0 )
	{
		label->SetText( StringUtils::Va( "%d", (int) value ) );
	}
	else if( SigFigs < 0 )
	{ // Hex, just for fun
		label->SetText( StringUtils::Va( "%#x", (int) value ) );
	}
	else if( SigFigs > 1000)
	{ // Why are you adjusting numbers large enough to need exponent notation with a slider?
		label->SetText( StringUtils::Va( "%.*g", SigFigs - 1000, value ) );
	}
	else
	{
		label->SetText( StringUtils::Va( "%.*f", SigFigs, value ) );
	}
}

eMsgStatus SliderComponent::OnEvent_Impl( OvrGuiSys & guiSys, VrFrame const & vrFrame,
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

eMsgStatus SliderComponent::OnFrame( OvrGuiSys & guiSys, VrFrame const & vrFrame,
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

			SetText( SeekTime, ScaleValue(progress) );
		}
	}

	return MSG_STATUS_ALIVE;
}

void SliderComponent::OnClick( OvrGuiSys & guiSys, VrFrame const & vrFrame, VRMenuEvent const & event )
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
		( *OnClickFunction )( this, OnClickObject, ScaleValue(progress) );
	}
}

} // namespace VRMatterStreamTheater
