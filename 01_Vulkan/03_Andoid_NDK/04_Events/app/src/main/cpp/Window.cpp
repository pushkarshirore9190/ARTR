#include<android_native_app_glue.h> // everuhting related with pure native activity need this (mainly android_main function and android_app struct)
#include<android/log.h> // for android_log_print function to print log in logcat
#include<math.h> // for sqrt function to calculate distance between two points

#include<memory.h>
typedef struct
{
    struct android_app *app;
    bool bActive;
} Engine;

ANativeWindow* androidNativeWindow = NULL;

// Event handling related global variables
long touchStartTime = 0;
long pendingSingleTapTime = 0;

bool bTouchDown = false;
bool bDragging = false;
bool bLongPressDetected = false;
bool bDoubleTabDetected = false;
bool bPendingSingleTap = false;

float touchStartX = 0.0f;
float touchStartY = 0.0f;
float lastTapX = 0.0f;
float lastTapY = 0.0f;


// global callback function declarations
void engine_handle_cmd(struct android_app*, int32_t);
int32_t engine_handle_input(struct android_app*, AInputEvent*);

void android_main(struct android_app* state)
{
    // full screen hide status bar
    JavaVM* vm = state->activity->vm;

    JNIEnv* env = NULL;

    // attach current thread to JVM and get JNIEnv
    vm->AttachCurrentThread(&env, NULL);

    jobject activityObject = state->activity->clazz;

    jclass activityClass = env->GetObjectClass(activityObject);

    jclass windowClass = env->FindClass("android/view/Window");

    jclass viewClass = env->FindClass("android/view/View");

    // get window method
    jmethodID getWindowMethod = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");

    jobject windowObject = env->CallObjectMethod(activityObject, getWindowMethod);

    // get decor view method
    jmethodID getDecorViewMethod = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

    jobject decorViewObject = env->CallObjectMethod(windowObject, getDecorViewMethod);

    // get 8 viewclass static fields

    const int flag_SYSTEM_UI_FLAG_IMMERSIVE =
    env->GetStaticIntField(viewClass,env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE", "I"));

    const int flag_SYSTEM_UI_FLAG_LAYOUT_STABLE = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LAYOUT_STABLE", "I"));
    
    const int flag_SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION", "I"));

    const int flag_SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN", "I"));

    const int flag_SYSTEM_UI_FLAG_HIDE_NAVIGATION = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I"));

    const int flag_SYSTEM_UI_FLAG_FULLSCREEN = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I"));

    const int flag_SYSTEM_UI_FLAG_LOW_PROFILE = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LOW_PROFILE", "I"));

    const int flag_SYSTEM_UI_FLAG_IMMERSIVE_STICKY = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I"));


    jmethodID setSystemUiVisibilityMethod = env->GetMethodID(viewClass, "setSystemUiVisibility", "(I)V");

    // set system UI visibility
    env->CallVoidMethod(decorViewObject, setSystemUiVisibilityMethod, flag_SYSTEM_UI_FLAG_IMMERSIVE | flag_SYSTEM_UI_FLAG_LAYOUT_STABLE | flag_SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | flag_SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | flag_SYSTEM_UI_FLAG_HIDE_NAVIGATION | flag_SYSTEM_UI_FLAG_FULLSCREEN | flag_SYSTEM_UI_FLAG_LOW_PROFILE | flag_SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

    // change to landscape mode
    jclass activityInfoClass = env->FindClass("android/content/pm/ActivityInfo");

    const int flag_SCREEN_ORIENTATION_LANDSCAPE = env->GetStaticIntField(activityInfoClass, env->GetStaticFieldID(activityInfoClass, "SCREEN_ORIENTATION_LANDSCAPE", "I"));

    jmethodID setRequestedOrientationMethod = env->GetMethodID(activityClass, "setRequestedOrientation", "(I)V");

    // Now actually call set orientation method and pass the required flag to set landscape mode
    env->CallVoidMethod(activityObject, setRequestedOrientationMethod, flag_SCREEN_ORIENTATION_LANDSCAPE);

    // detach current thread from JVM
    vm->DetachCurrentThread();

    // code
    Engine engine;
    memset((void*)&engine, 0, sizeof(Engine));

    // initialize state
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;

    engine.app = state;
    __android_log_print(ANDROID_LOG_INFO, "PRS:", "android_main() : Started Successfully\n"); 

    // loop
    while(1)
    {
        int Identifier;
        struct android_poll_source* source = NULL;
        while((Identifier = ALooper_pollOnce(engine.bActive ? 0 : -1, NULL, NULL, (void**)&source)) >= 0)
        {
            // process system events
            if(source != NULL)
            {
                source->process(state, source);
            }

            // check when to exit
            if(state ->destroyRequested != 0)
            {
                return;
            }
    
        }

        // handle long press
        if(bTouchDown == true && bDragging == false && bLongPressDetected == false && bDoubleTabDetected == false)
        {
            struct timespec ts;
            memset((void*)&ts, 0, sizeof(ts));

            clock_gettime(CLOCK_MONOTONIC, &ts);

            long now = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

            if(now - touchStartTime >= 500) // 500 ms for long press
            {
                bLongPressDetected = true;
                __android_log_print(ANDROID_LOG_INFO, "PRS:", "Long Press Detected\n");
            }

        }

        // handle single tap
        if(bPendingSingleTap == true)
        {
            struct timespec ts;
            memset((void*)&ts, 0, sizeof(ts));

            clock_gettime(CLOCK_MONOTONIC, &ts);

            long now = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

            if(now - pendingSingleTapTime >= 300) // 300 ms for single tap
            {
                bPendingSingleTap = false;
                __android_log_print(ANDROID_LOG_INFO, "PRS:", "Single Tap Detected\n");
            }
        } 
    }

}

void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
    // code
    Engine* engine = (Engine*)app->userData;
    switch(cmd)
    {
        case APP_CMD_SAVE_STATE:
            engine->bActive = false;
            __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_SAVE_STATE : Saving State\n");
            break;

        case APP_CMD_INIT_WINDOW: // like WM_CREATE
            if(app->window != NULL)
            {
                 
                engine->bActive = true;

                androidNativeWindow = engine->app->window;
                // draw background color with pixel by pixel coloring by cpu

                ANativeWindow_Buffer buffer;
                uint32_t* pixels = NULL;
                uint32_t color;
                int x, y;

                // set the buffer and format
                ANativeWindow_setBuffersGeometry(androidNativeWindow, 0, 0, WINDOW_FORMAT_RGBA_8888);

                if(ANativeWindow_lock(androidNativeWindow, &buffer, NULL) == 0)
                {
                    pixels = (uint32_t*)buffer.bits;

                    // fill the buffer with red color
                    color = 0xFFFF00FF; // ARGB format

                    for(y = 0; y < buffer.height; y++)
                    {
                        for(x = 0; x < buffer.width; x++)
                        {
                            pixels[y * buffer.stride + x] = color;
                        }
                    }

                    ANativeWindow_unlockAndPost(androidNativeWindow);
                }


                __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_INIT_WINDOW : Window is created\n");
            }
            else
            {
                androidNativeWindow = NULL;
            }

            break;

        case APP_CMD_TERM_WINDOW: // like WM_DESTROY
            __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_TERM_WINDOW : Window Terminated\n");
            break;

        case APP_CMD_GAINED_FOCUS:
            engine->bActive = true;
            __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_GAINED_FOCUS : Gained Focus\n");
            break;

        case APP_CMD_LOST_FOCUS:
            engine->bActive = false;
            __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_LOST_FOCUS : Lost Focus\n");
            break;
    }
}

int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
    Engine *engine = (Engine*)app->userData;
    int32_t eventType = AInputEvent_getType(event);

    switch(eventType)
    {
        case AINPUT_EVENT_TYPE_MOTION:
        {
            int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;

            switch (action)
            {
                case AMOTION_EVENT_ACTION_DOWN:
                {
                   touchStartX = AMotionEvent_getX(event, 0);
                   touchStartY = AMotionEvent_getY(event, 0);

                   struct timespec ts;
                   memset((void*)&ts, 0, sizeof(struct timespec));

                   clock_gettime(CLOCK_MONOTONIC, &ts);

                   touchStartTime = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
                   
                   bTouchDown = true;
                   bDragging = false;
                   bLongPressDetected = false;
                   bDoubleTabDetected = false; 

                   if(bPendingSingleTap == true)
                   {
                        long timeSinceLastTap = touchStartTime - pendingSingleTapTime;

                        float dX  = touchStartX - lastTapX;
                        float dY = touchStartY - lastTapY;

                        float distance = sqrtf((dX * dX) + (dY * dY));

                        if(timeSinceLastTap <= 300 && distance <= 100.0f)
                        {
                            bDoubleTabDetected = true;
                            __android_log_print(ANDROID_LOG_INFO, "PRS:", "Double Tap Detected\n");
                        }

                        bPendingSingleTap = false;
                   }   
                }
                break;

                case AMOTION_EVENT_ACTION_MOVE:
                {
                    float currentX = AMotionEvent_getX(event, 0);
                    float currentY = AMotionEvent_getY(event, 0);

                    float dX = currentX - touchStartX;
                    float dY = currentY - touchStartY;

                    float distance = sqrtf((dX * dX) + (dY * dY));

                    if(distance > 50.0f)
                    {
                        bDragging = true;
                    }
                }
                break;

                case AMOTION_EVENT_ACTION_UP:
                {
                   struct timespec ts;
                   memset((void*)&ts, 0, sizeof(struct timespec));

                   clock_gettime(CLOCK_MONOTONIC, &ts);

                   long currentTime = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
                   long duration = currentTime - touchStartTime;

                   float EndX = AMotionEvent_getX(event, 0);
                   float EndY = AMotionEvent_getY(event, 0);

                   float dX = EndX - touchStartX;
                   float dY = EndY - touchStartY;

                   float distance = sqrtf((dX * dX) + (dY * dY));

                   bTouchDown = false;

                   if(bDoubleTabDetected == true)
                   {
                   }
                   else if(bLongPressDetected == true)
                   {
                   }
                   else if(bDragging == true && distance > 150.0f)
                   {
                        __android_log_print(ANDROID_LOG_INFO, "PRS:", "Drag Detected\n");
                        ANativeActivity_finish(engine->app->activity);
                   }
                   else if(duration < 300 && distance < 50.0f)
                   {
                        bPendingSingleTap = true;
                        pendingSingleTapTime = currentTime;
                        lastTapX = EndX;
                        lastTapY = EndY;
                   }

                   bDragging = false;
                }
                break;
            }
        }
    }

    return 0;
}
