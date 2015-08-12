/************************************************************************************

Filename    :   PcManager.h
Content     :
Created     :	9/10/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( PcManager_h )
#define PcManager_h

#include "Kernel/OVR_String.h"
#include "Kernel/OVR_Array.h"
#include "GlTexture.h"
#include "Native.h"

namespace VRMatterStreamTheater {

class CinemaApp;

using namespace OVR;

enum PcCategory {
	CATEGORY_LIMELIGHT,
	CATEGORY_REMOTEDESKTOP,
	CATEGORY_VNC
};

class PcDef
{
public:
	String			Name;
	String			PosterFileName;
	String			UUID;
	String			Binding;
	int				Id;
	bool			isRunning;
	bool			isRemote;

	GLuint			Poster;
	int				PosterWidth;
	int				PosterHeight;

	PcCategory		Category;

	PcDef() : Name(), PosterFileName(), UUID(), Binding(), Poster( 0 ), PosterWidth( 0 ), PosterHeight( 0 ),
			Category( CATEGORY_LIMELIGHT ) {}
};

class PcManager
{
public:
							PcManager( CinemaApp &cinema );
	virtual					~PcManager();

	virtual void			OneTimeInit( const char * launchIntent );
	virtual void			OneTimeShutdown();

	void					AddPc(const String &name, const String& uuid, Native::PairState pairState, Native::Reachability reachability, const String &binding, const bool isRunning);
	void					RemovePc(const String &name);
	void					LoadPcs();
	Array<const PcDef *>	GetPcList( PcCategory category ) const;

public:
    Array<PcDef *>			Movies;

    static const int 		PosterWidth;
    static const int 		PosterHeight;

    bool					updated;

private:
	CinemaApp &				Cinema;

public:
	GLuint					PcPoster;
private:
	GLuint					PcPosterPaired;
    GLuint					PcPosterUnpaired;
    GLuint					PcPosterUnknown;
    GLuint					PcPosterWTF;


	PcCategory 				CategoryFromString( const String &categoryString ) const;
	virtual void 			ReadMetaData( PcDef *aPc );
};

} // namespace VRMatterStreamTheater

#endif // PcManager_h
