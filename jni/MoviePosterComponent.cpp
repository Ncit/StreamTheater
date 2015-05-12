/************************************************************************************

Filename    :   MoviePosterComponent.cpp
Content     :   Menu component for the movie selection menu.
Created     :   August 13, 2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "MoviePosterComponent.h"
#include "UI/UIContainer.h"
#include "UI/UIImage.h"
#include "UI/UILabel.h"

namespace VRMatterStreamTheater {

bool MoviePosterComponent::ShowShadows = true;

//==============================
//  MoviePosterComponent::
MoviePosterComponent::MoviePosterComponent() :
	CarouselItemComponent( VRMenuEventFlags_t() ),
	CurrentItem( NULL ),
    Poster( NULL ),
	PosterImage( NULL ),
    Is3DIcon( NULL ),
    Shadow( NULL )
{
}

//==============================
//  MoviePosterComponent::OnEvent_Impl
eMsgStatus MoviePosterComponent::OnEvent_Impl( App * app, VrFrame const & vrFrame, OvrVRMenuMgr & menuMgr,
        VRMenuObject * self, VRMenuEvent const & event )
{
	return MSG_STATUS_ALIVE;
}

//==============================
//  MoviePosterComponent::SetMenuObjects
void MoviePosterComponent::SetMenuObjects( const int width, const int height, UIContainer * poster, UIImage * posterImage, UIImage * is3DIcon, UIImage * shadow )
{
	Width = width;
	Height = height;
	Poster = poster;
	PosterImage = posterImage;
    Is3DIcon = is3DIcon;
    Shadow = shadow;

    CurrentItem = NULL;
    Is3DIcon->SetVisible( false );
	Shadow->SetVisible( false );
	PosterImage->SetVisible( false );
}

//==============================
//  MoviePosterComponent::SetItem
void MoviePosterComponent::SetItem( VRMenuObject * self, const CarouselItem * item, const PanelPose &pose )
{
	Poster->SetLocalPose( pose.Orientation, pose.Position );
	PosterImage->SetColor( pose.Color );
	Is3DIcon->SetColor( pose.Color );
	Shadow->SetColor( pose.Color );

	if ( item != CurrentItem )
	{
		if ( item != NULL )
		{
			PosterImage->SetImage( 0, SURFACE_TEXTURE_DIFFUSE, item->texture, Width, Height );

			Is3DIcon->SetVisible( ( item->userFlags & 1 ) != 0 );
			Shadow->SetVisible( ShowShadows );
			PosterImage->SetVisible( true );
		}
		else
		{
			Is3DIcon->SetVisible( false );
			Shadow->SetVisible( false );
			PosterImage->SetVisible( false );
		}
		CurrentItem = item;
	}
}

} // namespace VRMatterStreamTheater
