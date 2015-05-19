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

#include "Android/JniUtils.h"

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
	LOG( "nativeSetVideoSize: width=%i height=%i rotation=%i duration=%i", width, height, rotation, duration );

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

void Java_com_vrmatter_streamtheater_MainActivity_nativeDisplayMessage( JNIEnv *jni, jclass clazz, jlong interfacePtr, jstring text, int time, bool isError ) {}
void Java_com_vrmatter_streamtheater_MainActivity_nativeAddPc( JNIEnv *jni, jclass clazz, jlong interfacePtr, jstring name, jstring uuid, int psi, jstring binding)
{
	CinemaApp *cinema = ( CinemaApp * )( ( (App *)interfacePtr )->GetAppInterface() );
	JavaUTFChars utfName( jni, name );
	JavaUTFChars utfUUID( jni, uuid );
	JavaUTFChars utfBind( jni, binding );

	Native::PairState ps = (Native::PairState) psi;
	cinema->PcMgr.AddPc(utfName.ToStr(), utfUUID.ToStr(), ps, utfBind.ToStr());
}
void Java_com_vrmatter_streamtheater_MainActivity_nativeRemovePc( JNIEnv *jni, jclass clazz, jlong interfacePtr, jstring name)
{
	CinemaApp *cinema = ( CinemaApp * )( ( (App *)interfacePtr )->GetAppInterface() );
	JavaUTFChars utfName( jni, name );
	cinema->PcMgr.RemovePc(utfName.ToStr());
}
void Java_com_vrmatter_streamtheater_MainActivity_nativeAddApp( JNIEnv *jni, jclass clazz, jlong interfacePtr, jstring name, jstring posterfilename, int id)
{
	CinemaApp *cinema = ( CinemaApp * )( ( (App *)interfacePtr )->GetAppInterface() );
	JavaUTFChars utfName( jni, name );
	JavaUTFChars utfPosterFileName( jni, posterfilename );
	cinema->AppMgr.AddApp(utfName.ToStr(), utfPosterFileName.ToStr(), id);
}
void Java_com_vrmatter_streamtheater_MainActivity_nativeRemoveApp( JNIEnv *jni, jclass clazz, jlong interfacePtr, int id)
{
	CinemaApp *cinema = ( CinemaApp * )( ( (App *)interfacePtr )->GetAppInterface() );
	cinema->AppMgr.RemoveApp( id);
}


void Java_com_vrmatter_streamtheater_MainActivity_nativeShowPair( JNIEnv *jni, jclass clazz, jlong interfacePtr, jstring message )
{
	CinemaApp *cinema = ( CinemaApp * )( ( (App *)interfacePtr )->GetAppInterface() );
	JavaUTFChars utfMessage( jni, message );
	cinema->ShowPair(utfMessage.ToStr());
}
void Java_com_vrmatter_streamtheater_MainActivity_nativePairSuccess( JNIEnv *jni, jclass clazz, jlong interfacePtr )
{
	CinemaApp *cinema = ( CinemaApp * )( ( (App *)interfacePtr )->GetAppInterface() );
	cinema->PairSuccess();
}
void Java_com_vrmatter_streamtheater_MainActivity_nativeShowError( JNIEnv *jni, jclass clazz, jlong interfacePtr, jstring message )
{
	CinemaApp *cinema = ( CinemaApp * )( ( (App *)interfacePtr )->GetAppInterface() );
	JavaUTFChars utfMessage( jni, message );
	cinema->ShowError(utfMessage.ToStr());
}
void Java_com_vrmatter_streamtheater_MainActivity_nativeClearError( JNIEnv *jni, jclass clazz, jlong interfacePtr )
{
	CinemaApp *cinema = ( CinemaApp * )( ( (App *)interfacePtr )->GetAppInterface() );
	cinema->ClearError();
}

}	// extern "C"

//==============================================================

