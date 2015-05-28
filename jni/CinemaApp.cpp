/************************************************************************************

Filename    :   CinemaApp.cpp
Content     :   
Created     :	6/17/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "Kernel/OVR_String_Utils.h"
#include "CinemaApp.h"
#include "Native.h"
#include "CinemaStrings.h"
#include <sys/system_properties.h>

//=======================================================================================

namespace VRMatterStreamTheater {

CinemaApp::CinemaApp() :
	StartTime( 0 ),
	SceneMgr( *this ),
	ShaderMgr( *this ),
	ModelMgr( *this ),
	PcMgr( *this ),
	AppMgr( *this ),
	InLobby( true ),
	AllowDebugControls( false ),
	ViewMgr(),
	MoviePlayer( *this ),
	PcSelectionMenu( *this ),
	AppSelectionMenu( *this ),
	TheaterSelectionMenu( *this ),
	ResumeMovieMenu( *this ),
	vrFrame(),
	FrameCount( 0 ),
	CurrentMovie( NULL ),
	PlayList(),
	ShouldResumeMovie( false ),
	MovieFinishedPlaying( false ),
	DelayedError( NULL )

{
}

/*
 * OneTimeInit
 *
 */
void CinemaApp::OneTimeInit( const char * fromPackage, const char * launchIntentJSON, const char * launchIntentURI ) {
	LOG( "--------------- CinemaApp OneTimeInit ---------------");

	StartTime = ovr_GetTimeInSeconds();

	app->GetVrParms().colorFormat = COLOR_8888;
	//app->GetVrParms().depthFormat = DEPTH_16;
	app->GetVrParms().multisamples = 2;

	Native::OneTimeInit( app, ActivityClass );
	CinemaStrings::OneTimeInit( *this );
	ShaderMgr.OneTimeInit( launchIntentURI );
	ModelMgr.OneTimeInit( launchIntentURI );
	SceneMgr.OneTimeInit( launchIntentURI );
	PcMgr.OneTimeInit( launchIntentURI );
	AppMgr.OneTimeInit( launchIntentURI );
	MoviePlayer.OneTimeInit( launchIntentURI );
	ViewMgr.AddView( &MoviePlayer );
	PcSelectionMenu.OneTimeInit( launchIntentURI );
	ViewMgr.AddView( &PcSelectionMenu );
	AppSelectionMenu.OneTimeInit( launchIntentURI );
	ViewMgr.AddView( &AppSelectionMenu );
	TheaterSelectionMenu.OneTimeInit( launchIntentURI );
	ViewMgr.AddView( &TheaterSelectionMenu );
	ResumeMovieMenu.OneTimeInit( launchIntentURI );

	PcSelection( true );

	LOG( "CinemaApp::OneTimeInit: %3.1f seconds", ovr_GetTimeInSeconds() - StartTime );
}

void CinemaApp::OneTimeShutdown()
{
	LOG( "--------------- CinemaApp OneTimeShutdown ---------------");

	Native::OneTimeShutdown();
	ShaderMgr.OneTimeShutdown();
	ModelMgr.OneTimeShutdown();
	SceneMgr.OneTimeShutdown();
	PcMgr.OneTimeShutdown();
	AppMgr.OneTimeShutdown();
	MoviePlayer.OneTimeShutdown();
	PcSelectionMenu.OneTimeShutdown();
	AppSelectionMenu.OneTimeShutdown();
	TheaterSelectionMenu.OneTimeShutdown();
	ResumeMovieMenu.OneTimeShutdown();
}

const char * CinemaApp::RetailDir( const char *dir ) const
{
	static char subDir[ 256 ];
	StringUtils::SPrintf( subDir, "%s/%s", SDCardDir( "RetailMedia" ), dir );
	return subDir;
}

const char * CinemaApp::ExternalRetailDir( const char *dir ) const
{
	static char subDir[ 256 ];
	StringUtils::SPrintf( subDir, "%s/%s", ExternalSDCardDir( "RetailMedia" ), dir );
	return subDir;
}

const char * CinemaApp::SDCardDir( const char *dir ) const
{
	static char subDir[ 256 ];
	StringUtils::SPrintf( subDir, "/sdcard/%s", dir );
	return subDir;
}

