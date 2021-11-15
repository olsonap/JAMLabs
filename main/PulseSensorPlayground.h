/*
   A central Playground object to manage a set of PulseSensors.
   See https://www.pulsesensor.com to get started.

   Copyright World Famous Electronics LLC - see LICENSE
   Contributors:
     Joel Murphy, https://pulsesensor.com
     Yury Gitman, https://pulsesensor.com
     Bradford Needham, @bneedhamia, https://bluepapertech.com

   Licensed under the MIT License, a copy of which
   should have been included with this software.

   This software is not intended for medical use.
*/

/*
  NOTE: Every Sketch that uses the PulseSensor Playground
  must define the variable USE_ARDUINO_INTERRUPTS *before* including
  PulseSensorPlayground.h. If you don't, you will get a compiler error
  about "undefined reference to `PulseSensorPlayground::UsingInterrupts".

  In particular, if your Sketch wants the Playground to use interrupts
  to read and process PulseSensor data, your Sketch must contain the
  following two lines, in order:
    #define USE_ARDUINO_INTERRUPTS true
    #include <PulseSensorPlayground.h>

  If, instead, your Sketch does not use interrupts to read PulseSensor
  data,  your Sketch must instead contain the
  following two lines, in order:
    #define USE_ARDUINO_INTERRUPTS false
    #include <PulseSensorPlayground.h>

  See utility/interrupts.h for details.

  Internal, developer note: in the Playground code, don't use
  USE_ARDUINO_INTERRUPTS as a variable; instead, refer to
  PulseSensorPlayground::UsingInterrupts, which is a static variable
  that reflects what the Sketch defined USE_ARDUINO_INTERRUPTS to.
  Because USE_ARDUINO_INTERRUPTS is defined *only* in the user's Sketch,
  it doesn't exist when the various Playground modules are compiled.

  See further notes in interrupts.h
*/


#ifndef PULSE_SENSOR_PLAYGROUND_H
#define PULSE_SENSOR_PLAYGROUND_H

/*
   If you wish to perform timing statistics on your non-interrupt Sketch:

   Uncomment the line below: #define PULSE_SENSOR_TIMING_ANALYSIS true
   Compile and download your Sketch.
   Start the Arduino IDE Serial Monitor.
   Wait about 30 seconds. The Sketch should then print 3 numbers and hang.
   The three numbers are:
     Minimum variation (microseconds) from the 2 millisecond sample time.
     Average variation in that number.
     Maximum variation in that number.
   For example and output of -4 0 18 says that samples were made between
   4 microseconds short of 2 milliseconds, and 18 microseconds longer,
   with an average sample time right at 2 milliseconds (0 microseconds offset).

   If the average number is larger than, say, 50 microseconds, your Sketch
   is taking too much time per loop(), causing inaccuracies in the
   measured signal, heart rate, and inter-beat interval.

   You should aim for an average offset of under 50 microseconds.

   NOTES:

   1) This is an approximate measure, because interrupts can occur that
   the timing statistics cannot measure.

   2) These statistics compile only for non-interrupt Sketches. If your
   Sketch uses Interrupts to sample the PulseSensor signal, enabling
   this timing analysis will have no effect and will print nothing.

   3) Because timing analysis results are printed on Serial, you cannot
   use the Arduino IDE Serial Plotter or the Processing Visualizer to
   examine output when timing analysis is enabled.

   4) If the average is a negative number, your assumed Arduino clock
   speed may be incorrect. For example, if you compiled for an 8MHz clock
   and your Arduino actually runs at 16MHz, you would likely see an
   average offset of something like -1000.

*/
#define PULSE_SENSOR_TIMING_ANALYSIS false
//#define PULSE_SENSOR_TIMING_ANALYSIS true

