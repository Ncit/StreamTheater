/************************************************************************************

Filename    :   CinemaStrings.h
Content     :	Text strings used by app.  Located in one place to make translation easier.
Created     :	9/30/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( CinemaStrings_h )
#define CinemaStrings_h

#include "Kernel/OVR_String.h"

using namespace OVR;

namespace VRMatterStreamTheater {

class CinemaApp;

class CinemaStrings {
public:
	static void		OneTimeInit( CinemaApp &cinema );

	static String	LoadingMenu_Title;

	static String	Category_Trailers;
	static String	Category_MyVideos;
	static String	Category_LimeLight;
	static String	Category_RemoteDesktop;
	static String	Category_VNC;

	static String	MovieSelection_Resume;
	static String	MovieSelection_Next;

	static String	ResumeMenu_Title;
	static String	ResumeMenu_Resume;
	static String	ResumeMenu_Restart;

	static String	TheaterSelection_Title;

	static String	Error_NoVideosOnPhone;
	static String	Error_NoVideosInLimeLight;
	static String	Error_UnableToPlayMovie;

	static String	MoviePlayer_Reorient;

	static String	ButtonText_ButtonSaveApp;
	static String	ButtonText_ButtonSaveDefault;
	static String	ButtonText_ButtonResetSettings;
	static String	ButtonText_ButtonSaveSettings1;
	static String	ButtonText_ButtonSaveSettings2;
	static String	ButtonText_ButtonSaveSettings3;
	static String	ButtonText_ButtonLoadSettings1;
	static String	ButtonText_ButtonLoadSettings2;
	static String	ButtonText_ButtonLoadSettings3;
	static String	ButtonText_ButtonMapKeyboard;
	static String	ButtonText_ButtonSpeed;
	static String	ButtonText_ButtonComfortMode;
	static String	ButtonText_Button720p30;
	static String	ButtonText_ButtonHostAudio;
	static String	ButtonText_Button1080p30;
	static String	ButtonText_Button720p60;
	static String	ButtonText_ButtonDistance;
	static String	ButtonText_ButtonSize;
	static String	ButtonText_ButtonSBS;
	static String	ButtonText_ButtonChangeSeat;
	static String	ButtonText_ButtonGaze;
	static String	ButtonText_ButtonTrackpad;
	static String	ButtonText_LabelTrackpadScale;
	static String	ButtonText_Button1080p60;
	static String	ButtonText_ButtonOff;
	static String	ButtonText_LabelGazeScale;

	static String	Error_UnknownHost;
	static String	Error_AddPCFailed;
};

} // namespace VRMatterStreamTheater

#endif // CinemaStrings_h