const char * CinemaApp::ExternalSDCardDir( const char *dir ) const
{
	static char subDir[ 256 ];
	StringUtils::SPrintf( subDir, "/storage/extSdCard/%s", dir );
	return subDir;
}

const char * CinemaApp::ExternalCacheDir( const char *dir ) const
{
	static char subDir[ 256 ];
	StringUtils::SPrintf( subDir, "%s/%s", Native::GetExternalCacheDirectory( app ).ToCStr(), dir );
	return subDir;
}

bool CinemaApp::IsExternalSDCardDir( const char *dir ) const
{
	const char * sdcardDir = ExternalSDCardDir( "" );
	const int l = strlen( sdcardDir );
	return ( 0 == strncmp( sdcardDir, dir, l ) );
}

bool CinemaApp::FileExists( const char *filename ) const
{
	FILE *f = fopen( filename, "r" );
	if ( !f )
	{
		return false;
	}
	else
	{
		fclose( f );
		return true;
	}
}

void CinemaApp::SetPlaylist( const Array<const PcDef *> &playList, const int nextMovie )
{
	PlayList = playList;

	assert( nextMovie < PlayList.GetSizeI() );
	SetMovie( PlayList[ nextMovie ] );
}

void CinemaApp::SetMovie( const PcDef *movie )
{
	LOG( "SetMovie( %s )", movie->Name.ToCStr() );
	CurrentMovie = movie;
	MovieFinishedPlaying = false;
}

void CinemaApp::SetPc( const PcDef *pc )
{
	LOG( "SetPc( %s )", pc->Name.ToCStr() );
	CurrentPc = pc;
}

void CinemaApp::MovieLoaded( const int width, const int height, const int duration )
{
	MoviePlayer.MovieLoaded( width, height, duration );
}

const PcDef *CinemaApp::GetNextMovie() const
{
	const PcDef *next = NULL;
	if ( PlayList.GetSizeI() != 0 )
	{
		for( int i = 0; i < PlayList.GetSizeI() - 1; i++ )
		{
			if ( PlayList[ i ] == CurrentMovie )
			{
				next = PlayList[ i + 1 ];
				break;
			}
		}

		if ( next == NULL )
		{
			next = PlayList[ 0 ];
		}
	}

	return next;
}

const PcDef *CinemaApp::GetPreviousMovie() const
{
	const PcDef *previous = NULL;
	if ( PlayList.GetSizeI() != 0 )
	{
		for( int i = 0; i < PlayList.GetSizeI(); i++ )
		{
			if ( PlayList[ i ] == CurrentMovie )
			{
				break;
			}
			previous = PlayList[ i ];
		}

		if ( previous == NULL )
		{
			previous = PlayList[ PlayList.GetSizeI() - 1 ];
		}
	}

	return previous;
}

void CinemaApp::StartMoviePlayback()
{
	if ( CurrentMovie != NULL )
	{
		MovieFinishedPlaying = false;
		Native::StartMovie( app, CurrentPc->UUID.ToCStr(), CurrentMovie->Name.ToCStr(), CurrentMovie->Id, CurrentPc->Binding.ToCStr() );
		ShouldResumeMovie = false;
	}
}

void CinemaApp::ResumeMovieFromSavedLocation()
{
	LOG( "ResumeMovie");
	InLobby = false;
	ShouldResumeMovie = true;
	ViewMgr.OpenView( MoviePlayer );
}

void CinemaApp::PlayMovieFromBeginning()
{
	LOG( "PlayMovieFromBeginning");
	InLobby = false;
	ShouldResumeMovie = false;
	ViewMgr.OpenView( MoviePlayer );
}

void CinemaApp::ResumeOrRestartMovie()
{
	LOG( "StartMovie");
	PlayMovieFromBeginning();
}

void CinemaApp::MovieFinished()
{
	InLobby = false;
	MovieFinishedPlaying = true;
	AppSelectionMenu.SetAppList( PlayList, GetNextMovie() );
	ViewMgr.OpenView( AppSelectionMenu );
}

