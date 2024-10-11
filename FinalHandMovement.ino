#include <Servo.h>
#include "EMGFilters.h"

#define s_input_1 A0
#define s_input_2 A1
#define s_input_3 A2
#define s_input_4 A3

#define SAMPLE_FREQ_1000HZ 1000
#define NOTCH_FREQ_50HZ 50
#define TIMING_DEBUG 1
#define m_avg 10  // Define the size of the moving

//Initialise EMGFilter
EMGFilters myFilter;
int sampleRate = SAMPLE_FREQ_1000HZ;
int humFreq = NOTCH_FREQ_50HZ;

//Initialise Servo motors
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

int t1 = 1000; // Threshold for electrode 1
int t2 = 700; // Threshold for electrode 2
int t3 = 2000; // Threshold for electrode 3
int t4 = 600; // Threshold for electrode 4

// Initialisation for EMG data processing
int value1, value2, value3, value4;
//d1-4 = Data After Filter 1-4
int d1, d2, d3, d4;
int envlope1, envlope2, envlope3, envlope4;
int movingAverage1[m_avg] = {0};
int movingAverage2[m_avg] = {0};
int movingAverage3[m_avg] = {0};
int movingAverage4[m_avg] = {0};
// Function prototypes
int computeMovingAverage(int *values, int newData);


void setup() {
  myFilter.init(sampleRate, humFreq, true, true, true);
  Serial.begin(9600);

// Attaching the servos to their pins
  servo1.attach(4); 
  servo2.attach(5);
  servo3.attach(6);
  servo4.attach(7);
  servo5.attach(8);
}

void loop() {
  // Collect data from sensors
  value1 = analogRead(s_input_1);
  value2 = analogRead(s_input_2);
  value3 = analogRead(s_input_3);
  value4 = analogRead(s_input_4);

  // Process data through filters
  d1 = myFilter.update(value1);
  d2 = myFilter.update(value2);
  d3 = myFilter.update(value3);
  d4 = myFilter.update(value4);

  // Apply moving average filter to smooth the signal
  envlope1 = computeMovingAverage(movingAverage1, sq(d1));
  envlope2 = computeMovingAverage(movingAverage2, sq(d2));
  envlope3 = computeMovingAverage(movingAverage3, sq(d3));
  envlope4 = computeMovingAverage(movingAverage4, sq(d4));

 // Apply Threshold and control servos
  servo1.write(envlope1 > t1 ? 0 : 180);
  servo2.write(envlope2 > t2 ? 0 : 180);
  servo3.write(envlope3 > t3 ? 0 : 180);
  servo4.write(envlope4 > t4 ? 0 : 180);
  servo5.write(envlope4 > t4 ? 0 : 180);
  delay(5); // Delay
}

// Smoothing Data
int computeMovingAverage(int *values, int newData) {
  
  // Move data to the right in the array
  for (int i = m_avg - 1; i > 0; --i) {
    values[i] = values[i - 1];
  }
  // New data in [0] array spot
  values[0] = newData; 

  // Compute the average of the values
  long sum = 0;
  for (int i = 0; i < m_avg; ++i) {
    sum += values[i];
  }
  return sum / m_avg;