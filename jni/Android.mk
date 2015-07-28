TOP_LOCAL_PATH := $(call my-dir)

include $(call all-subdir-makefiles)

LOCAL_PATH := $(TOP_LOCAL_PATH)

include $(CLEAR_VARS)					# clean everything up to prepare for a module

include ../../OculusSDK/cflags.mk

LOCAL_MODULE    := cinema				# generate libcinema.so
LOCAL_SRC_FILES	:= 	CinemaApp.cpp \
					Native.cpp \
					View.cpp \
					SceneManager.cpp \
					ViewManager.cpp \
					ShaderManager.cpp \
					ModelManager.cpp \
					AppManager.cpp \
					PcManager.cpp \
					MoviePlayerView.cpp \
					SelectionView.cpp \
					PcSelectionView.cpp \
					AppSelectionView.cpp \
					TheaterSelectionView.cpp \
					TheaterSelectionComponent.cpp \
					CarouselBrowserComponent.cpp \
					PcCategoryComponent.cpp \
					MoviePosterComponent.cpp \
					MovieSelectionComponent.cpp \
					ResumeMovieView.cpp \
					ResumeMovieComponent.cpp \
					SwipeHintComponent.cpp \
					CinemaStrings.cpp \
					Settings.cpp \
					UI/UITexture.cpp \
					UI/UIMenu.cpp \
					UI/UIWidget.cpp \
					UI/UIContainer.cpp \
					UI/UILabel.cpp \
					UI/UIImage.cpp \
					UI/UIButton.cpp \
					UI/UITextButton.cpp

LOCAL_STATIC_LIBRARIES += vrappframework libovr
LOCAL_SHARED_LIBRARIES += vrapi

include $(BUILD_SHARED_LIBRARY)			# start building based on everything since CLEAR_VARS

$(call import-module,LibOVR/Projects/Android/jni)
$(call import-module,VrApi/Projects/AndroidPrebuilt/jni)
$(call import-module,VrAppFramework/Projects/Android/jni)
