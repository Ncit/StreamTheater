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

namespace OculusCinema {

class CinemaApp;

class CinemaStrings {
public:
	static void		OneTimeInit( CinemaApp &cinema );

	static String	LoadingMenu_Title;

	static String	Category_Trailers;
	static String	Category_MyVideos;

	static String	MovieSelection_Resume;
	static String	MovieSelection_Next;

	static String	ResumeMenu_Title;
	static String	ResumeMenu_Resume;
	static String	ResumeMenu_Restart;

	static String	TheaterSelection_Title;

	static String	Error_NoVideosOnPhone;
	static String	Error_NoVideosInMyVideos;
	static String	Error_UnableToPlayMovie;

	static String	MoviePlayer_Reorient;
};

} // namespace OculusCinema

#endif // CinemaStrings_h
