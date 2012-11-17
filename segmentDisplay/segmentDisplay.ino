/*
    15    111
    15   6   2
    15   6   2
    15   6   2
    15    777
    15   5   3
    15   5   3
    15   5   3
    15    444 
*/

// Pin assignments
const int latchPin = 8;
const int clockPin = 12;
const int leftScorePin= 11;
const int rightScorePin= 10;

// Index corrseponds to output number
const char nums[13] = {
  0b01111110,
  0b00110000,
  0b01101101,
  0b01111001,
  0b00110011,
  0b01011011,
  0b01011111,
  0b01110000,
  0b01111111,
  0b01111011,
  0b11111110,
  0b10110000,
  0b11101101,
};

typedef enum dispState {
  preMatch = 'p',
  inMatch = 'i',
  postMatch = 'e'
} dispState;

enum mType {
  state = 't',
  score = 's'
};

// Game state stuff
dispState currState = preMatch;
int greenScore = 5;
int whiteScore = 5;

// Serial stuff
byte buf[8] = { 0 };
int bufIndex = 0;

// Animation stuff
const int animStepTime = 250; // ms
const int maxAnimIndex = 6;
int animIndex = 0;
double animTime;
bool animUpdated = false;

byte preMatchAnim[6] = {
  0b01000000,
  0b00100000,
  0b00010000,
  0b00001000,
  0b00000100,
  0b00000010,
};

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(leftScorePin, OUTPUT);
  Serial.begin(115200);
  animTime = millis();
  Serial.println("STARTING!");
}

void parseBuf() 
{
  switch (buf[0]) {
    case state:
      if (bufIndex != 2)
        return; //ERROR!
      switch(buf[1]) {
        case preMatch:
          currState = preMatch;        
          greenScore = 5;
          whiteScore = 5;
          break;
        case inMatch:
          Serial.println("starting match");
          currState = inMatch;        
          break;
        case postMatch:
          currState = postMatch;        
          break;
      }  
      break;
    case score:
      Serial.println("Update score");
      if (bufIndex != 3)
        return;
      Serial.println("Update score 2");
      greenScore = buf[1];
      whiteScore = buf[2];
      break;
  } 
}


void loop() {
  // Process serial
  if (Serial.available() > 0) {
    int in = Serial.read();
    if (in == 0) {
      parseBuf();
      bufIndex = 0;
    } else
      buf[bufIndex++] = in;

  }

  if ( (millis() - animTime) > animStepTime) {
    animTime = millis();
    animIndex = (animIndex + 1) % maxAnimIndex;
    animUpdated = true;
  }

  //Serial.println(currState);
  // Display!
  switch(currState) {
    case preMatch:
      if (!animUpdated) {
        break;
      }
      digitalWrite(latchPin,LOW);
      shiftOut(rightScorePin,clockPin,LSBFIRST,preMatchAnim[animIndex]);
      shiftOut(leftScorePin,clockPin,LSBFIRST,preMatchAnim[animIndex]);
      digitalWrite(latchPin,HIGH);
      animUpdated = false;
      break;
    case inMatch:
      digitalWrite(latchPin,LOW);
      shiftOut(rightScorePin,clockPin,LSBFIRST,nums[whiteScore]);
      shiftOut(leftScorePin,clockPin,LSBFIRST,nums[greenScore]);
      digitalWrite(latchPin,HIGH);
      break;
    case postMatch:
      break;
  }
}
