/*

Not in use.
Here for reference only.
This file works well in arduino.


*/



#define USE_ARDUINO_INTERRUPTS false    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>  

const int PulseWire = 4;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int PulseWire2 = 32;
const int PULSE_SENSOR_COUNT = 2;
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value. 

PulseSensorPlayground pulseSensor(PULSE_SENSOR_COUNT); 

byte samplesUntilReport;
//const byte SAMPLES_PER_SERIAL_SAMPLE = 10;
//Beats Per Minute (bpm) variables
int val1 = 0x01;
int val2 = 0x00;
int val3 = 0x00;
//Pulse Transit Time (PTT) variables
int val4 = 0x01;
int val5 = 0x00;
int val6 = 0x00;
int val7 = 0x00;


unsigned long lastBeatSampleNumber[PULSE_SENSOR_COUNT];
int PTT;
int PTTdisplay;
int BPMdisplay;

extern "C" void getHRSensorValues()
{ 
    unsigned long lastBeatSampleNumber[PULSE_SENSOR_COUNT];
    int PTT;
    int PTTdisplay;
    int BPMdisplay;
    int PULSE_SENSOR_COUNT = 2;

    while (true) {
        pulseSensor.analogInput(PulseWire, 0);
        pulseSensor.analogInput(PulseWire2, 1);   
        pulseSensor.setThreshold(Threshold); 

        int myBPM = pulseSensor.getBeatsPerMinute(0);
        int myBPM2 = pulseSensor.getBeatsPerMinute(1);

        if (pulseSensor.sawNewSample()) {

            if (--samplesUntilReport == (byte) 0) {
                samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
                pulseSensor.outputSample();
                for (int i = 0; i < PULSE_SENSOR_COUNT; ++i) {
                    if (pulseSensor.sawStartOfBeat(i)) {
                        pulseSensor.outputBeat(i);
                        lastBeatSampleNumber[i] = pulseSensor.getLastBeatTime(i);
                        if(i == 1){
                            PTT = lastBeatSampleNumber[1] - lastBeatSampleNumber[0];
                        }
                        // If both sensors match
                        if (((myBPM - myBPM2) < 5) && ((myBPM - myBPM2) > -5)){
                            BPMdisplay = (myBPM + myBPM2) / 2;
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

                        if ((lenBPM <= 3) && (BPMdisplay > 0)){
                            val1 = BPMdisplay / 100 % 10;
                            val2 = BPMdisplay / 10 % 10;
                            val3 = BPMdisplay % 10;
                        }
                        if ((lenPTT <= 4) && (PTTdisplay > 0)){
                            val4 = PTTdisplay / 1000 % 10;
                            val5 = PTTdisplay / 100 % 10;
                            val6 = PTTdisplay / 10 % 10;
                            val7 = PTTdisplay % 10;
                        }
                    }
                }
            }
        }
    }
}