#ifndef __GamePCH_H__
#define __GamePCH_H__

#include "../../Libraries/Framework/Source/FrameworkPCH.h"
#include "../../Libraries/Box2D/Box2D/Box2D.h"

//audio library
#include <xaudio2.h>
#pragma comment(lib, "Xaudio2")

#include "cJSONHelpers.h"

#if ANDROID
#include "android_native_app_glue.h"
#include "UtilityAndroid.h"
#endif

#endif //__GamePCH_H__
