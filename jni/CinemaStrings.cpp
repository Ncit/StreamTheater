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
String CinemaStrings::Category_LimeLight;
String CinemaStrings::Category_RemoteDesktop;
String CinemaStrings::Category_VNC;

String CinemaStrings::MovieSelection_Resume;
String CinemaStrings::MovieSelection_Next;

String CinemaStrings::ResumeMenu_Title;
String CinemaStrings::ResumeMenu_Resume;
String CinemaStrings::ResumeMenu_Restart;

String CinemaStrings::TheaterSelection_Title;

String CinemaStrings::Error_NoVideosOnPhone;
String CinemaStrings::Error_NoVideosInLimeLight;
String CinemaStrings::Error_UnableToPlayMovie;

String CinemaStrings::MoviePlayer_Reorient;

String CinemaStrings::ButtonText_ButtonMapKeyboard;
String CinemaStrings::ButtonText_ButtonSpeed;
String CinemaStrings::ButtonText_ButtonComfortMode;
String CinemaStrings::ButtonText_Button720p30;
String CinemaStrings::ButtonText_ButtonHostAudio;
String CinemaStrings::ButtonText_Button1080p30;
String CinemaStrings::ButtonText_Button720p60;
String CinemaStrings::ButtonText_ButtonDistance;
String CinemaStrings::ButtonText_ButtonSize;
String CinemaStrings::ButtonText_ButtonSBS;
String CinemaStrings::ButtonText_ButtonChangeSeat;
String CinemaStrings::ButtonText_ButtonGaze;
String CinemaStrings::ButtonText_ButtonTrackpad;
String CinemaStrings::ButtonText_ButtonYSensitivity;
String CinemaStrings::ButtonText_Button1080p60;
String CinemaStrings::ButtonText_ButtonOff;
String CinemaStrings::ButtonText_ButtonXSensitivity;

void CinemaStrings::OneTimeInit( CinemaApp &cinema )
{
	LOG( "CinemaStrings::OneTimeInit" );

	App *app = cinema.app;

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/LoadingMenu_Title", 		"@string/LoadingMenu_Title", 		LoadingMenu_Title );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Category_Trailers", 		"@string/Category_Trailers", 		Category_Trailers );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Category_MyVideos", 		"@string/Category_MyVideos", 		Category_MyVideos );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Category_LimeLight", 		"@string/Category_LimeLight", 		Category_LimeLight );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Category_RemoteDesktop", 	"@string/Category_RemoteDesktop", 	Category_RemoteDesktop );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Category_VNC", 			"@string/Category_VNC", 			Category_VNC );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/MovieSelection_Resume",	"@string/MovieSelection_Resume",	MovieSelection_Resume );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/MovieSelection_Next", 		"@string/MovieSelection_Next", 		MovieSelection_Next );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ResumeMenu_Title", 		"@string/ResumeMenu_Title", 		ResumeMenu_Title );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ResumeMenu_Resume", 		"@string/ResumeMenu_Resume", 		ResumeMenu_Resume );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ResumeMenu_Restart", 		"@string/ResumeMenu_Restart", 		ResumeMenu_Restart );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/TheaterSelection_Title", 	"@string/TheaterSelection_Title", 	TheaterSelection_Title );

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Error_NoVideosOnPhone", 	"@string/Error_NoVideosOnPhone", 	Error_NoVideosOnPhone );

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Error_NoVideosInLimeLight", "@string/Error_NoVideosInLimeLight", Error_NoVideosInLimeLight );

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/Error_UnableToPlayMovie", 	"@string/Error_UnableToPlayMovie",	Error_UnableToPlayMovie );

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/MoviePlayer_Reorient", 	"@string/MoviePlayer_Reorient", 	MoviePlayer_Reorient );

	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonMapKeyboard", "@string/ButtonText_ButtonMapKeyboard", 	ButtonText_ButtonMapKeyboard );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonSpeed", 		"@string/ButtonText_ButtonSpeed", 			ButtonText_ButtonSpeed );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonComfortMode", "@string/ButtonText_ButtonComfortMode", 	ButtonText_ButtonComfortMode );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_Button720p30", 		"@string/ButtonText_Button720p30", 			ButtonText_Button720p30 );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonHostAudio", 	"@string/ButtonText_ButtonHostAudio", 		ButtonText_ButtonHostAudio );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_Button1080p30", 	"@string/ButtonText_Button1080p30", 		ButtonText_Button1080p30 );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_Button720p60", 		"@string/ButtonText_Button720p60", 			ButtonText_Button720p60 );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonDistance", 	"@string/ButtonText_ButtonDistance", 		ButtonText_ButtonDistance );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonSize", 		"@string/ButtonText_ButtonSize", 			ButtonText_ButtonSize );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonSBS", 		"@string/ButtonText_ButtonSBS", 			ButtonText_ButtonSBS );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonChangeSeat", 	"@string/ButtonText_ButtonChangeSeat", 		ButtonText_ButtonChangeSeat );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonGaze",	 	"@string/ButtonText_ButtonGaze", 			ButtonText_ButtonGaze );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonTrackpad", 	"@string/ButtonText_ButtonTrackpad", 		ButtonText_ButtonTrackpad );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonYSensitivity","@string/ButtonText_ButtonYSensitivity", 	ButtonText_ButtonYSensitivity );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_Button1080p60", 	"@string/ButtonText_Button1080p60", 		ButtonText_Button1080p60 );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonOff", 		"@string/ButtonText_ButtonOff", 			ButtonText_ButtonOff );
	VrLocale::GetString( app->GetVrJni(), app->GetJavaObject(), "@string/ButtonText_ButtonXSensitivity","@string/ButtonText_ButtonXSensitivity", 	ButtonText_ButtonXSensitivity );

}

} // namespace VRMatterStreamTheater
