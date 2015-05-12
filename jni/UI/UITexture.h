/************************************************************************************

Filename    :   UITexture.h
Content     :
Created     :	1/8/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( UITexture_h )
#define UITexture_h

#include "Android/GLUtils.h"

namespace VRMatterStreamTheater {

class UITexture
{
public:
										UITexture();
										~UITexture();

	void 								LoadTextureFromApplicationPackage( const char *assetPath );

	int 								Width;
	int									Height;
	GLuint 								Texture;
};

} // namespace VRMatterStreamTheater

#endif // UITexture_h
