/************************************************************************************

Filename    :   UITextButton.h
Content     :
Created     :	1/23/2015
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/

#if !defined( UITextButton_h )
#define UITextButton_h

#include "VRMenu/VRMenu.h"
#include "UI/UIWidget.h"
#include "VRMenu/VRMenuComponent.h"
#include "UI/UITexture.h"

using namespace OVR;

namespace VRMatterStreamTheater {

class CinemaApp;
class UITextButton;

//==============================================================
// UITextButtonComponent
class UITextButtonComponent : public VRMenuComponent
{
public:
	static const int TYPE_ID = 159493;

					UITextButtonComponent( UITextButton &button );

	virtual int		GetTypeId() const { return TYPE_ID; }

	bool			IsPressed() const { return TouchDown; }

private:
	UITextButton &		Button;

    SoundLimiter    GazeOverSoundLimiter;
    SoundLimiter    DownSoundLimiter;
    SoundLimiter    UpSoundLimiter;

    bool			TouchDown;

private:
    virtual eMsgStatus      OnEvent_Impl( OvrGuiSys & guiSys, VrFrame const & vrFrame,
                                    VRMenuObject * self, VRMenuEvent const & event );

    eMsgStatus              FocusGained( OvrGuiSys & guiSys, VrFrame const & vrFrame,
                                    VRMenuObject * self, VRMenuEvent const & event );
    eMsgStatus              FocusLost( OvrGuiSys & guiSys, VrFrame const & vrFrame,
                                    VRMenuObject * self, VRMenuEvent const & event );
};

//==============================================================
// UITextButton

class UITextButton : public UIWidget
{
	friend class UITextButtonComponent;

public:
										UITextButton( CinemaApp &cinema );
										~UITextButton();

	void 								AddToMenu( OvrGuiSys & guiSys, UIMenu *menu, UIWidget *parent = NULL );

	void								SetButtonImages( const UITexture &normal, const UITexture &hover, const UITexture &pressed );

	void								SetOnClick( void ( *callback )( UITextButton *, void * ), void *object );

	void								SetIsSelected( bool ( *callback )( UITextButton *, void * ), void *object );

	void								SetIsEnabled( bool ( *callback )( UITextButton *, void * ), void *object );

	void								UpdateButtonState();

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

	Vector4f const &					GetSelectedColor() const;
	void								SetSelectedColor( Vector4f const & c );
	Vector4f const &					GetHoverColor() const;
	void								SetHoverColor( Vector4f const & c );
	Vector4f const &					GetEnabledColor() const;
	void								SetEnabledColor( Vector4f const & c );
	Vector4f const &					GetDisabledColor() const;
	void								SetDisabledColor( Vector4f const & c );

	Bounds3f            				GetTextLocalBounds( BitmapFont const & font ) const;


private:
	UITextButtonComponent				ButtonComponent;

	Vector4f							SelectedColor;
	Vector4f							HoverColor;
	Vector4f							EnabledColor;
	Vector4f							DisabledColor;

	void 								( *OnClickFunction )( UITextButton *button, void *object );
	void *								OnClickObject;
	bool 								( *IsSelectedFunction )( UITextButton *button, void *object );
	void *								IsSelectedObject;
	bool 								( *IsEnabledFunction )( UITextButton *button, void *object );
	void *								IsEnabledObject;

	void 								OnClick();
	bool								IsSelected();
	bool								IsEnabled();

};

} // namespace VRMatterStreamTheater

#endif // UITextButton_h
