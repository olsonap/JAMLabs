#include "Arduino.h"
#define USE_ARDUINO_INTERRUPTS false
//#include <PulseSensorPlayground.h>   


extern "C" void getHRSensorValues();

extern "C" void run();

extern "C" void app_main();
