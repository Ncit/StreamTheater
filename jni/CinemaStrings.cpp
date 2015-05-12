/************************************************************************************

Filename    :   CinemaStrings.cpp
Content     :	Text strings used by app.  Located in one place to make translation easier.
Created     :	9/30/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "CinemaStrings.h"
#include "VrLocale.h"
#include "CinemaApp.h"
#include "BitmapFont.h"

namespace VRMatterStreamTheater
{
String CinemaStrings::LoadingMenu_Title;

String CinemaStrings::Category_Trailers;
String CinemaStrings::Category_MyVideos;

String CinemaStrings::MovieSelection_Resume;
String CinemaStrings::MovieSelection_Next;

String CinemaStrings::ResumeMenu_Title;
String CinemaStrings::ResumeMenu_Resume;
String CinemaStrings::ResumeMenu_Restart;

String CinemaStrings::TheaterSelection_Title;

String CinemaStrings::Error_NoVideosOnPhone;
String CinemaStrings::Error_NoVideosInMyVideos;
String CinemaStrings::Error_UnableToPlayMovie;

String CinemaStrings::MoviePlayer_Reorient;

void CinemaStrings::OneTimeInit( CinemaApp &cinema )
{
	LOG( "CinemaStrings::OneTimeInit" );

	App *app = cinema.app;

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/LoadingMenu_Title", 		"@string/LoadingMenu_Title", 		LoadingMenu_Title );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Category_Trailers", 		"@string/Category_Trailers", 		Category_Trailers );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Category_MyVideos", 		"@string/Category_MyVideos", 		Category_MyVideos );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/MovieSelection_Resume",	"@string/MovieSelection_Resume",	MovieSelection_Resume );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/MovieSelection_Next", 		"@string/MovieSelection_Next", 		MovieSelection_Next );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ResumeMenu_Title", 		"@string/ResumeMenu_Title", 		ResumeMenu_Title );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ResumeMenu_Resume", 		"@string/ResumeMenu_Resume", 		ResumeMenu_Resume );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ResumeMenu_Restart", 		"@string/ResumeMenu_Restart", 		ResumeMenu_Restart );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/TheaterSelection_Title", 	"@string/TheaterSelection_Title", 	TheaterSelection_Title );

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Error_NoVideosOnPhone", 	"@string/Error_NoVideosOnPhone", 	Error_NoVideosOnPhone );

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Error_NoVideosInMyVideos", "@string/Error_NoVideosInMyVideos", Error_NoVideosInMyVideos );

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Error_UnableToPlayMovie", 	"@string/Error_UnableToPlayMovie",	Error_UnableToPlayMovie );

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/MoviePlayer_Reorient", 	"@string/MoviePlayer_Reorient", 	MoviePlayer_Reorient );
}

} // namespace VRMatterStreamTheater
