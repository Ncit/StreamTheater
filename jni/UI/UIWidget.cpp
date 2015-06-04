/************************************************************************************

Filename    :   UIWidget.cpp
Content     :
Created     :	1/5/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#include "UI/UIWidget.h"
#include "UI/UIMenu.h"
#include "VRMenu/VRMenuMgr.h"
#include "VRMenu/GuiSys.h"
#include "CinemaApp.h"

namespace VRMatterStreamTheater {

UIWidget::UIWidget( CinemaApp &cinema ) :
	Cinema( cinema ),
	Parent( NULL ),
	Id(),
	Handle(),
	Object( NULL )

{
}

UIWidget::~UIWidget()
{
	//DeletePointerArray( MovieBrowserItems );
}

VRMenuObject * UIWidget::GetMenuObject() const
{
	return Cinema.GetGuiSys().GetVRMenuMgr().ToObject( GetHandle() );
}

void UIWidget::AddToMenuWithParms( OvrGuiSys & guiSys, UIMenu *menu, UIWidget *parent, VRMenuObjectParms &parms )
{
	Menu = menu;
	Parent = parent;

	Id = parms.Id;

    Array< VRMenuObjectParms const * > parmArray;
	parmArray.PushBack( &parms );

	menuHandle_t parentHandle = ( parent == NULL ) ? menu->GetVRMenu()->GetRootHandle() : parent->GetHandle();
    Menu->GetVRMenu()->AddItems( guiSys, parmArray, parentHandle, false );
    parmArray.Clear();

    if ( parent == NULL )
    {
    	Handle = Menu->GetVRMenu()->HandleForId( Cinema.GetGuiSys().GetVRMenuMgr(), Id );
    }
    else
    {
    	Handle = parent->GetMenuObject()->ChildHandleForId( Cinema.GetGuiSys().GetVRMenuMgr(), Id );
    }
}

bool UIWidget::IsHilighted() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	return object->IsHilighted();
}

void UIWidget::SetHilighted( bool const b )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	object->SetHilighted( b );
}

bool UIWidget::IsSelected() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	return object->IsSelected();
}

void UIWidget::SetSelected( bool const b )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	object->SetSelected( b );
}

void UIWidget::SetLocalPose( const Posef &pose )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	if ( object != NULL )
	{
		object->SetLocalPose( pose );
	}
}

void UIWidget::SetLocalPose( const Quatf &orientation, const Vector3f &position )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	if ( object != NULL )
	{
		object->SetLocalPose( Posef( orientation, position ) );
	}
}

Posef const & UIWidget::GetLocalPose() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	return object->GetLocalPose();
}

void UIWidget::SetLocalPosition( Vector3f const & pos )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	if ( object != NULL )
	{
		object->SetLocalPosition( pos );
	}
}

Vector3f const & UIWidget::GetLocalPosition() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	return object->GetLocalPosition();
}

void UIWidget::SetLocalRotation( Quatf const & rot )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	if ( object != NULL )
	{
		object->SetLocalRotation( rot );
	}
}

Quatf const & UIWidget::GetLocalRotation() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	return object->GetLocalRotation();
}

void UIWidget::SetLocalScale( Vector3f const & scale )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	if ( object != NULL )
	{
		object->SetLocalScale( scale );
	}
}

void UIWidget::SetLocalScale( float const & scale )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	if ( object != NULL )
	{
		object->SetLocalScale( Vector3f( scale ) );
	}
}

Vector3f UIWidget::GetLocalScale() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	return object->GetLocalScale();
}

Posef UIWidget::GetWorldPose() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );

	Posef const & localPose = object->GetLocalPose();

	if ( Parent == NULL )
	{
		return localPose;
	}

	Posef parentModelPose = Parent->GetWorldPose();
	Vector3f parentScale = Parent->GetWorldScale();

	Posef curModelPose;
	curModelPose.Position = parentModelPose.Position + ( parentModelPose.Orientation * parentScale.EntrywiseMultiply( localPose.Position ) );
	curModelPose.Orientation = localPose.Orientation * parentModelPose.Orientation;

	return curModelPose;
}

Vector3f UIWidget::GetWorldPosition() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );

	Posef const & localPose = object->GetLocalPose();

	if ( Parent == NULL )
	{
		return localPose.Position;
	}

	Posef parentModelPose = Parent->GetWorldPose();
	Vector3f parentScale = Parent->GetWorldScale();

	return parentModelPose.Position + ( parentModelPose.Orientation * parentScale.EntrywiseMultiply( localPose.Position ) );
}

Quatf UIWidget::GetWorldRotation() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );

	Quatf const & rotation = object->GetLocalRotation();
	if ( Parent == NULL )
	{
		return rotation;
	}

	return rotation * Parent->GetWorldRotation();
}

Vector3f UIWidget::GetWorldScale() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );

	Vector3f scale = object->GetLocalScale();

	if ( Parent == NULL )
	{
		return scale;
	}

	Vector3f parentScale = Parent->GetWorldScale();
	return parentScale.EntrywiseMultiply( scale );
}

bool UIWidget::GetVisible() const
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	return ( object->GetFlags() & VRMENUOBJECT_DONT_RENDER ) == 0;
}

void UIWidget::SetVisible( const bool visible )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );
	if ( visible )
	{
		object->RemoveFlags( VRMENUOBJECT_DONT_RENDER );
	}
	else
	{
		object->AddFlags( VRMENUOBJECT_DONT_RENDER );
	}
}

void UIWidget::SetImage( const int surfaceIndex, const eSurfaceTextureType textureType, char const * imageName )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );

	while( object->NumSurfaces() <= surfaceIndex )
	{
		object->AllocSurface();
	}

	VRMenuSurfaceParms parms( "",
		imageName, textureType,
		NULL, SURFACE_TEXTURE_MAX,
		NULL, SURFACE_TEXTURE_MAX );

	object->CreateFromSurfaceParms( 0, parms );
}

void UIWidget::SetImage( const int surfaceIndex, const eSurfaceTextureType textureType, const GLuint image, const int width, const int height )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );

	while( object->NumSurfaces() <= surfaceIndex )
	{
		object->AllocSurface();
	}

	VRMenuSurfaceParms parms( "",
		image, width, height, textureType,
		0, 0, 0, SURFACE_TEXTURE_MAX,
		0, 0, 0, SURFACE_TEXTURE_MAX );

	object->CreateFromSurfaceParms( 0, parms );
}

void UIWidget::SetImage( const int surfaceIndex, const eSurfaceTextureType textureType, const UITexture &image )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );

	while( object->NumSurfaces() <= surfaceIndex )
	{
		object->AllocSurface();
	}

	VRMenuSurfaceParms parms( "",
		image.Texture, image.Width, image.Height, textureType,
		0, 0, 0, SURFACE_TEXTURE_MAX,
		0, 0, 0, SURFACE_TEXTURE_MAX );

	object->CreateFromSurfaceParms( 0, parms );
}

void UIWidget::SetImage( const int surfaceIndex, const eSurfaceTextureType textureType, const UITexture &image, const float dimsX, const float dimsY )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );

	while( object->NumSurfaces() <= surfaceIndex )
	{
		object->AllocSurface();
	}

	VRMenuSurfaceParms parms( "",
		image.Texture, image.Width, image.Height, textureType,
		0, 0, 0, SURFACE_TEXTURE_MAX,
		0, 0, 0, SURFACE_TEXTURE_MAX );

	parms.Dims.x = dimsX;
	parms.Dims.y = dimsY;

	object->CreateFromSurfaceParms( 0, parms );
}

void UIWidget::SetImage( const int surfaceIndex, const eSurfaceTextureType textureType, const UITexture &image, const float dimsX, const float dimsY, const Vector4f &border )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );

	while( object->NumSurfaces() <= surfaceIndex )
	{
		object->AllocSurface();
	}

	VRMenuSurfaceParms parms( "",
		image.Texture, image.Width, image.Height, textureType,
		0, 0, 0, SURFACE_TEXTURE_MAX,
		0, 0, 0, SURFACE_TEXTURE_MAX );

	parms.Dims.x = dimsX;
	parms.Dims.y = dimsY;
	parms.Border = border;

	object->CreateFromSurfaceParms( 0, parms );
}

void UIWidget::SetImage( const int surfaceIndex, VRMenuSurfaceParms const & parms )
{
	VRMenuObject * object = GetMenuObject();
	OVR_ASSERT( object );

	while( object->NumSurfaces() <= surfaceIndex )
	{
		object->AllocSurface();
	}

	object->CreateFromSurfaceParms( 0, parms );
}

void UIWidget::AddComponent( VRMenuComponent * component )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->AddComponent( component );
}

void UIWidget::RemoveComponent( VRMenuComponent * component )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->RemoveComponent( component );
}

Array< VRMenuComponent* > const & UIWidget::GetComponentList() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetComponentList();
}

void UIWidget::SetColorTableOffset( Vector2f const & ofs )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetColorTableOffset( ofs );
}

Vector2f const & UIWidget::GetColorTableOffset() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetColorTableOffset();
}

void UIWidget::SetColor( Vector4f const & c )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetColor( c );
}

Vector4f const & UIWidget::GetColor() const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetColor();
}

void UIWidget::RegenerateSurfaceGeometry( int const surfaceIndex, const bool freeSurfaceGeometry )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->RegenerateSurfaceGeometry( surfaceIndex, freeSurfaceGeometry );
}

Vector2f const & UIWidget::GetSurfaceDims( int const surfaceIndex ) const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetSurfaceDims( surfaceIndex );
}

void UIWidget::SetSurfaceDims( int const surfaceIndex, Vector2f const &dims )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetSurfaceDims( surfaceIndex, dims );
}

Vector4f const & UIWidget::GetSurfaceBorder( int const surfaceIndex )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetSurfaceBorder( surfaceIndex );
}

void UIWidget::SetSurfaceBorder( int const surfaceIndex, Vector4f const & border )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->SetSurfaceBorder( surfaceIndex, border );
}

bool UIWidget::GetSurfaceVisible( int const surfaceIndex ) const
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	return object->GetSurfaceVisible( surfaceIndex );
}

void UIWidget::SetSurfaceVisible( int const surfaceIndex, const bool visible )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetSurfaceVisible( surfaceIndex, visible );
}

void UIWidget::SetLocalBoundsExpand( Vector3f const mins, Vector3f const & maxs )
{
	VRMenuObject * object = GetMenuObject();
	assert( object );
	object->SetLocalBoundsExpand( mins, maxs );
}

} // namespace VRMatterStreamTheater
