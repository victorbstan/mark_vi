LOCAL_PATH := $(call my-dir)

LOCAL_SHORT_COMMANDS := true
APP_SHORT_COMMANDS := true

TO_PROJECT_FOLDER := ../../../..

CORE_PATH_SDK_FOR_PROJECT := ./SDK
#March 16 - Core --> CoreQ
CORE_PATH_CORE_FOR_PROJECT := ./SDK/Core
CORE_PATH_HTTP_FOR_PROJECT := ./SDK/HTTPClient
CORE_PATH_FMOD_FOR_PROJECT := ./SDK/fmod
CORE_PATH_SDK_FOR_PROJECT := ./SDK
CORE_PATH_HTTP_API_FOR_PROJECT := ./SDK/HTTPClient/API
CORE_PATH_SDK := $(TO_PROJECT_FOLDER)/$(CORE_PATH_SDK_FOR_PROJECT)
CORE_PATH_CORE:= $(TO_PROJECT_FOLDER)/$(CORE_PATH_CORE_FOR_PROJECT)
CORE_PATH_HTTP:= $(TO_PROJECT_FOLDER)/$(CORE_PATH_HTTP_FOR_PROJECT)
CORE_PATH_FMOD:= $(TO_PROJECT_FOLDER)/$(CORE_PATH_FMOD_FOR_PROJECT)
CORE_PATH_SDK := $(TO_PROJECT_FOLDER)/$(CORE_PATH_SDK_FOR_PROJECT)
CORE_PATH_HTTP_API:= $(TO_PROJECT_FOLDER)/$(CORE_PATH_HTTP_API_FOR_PROJECT)
CORE_SDL_PATH := SDL2-devel-2.0.8-mingw/SDL2-2.0.8/i686-w64-mingw32/include
SDL_PATH := $(CORE_PATH_SDK)/$(CORE_SDL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(TO_PROJECT_FOLDER)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(TO_PROJECT_FOLDER)/Mark_V
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(CORE_PATH_CORE)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(CORE_PATH_HTTP)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(CORE_PATH_HTTP_API)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(CORE_PATH_FMOD)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(CORE_PATH_SDK)

# following line prints which is what the info command does.
$(info $(LOCAL_C_INCLUDES))

MY_FILE_LIST  := $(wildcard $(LOCAL_PATH)/$(TO_PROJECT_FOLDER)/Mark_V/*.c)
MY_FILE_LIST  += $(wildcard $(LOCAL_PATH)/$(CORE_PATH_CORE)/*.c)
MY_FILE_LIST  += $(wildcard $(LOCAL_PATH)/$(CORE_PATH_HTTP)/*.c)
MY_FILE_LIST  += $(wildcard $(LOCAL_PATH)/$(CORE_PATH_HTTP_API)/*.c)
MY_FILE_LIST  += $(wildcard $(LOCAL_PATH)/$(CORE_PATH_FMOD)/*.c)

LOCAL_SRC_FILES := $(MY_FILE_LIST:$(LOCAL_PATH)/%=%)
#-Werror 
# 
#March 16 - 
#					<Add option="-D_WIN32" />
#					<Add option="-DWIN32" />
#					<Add option="-D_WINDOWS" />
#					<Add option="-D_DEBUG" />
#					<Add option="-DDEBUG" />			// YES
#					<Add option="-DCORE_PTHREADS" />	// YES
#					<Add option="-Did386=0" />
#					<Add option="-pg" />
#					<Add option="-g" />
#					<Add option="-ggdb" />
#					<Add option="-D_WIN32" />
#					<Add option="-DWIN32" />
#					<Add option="-D_WINDOWS" />
#					<Add option="-D_DEBUG" />
#					<Add option="-DDEBUG" />
#					<Add option="-DDEBUGGL" />
#					<Add option="-DCORE_GL" />
#					<Add option="-DCORE_SDL" />
#					<Add option="-Did386=0" />
#			<Add option="-Winline" />
#			<Add option="-W" />
#			<Add option="-fexceptions" />
#			<Add option="-Wno-trigraphs" />
#			<Add option="-DQUAKE_GAME" />

# -DDEBUG
LOCAL_CFLAGS    := -DCORE_PTHREADS -DCORE_SDL -Did386=0 -DCORE_GL -DQUAKE_GAME

#$(info $(LOCAL_PATH))
#$(info $(MY_FILE_LIST))
#$(error Bob)
LOCAL_SHORT_COMMANDS := true
APP_SHORT_COMMANDS := true
LOCAL_SHARED_LIBRARIES := SDL2
# libdl is the dynamic linking library.
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog

LOCAL_SHORT_COMMANDS := true
APP_SHORT_COMMANDS := true

include $(BUILD_SHARED_LIBRARY)
