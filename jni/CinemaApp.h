/************************************************************************************

Filename    :   CinemaApp.h
Content     :   
Created     :	6/17/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "App.h"
#include "ShaderManager.h"
#include "ModelManager.h"
#include "SceneManager.h"
#include "ViewManager.h"
#include "MovieManager.h"
#include "MoviePlayerView.h"
#include "MovieSelectionView.h"
#include "TheaterSelectionView.h"
#include "ResumeMovieView.h"

using namespace OVR;

namespace OculusCinema {

class CinemaApp : public OVR::VrAppInterface
{
public:
							CinemaApp();

	virtual void 			OneTimeInit( const char * fromPackage, const char * launchIntentJSON, const char * launchIntentURI );
	virtual void			OneTimeShutdown();

	virtual Matrix4f 		DrawEyeView( const int eye, const float fovDegrees );

	virtual void			ConfigureVrMode( ovrModeParms & modeParms );

	virtual void 			Command( const char * msg );
	virtual bool 			OnKeyEvent( const int keyCode, const KeyState::eKeyEventType eventType );

	// Called by App loop
	virtual Matrix4f 		Frame( const VrFrame vrFrame );

	void			    	SetPlaylist( const Array<const MovieDef *> &playList, const int nextMovie );
	void			    	SetMovie( const MovieDef * nextMovie );
	void 					MovieLoaded( const int width, const int height, const int duration );

	const MovieDef *		GetCurrentMovie() const { return CurrentMovie; }
	const MovieDef *		GetNextMovie() const;
	const MovieDef *		GetPreviousMovie() const;

	const SceneDef & 		GetCurrentTheater() const;

	void 					StartMoviePlayback();
	void 					ResumeMovieFromSavedLocation();
	void					PlayMovieFromBeginning();
	void 					ResumeOrRestartMovie();
	void 					TheaterSelection();
	void 					MovieSelection( bool inLobby );
	void					MovieFinished();
	void					UnableToPlayMovie();

	bool 					AllowTheaterSelection() const;
	bool 					IsMovieFinished() const;

	const char *			RetailDir( const char *dir ) const;
	const char *			ExternalRetailDir( const char *dir ) const;
	const char *			SDCardDir( const char *dir ) const;
	const char * 			ExternalSDCardDir( const char *dir ) const;
	const char * 			ExternalCacheDir( const char *dir ) const;
	bool 					IsExternalSDCardDir( const char *dir ) const;
	bool 					FileExists( const char *filename ) const;

public:
	double					StartTime;

	jclass					MainActivityClass;	// need to look up from main thread

	SceneManager			SceneMgr;
	ShaderManager 			ShaderMgr;
	ModelManager 			ModelMgr;
	MovieManager 			MovieMgr;

	bool					InLobby;
	bool					AllowDebugControls;

private:
	ViewManager				ViewMgr;
	MoviePlayerView			MoviePlayer;
	MovieSelectionView		MovieSelectionMenu;
	TheaterSelectionView	TheaterSelectionMenu;
	ResumeMovieView			ResumeMovieMenu;

	VrFrame					vrFrame;
	int						FrameCount;

    const MovieDef *		CurrentMovie;
	Array<const MovieDef *> PlayList;

	bool					ShouldResumeMovie;
	bool					MovieFinishedPlaying;
};

} // namespace OculusCinema