// Java method ids
static jmethodID 	getExternalCacheDirectoryMethodId = NULL;
static jmethodID	createVideoThumbnailMethodId = NULL;
static jmethodID 	isPlayingMethodId = NULL;
static jmethodID 	isPlaybackFinishedMethodId = NULL;
static jmethodID 	hadPlaybackErrorMethodId = NULL;
static jmethodID 	startMovieMethodId = NULL;
static jmethodID 	stopMovieMethodId = NULL;
static jmethodID 	initPcSelectorMethodId = NULL;
static jmethodID 	pairPcMethodId = NULL;
static jmethodID 	getPcPairStateMethodId = NULL;
static jmethodID 	getPcStateMethodId = NULL;
static jmethodID 	getPcReachabilityMethodId = NULL;
static jmethodID 	initAppSelectorMethodId = NULL;
static jmethodID	mouseMoveMethodId = NULL;
static jmethodID	mouseClickMethodId = NULL;
static jmethodID	mouseScrollMethodId = NULL;
static jmethodID	controllerStateMethodId = NULL;

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
	createVideoThumbnailMethodId 		= GetMethodID( app, mainActivityClass, "createVideoThumbnail", "(Ljava/lang/String;ILjava/lang/String;II)Z" );
	isPlayingMethodId 					= GetMethodID( app, mainActivityClass, "isPlaying", "()Z" );
	isPlaybackFinishedMethodId			= GetMethodID( app, mainActivityClass, "isPlaybackFinished", "()Z" );
	hadPlaybackErrorMethodId			= GetMethodID( app, mainActivityClass, "hadPlaybackError", "()Z" );
	startMovieMethodId 					= GetMethodID( app, mainActivityClass, "startMovie", "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)V" );
	stopMovieMethodId 					= GetMethodID( app, mainActivityClass, "stopMovie", "()V" );
	initPcSelectorMethodId 				= GetMethodID( app, mainActivityClass, "initPcSelector", "()V" );
	pairPcMethodId 						= GetMethodID( app, mainActivityClass, "pairPc", "(Ljava/lang/String;)V" );
	getPcPairStateMethodId 				= GetMethodID( app, mainActivityClass, "getPcPairState", "(Ljava/lang/String;)I" );
	getPcStateMethodId 					= GetMethodID( app, mainActivityClass, "getPcState", "(Ljava/lang/String;)I" );
	getPcReachabilityMethodId 			= GetMethodID( app, mainActivityClass, "getPcReachability", "(Ljava/lang/String;)I" );
	initAppSelectorMethodId 			= GetMethodID( app, mainActivityClass, "initAppSelector", "(Ljava/lang/String;)V" );
	mouseMoveMethodId 					= GetMethodID( app, mainActivityClass, "mouseMove", "(II)V" );
	mouseClickMethodId 					= GetMethodID( app, mainActivityClass, "mouseClick", "(IZ)V" );
	mouseScrollMethodId 				= GetMethodID( app, mainActivityClass, "mouseScroll", "(B)V" );
	controllerStateMethodId				= GetMethodID( app, mainActivityClass, "controllerState", "(FFFFFFI)V" );
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

bool Native::CreateVideoThumbnail( App *app, const char *uuid, int appId, const char *outputFilePath, const int width, const int height )
{
	LOG( "CreateVideoThumbnail( %s, %i, %s )", uuid, appId, outputFilePath );

	jstring jstrUUID = app->GetVrJni()->NewStringUTF( uuid );
	jstring jstrOutputFilePath = app->GetVrJni()->NewStringUTF( outputFilePath );

	jboolean result = app->GetVrJni()->CallBooleanMethod( app->GetJavaObject(), createVideoThumbnailMethodId, jstrUUID, appId, jstrOutputFilePath, width, height );
	LOG( "Done creating thumbnail!");
	app->GetVrJni()->DeleteLocalRef( jstrUUID );
	app->GetVrJni()->DeleteLocalRef( jstrOutputFilePath );

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

void Native::StartMovie( App *app, const char * uuid, const char * appName, int id, const char * binder )
{
	LOG( "StartMovie( %s )", appName );

	jstring jstrUUID = app->GetVrJni()->NewStringUTF( uuid );
	jstring jstrAppName = app->GetVrJni()->NewStringUTF( appName );
	jstring jstrBinder = app->GetVrJni()->NewStringUTF( binder );

	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), startMovieMethodId, jstrUUID, jstrAppName, id, jstrBinder );

	app->GetVrJni()->DeleteLocalRef( jstrUUID );
	app->GetVrJni()->DeleteLocalRef( jstrAppName );
	app->GetVrJni()->DeleteLocalRef( jstrBinder );
}

void Native::StopMovie( App *app )
{
	LOG( "StopMovie()" );
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), stopMovieMethodId );
}

void Native::InitPcSelector( App *app )
{
	LOG( "InitPcSelector()" );
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), initPcSelectorMethodId );
}

void Native::InitAppSelector( App *app, const char* uuid)
{
	LOG( "InitAppSelector()" );

	jstring jstrUUID = app->GetVrJni()->NewStringUTF( uuid );
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), initAppSelectorMethodId, jstrUUID );
}

Native::PairState Native::GetPairState( App *app, const char* uuid)
{
	LOG( "GetPairState()" );

	jstring jstrUUID = app->GetVrJni()->NewStringUTF( uuid );
	return (PairState)app->GetVrJni()->CallIntMethod( app->GetJavaObject(), getPcPairStateMethodId, jstrUUID );
}

void Native::Pair( App *app, const char* uuid)
{
	LOG( "Pair()" );

	jstring jstrUUID = app->GetVrJni()->NewStringUTF( uuid );
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), pairPcMethodId, jstrUUID );
}

void Native::MouseMove(App *app, int deltaX, int deltaY)
{
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), mouseMoveMethodId, deltaX, deltaY );
}

void Native::MouseClick(App *app, int buttonId, bool down)
{
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), mouseClickMethodId, buttonId, down );
}

void Native::MouseScroll(App *app, char amount)
{
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), mouseScrollMethodId, amount );
}

void Native::ControllerState(App *app, float stick1x, float stick1y, float stick2x, float stick2y, float leftTrigger, float rightTrigger, int buttons )
{
	app->GetVrJni()->CallVoidMethod( app->GetJavaObject(), controllerStateMethodId, stick1x, stick1y, stick2x, stick2y, leftTrigger, rightTrigger, buttons );
}


} // namespace VRMatterStreamTheater
