#include <nds.h>

void initialise() {
	// install the default exception handler
	defaultExceptionHandler();

	videoSetMode(0);	//not using the main screen
	consoleDemoInit();
}

void waitForInput() {
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if (keysDown() & KEY_START) break;
	}
}

void beginTiming() {
	cpuStartTiming(0);
}

uint32_t endTiming() {
	return cpuEndTiming();
}