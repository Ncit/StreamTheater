/************************************************************************************

Filename    :   UIImage.h
Content     :
Created     :	1/5/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( UIImage_h )
#define UIImage_h

#include "VRMenu/VRMenu.h"
#include "UI/UIWidget.h"

using namespace OVR;

namespace VRMatterStreamTheater {

class CinemaApp;

class UIImage : public UIWidget
{
public:
										UIImage( CinemaApp &cinema );
										~UIImage();

	void 								AddToMenu( OvrGuiSys & guiSys, UIMenu *menu, UIWidget *parent = NULL );
	void 								AddToMenuFlags( OvrGuiSys & guiSys, UIMenu *menu, UIWidget *parent, VRMenuObjectFlags_t const flags );
};

} // namespace VRMatterStreamTheater

#endif // UIImage_h
