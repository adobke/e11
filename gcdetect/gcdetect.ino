#define LOOP_TIME 250 
#define LEN 31 
#define PHOTO_PIN A5
#define GOLD_PIN 5
#define CorrelateTreshold 22 
#define NumGoldCodes 8 
const long gc1 = 0b0000000100011011000011001110011;
const long gc2 = 0b1100011111110001000111100010100;
const long gc3 = 0b0100001101000010111111010101110;
const long gc4 = 0b1010000000110111111101000011101;
const long gc5 = 0b0010010010000100000101110100111;
const long gc6 = 0b1110001001101110000001011000000;
const long gc7 = 0b0110011011011101111001101111010;
const long gc8 = 0b0100101110110011101100010010001;//1 maybe a 1 at end instead of 0 at front

const long GC[NumGoldCodes] = { gc1, gc2, gc3, gc4, gc5, gc6,
  gc7, gc8 };
//long GC[NumGoldCodes] = {
//  9274995, 1677233940, 564231854, 1344010781, 306318247, 
//  1899430592, 862909306, 1270067491};  


int correlateBits(long A, long B) 
{ 
  long result = A^B;
  // Count 1s
  // return -count
  // placeholder code
  int count = 0;
  for( int i = 0; i < LEN; ++i)
  {
    count += !((result >> i) & 1);
  }
  
  return count;
}

bool correlateGC( long GC1, long GC2) 
{ 
  for (char i = 0; i<LEN; i++) 
  { 
    int count = correlateBits(GC1,GC2);
    if (count > CorrelateTreshold ) 
      return true; 
    GC2 = ((GC2>>1)&(0x3FFFFFFF))|((GC2&0x1)<<(LEN)); 
  } 
  return false; 
} 

void test() {
  long testGc6 = 0b1110001001101110000001011000011;//00;
  long testGc6Shift = 0b1111100010011011100000010110000;//00;
  
  Serial.print("correlateBits testGc6 with gc6: ");
  Serial.println( correlateBits( testGc6, gc6 ) );
  Serial.print("correlateBits shifttestGc6 with gc6: ");
  Serial.println( correlateBits( testGc6Shift, gc6 ) );
  Serial.print("correlateGC shifttestGc6 with gc6: ");
  Serial.println( correlateGC( testGc6Shift, gc6 ) );
  Serial.print("correlateGC testGc6 with gc6: ");
  Serial.println( correlateGC( testGc6, gc6 ) );
  Serial.print("correlateGC testGc6 with gc5: ");
  Serial.println( correlateGC( testGc6, gc5 ) );
  Serial.print("correlateGC testGc6 with gc4: ");
  Serial.println( correlateGC( testGc6, gc4 ) );

  delay(5000);
}

void setup() 
{ 
  pinMode(PHOTO_PIN, INPUT); 
  pinMode(GOLD_PIN, OUTPUT);
  pinMode(5, OUTPUT); 
  Serial.begin(57600); 
  Serial.println("begin");
  test();
} 

void loop() 
{ 
  long curr_time = micros();  // the current time 
  // the next time we want to take some action... 
  long next_time = curr_time - 1; 
  char counter = 0; 
  int SENSOR_ARRAY[LEN]; 
  // An example of how to loop every LOOP_TIME microseconds 

  long sensorCode = 0;
  while(true) {
    curr_time = micros();  // the current time 
    if (curr_time > next_time) { 
      // add our timer interval to next_time 
      next_time = curr_time + LOOP_TIME; 
      // code for reading from the phototransistor 

      SENSOR_ARRAY[counter++] = analogRead( PHOTO_PIN ); 
      sensorCode = 0;
      if (counter == LEN) { 
        unsigned long avg = 0;  // Too small??
        for (int p = 0; p < LEN; p++) 
          avg = avg + SENSOR_ARRAY[p]; 
        avg = avg/LEN; 
        Serial.println(avg);

        for (int p = 0; p < LEN; p++) { 
          if (SENSOR_ARRAY[p] < avg) 
            sensorCode |= (1 << p);
        } 

        for (int p = 0; p < LEN; ++p)
          Serial.println( (sensorCode >> p) & 1 );

        int beacon = 0; 
        for (char i = 0; i < NumGoldCodes; i++) { 
          if (correlateGC(sensorCode, GC[i])) { 
            //Serial.print("Found: "); 
            //Serial.println( (int)(i+1) );
            beacon = i+1; 
            break;
          } 
        } 

        if (beacon) 
          digitalWrite(5, HIGH); 
        else 
          digitalWrite(5, LOW); 

        Serial.println("=");
        delay(100);
        counter = 0; 
      }       
    } 
  }
} 
