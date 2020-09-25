#include <SFML/Graphics.hpp>
#include "WitchBlastGame.h"



#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

int main()
{
#ifdef __APPLE__
    // -------------------------------------------------------------------
    // http://stackoverflow.com/a/520951/2038264
    // This makes relative paths work in C++ in Xcode by changing
    // directory to the Resources folder inside the .app bundle
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
        // error!
    }
    CFRelease(resourcesURL);
    chdir(path);
    // -------------------------------------------------------------------
#endif
			LOGV("game!!");
			LOGV("bbb!!");
    WitchBlastGame game;
		LOGV("aastartGame!!");
		LOGV("startGame!!");
    game.startGame();

    return 0;
}

extern "C" int Android_Main();
extern "C" int Android_Update();
	
extern "C" int Android_Main() {

			LOGV("Android_Main!!");
		printf("\nAndroid_Main!");
		return main();
}
	
extern "C" int Android_Update() {
	LOGV("Android_upd!");
	printf("\nAndroid_upd!");
	return 0;
}
