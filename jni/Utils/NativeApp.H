#ifndef _ANDROID_NATIVE_APP_GLUE_H
#define _ANDROID_NATIVE_APP_GLUE_H

#include <jni.h>
#include <android/native_activity.h>
#include <android/configuration.h>
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "NativeApp", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "NativeApp", __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif


struct android_app {
    void* userData;
    void (*onAppCmd)(struct android_app* app, int32_t cmd);
    int32_t (*onInputEvent)(struct android_app* app, AInputEvent* event);
    ANativeActivity* activity;
    AConfiguration* config;     // Current configuration
    void* savedState;           // Saved state of the app
    size_t savedStateSize;
    ALooper* looper;            // The ALooper for app's thread
    AInputQueue* inputQueue;    // Input event queue
    ANativeWindow* window;      // Window for drawing
};

void app_dummy();

#ifdef __cplusplus
}
#endif

static android_app* g_App = nullptr;

#endif /* _ANDROID_NATIVE_APP_GLUE_H */
