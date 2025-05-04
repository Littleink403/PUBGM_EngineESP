#include <thread>
#include <Game.hpp>


/*
 * @ Time: 2023/09/10
 * @ Author: XiaoMo
 */


__attribute__((constructor)) void Init() {
    static std::thread hack([]() {
        do {
            libUE4 = GetModule("libUE4.so");
    		sleep(1);
        } while (!libUE4);
        
        do {
            g_App = *(android_app**)(libUE4 + Off::GNativeApp);
            sleep(1);
        } while (!g_App);
    
        orig_onInputEvent = g_App->onInputEvent;
        g_App->onInputEvent = onInputEvent;
        
        Data::LoadFunc(libUE4);
    });
    hack.detach();
}
