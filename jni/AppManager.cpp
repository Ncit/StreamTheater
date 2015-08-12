/************************************************************************************

Filename    :   AppManager.cpp
Content     :
Created     :	9/10/2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include "Kernel/OVR_String_Utils.h"
#include "Kernel/OVR_JSON.h"

#include "AppManager.h"
#include "CinemaApp.h"
#include "PackageFiles.h"
#include "Native.h"


namespace VRMatterStreamTheater {

const int AppManager::PosterWidth = 228;
const int AppManager::PosterHeight = 344;

//=======================================================================================

AppManager::AppManager( CinemaApp &cinema ) :
	PcManager( cinema ),
    Apps(),
    updated( false ),
    Cinema( cinema ),
    DefaultPoster(0)
{
}

AppManager::~AppManager()
{
}

void AppManager::OneTimeInit( const char * launchIntent )
{
	LOG( "AppManager::OneTimeInit" );
	const double start = vrapi_GetTimeInSeconds();

	int width, height;
	DefaultPoster = LoadTextureFromApplicationPackage(
			"assets/default_poster.png",
			TextureFlags_t(TEXTUREFLAG_NO_DEFAULT), width, height);
	LOG(" Default gluint: %i", DefaultPoster);

	BuildTextureMipmaps( DefaultPoster );
	MakeTextureTrilinear( DefaultPoster );
	MakeTextureClamped( DefaultPoster );

	LoadApps();

	LOG( "AppManager::OneTimeInit: %i movies loaded, %3.1f seconds", Apps.GetSizeI(), vrapi_GetTimeInSeconds() - start );
}

void AppManager::OneTimeShutdown()
{
	LOG( "AppManager::OneTimeShutdown" );
}

void AppManager::LoadApps()
{
	LOG( "LoadApps" );

	const double start = vrapi_GetTimeInSeconds();

	Array<String> appNames; // TODO: Get app list from JNI AppSelector
	LOG( "%i movies scanned, %3.1f seconds", appNames.GetSizeI(), vrapi_GetTimeInSeconds() - start );

	for( UPInt i = 0; i < appNames.GetSize(); i++ )
	{
		AppDef *app = new AppDef();
		Apps.PushBack( app );

		app->Name = appNames[ i ];

		ReadMetaData( app );
		LoadPoster( app );
	}

	LOG( "%i movies panels loaded, %3.1f seconds", Apps.GetSizeI(), vrapi_GetTimeInSeconds() - start );
}

void AppManager::AddApp(const String &name, const String &posterFileName, int id, bool isRunning)
{
	LOG( "App %s with id %i added!", name.ToCStr(), id);
	AppDef *anApp = NULL;
	bool isNew = false;

	for(UPInt i=0; i < Apps.GetSize(); i++)
	{
		if(Apps[i]->Name.CompareNoCase(name) == 0)
			anApp = Apps[i];
	}
	if(anApp == NULL)
	{
		anApp = new AppDef();
		Apps.PushBack( anApp );
		isNew = true;
	}

	anApp->Name = name;
	anApp->Id = id;
	anApp->PosterFileName = posterFileName;
	anApp->isRunning = isRunning;

	if( isNew ) ReadMetaData( anApp );
	if( anApp->Poster == 0) LoadPoster(anApp);

	updated = true;
}

void AppManager::RemoveApp( int id)
{
	for(UPInt i=0; i < Apps.GetSize(); i++)
	{
		if(Apps[i]->Id == id )
		{
			Apps.RemoveAt(i);
			i--;
		}
	}
}

void AppManager::ReadMetaData( PcDef *anApp )
{
	String filename = anApp->Name;
	filename.StripExtension();
	filename.AppendString(".app.txt" );

	const char* error = NULL;

	if ( !Cinema.FileExists( filename.ToCStr() ) )
	{
		return;
	}

	if ( JSON* metadata = JSON::Load( filename.ToCStr(), &error ) )
	{

		metadata->Release();

		LOG( "Loaded metadata: %s", filename.ToCStr() );
	}
	else
	{
		LOG( "Error loading metadata for %s: %s", filename.ToCStr(), ( error == NULL ) ? "NULL" : error );
	}
}

void AppManager::LoadPoster( PcDef *anApp )
{
	String posterFilename = anApp->PosterFileName;
	posterFilename.StripExtension();
	posterFilename.AppendString( ".png" );

	anApp->Poster = LoadTextureFromBuffer( posterFilename.ToCStr(), MemBufferFile( posterFilename.ToCStr() ),
			TextureFlags_t( TEXTUREFLAG_NO_DEFAULT ), anApp->PosterWidth, anApp->PosterHeight );
	LOG( "Poster loaded: %s %i %i %i", posterFilename.ToCStr(), anApp->Poster, anApp->PosterWidth, anApp->PosterHeight);
	if ( anApp->Poster == 0 )
	{
		// no thumbnail found, so create it.  if it's on an external sdcard, posterFilename will contain the new filename at this point and will load it from the cache
		if ( ( anApp->Poster == 0 )
///* JNI wonkyness made this unhappy */					&& Native::CreateVideoThumbnail( Cinema.app, Cinema.GetCurrentPc()->UUID.ToCStr(), anApp->Id, posterFilename.ToCStr(), PosterWidth, PosterHeight )
			)
		{
			anApp->Poster = LoadTextureFromBuffer( posterFilename.ToCStr(), MemBufferFile( posterFilename.ToCStr() ),
				TextureFlags_t( TEXTUREFLAG_NO_DEFAULT ), anApp->PosterWidth, anApp->PosterHeight );
		}
	
	}

	// if all else failed, then just use the default poster
	if ( anApp->Poster == 0 )
	{
		anApp->Poster = DefaultPoster;
	}
	else
	{
		BuildTextureMipmaps( anApp->Poster );
		MakeTextureTrilinear( anApp->Poster );
		MakeTextureClamped( anApp->Poster );
	}
}


Array<const PcDef *> AppManager::GetAppList( PcCategory category ) const
{
	Array<const PcDef *> result;

	for( UPInt i = 0; i < Apps.GetSize(); i++ )
	{
		LOG("App: %s Poster %i", Apps[i]->Name.ToCStr(), Apps[i]->Poster);
		if ( Apps[ i ]->Category == category && Apps[i]->Poster != 0)
		{
			result.PushBack( Apps[ i ] );
		}
	}

	return result;
}

} // namespace VRMatterStreamTheater
