LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)					# clean everything up to prepare for a module

include ../../VRLib/import_vrlib.mk		# import VRLib for this module.  Do NOT call $(CLEAR_VARS) until after building your module.
										# use += instead of := when defining the following variables: LOCAL_LDLIBS, LOCAL_CFLAGS, LOCAL_C_INCLUDES, LOCAL_STATIC_LIBRARIES 

include ../../VRLib/cflags.mk

LOCAL_MODULE    := cinema				# generate libcinema.so
LOCAL_SRC_FILES	:= 	CinemaApp.cpp \
					Native.cpp \
					View.cpp \
					SceneManager.cpp \
					ViewManager.cpp \
					ShaderManager.cpp \
					ModelManager.cpp \
					MovieManager.cpp \
					MoviePlayerView.cpp \
					MovieSelectionView.cpp \
					TheaterSelectionView.cpp \
					TheaterSelectionComponent.cpp \
					CarouselBrowserComponent.cpp \
					MovieCategoryComponent.cpp \
					MoviePosterComponent.cpp \
					MovieSelectionComponent.cpp \
					ResumeMovieView.cpp \
					ResumeMovieComponent.cpp \
					SwipeHintComponent.cpp \
					CinemaStrings.cpp \
					UI/UITexture.cpp \
					UI/UIMenu.cpp \
					UI/UIWidget.cpp \
					UI/UIContainer.cpp \
					UI/UILabel.cpp \
					UI/UIImage.cpp \
					UI/UIButton.cpp

NDK_MODULE_PATH := ../../Tools/
$(call import-add-path,$(NDK_MODULE_PATH))

include $(BUILD_SHARED_LIBRARY)			# start building based on everything since CLEAR_VARS
