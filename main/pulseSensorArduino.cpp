#include "Arduino.h"
#define USE_ARDUINO_INTERRUPTS false    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   
#include "main.h"


extern "C" void getHRSensorValues();
extern "C" void run();
extern "C" void app_main2();


//  Variables
const int OUTPUT_TYPE = SERIAL_PLOTTER;

const int PulseWire = 4;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int PulseWire2 = 32;
const int PULSE_SENSOR_COUNT = 2;
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value. 

byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

PulseSensorPlayground pulseSensor(PULSE_SENSOR_COUNT);  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

unsigned long lastBeatSampleNumber[PULSE_SENSOR_COUNT];
int PTT;
int PTTdisplay;
int BPMdisplay;
int PULSE_SENSOR_COUNT = 2;

void setup() {   
  Serial.begin(115200);
  printf("SASQUATCH CHECK");
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.analogInput(PulseWire, 0);
  pulseSensor.analogInput(PulseWire2, 1);   
  pulseSensor.setThreshold(Threshold);   
}

// Initializes app_main2
extern "C" void getHRSensorValues(){
    printf("Am I in this one yet?");
    app_main2();
    return;
}

// PulseSensor Engine
void run() {
    //app_main();
    
    //while (true){
        int myBPM = pulseSensor.getBeatsPerMinute(0);  // Calls function on our pulseSensor object that returns BPM as an "int".
                                                // "myBPM" hold this BPM value now. 
        int myBPM2 = pulseSensor.getBeatsPerMinute(1);

        //printf("%i", myBPM);
        if (pulseSensor.sawNewSample()) {
            if (--samplesUntilReport == (byte) 0) {
                samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
                pulseSensor.outputSample();
                //if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened". 
                // Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
                // Serial.print("BPM: ");                        // Print phrase "BPM: " 
                // Serial.println(myBPM);                        // Print the value inside of myBPM. 
                // Serial.print("BPM2: ");
                // Serial.println(myBPM2);
                //}
                for (int i = 0; i < PULSE_SENSOR_COUNT; ++i) {
                    printf("I can get here");
                    if (pulseSensor.sawStartOfBeat(i)) {
                        printf("But I can't get here");
                        pulseSensor.outputBeat(i); 
                        lastBeatSampleNumber[i] = pulseSensor.getLastBeatTime(i);
                        if(i == 1){
                            PTT = lastBeatSampleNumber[1] - lastBeatSampleNumber[0];
                        }
                        // If both sensors match
                        if (((myBPM - myBPM2) < 5) && ((myBPM - myBPM2) > -5)){
                            BPMdisplay = int((myBPM + myBPM2) / 2);
                            if ((PTT > 500) && (PTT < 1000)){
                                PTTdisplay = PTT;
                            }
                        }
                        // If both sensors don't match
                        else if ((myBPM > 30) && (myBPM < 210)){
                            BPMdisplay = myBPM;
                        }
                        else if (myBPM >= 210){
                            BPMdisplay = 210;
                        }
                        else if (myBPM <= 0){
                            BPMdisplay = 0;
                        }
                        // Find length of numbers for bpm and ptt
                        unsigned int lenBPM = 0;
                        unsigned int lenPTT = 0;
                        int n = BPMdisplay;
                        int p = PTTdisplay;
                        do {
                            ++lenBPM; 
                            n /= 10;
                        } while (n);
                        do {
                            ++lenPTT;
                            p /= 10;
                        } while (p);
                        // The "val*" variables are declared in main.cpp and accessed using main.h
                        if ((lenBPM <= 3) && (BPMdisplay > 0)){
                            *p1 = BPMdisplay / 100 % 10;
                            *p2 = BPMdisplay / 10 % 10;
                            *p3 = BPMdisplay % 10;
                        }
                        if ((lenPTT <= 4) && (PTTdisplay > 0)){
                            *p4 = PTTdisplay / 1000 % 10;
                            *p5 = PTTdisplay / 100 % 10;
                            *p6 = PTTdisplay / 10 % 10;
                            *p7 = PTTdisplay % 10;
                        printf("Here I am again");
                        }
                    }
                }
            }
        }
    //}
    return;
}
// App_main2 initializes Arduino library
void app_main2() {
    initArduino();
    printf("Am I in this one yet? 2");
    // Trying to get it to run
    //run();
    return;
}
