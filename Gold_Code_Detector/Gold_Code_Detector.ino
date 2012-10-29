/*******************************************************************
Problem Set 3 Solution: Gold Code Correlation
Author: Joshua Vasquez
Date: October 13, 2012

    
*******************************************************************/

#include <math.h>  // for the absolute value function


/* CONSTANTS */
const int PhotoTransistor = 5; // phototransistorPin
const unsigned long delayTime = 250; // time beteen samples in [us]
const byte numSamples = 31;
const byte maxCorrelation = 31;
const byte numGoldCodes = 8;
/* Global Variables */

int rawValues[numSamples];
boolean cleanValues[numSamples];

const boolean GoldCodes[numGoldCodes][numSamples] = 
{ 
  {0,0,0,0,0,0,0,1,0,0,0,1,1,0,1,1,0,0,0,0,1,1,0,0,1,1,1,0,0,1,1},
  {1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,0,1,0,0},
  {0,1,0,0,0,0,1,1,0,1,0,0,0,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0},
  {1,0,1,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,0,1},
  {0,0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,1,1,0,1,0,0,1,1,1},
  {1,1,1,0,0,0,1,0,0,1,1,0,1,1,1,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0},
  {0,1,1,0,0,1,1,0,1,1,0,1,1,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,0},
  {1,0,0,1,0,1,1,1,0,1,1,0,0,1,1,1,0,1,1,0,0,0,1,0,0,1,0,0,0,1,1}
};


/*******************************************************************
void setup()
  
*******************************************************************/
void setup()
{
  // analog inputs don't need to be defined as inputs.
  
  Serial.begin(9600);
}


/*******************************************************************
*******************************************************************/
void loop()
{
  takeMeasurement();  // take 31 light-readings from the sensor.
  binarizeMeasurement();  // convert to rawInput to 1s and 0s.
  
  // compare against all gold codes:
  int goldCode = fullCorrelate();
  
  if (goldCode != (-1))  // if a gold code was detected:
  {
    Serial.print("Detected Gold Code: ");
    Serial.println(goldCode);
  }
  
  
  
}



/*******************************************************************
*******************************************************************/
void takeMeasurement()
{
 unsigned long currTime = micros();  // total elapsed microseconds.
 unsigned long nextSampleTime = micros() + delayTime; // time at which we take the next measurement
 
  for (byte i=0;i<numSamples;i++)
  {

  // do nothing until it's time to take the next measurement:
    while ( currTime < nextSampleTime) 
    {
      // update time:
      currTime = micros();
    };
    
    nextSampleTime = nextSampleTime + delayTime;  // increment the time
    rawValues[i] = analogRead(PhotoTransistor);  // 1 is HIGH. 0 is LOW
  } 
}


/*******************************************************************
void binarizeMeasurement()
DESCRIPTION: convert the values in the rawValues array to either 1s
             or 0s, and store them to the cleanValues array.
*******************************************************************/
void binarizeMeasurement()
{
  // compute the average of the rawValues 
  int rawAverage = 0;
  
  for(byte i=0;i<numSamples;i++)
    rawAverage += rawValues[i];
  
  rawAverage /= numSamples;
  
  // compare every rawValue with the average, and assign 1 or 0
  for (byte j=0;j<numSamples;j++)
  {
   if (rawValues[j] > rawAverage)
     cleanValues[j] = 1;
   else
     cleanValues[j] = 0;
  }
}

/*******************************************************************
*******************************************************************/
int fullCorrelate()
{
 int correlation;
 
 for (byte i=0;i<numGoldCodes;i++)
 {
   for (byte j=0;j<numSamples;j++)
   {
     correlation = correlate( cleanValues, GoldCodes[i]);
     if (fabs(correlation) == maxCorrelation)
     //if (fabs(correlation) >= 22)
     {
       Serial.print("shift: ");
       Serial.println(j);
       Serial.print("correlation: ");
       Serial.println(correlation);

       return (i + 1);  // the current gold code 
     }
     // else
     oneRightShift(cleanValues);
   } 
 }
 return -1; // error: no gold codes detected.
}




/*******************************************************************
byte correlate ( boolean* inputArray, boolean* GCode_2)
INPUTS: the names of two 31-element sequences (aka: a gold code or a 
        shifted version of a gold code)
OUTPUT: the dot Product of the two gold codes
*******************************************************************/
int correlate ( boolean* inputArray, const boolean* GCode)
{
  int dotProduct = 0;
  boolean both_equal;
 for (byte i=0;i<numSamples;i++)
 {  
   if (inputArray[i] == GCode[i])
     dotProduct++;
   else
     dotProduct--;
 } 
 return dotProduct;
}



/*******************************************************************
*******************************************************************/
void oneRightShift( boolean* inputArray)
{
 boolean oldLastElement = inputArray[(numSamples-1)];//arg... finding this indexing error took a while.
 
 for (byte i=(numSamples - 1);i>0;i--)
 {
   inputArray[i] = inputArray[i-1];
 } 
 
 inputArray[0] = oldLastElement; 
}


