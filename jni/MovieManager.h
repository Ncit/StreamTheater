/************************************************************************************

Filename    :   MovieManager.h
Content     :
Created     :	9/10/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( MovieManager_h )
#define MovieManager_h

#include "LibOVR/Src/Kernel/OVR_String.h"
#include "LibOVR/Src/Kernel/OVR_Array.h"
#include "GlTexture.h"

namespace OculusCinema {

class CinemaApp;

using namespace OVR;

enum MovieFormat
{
	VT_UNKNOWN,
	VT_2D,
	VT_LEFT_RIGHT_3D,			// Left & right are scaled horizontally by 50%.
	VT_LEFT_RIGHT_3D_FULL,		// Left & right are unscaled.
	VT_TOP_BOTTOM_3D,			// Top & bottom are scaled vertically by 50%.
	VT_TOP_BOTTOM_3D_FULL,		// Top & bottom are unscaled.
};

enum MovieCategory {
	CATEGORY_MYVIDEOS,
	CATEGORY_TRAILERS
};

class MovieDef
{
public:
	String			Filename;

	String			Title;

	bool			Is3D;
	MovieFormat 	Format;

	GLuint			Poster;
	int				PosterWidth;
	int				PosterHeight;

	String			Theater;
	MovieCategory	Category;

	bool            IsEncrypted;
	bool			AllowTheaterSelection;


	MovieDef() : Filename(), Title(), Is3D( false ), Format( VT_2D ), Poster( 0 ), PosterWidth( 0 ), PosterHeight( 0 ),
			Theater(), Category( CATEGORY_MYVIDEOS ), IsEncrypted( false ), AllowTheaterSelection( false ) {}
};

class MovieManager
{
public:
							MovieManager( CinemaApp &cinema );
							~MovieManager();

	void					OneTimeInit( const char * launchIntent );
	void					OneTimeShutdown();

	Array<const MovieDef *>	GetMovieList( MovieCategory category ) const;

	static const String 	GetMovieTitleFromFilename( const char *filepath );

public:
    Array<MovieDef *> 		Movies;

    static const int 		PosterWidth;
    static const int 		PosterHeight;

    static const char *		SupportedFormats[];

private:
	CinemaApp &				Cinema;

	void					LoadMovies();
	MovieFormat				FormatFromString( const String &formatString ) const;
	MovieCategory 			CategoryFromString( const String &categoryString ) const;
	void 					ReadMetaData( MovieDef *movie );
	void 					LoadPoster( MovieDef *movie );
	void 					MoviesInDirectory( Array<String> &movies, const char * dirName ) const;
	Array<String> 			ScanMovieDirectories() const;
	bool					IsSupportedMovieFormat( const String &extension ) const;
};

} // namespace OculusCinema

#endif // MovieManager_h
