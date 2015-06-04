/************************************************************************************

Filename    :   UIWidget.h
Content     :
Created     :	1/5/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( UIWidget_h )
#define UIWidget_h

#include "VRMenu/VRMenu.h"

using namespace OVR;

namespace VRMatterStreamTheater {

class CinemaApp;
class UIMenu;
class UITexture;

class UIWidget
{
public:
										UIWidget( CinemaApp &cinema );
										~UIWidget();

	menuHandle_t						GetHandle() const { return Handle; }
	VRMenuObject *						GetMenuObject() const;

	UIWidget *							GetParent() const { return Parent; }

	bool								IsHilighted() const;
	void								SetHilighted( bool const b );
	bool								IsSelected() const;
	void								SetSelected( bool const b );

	void								SetLocalPose( const Posef &pose );
	void								SetLocalPose( const Quatf &orientation, const Vector3f &position );
	Posef const & 						GetLocalPose() const;
	Vector3f const &					GetLocalPosition() const;
	void								SetLocalPosition( Vector3f const & pos );
	Quatf const &						GetLocalRotation() const;
	void								SetLocalRotation( Quatf const & rot );
	Vector3f            				GetLocalScale() const;
	void								SetLocalScale( Vector3f const & scale );
	void								SetLocalScale( float const & scale );

	Posef 								GetWorldPose() const;
	Vector3f 							GetWorldPosition() const;
	Quatf 								GetWorldRotation() const;
	Vector3f            				GetWorldScale() const;

	Vector2f const &					GetColorTableOffset() const;
	void								SetColorTableOffset( Vector2f const & ofs );
	Vector4f const &					GetColor() const;
	void								SetColor( Vector4f const & c );
	bool								GetVisible() const;
	void								SetVisible( const bool visible );

	void								SetImage( const int surfaceIndex, const eSurfaceTextureType textureType, char const * imageName );
	void								SetImage( const int surfaceIndex, const eSurfaceTextureType textureType, const GLuint image, const int width, const int height );
	void								SetImage( const int surfaceIndex, const eSurfaceTextureType textureType, const UITexture &image );
	void								SetImage( const int surfaceIndex, const eSurfaceTextureType textureType, const UITexture &image, const float dimsX, const float dimsY );
	void								SetImage( const int surfaceIndex, const eSurfaceTextureType textureType, const UITexture &image, const float dimsX, const float dimsY, const Vector4f &border );
	void 								SetImage( const int surfaceIndex, VRMenuSurfaceParms const & parms );

	void 								RegenerateSurfaceGeometry( int const surfaceIndex, const bool freeSurfaceGeometry );

	Vector2f const &					GetSurfaceDims( int const surfaceIndex ) const;
	void								SetSurfaceDims( int const surfaceIndex, Vector2f const &dims );	// requires call to RegenerateSurfaceGeometry() to take effect

	Vector4f const &					GetSurfaceBorder( int const surfaceIndex );
	void								SetSurfaceBorder( int const surfaceIndex, Vector4f const & border );	// requires call to RegenerateSurfaceGeometry() to take effect

	bool								GetSurfaceVisible( int const surfaceIndex ) const;
	void								SetSurfaceVisible( int const surfaceIndex, const bool visible );

	void								SetLocalBoundsExpand( Vector3f const mins, Vector3f const & maxs );

	void								AddComponent( VRMenuComponent * component );
	void								RemoveComponent( VRMenuComponent * component ) ;
	Array< VRMenuComponent* > const & 	GetComponentList() const;

protected:
	void 								AddToMenuWithParms( OvrGuiSys & guiSys, UIMenu *menu, UIWidget *parent, VRMenuObjectParms &parms );

    CinemaApp &							Cinema;

	UIMenu *							Menu;

	UIWidget *							Parent;

	VRMenuId_t 							Id;
	menuHandle_t						Handle;
	VRMenuObject *						Object;
};

} // namespace VRMatterStreamTheater

#endif // UIMenu_h
