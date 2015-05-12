/************************************************************************************

Filename    :   UIMenu.h
Content     :
Created     :	1/5/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( UIMenu_h )
#define UIMenu_h

#include "VRMenu/VRMenu.h"

using namespace OVR;

namespace VRMatterStreamTheater {

class CinemaApp;

class UIMenu
{
public:
										UIMenu( CinemaApp &cinema );
										~UIMenu();

	VRMenuId_t 							AllocId();

	void 								Create( const char *menuName );

	void 								Open();
	void 								Close();

	bool								IsOpen() const { return MenuOpen; }

	VRMenu *							GetVRMenu() const { return Menu; }

    VRMenuFlags_t const &				GetFlags() const;
	void								SetFlags( VRMenuFlags_t	const & flags );

private:
    CinemaApp &							Cinema;
    String								MenuName;
	VRMenu *							Menu;

	bool								MenuOpen;

	VRMenuId_t							IdPool;
};

} // namespace VRMatterStreamTheater

#endif // UIMenu_h
