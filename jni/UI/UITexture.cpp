/************************************************************************************

Filename    :   UITexture.cpp
Content     :
Created     :	1/8/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "UI/UITexture.h"
#include "CinemaApp.h"
#include "PackageFiles.h"

namespace OculusCinema {

UITexture::UITexture() :
	Width( 0 ),
	Height( 0 ),
	Texture( 0 )

{
}

UITexture::~UITexture()
{
}

void UITexture::LoadTextureFromApplicationPackage( const char *assetPath )
{
	Texture = OVR::LoadTextureFromApplicationPackage( assetPath, TextureFlags_t( TEXTUREFLAG_NO_DEFAULT ), Width, Height );
}

} // namespace OculusCinema
