/************************************************************************************

Filename    :   Native.cpp
Content     :
Created     :	6/20/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "CinemaApp.h"
#include "Native.h"


namespace VRMatterStreamTheater
{

extern "C" {

long Java_com_vrmatter_streamtheater_MainActivity_nativeSetAppInterface( JNIEnv *jni, jclass clazz, jobject activity,
		jstring fromPackageName, jstring commandString, jstring uriString )
{
	LOG( "nativeSetAppInterface" );
	return (new CinemaApp())->SetActivity( jni, clazz, activity, fromPackageName, commandString, uriString );
}

void Java_com_vrmatter_streamtheater_MainActivity_nativeSetVideoSize( JNIEnv *jni, jclass clazz, jlong interfacePtr, int width, int height, int rotation, int duration ) {
	LOG( "nativeSetVideoSizes: width=%i height=%i rotation=%i duration=%i", width, height, rotation, duration );

	CinemaApp *cinema = ( CinemaApp * )( ( (App *)interfacePtr )->GetAppInterface() );
	cinema->app->GetMessageQueue().PostPrintf( "video %i %i %i %i", width, height, rotation, duration );
}

jobject Java_com_vrmatter_streamtheater_MainActivity_nativePrepareNewVideo( JNIEnv *jni, jclass clazz, jlong interfacePtr ) {
	CinemaApp *cinema = ( CinemaApp * )( ( (App *)interfacePtr )->GetAppInterface() );

	// set up a message queue to get the return message
	// TODO: make a class that encapsulates this work
	MessageQueue	result(1);
	cinema->app->GetMessageQueue().PostPrintf( "newVideo %p", &result);

	result.SleepUntilMessage();
	const char * msg = result.GetNextMessage();
	jobject	texobj;
	sscanf( msg, "surfaceTexture %p", &texobj );
	free( (void *)msg );

	return texobj;
}

}	// extern "C"

//==============================================================

// Java method ids
static jmethodID 	getExternalCacheDirectoryMethodId = NULL;
static jmethodID	createVideoThumbnailMethodId = NULL;
static jmethodID	checkForMovieResumeId = NULL;
static jmethodID 	isPlayingMethodId = NULL;
static jmethodID 	isPlaybackFinishedMethodId = NULL;
static jmethodID 	hadPlaybackErrorMethodId = NULL;
static jmethodID	getPositionMethodId = NULL;
static jmethodID	getDurationMethodId = NULL;
static jmethodID	setPositionMethodId = NULL;
static jmethodID 	seekDeltaMethodId = NULL;
static jmethodID 	startMovieMethodId = NULL;
static jmethodID 	pauseMovieMethodId = NULL;
static jmethodID 	resumeMovieMethodId = NULL;
static jmethodID 	stopMovieMethodId = NULL;
static jmethodID 	togglePlayingMethodId = NULL;

// Error checks and exits on failure
static jmethodID GetMethodID( App *app, jclass cls, const char * name, const char * signature )
{
	jmethodID mid = app->GetVrJni()->GetMethodID( cls, name, signature );
	if ( !mid )
	{
    	FAIL( "Couldn't find %s methodID", name );
    }

	return mid;
}

void Native::OneTimeInit( App *app, jclass mainActivityClass )
{
	LOG( "Native::OneTimeInit" );

	const double start = ovr_GetTimeInSeconds();

	getExternalCacheDirectoryMethodId 	= GetMethodID( app, mainActivityClass, "getExternalCacheDirectory", "()Ljava/lang/String;" );
	createVideoThumbnailMethodId 		= GetMethodID( app, mainActivityClass, "createVideoThumbnail", "(Ljava/lang/String;Ljava/lang/String;II)Z" );
	checkForMovieResumeId 				= GetMethodID( app, mainActivityClass, "checkForMovieResume", "(Ljava/lang/String;)Z" );
	isPlayingMethodId 					= GetMethodID( app, mainActivityClass, "isPlaying", "()Z" );
	isPlaybackFinishedMethodId			= GetMethodID( app, mainActivityClass, "isPlaybackFinished", "()Z" );
	hadPlaybackErrorMethodId			= GetMethodID( app, mainActivityClass, "hadPlaybackError", "()Z" );
	getPositionMethodId 				= GetMethodID( app, mainActivityClass, "getPosition", "()I" );
	getDurationMethodId 				= GetMethodID( app, mainActivityClass, "getDuration", "()I" );
	setPositionMethodId 				= GetMethodID( app, mainActivityClass, "setPosition", "(I)V" );
	seekDeltaMethodId					= GetMethodID( app, mainActivityClass, "seekDelta", "(I)V" );
	startMovieMethodId 					= GetMethodID( app, mainActivityClass, "startMovie", "(Ljava/lang/String;ZZZ)V" );
	pauseMovieMethodId 					= GetMethodID( app, mainActivityClass, "pauseMovie", "()V" );
	resumeMovieMethodId 				= GetMethodID( app, mainActivityClass, "resumeMovie", "()V" );
	stopMovieMethodId 					= GetMethodID( app, mainActivityClass, "stopMovie", "()V" );
	togglePlayingMethodId 				= GetMethodID( app, mainActivityClass, "togglePlaying", "()Z" );

	LOG( "Native::OneTimeInit: %3.1f seconds", ovr_GetTimeInSeconds() - start );
}

void Native::OneTimeShutdown()
{
	LOG( "Native::OneTimeShutdown" );
}

String Native::GetExternalCacheDirectory( App *app )
{
	jstring externalCacheDirectoryString = (jstring)app->GetVrJni()->CallObjectMethod( app->GetJavaObject(), getExternalCacheDirectoryMethodId );

	const char *externalCacheDirectoryStringUTFChars = app->GetVrJni()->GetStringUTFChars( externalCacheDirectoryString, NULL );
	String externalCacheDirectory = externalCacheDirectoryStringUTFChars;

	app->GetVrJni()->ReleaseStringUTFChars( externalCacheDirectoryString, externalCacheDirectoryStringUTFChars );
	app->GetVrJni()->DeleteLocalRef( externalCacheDirectoryString );

	return externalCacheDirectory;
}

bool Native::CreateVideoThumbnail( App *app, const char *videoFilePath, const char *outputFilePath, const int width, const int height )
{
	LOG( "CreateVideoThumbnail( %s, %s )", videoFilePath, outputFilePath );

	jstring jstrVideoFilePath = app->GetVrJni()->NewStringUTF( videoFilePath );
	jstring jstrOutputFilePath = app->GetVrJni()->NewStringUTF( outputFilePath );

	jboolean result = app->GetVrJni()->CallBooleanMethod( app->GetJavaObject(), createVideoThumbnailMethodId, jstrVideoFilePath, jstrOutputFilePath, width, height );

	app->GetVrJni()->DeleteLocalRef( jstrVideoFilePath );
	app->GetVrJni()->DeleteLocalRef( jstrOutputFilePath );

	LOG( "CreateVideoThumbnail( %s, %s )", videoFilePath, outputFilePath );

	return result;
}

bool Native::CheckForMovieResume( App *app, const char * movieName )
{
	LOG( "CheckForMovieResume( %s )", movieName );

	jstring jstrMovieName = app->GetVrJni()->NewStringUTF( movieName );

	jboolean result = app->GetVrJni()->CallBooleanMethod( app->GetJavaObject(), checkForMovieResumeId, jstrMovieName );

	app->GetVrJni()->DeleteLocalRef( jstrMovieName );

	return result;
}

bool Native::IsPlaying( App *app )
{
	LOG( "IsPlaying()" );
	return app->GetVrJni()->CallBooleanMethod( app->GetJavaObject(), isPlayingMethodId );
}

bool Native::IsPlaybackFinished( App *app )
{
	jboolean result = app->GetVrJni()->CallBooleanMethod( app->GetJavaObject(), isPlaybackFinishedMethodId );
	return ( result != 0 );
}

bool Native::HadPlaybackError( App *app )
{
	jboolean result = app->GetVrJni()->CallBooleanMethod( app->GetJavaObject(), hadPlaybackErrorMethodId );
	return ( result != 0 );
}

int Native::GetPosition( App *app )
{
	LOG( "GetPosition()" );
	return app->GetVrJni()->CallIntMethod( app->GetJavaObject(), getPositionMethodId );
}

int Native::GetDuration( App *app )
{
	LOG( "GetDuration()" );
	return app->GetVrJni()->CallIntMethod( app->GetJavaObject(), getDurationMethodId );
}

void Native::SetPosition( App *app, int positionMS )
{
	LOG( "SetPosition()" );
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), setPositionMethodId, positionMS );
}

void Native::SeekDelta( App *app, int deltaMS )
{
	LOG( "SeekDelta()" );
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), seekDeltaMethodId, deltaMS );
}

void Native::StartMovie( App *app, const char * movieName, bool resumePlayback, bool isEncrypted, bool loop )
{
	LOG( "StartMovie( %s )", movieName );

	jstring jstrMovieName = app->GetVrJni()->NewStringUTF( movieName );

	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), startMovieMethodId, jstrMovieName, resumePlayback, isEncrypted, loop );

	app->GetVrJni()->DeleteLocalRef( jstrMovieName );
}

void Native::PauseMovie( App *app )
{
	LOG( "PauseMovie()" );
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), pauseMovieMethodId );
}

void Native::ResumeMovie( App *app )
{
	LOG( "ResumeMovie()" );
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), resumeMovieMethodId );
}

void Native::StopMovie( App *app )
{
	LOG( "StopMovie()" );
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), stopMovieMethodId );
}

bool Native::TogglePlaying( App *app )
{
	LOG( "TogglePlaying()" );
	return app->GetVrJni()->CallBooleanMethod( app->GetJavaObject(), togglePlayingMethodId );
}

} // namespace VRMatterStreamTheater
