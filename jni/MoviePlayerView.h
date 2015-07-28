/************************************************************************************

Filename    :   MoviePlayerView.h
Content     :
Created     :	6/17/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( MoviePlayerView_h )
#define MoviePlayerView_h

#include "GazeCursor.h"
#include "VRMenu/GuiSys.h"
#include "Lerp.h"
#include "UI/UITexture.h"
#include "UI/UIMenu.h"
#include "UI/UIContainer.h"
#include "UI/UILabel.h"
#include "UI/UIImage.h"
#include "UI/UIButton.h"
#include "UI/UITextButton.h"
#include "Settings.h"

using namespace OVR;

namespace VRMatterStreamTheater {

enum ePlaybackControlsEvent
{
	UI_NO_EVENT = 0,
	UI_RW_PRESSED = 1,
	UI_PLAY_PRESSED = 2,
	UI_FF_PRESSED = 3,
	UI_CAROUSEL_PRESSED = 4,
	UI_CLOSE_UI_PRESSED = 5,
	UI_USER_TIMEOUT = 6,
	UI_SEEK_PRESSED = 7
};

enum MouseMode
{
	MOUSE_OFF = 0,
	MOUSE_GAZE,
	MOUSE_TRACKPAD
};

class CinemaApp;

class ControlsGazeTimer : public VRMenuComponent
{
public:
	static const int 		TYPE_ID = 152413;

							ControlsGazeTimer();

	virtual int				GetTypeId() const { return TYPE_ID; }

	void					SetGazeTime();
	double					GetLastGazeTime() const { return LastGazeTime; }

	bool					IsFocused() const { return HasFocus; }

private:
	double					LastGazeTime;
	bool					HasFocus;

private:
	virtual eMsgStatus		OnEvent_Impl( OvrGuiSys & guiSys, VrFrame const & vrFrame,
									VRMenuObject * self, VRMenuEvent const & event );
};

class SliderComponent : public VRMenuComponent
{
public:
	static const int 		TYPE_ID = 152414;

							SliderComponent();

	virtual int				GetTypeId() const { return TYPE_ID; }

	void					SetExtents( const float max, const float min, const int sigfigs );
	void					SetOnClick( void ( *callback )( SliderComponent *, void *, float ), void *object );
	void					SetWidgets( UIWidget *background, UIWidget *scrubBar, UILabel *currentTime, UILabel *seekTime, const int scrubBarWidth );
	void 					SetProgress( const float progress );
	void					SetValue( const float value );

	float					ScaleValue(const float value);
	bool					IsScrubbing() const { return TouchDown; }

private:
	bool					HasFocus;
	bool					TouchDown;

	float					Progress;
	float					Max;
	float					Min;
	int						SigFigs;

	UIWidget *				Background;
	UIWidget *				ScrubBar;
	UILabel *				CurrentTime;
	UILabel *				SeekTime;
	int 					ScrubBarWidth;

	void 					( *OnClickFunction )( SliderComponent *button, void *object, float progress );
	void *					OnClickObject;

private:
	virtual eMsgStatus      OnEvent_Impl( OvrGuiSys & guiSys, VrFrame const & vrFrame,
                                    VRMenuObject * self, VRMenuEvent const & event );

	eMsgStatus 				OnFrame( OvrGuiSys & guiSys, VrFrame const & vrFrame,
									VRMenuObject * self, VRMenuEvent const & event );

	void 					OnClick( OvrGuiSys & guiSys, VrFrame const & vrFrame, VRMenuEvent const & event );

	void 					SetText( UILabel *label, const float value );
};

class MoviePlayerView : public View
{
public:
							MoviePlayerView( CinemaApp &app_ );
	virtual 				~MoviePlayerView();

	virtual void 			OneTimeInit( const char * launchIntent );
	virtual void			OneTimeShutdown();

	virtual void 			OnOpen();
	virtual void 			OnClose();

	virtual bool 			Command( const char * msg );
	virtual bool 			OnKeyEvent( const int keyCode, const int repeatCount, const KeyEventType eventType );

	void 					MovieLoaded( const int width, const int height, const int duration );

	virtual Matrix4f 		DrawEyeView( const int eye, const float fovDegrees );
	virtual Matrix4f 		Frame( const VrFrame & vrFrame );

		virtual void			SetError( const char *text, bool showSDCard, bool showErrorIcon );

private:
	CinemaApp &				Cinema;

	gazeCursorUserId_t		GazeUserId;	// id unique to this swipe view for interacting with gaze cursor

	bool					uiActive;

	bool					RepositionScreen;

	static const double 	GazeTimeTimeout;

	UITexture				BackgroundTintTexture;

	UITexture				RWTexture;
	UITexture				RWHoverTexture;
	UITexture				RWPressedTexture;

	UITexture				FFTexture;
	UITexture				FFHoverTexture;
	UITexture				FFPressedTexture;

	UITexture				PlayTexture;
	UITexture				PlayHoverTexture;
	UITexture				PlayPressedTexture;

	UITexture				PauseTexture;
	UITexture				PauseHoverTexture;
	UITexture				PausePressedTexture;

	UITexture				CarouselTexture;
	UITexture				CarouselHoverTexture;
	UITexture				CarouselPressedTexture;

	UITexture				SeekbarBackgroundTexture;
	UITexture				SeekbarProgressTexture;

	UITexture				SeekPosition;

	UIMenu *				MoveScreenMenu;
	UILabel 				MoveScreenLabel;
	Lerp					MoveScreenAlpha;

	UIMenu *				PlaybackControlsMenu;
	UIContainer 			PlaybackControlsPosition;
	UIContainer 			PlaybackControlsScale;
	UILabel 				MovieTitleLabel;

	UIImage					ControlsBackground;
	ControlsGazeTimer		GazeTimer;

	UIButton				RewindButton;
	UIButton				PlayButton;
	UIButton				FastForwardButton;
	UIButton				CarouselButton;

	UITexture				MouseTexture;
	UITexture				MouseHoverTexture;
	UITexture				MousePressedTexture;

	UITexture				StreamTexture;
	UITexture				StreamHoverTexture;
	UITexture				StreamPressedTexture;

	UITexture				ScreenTexture;
	UITexture				ScreenHoverTexture;
	UITexture				ScreenPressedTexture;

	UITexture				ControllerTexture;
	UITexture				ControllerHoverTexture;
	UITexture				ControllerPressedTexture;

	UIContainer *			SaveMenu;
	UITextButton			ButtonSaveApp;
	UITextButton			ButtonSaveDefault;
	UITextButton			ButtonResetSettings;
	UITextButton			ButtonSaveSettings1;
	UITextButton			ButtonSaveSettings2;
	UITextButton			ButtonSaveSettings3;
	UITextButton			ButtonLoadSettings1;
	UITextButton			ButtonLoadSettings2;
	UITextButton			ButtonLoadSettings3;

	UIButton				MouseMenuButton;
	UIContainer *			MouseMenu;
	UITextButton			ButtonGaze;
	UITextButton			ButtonTrackpad;
	UITextButton			ButtonOff;
	UILabel					GazeScale;
	UIImage					GazeSliderBackground;
	UIImage					GazeSliderIndicator;
	UILabel 				GazeCurrentSetting;
	UILabel 				GazeNewSetting;
	SliderComponent 		GazeSlider;

	UILabel					TrackpadScale;
	UIImage					TrackpadSliderBackground;
	UIImage					TrackpadSliderIndicator;
	UILabel 				TrackpadCurrentSetting;
	UILabel 				TrackpadNewSetting;
	SliderComponent 		TrackpadSlider;

	UIButton				StreamMenuButton;
	UIContainer *			StreamMenu;
	UITextButton			Button1080p60;
	UITextButton			Button1080p30;
	UITextButton			Button720p60;
	UITextButton			Button720p30;
	UITextButton			ButtonHostAudio;

	UIButton				ScreenMenuButton;
	UIContainer *			ScreenMenu;
	UITextButton			ButtonSBS;
	UITextButton			ButtonChangeSeat;

	UILabel					ScreenDistance;
	UIImage					DistanceSliderBackground;
	UIImage					DistanceSliderIndicator;
	UILabel 				DistanceCurrentSetting;
	UILabel 				DistanceNewSetting;
	SliderComponent 		DistanceSlider;

	UILabel					ScreenSize;
	UIImage					SizeSliderBackground;
	UIImage					SizeSliderIndicator;
	UILabel 				SizeCurrentSetting;
	UILabel 				SizeNewSetting;
	SliderComponent 		SizeSlider;

	UIButton				ControllerMenuButton;
	UIContainer *			ControllerMenu;
	UITextButton			ButtonSpeed;
	UITextButton			ButtonComfortMode;
	UITextButton			ButtonMapKeyboard;

	float					settingsVersion;
	String					defaultSettingsPath;
	String					settings1Path;
	String					settings2Path;
	String					settings3Path;
	String					appSettingsPath;
	Settings*				defaultSettings;
	Settings*				settings1;
	Settings*				settings2;
	Settings*				settings3;
	Settings*				appSettings;

	bool					BackgroundClicked;
	bool					UIOpened;							// Used to ignore button A or touchpad until release so we don't close the UI immediately after opening it

	float 					s00,s01,s10,s11,s20,s21;		// Last stick positions so we don't have to make network traffic when not changing
	bool					allowDrag;
	bool					mouseMoving;
	double					clickStartTime;
	signed char				lastScroll;
	Vector2f 				lastMouse;
	bool					mouseDownLeft;
	bool					mouseDownRight;
	bool					mouseDownMiddle;

	MouseMode				mouseMode;
	float					gazeScaleValue;
	float					trackpadScaleValue;

	int						streamWidth;
	int 					streamHeight;
	int						streamFPS;
	bool					streamHostAudio;

	float					GazeMin;
	float					GazeMax;
	float					TrackpadMin;
	float					TrackpadMax;
	float					VoidScreenDistanceMin;
	float					VoidScreenDistanceMax;
	float					VoidScreenScaleMin;
	float					VoidScreenScaleMax;

private:
	void					TextButtonHelper(UITextButton& button, float scale = 1.0f, int w = 320, int h = 120);
	void					SetUpSlider(OvrGuiSys & guiSys, UIWidget *parent, SliderComponent& scrub, UIImage& bg,
								UIImage& ind, UILabel& cur, UILabel& set, int slideWidth, int xoff, int yoff);
	void 					CreateMenu( OvrGuiSys & guiSys );

	void					BackPressed();
	void					BackPressedDouble();

	friend void		MouseMenuButtonCallback( UIButton *button, void *object );
	void			MouseMenuButtonPressed();
	friend void		StreamMenuButtonCallback( UIButton *button, void *object );
	void			StreamMenuButtonPressed();
	friend void		ScreenMenuButtonCallback( UIButton *button, void *object );
	void			ScreenMenuButtonPressed();
	friend void		ControllerMenuButtonCallback( UIButton *button, void *object );
	void			ControllerMenuButtonPressed();

	friend void		SaveAppCallback( UITextButton *button, void *object );
	void			SaveAppPressed();
	friend void		SaveDefaultCallback( UITextButton *button, void *object );
	void			SaveDefaultPressed();
	friend void		ResetDefaultCallback( UITextButton *button, void *object );
	void			ResetDefaultPressed();
	friend void		Save1Callback( UITextButton *button, void *object );
	void			Save1Pressed();
	friend void		Save2Callback( UITextButton *button, void *object );
	void			Save2Pressed();
	friend void		Save3Callback( UITextButton *button, void *object );
	void			Save3Pressed();
	friend void		Load1Callback( UITextButton *button, void *object );
	void			Load1Pressed();
	friend void		Load2Callback( UITextButton *button, void *object );
	void			Load2Pressed();
	friend void		Load3Callback( UITextButton *button, void *object );
	void			Load3Pressed();

	friend void		GazeCallback( UITextButton *button, void *object );
	void			GazePressed();
	friend void		TrackpadCallback( UITextButton *button, void *object );
	void			TrackpadPressed();
	friend void		OffCallback( UITextButton *button, void *object );
	void			OffPressed();
	friend void		GazeScaleCallback( SliderComponent *button, void *object, const float value );
	void			GazeScalePressed(const float value);
	friend void		TrackpadScaleCallback( SliderComponent *button, void *object, const float value );
	void			TrackpadScalePressed(const float value);

	friend bool		GazeActiveCallback( UITextButton *button, void *object );
	bool			GazeActive();
	friend bool		TrackpadActiveCallback( UITextButton *button, void *object );
	bool			TrackpadActive();
	friend bool		OffActiveCallback( UITextButton *button, void *object );
	bool			OffActive();


	friend void		Button1080p60Callback( UITextButton *button, void *object );
	void			Button1080p60Pressed();
	friend void		Button1080p30Callback( UITextButton *button, void *object );
	void			Button1080p30Pressed();
	friend void		Button720p60Callback( UITextButton *button, void *object );
	void			Button720p60Pressed();
	friend void		Button720p30Callback( UITextButton *button, void *object );
	void			Button720p30Pressed();
	friend void		HostAudioCallback( UITextButton *button, void *object );
	void			HostAudioPressed();
	friend void		SBSCallback( UITextButton *button, void *object );
	void			SBSPressed();

	friend bool		Button1080p60IsSelectedCallback( UITextButton *button, void *object );
	bool			Button1080p60IsSelected();
	friend bool		Button1080p30IsSelectedCallback( UITextButton *button, void *object );
	bool			Button1080p30IsSelected();
	friend bool		Button720p60IsSelectedCallback( UITextButton *button, void *object );
	bool			Button720p60IsSelected();
	friend bool		Button720p30IsSelectedCallback( UITextButton *button, void *object );
	bool			Button720p30IsSelected();
	friend bool		HostAudioIsSelectedCallback( UITextButton *button, void *object );
	bool			HostAudioIsSelected();


	friend void		ChangeSeatCallback( UITextButton *button, void *object );
	void			ChangeSeatPressed();
	friend void		DistanceCallback( SliderComponent *button, void *object, const float value );
	void			DistancePressed( const float value);
	friend void		SizeCallback( SliderComponent *button, void *object, const float value );
	void			SizePressed( const float value);

	friend bool		IsChangeSeatsEnabledCallback( UITextButton *button, void *object );
	bool			IsChangeSeatsEnabled();


	friend void		SpeedCallback( UITextButton *button, void *object );
	void			SpeedPressed();
	friend void		ComfortModeCallback( UITextButton *button, void *object );
	void			ComfortModePressed();
	friend void		MapKeyboardCallback( UITextButton *button, void *object );
	void			MapKeyboardPressed();


	Vector2f 				GazeCoordinatesOnScreen( const Matrix4f & viewMatrix, const Matrix4f panelMatrix ) const;

	void					LoadSettings(Settings* set);
	void					InitializeSettings();
	void					UpdateMenus();

	void 					UpdateUI( const VrFrame & vrFrame );
	void 					CheckInput( const VrFrame & vrFrame );
	void					HandleGazeMouse( const VrFrame & vrFrame, bool onscreen, const Vector2f screenCursor );
	void					HandleTrackpadMouse( const VrFrame & vrFrame );
	void 					CheckDebugControls( const VrFrame & vrFrame );

	void 					ShowUI();
	void 					HideUI();
};

} // namespace VRMatterStreamTheater

#endif // MoviePlayerView_h
