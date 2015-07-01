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

class ScrubBarComponent : public VRMenuComponent
{
public:
	static const int 		TYPE_ID = 152414;

							ScrubBarComponent();

	virtual int				GetTypeId() const { return TYPE_ID; }

	void					SetDuration( const int duration );
	void					SetOnClick( void ( *callback )( ScrubBarComponent *, void *, float ), void *object );
	void					SetWidgets( UIWidget *background, UIWidget *scrubBar, UILabel *currentTime, UILabel *seekTime, const int scrubBarWidth );
	void 					SetProgress( const float progress );

	bool					IsScrubbing() const { return TouchDown; }

private:
	bool					HasFocus;
	bool					TouchDown;

	float					Progress;
	int						Duration;

	UIWidget *				Background;
	UIWidget *				ScrubBar;
	UILabel *				CurrentTime;
	UILabel *				SeekTime;
	int 					ScrubBarWidth;

	void 					( *OnClickFunction )( ScrubBarComponent *button, void *object, float progress );
	void *					OnClickObject;

private:
	virtual eMsgStatus      OnEvent_Impl( OvrGuiSys & guiSys, VrFrame const & vrFrame,
                                    VRMenuObject * self, VRMenuEvent const & event );

	eMsgStatus 				OnFrame( OvrGuiSys & guiSys, VrFrame const & vrFrame,
									VRMenuObject * self, VRMenuEvent const & event );

	void 					OnClick( OvrGuiSys & guiSys, VrFrame const & vrFrame, VRMenuEvent const & event );

	void 					SetTimeText( UILabel *label, const int time );
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

	static const int		MaxSeekSpeed;
	static const int 		ScrubBarWidth;

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

	UIImage					SeekbarBackground;
	UIImage					SeekbarProgress;
	ScrubBarComponent 		ScrubBar;

	UILabel 				CurrentTime;
	UILabel 				SeekTime;

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

	UIButton				MouseMenuButton;
	UIContainer *			MouseMenu;
	UITextButton			ButtonGaze;
	UITextButton			ButtonTrackpad;
	UITextButton			ButtonOff;
	UITextButton			ButtonXSensitivity;
	UITextButton			ButtonYSensitivity;

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
	UITextButton			ButtonDistance;
	UITextButton			ButtonSize;

	UIButton				ControllerMenuButton;
	UIContainer *			ControllerMenu;
	UITextButton			ButtonSpeed;
	UITextButton			ButtonComfortMode;
	UITextButton			ButtonMapKeyboard;


	bool					BackgroundClicked;
	bool					UIOpened;							// Used to ignore button A or touchpad until release so we don't close the UI immediately after opening it

	float 					s00,s01,s10,s11,s20,s21;		// Last stick positions so we don't have to make network traffic when not changing
	bool					allowDrag;
	double					clickStartTime;
	signed char				lastScroll;
	Vector2f 				lastMouse;
	bool					mouseDownLeft;
	bool					mouseDownRight;
	bool					mouseDownMiddle;

private:
	void					TextButtonHelper(UITextButton& button);
	void 					CreateMenu( OvrGuiSys & guiSys );

	void					BackPressed();

	friend void 			PlayPressedCallback( UIButton *button, void *object );
	void					RewindPressed();
	friend void 			RewindPressedCallback( UIButton *button, void *object );
	void					FastForwardPressed();
	friend void 			FastForwardPressedCallback( UIButton *button, void *object );
	void					CarouselPressed();
	friend void 			CarouselPressedCallback( UIButton *button, void *object );
	void					ScrubBarClicked( const float progress );
	friend void				ScrubBarCallback( ScrubBarComponent *, void *, float );

	friend void		MouseMenuButtonCallback( UIButton *button, void *object );
	void			MouseMenuButtonPressed();
	friend void		StreamMenuButtonCallback( UIButton *button, void *object );
	void			StreamMenuButtonPressed();
	friend void		ScreenMenuButtonCallback( UIButton *button, void *object );
	void			ScreenMenuButtonPressed();
	friend void		ControllerMenuButtonCallback( UIButton *button, void *object );
	void			ControllerMenuButtonPressed();

	friend void		GazeCallback( UITextButton *button, void *object );
	void			GazePressed();
	friend void		TrackpadCallback( UITextButton *button, void *object );
	void			TrackpadPressed();
	friend void		OffCallback( UITextButton *button, void *object );
	void			OffPressed();
	friend void		XSensitivityCallback( UITextButton *button, void *object );
	void			XSensitivityPressed();
	friend void		YSensitivityCallback( UITextButton *button, void *object );
	void			YSensitivityPressed();
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
	friend void		ChangeSeatCallback( UITextButton *button, void *object );
	void			ChangeSeatPressed();
	friend void		DistanceCallback( UITextButton *button, void *object );
	void			DistancePressed();
	friend void		SizeCallback( UITextButton *button, void *object );
	void			SizePressed();
	friend void		SpeedCallback( UITextButton *button, void *object );
	void			SpeedPressed();
	friend void		ComfortModeCallback( UITextButton *button, void *object );
	void			ComfortModePressed();
	friend void		MapKeyboardCallback( UITextButton *button, void *object );
	void			MapKeyboardPressed();


	Vector2f 				GazeCoordinatesOnScreen( const Matrix4f & viewMatrix, const Matrix4f panelMatrix ) const;

	void 					UpdateUI( const VrFrame & vrFrame );
	void 					CheckInput( const VrFrame & vrFrame );
	void					HandleGazeMouse( const VrFrame & vrFrame, bool onscreen, const Vector2f screenCursor  );
	void 					CheckDebugControls( const VrFrame & vrFrame );

	void 					ShowUI();
	void 					HideUI();
};

} // namespace VRMatterStreamTheater

#endif // MoviePlayerView_h
