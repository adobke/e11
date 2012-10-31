#define leftneg 9
#define leftpos 8
#define leftenable 6

#define rightneg 7 
#define rightpos 12
#define rightenable 11

#define maxCorrThres 21 // threshold for determining if detected sequence is the gold code.
const boolean PT = true; //false pushes yellow
#define looplen 217 // 31 * 7 One big array for all our values
#define LEN 31

int GCLED = 5;
int REFLECT_PIN = A4;    // for gc detection
int PHOTO_PIN = A5;    // for gc detection
const long LOOP_TIME = 250;  // number of microseconds per loop

//int DIST_PIN_FRONT = 0;
//int DIST_PIN_SIDE = 3;
int raw_distance_front = 0;
int raw_distance_side = 0;


int gc1[31] = {
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1};
int gc2[31] = {
  1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,0,1,0,0};
int gc3[31] = {
  0,1,0,0,0,0,1,1,0,1,0,0,0,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0};
int gc4[31] = {
  1,0,1,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,0,1};  
int gc5[31] = {
  0,0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,1,1,0,1,0,0,1,1,1};
int gc6[31] = {
  1,1,1,0,0,0,1,0,0,1,1,0,1,1,1,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0};
int gc7[31] = {
  0,1,1,0,0,1,1,0,1,1,0,1,1,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,0};
int gc8[31] = {
  1,0,0,1,0,1,1,1,0,1,1,0,0,1,1,1,0,1,1,0,0,0,1,0,0,1,0,0,0,1,1}; 


void setup()
{
  pinMode(leftneg, OUTPUT);
  pinMode(leftpos, OUTPUT);
  pinMode(leftenable, OUTPUT);
  pinMode(rightneg, OUTPUT);
  pinMode(rightpos, OUTPUT);
  pinMode(rightenable, OUTPUT);
  pinMode(GCLED,OUTPUT);
  pinMode(PHOTO_PIN,INPUT);
  pinMode(REFLECT_PIN,INPUT);
//  pinMode(DIST_PIN_FRONT, INPUT);
//  pinMode(DIST_PIN_SIDE, INPUT);
  //pinMode(6,OUTPUT);
  pinMode(2,OUTPUT);
  Serial.begin(115200);
  Serial.println("Hi there!!!!");
}

int gcnum=0;

void loop()
{
  int magic = 500;
  int reflect = analogRead(REFLECT_PIN);
  Serial.println(reflect);
  if (reflect > magic + 50)
    go(35,125);
  else if (reflect < magic - 50)
    go(125,35);
  else
    go(125,125);
//  while(true)
//  {
//    //go(-255,255);
//    
//    //raw_distance_front = analogRead(DIST_PIN_FRONT);
//    //raw_distance_side = analogRead(DIST_PIN_SIDE);
//	
//  //  analogWrite(10,150);
//  
//  
     int gcnum = goldcode(); //get the number of the beacon seen.
     Serial.println("gc found: " + gcnum);
//     Serial.println(gcnum);
//     int test = analogRead( PHOTO_PIN );
//     Serial.println(test );
//     delay(10);
   if (gcnum>0)
    {
        digitalWrite(GCLED,HIGH);
        press();
        //delay(10);
    }
    else
    {
        digitalWrite(GCLED,LOW);
        delay(50);
    }
//	delay(50);
//	
///*
//    Serial.println(raw_distance_front);
//    Serial.println("^");
//    Serial.println(raw_distance_side);
//    if (raw_distance_front<450)
//    {
//      digitalWrite(GCLED,HIGH);
//      if (raw_distance_side>500)
//      {
//        digitalWrite(6,HIGH);
//        go(150,255);
//        delay(10);
//      }
//      else if (raw_distance_side>450)
//      {
//        digitalWrite(6,LOW);
//        go(255,255);
//        delay(10);
//      }
//      else
//      {
//        digitalWrite(6,HIGH);
//        go(255,150);
//        delay(10);
//      }
//    }
//    else
//    {
//      digitalWrite(GCLED,LOW);
//      go(-100,255);
//      delay(10);
//    }
//
//
//  if( digitalRead(7) )
//  {
//    go(200,-200);
//  }
//  else
//  {
//   go(-200,200); 
//  }
//*/
//  }




}

void cheer() {
  go(0,0);
}

void press() {
  go(125,0);
  delay(1400);
  go(-125,0);
  delay(1000);  
}

// GOLD CODE STUFF

