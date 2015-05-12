/************************************************************************************

Filename    :   AppManager.h
Content     :
Created     :	9/10/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( AppManager_h )
#define AppManager_h

#include "LibOVR/Src/Kernel/OVR_String.h"
#include "LibOVR/Src/Kernel/OVR_Array.h"
#include "GlTexture.h"
#include "PcManager.h"

namespace VRMatterStreamTheater {

class CinemaApp;

using namespace OVR;

class AppDef : public PcDef
{
public:
/*	String			Name;
	int				Id;

	GLuint			Poster;
	int				PosterWidth;
	int				PosterHeight;

	PcCategory		Category;
*/
	AppDef() : PcDef() {}
};

class AppManager : public PcManager
{
public:
							AppManager( CinemaApp &cinema );
	virtual					~AppManager();

	virtual void			OneTimeInit( const char * launchIntent );
	virtual void			OneTimeShutdown();
	void					LoadApps();
	void					AddApp(const String &name, const String &posterFileName, int id);
	void					RemoveApp( int id);

	Array<const PcDef *>	GetAppList( PcCategory category ) const;

public:
    Array<AppDef *> 		Apps;

    static const int 		PosterWidth;
    static const int 		PosterHeight;

    bool					updated;


private:
	CinemaApp &				Cinema;

    GLuint					DefaultPoster;

    virtual void 			ReadMetaData( PcDef *app );
    virtual void 			LoadPoster( PcDef *app );

};

} // namespace VRMatterStreamTheater

#endif // AppManager_h
