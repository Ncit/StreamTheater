/************************************************************************************

Filename    :   PcCategoryComponent.h
Content     :   Menu component for the movie category menu.
Created     :   August 13, 2014
Authors     :   Jim Dosé

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the Cinema/ directory. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.

*************************************************************************************/
#if !defined( PcCategoryComponent_h )
#define PcCategoryComponent_h

#include "VRMenu/VRMenuComponent.h"
#include "PcManager.h"
#include "SelectionView.h"

using namespace OVR;

namespace VRMatterStreamTheater {

class MovieSelectionView;

//==============================================================
// PcCategoryComponent
class PcCategoryComponent : public VRMenuComponent
{
public:
							PcCategoryComponent( SelectionView *view, PcCategory category );

	static const Vector4f	HighlightColor;
	static const Vector4f	FocusColor;
	static const Vector4f	NormalColor;

private:
    SoundLimiter			Sound;

	bool					HasFocus;

	PcCategory			Category;

    SelectionView *		CallbackView;

private:
    virtual eMsgStatus      OnEvent_Impl( OvrGuiSys & guiSys, VrFrame const & vrFrame,
                                    VRMenuObject * self, VRMenuEvent const & event );

	void					UpdateColor( VRMenuObject * self );

    eMsgStatus              Frame( OvrGuiSys & guiSys, VrFrame const & vrFrame,
                                    VRMenuObject * self, VRMenuEvent const & event );
    eMsgStatus              FocusGained( OvrGuiSys & guiSys, VrFrame const & vrFrame,
                                    VRMenuObject * self, VRMenuEvent const & event );
    eMsgStatus              FocusLost( OvrGuiSys & guiSys, VrFrame const & vrFrame,
                                    VRMenuObject * self, VRMenuEvent const & event );
};

} // namespace VRMatterStreamTheater

#endif // PcCategoryComponent_h