void wrap_dest(int A[], int ALEN) // Shift an array to the left and wrap around
{
  int a0 = A[ALEN-1];
  for(int i = (ALEN-1); i> 0; i--)
  {
    A[i]=A[i-1];
  }
  A[0]=a0;
}


int correlate( int A[], int B[], int ALEN) // Correlate two codes 31 or -31 is best
{
  int score=0;
  for(int i = 0; i<ALEN; i++)
  {
    if(A[i]==B[i])
    {
      score++; 
    }
    else
    {
      score--; 
    }
  }
  return score;
}

int correlateGoldCodes( int GC1[], int GC2[], int ALEN) //returns the max correlation
{
  int maxcorr = 0;
  for(int i = 0;i<ALEN;i++)
  {
    int corr = correlate(GC1,GC2,ALEN);
    if (abs(corr) > abs(maxcorr)) // We want to detect ~-31 or ~31
    {
      maxcorr=corr; 
    } 
    wrap_dest(GC2,ALEN);
  }
  return maxcorr;
}

void binaryize(int ledInput[]) //Find average value. Change all values in array to
{                              //                    1 if higher, 0 if lower
  long average=0;
  for(int i = 0; i < looplen; i++)
  {
    average += ledInput[i];
  }

  average = average / looplen;
  for(int i = 0; i < looplen; i++)
  {
    if(ledInput[i]<average)
    {
      ledInput[i]=1; 
    }
    else
    {
      ledInput[i]=0; 
    }
  } 
}

void averagize(int ledInput[]) // Takes the 31 by 7 array and average the columns
{
  int average=0;
  for(int i = 0; i < LEN; i++)
  {
    average = 0;
    for(int n = 0; n < 7; n++)
    { 
      average += ledInput[i + 31*n];
    }
    if(average>4)
    {
      ledInput[i]=1;
    } 
    else
    {
      ledInput[i]=0;
    }

  }
}

boolean checkcorr(int ledInput[],int gc[]) //Check if given code is detected
{    
  int maxcorr = correlateGoldCodes(ledInput,gc,31);
  if (abs(maxcorr)>=maxCorrThres)
  {
    if(PT&&(maxcorr>0)) //If maxcorr is positive and above threshold then we see a blue beacon
    {
      return true;
    }
    if(!PT&&(maxcorr<0)) // If corr is negative then we see a yellow beacon
    {
      return true;
    }
    return false;

  }
  else
  {
    // Serial.println(" This is probably not the gold code :("); 
    return false;
  }

}

int goldcode()
{
  long curr_time = 0;  // the current time
  // the next time we want to take some action...
  long next_time = micros() + LOOP_TIME;  
  int ledInput[looplen] = {
  };
  int counter = 0;

  while(true)
  {
    curr_time = micros();     // get current time...
    if (curr_time > next_time) // is it time yet?
    {
      next_time = next_time + LOOP_TIME;
      ledInput[counter++]=analogRead( PHOTO_PIN );
      // Serial.println(ledInput[counter-1]);
    }
    if(counter==216)
      break;
  }
  if(counter==(216))
  {
    // printArray(ledInput, LEN);
    // Serial.println(" AA");
    binaryize(ledInput);
    // printArray(ledInput, LEN);
    averagize(ledInput);
    //  printArray(ledInput, LEN);

    if (checkcorr(ledInput,gc1))
    {
      // Serial.println(" This is probably the gold code. yay :)");
      return 1;
    }

    else if(checkcorr(ledInput,gc2))
    {
      // Serial.println(" This is probably not the gold code :("); 
      return 2;
    }

    else if(checkcorr(ledInput,gc3))
    {
      // Serial.println(" This is probably not the gold code :("); 
      return 3;
    }
    else if(checkcorr(ledInput,gc4))
    {
      // Serial.println(" This is probably not the gold code :("); 
      return 4;
    }
    else if(checkcorr(ledInput,gc5))
    {
      // Serial.println(" This is probably not the gold code :("); 
      return 5;
    }
    else if(checkcorr(ledInput,gc6))
    {
      // Serial.println(" This is probably not the gold code :("); 
      return 6;
    }
    else if(checkcorr(ledInput,gc7))
    {
      // Serial.println(" This is probably not the gold code :("); 
      return 7;
    }
    else if(checkcorr(ledInput,gc8))
    {
      // Serial.println(" This is probably not the gold code :("); 
      return 8;
    }
    else
    {
      return 0; 
    }


  }
}