/*
   If you wish to print the amount of memory used before your Sketch
   starts:

   Uncomment the line below: #define PULSE_SENSOR_MEMORY_USAGE true
   Compile and download your Sketch.
   Start the Arduino IDE Serial Monitor
   Your Sketch will start normally, then print memory usage, then hang.

   The memory usage consists of five numbers:
   data = bytes of global, uninitialized data storage (int x;)
   bss = bytes of global, initialized data storage (int x = 5;)
   heap = bytes of dynamically allocated memory (new Stream;)
   stack = bytes of local variables (those defined within a function)
   total = the total of data, bss, heap, and stack memory used.

   The amount of flash memory used is printed by the Arduino IDE
   when compilation finishes, with a printout such as:
     Sketch uses 5036 bytes (15%) of program storage space.

   NOTE: you must call pulse.setSerial(Serial) in your Sketch's setup().
*/
#define PULSE_SENSOR_MEMORY_USAGE false
// #define PULSE_SENSOR_MEMORY_USAGE true


#include "Arduino.h"
#include <Stream.h>
#include "utility/PulseSensor.h"
#include "utility/PulseSensorSerialOutput.h"
#include "utility/PulseSensorTimingStatistics.h"

class PulseSensorPlayground {
  public:
    
    static const unsigned long MICROS_PER_READ = (2 * 1000L); // usecs per sample.

    PulseSensorPlayground(int numberOfSensors = 1);
 
    boolean begin();

    boolean sawNewSample();

    void analogInput(int inputPin, int sensorIndex = 0);

    void blinkOnPulse(int blinkPin, int sensorIndex = 0);

    void fadeOnPulse(int fadePin, int sensorIndex = 0);

    void onSampleTime();

    int getLatestSample(int sensorIndex = 0);

    int getBeatsPerMinute(int sensorIndex = 0);

    int getInterBeatIntervalMs(int sensorIndex = 0);

    boolean sawStartOfBeat(int sensorIndex = 0);

    boolean isInsideBeat(int sensorIndex = 0);
    
    void setSerial(Stream &output);

    void setOutputType(byte outputType);

    void setThreshold(int threshold, int sensorIndex = 0);

    void outputSample();

    void outputToSerial(char symbol, int data);

    int getPulseAmplitude(int sensorIndex = 0);

    unsigned long getLastBeatTime(int sensorIndex = 0);

    void outputBeat(int sensorIndex = 0);

		boolean isPaused();

		boolean pause();

		boolean resume();

    static PulseSensorPlayground *OurThis;

  private:

    /*
       Configure and enable interrupts to read samples.
       Call only if PulseSensorPlayground::UsingInterrupts is true.

       This function is defined (vs. declared here) in interrupts.h
    */
    // void setupInterrupt();
		// bool disableInterrupt();
		// bool enableInterrupt();

#if PULSE_SENSOR_MEMORY_USAGE
    /*
       Print our RAM usage. See PULSE_SENSOR_MEMORY_USAGE
    */
    void printMemoryUsage();
#endif // PULSE_SENSOR_MEMORY_USAGE

    /*
       If true, the Sketch wants to use interrupts to read the PulseSensor(s).

       This variable is defined (vs. declared here) in interrupts.h
    */
    static boolean UsingInterrupts;
		boolean Paused;
    byte SensorCount;              // number of PulseSensors in Sensors[].
    PulseSensor *Sensors;          // use Sensors[idx] to access a sensor.
    volatile unsigned long NextSampleMicros; // Desired time to sample next.
    volatile boolean SawNewSample; // "A sample has arrived from the ISR"
    PulseSensorSerialOutput SerialOutput; // Serial Output manager.
#if PULSE_SENSOR_TIMING_ANALYSIS   // Don't use ram and flash we don't need.
    PulseSensorTimingStatistics *pTiming;
#endif // PULSE_SENSOR_TIMING_ANALYSIS
};

/*
   We include interrupts.h here instead of above
   because it depends on variables and functions we declare (vs. define)
   in PulseSensorPlayground.h.
*/
#include "utility/Interrupts.h"

#endif // PULSE_SENSOR_PLAYGROUND_H
