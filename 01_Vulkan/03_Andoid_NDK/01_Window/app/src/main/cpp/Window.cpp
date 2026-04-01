#include<android_native_app_glue.h> // everuhting related with pure native activity need this (mainly android_main function and android_app struct)
#include<android/log.h> // for android_log_print function to print log in logcat

#include<memory.h>
typedef struct
{
    struct android_app *app;
    bool bActive;
} Engine;

ANativeWindow* androidNativeWindow = NULL;

// global callback function declarations
void engine_handle_cmd(struct android_app*, int32_t);
int32_t engine_handle_input(struct android_app*, AInputEvent*);

void android_main(struct android_app* state)
{
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
                androidNativeWindow = engine->app->window;
                __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_INIT_WINDOW : Window is created\n");
            }
            else
            {
                androidNativeWindow = NULL;
            }

            engine->bActive = true;
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
    // code
    return 0;
}