void CinemaApp::UnableToPlayMovie()
{
	InLobby = false;
	AppSelectionMenu.SetError( CinemaStrings::Error_UnableToPlayMovie.ToCStr(), false, true );
	ViewMgr.OpenView( AppSelectionMenu );
}

void CinemaApp::TheaterSelection()
{
	ViewMgr.OpenView( TheaterSelectionMenu );
}

void CinemaApp::PcSelection( bool inLobby )
{
	InLobby = inLobby;
	ViewMgr.OpenView( PcSelectionMenu );
}

void CinemaApp::AppSelection( bool inLobby )
{
	InLobby = inLobby;
	ViewMgr.OpenView( AppSelectionMenu );
}

bool CinemaApp::AllowTheaterSelection() const
{
	return true;
}

bool CinemaApp::IsMovieFinished() const
{
	return MovieFinishedPlaying;
}


const SceneDef & CinemaApp::GetCurrentTheater() const
{
	return ModelMgr.GetTheater( TheaterSelectionMenu.GetSelectedTheater() );
}

/*
 * DrawEyeView
 */
Matrix4f CinemaApp::DrawEyeView( const int eye, const float fovDegrees ) {
	return ViewMgr.DrawEyeView( eye, fovDegrees );
}

void CinemaApp::ConfigureVrMode( ovrModeParms & modeParms ) {
	// We need very little CPU for movie playing, but a fair amount of GPU.
	// The CPU clock should ramp up above the minimum when necessary.
	LOG( "ConfigureClocks: Cinema only needs minimal clocks" );

	char model_id[PROP_VALUE_MAX]; // PROP_VALUE_MAX from <sys/system_properties.h>.
	int len;
	len = __system_property_get("ro.build.version.sdk", model_id);
	if (len >= 2 && model_id[0] == '2' && model_id[1] == '1') {
		LOG("Adjusting CPU level for Lollipop");
		modeParms.CpuLevel = 2;
		modeParms.GpuLevel = 1;
	}
	else
	{
		LOG("Adjusting CPU level for KitKat: %s", model_id);
		modeParms.CpuLevel = 1;
		modeParms.GpuLevel = 2;
	}

	// when the app is throttled, go to the platform UI and display a
	// dismissable warning. On return to the app, force 30hz timewarp.
	modeParms.AllowPowerSave = true;

	// Always use 2x MSAA for now
	app->GetVrParms().multisamples = 2;
}

/*
 * Command
 *
 * Actions that need to be performed on the render thread.
 */
void CinemaApp::Command( const char * msg )
{
	if ( ModelMgr.Command( msg ) )
	{
		return;
	}

	if ( ViewMgr.Command( msg ) )
	{
		return;
	}

	if ( SceneMgr.Command( msg ) )
	{
		return;
	}
}

/*
 * Frame()
 *
 * App override
 */
Matrix4f CinemaApp::Frame( const VrFrame vrFrame )
{
	FrameCount++;
	this->vrFrame = vrFrame;

	if(DelayedError != NULL && !ViewMgr.ChangingViews())
	{
		ShowError(*DelayedError);
		delete DelayedError;
		DelayedError = NULL;
	}

	return ViewMgr.Frame( vrFrame );
}

bool CinemaApp::OnKeyEvent( const int keyCode, const KeyState::eKeyEventType eventType )
{
	return ViewMgr.OnKeyEvent( keyCode, eventType );
}


void CinemaApp::ShowPair( const String& msg )
{
	AppSelectionMenu.SetError(msg.ToCStr(),false,false);
}

void CinemaApp::PairSuccess()
{
	AppSelectionMenu.ClearError();
	AppSelectionMenu.PairSuccess();
}

void CinemaApp::ShowError( const String& msg )
{
	if(ViewMgr.ChangingViews())
	{
		if( DelayedError == NULL)
		{
			DelayedError = new OVR::String(msg);
		}
	}
	else
	{
		View *view = ViewMgr.GetCurrentView();
		if(view) view->SetError(msg.ToCStr(), false, true);
	}
}

void CinemaApp::ClearError()
{
	View *view = ViewMgr.GetCurrentView();
	if(view) view->ClearError();
}


} // namespace VRMatterStreamTheater
