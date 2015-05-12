/************************************************************************************

Filename    :   UILabel.h
Content     :
Created     :	1/5/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( UILabel_h )
#define UILabel_h

#include "VRMenu/VRMenu.h"
#include "UI/UIWidget.h"

using namespace OVR;

namespace VRMatterStreamTheater {

class CinemaApp;

class UILabel : public UIWidget
{
public:
										UILabel( CinemaApp &cinema );
										~UILabel();

	void 								AddToMenu( UIMenu *menu, UIWidget *parent = NULL );

	void								SetText( const char *text );
	void								SetText( const String &text );
	void								SetTextWordWrapped( char const * text, class BitmapFont const & font, float const widthInMeters );
	const String & 						GetText() const;

	void								SetFontScale( float const scale );
	float 								GetFontScale() const;

	void     				           	SetTextOffset( Vector3f const & pos );
    Vector3f const &    				GetTextOffset() const;

	Vector4f const &					GetTextColor() const;
	void								SetTextColor( Vector4f const & c );

	Bounds3f            				GetTextLocalBounds( BitmapFont const & font ) const;
};

} // namespace VRMatterStreamTheater

#endif // UILabel_h
