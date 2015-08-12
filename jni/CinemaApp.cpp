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

//=======================================================================================

namespace VRMatterStreamTheater {

CinemaApp::CinemaApp() :
	GuiSys( OvrGuiSys::Create() ),
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
	MessageQueue( 100 ),
	vrFrame(),
	FrameCount( 0 ),
	CurrentMovie( NULL ),
	PlayList(),
	ShouldResumeMovie( false ),
	MovieFinishedPlaying( false ),
	DelayedError( NULL )

{
}

CinemaApp::~CinemaApp()
{
	OvrGuiSys::Destroy( GuiSys );
}

void CinemaApp::Configure( ovrSettings & settings )
{
	// We need very little CPU for movie playing, but a fair amount of GPU.
	// The CPU clock should ramp up above the minimum when necessary.
	settings.ModeParms.CpuLevel = 1;
	settings.ModeParms.GpuLevel = 2;

	// when the app is throttled, go to the platform UI and display a
	// dismissable warning. On return to the app, force 30Hz timewarp.
	settings.ModeParms.AllowPowerSave = true;

	// Default to 2x MSAA.
	settings.EyeBufferParms.colorFormat = COLOR_8888;
	//settings.EyeBufferParms.depthFormat = DEPTH_16;
	settings.EyeBufferParms.multisamples = 2;
}

void CinemaApp::OneTimeInit( const char * fromPackage, const char * launchIntentJSON, const char * launchIntentURI )
{
	LOG( "--------------- CinemaApp OneTimeInit ---------------");

	GuiSys->Init( app, &app->GetSoundMgr(), app->LoadFontForLocale(), &app->GetDebugLines() );

	StartTime = vrapi_GetTimeInSeconds();

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

	LOG( "CinemaApp::OneTimeInit: %3.1f seconds", vrapi_GetTimeInSeconds() - StartTime );
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

void CinemaApp::StartMoviePlayback(int width, int height, int fps, bool hostAudio)
{
	if ( CurrentMovie != NULL )
	{
		MovieFinishedPlaying = false;
		bool remote = CurrentPc->isRemote;
		Native::StartMovie( app, CurrentPc->UUID.ToCStr(), CurrentMovie->Name.ToCStr(), CurrentMovie->Id, CurrentPc->Binding.ToCStr(), width, height, fps, hostAudio, remote );
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

bool CinemaApp::OnKeyEvent( const int keyCode, const int repeatCount, const KeyEventType eventType )
{
	if ( GuiSys->OnKeyEvent( keyCode, repeatCount, eventType ) )
	{
		return true;
	}

	return ViewMgr.OnKeyEvent( keyCode, repeatCount, eventType );
}

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

Matrix4f CinemaApp::Frame( const VrFrame & vrFrame )
{
	// Reset any VR menu submissions from previous frame.
	GuiSys->BeginFrame();

	// Process incoming messages until the queue is empty.
	for ( ; ; )
	{
		const char * msg = MessageQueue.GetNextMessage();
		if ( msg == NULL )
		{
			break;
		}
		Command( msg );
		free( (void *)msg );
	}

	if(DelayedError != NULL && !ViewMgr.ChangingViews())
	{
		ShowError(*DelayedError);
		delete DelayedError;
		DelayedError = NULL;
	}

	FrameCount++;
	this->vrFrame = vrFrame;

	CenterViewMatrix = ViewMgr.Frame( vrFrame );

	// update gui systems after the app frame, but before rendering anything
	GuiSys->Frame( vrFrame, CenterViewMatrix );

	return CenterViewMatrix;
}

Matrix4f CinemaApp::DrawEyeView( const int eye, const float fovDegrees )
{
	Matrix4f mvpForEye = ViewMgr.DrawEyeView( eye, fovDegrees );

	GuiSys->RenderEyeView( CenterViewMatrix, mvpForEye );

	return mvpForEye;
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

void CinemaApp::MovieScreenUpdated()
{
	MoviePlayer.MovieScreenUpdated();
}

} // namespace VRMatterStreamTheater
