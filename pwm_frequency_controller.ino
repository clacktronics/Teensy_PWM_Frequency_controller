//******Encoder Setup*****************************

// This optional setting causes Encoder to use more optimized code,
// It must be defined before Encoder.h is included.
#define ENCODER_OPTIMIZE_INTERRUPTS

#include <Encoder.h>

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
//   All pins on the teensy 3.5 have interrupt capability - Ben
Encoder knobRed(5, 6);
Encoder knobGreen(7, 8);
Encoder knobBlue(9, 10);

//******* OLED ***********************************

// Libraries here are for the OLED 

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  29 // D1
#define OLED_CLK   28 // D0
#define OLED_DC    27
#define OLED_CS    26 
#define OLED_RESET 25

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


//******* Pin Setup ******************************


// Pins used for PWM output - they are selected because they are assosiated with certain timers
// You can't have different frequencies if the output pins are on the same timer

// Teensy 3.5 timers
//FTM0  5, 6, 9, 10, 20, 21, 22, 23
//FTM1  3, 4
//FTM2  29, 30
//FTM3  2, 7, 8, 14, 35, 36, 37, 3

#define RED_PWM 4 //FTM1
#define GRE_PWM 30 // FTM2
#define BLU_PWM 2 // FTM3

// Button setting, uses bounce library for simpleness

#include <Bounce.h>
#define RED_BT 23
#define GRE_BT 22
#define BLU_BT 21

Bounce redbutton = Bounce(RED_BT, 10);  // 10 ms debounce
Bounce greenbutton = Bounce(GRE_BT, 10);  // 10 ms debounce
Bounce bluebutton = Bounce(BLU_BT, 10);  // 10 ms debounce


// General global variables used******************

bool redMode, greenMode, blueMode;
long redPWM, greenPWM, bluePWM;
long redFreq, greenFreq, blueFreq;



void setup() {

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  
  Serial.begin(9600);
  Serial.println("PWM controller:");


  analogWriteResolution(8);
  

  pinMode(RED_PWM, OUTPUT);
  analogWrite(RED_PWM,100);
  pinMode(RED_BT, INPUT_PULLUP);
  
  
  pinMode(GRE_PWM, OUTPUT);
  analogWrite(GRE_PWM,100);
  pinMode(GRE_BT, INPUT_PULLUP);

  pinMode(BLU_PWM, OUTPUT);
  analogWrite(BLU_PWM,100);
  pinMode(BLU_BT, INPUT_PULLUP);

  writeDisplay();

}

void writeDisplay() {
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(WHITE);       
  display.setCursor(0,0);            

  if(redMode){display.setTextColor(BLACK, WHITE);}
  display.print(F(" Red PWM: "));
  display.println(redPWM);
  display.setTextColor(WHITE, BLACK);
  if(!redMode){display.setTextColor(BLACK, WHITE);}
  display.print(F(" Red Freqency: "));
  display.println(redFreq);
  display.setTextColor(WHITE, BLACK);

  display.println("---------------------");

  if(greenMode){display.setTextColor(BLACK, WHITE);}
  display.print(F(" Green PWM: "));
  display.println(greenPWM);
  display.setTextColor(WHITE, BLACK);
  if(!greenMode){display.setTextColor(BLACK, WHITE);}
  display.print(F(" Green Freqency: "));
  display.println(greenFreq);
  display.setTextColor(WHITE, BLACK);

  display.println("---------------------");

  if(blueMode){display.setTextColor(BLACK, WHITE);}
  display.print(F(" Blue PWM: "));
  display.println(bluePWM);
  display.setTextColor(WHITE, BLACK);
  if(!blueMode){display.setTextColor(BLACK, WHITE);}
  display.print(F(" Blue Freqency: "));
  display.println(blueFreq);
  display.setTextColor(WHITE, BLACK);
  
  display.display();
}

long positionRed  = -999;
long positionGreen = -999;
long positionBlue = -999;

void loop() {

  if (redbutton.update()) { 
    if (redbutton.fallingEdge()) { 
      redMode = !redMode; 
      if(redMode) {
        Serial.println("Red in PWM mode"); 
        knobRed.write(redPWM*4);
        
      } 
      else {
      Serial.println("Red in FRQ mode");
      knobRed.write(redFreq*4);
      } 
     }
  }

  if (greenbutton.update()) { 
    if (greenbutton.fallingEdge()) { 
      greenMode = !greenMode; 
      if(greenMode) {
        Serial.println("Green in PWM mode"); 
        knobGreen.write(greenPWM*4);
        
      } 
      else {
      Serial.println("Green in FRQ mode");
      knobGreen.write(greenFreq*4);
      } 
     }
  }

    if (bluebutton.update()) { 
    if (bluebutton.fallingEdge()) { 
      blueMode = !blueMode; 
      if(blueMode) {
        Serial.println("Blue in PWM mode"); 
        knobBlue.write(bluePWM*4);
        
      } 
      else {
      Serial.println("Blue in FRQ mode");
      knobBlue.write(blueFreq*4);
      } 
     }
  }

  
  long newRed, newGreen, newBlue;
  
  newRed = max(0,knobRed.read())/4;
  newGreen = max(0,knobGreen.read())/4;
  newBlue = max(0,knobBlue.read())/4;
  
  if (newRed != positionRed || newGreen != positionGreen || newBlue != positionBlue ) {
    
    if(redMode){redPWM = newRed;} else { redFreq = newRed;}
    
    Serial.print("RED: PWM(");
    Serial.print(redPWM);
    Serial.print(") FREQ(");
    Serial.print(redFreq);
    Serial.print(")  ");

    writeDisplay();
    
    analogWriteFrequency(RED_PWM, redFreq); 
    analogWrite(RED_PWM,redPWM);


    if(greenMode){greenPWM = newGreen;} else { greenFreq = newGreen;}
    
    Serial.print("GREEN: PWM(");
    Serial.print(greenPWM);
    Serial.print(") FREQ(");
    Serial.print(greenFreq);
    Serial.print(")  ");
    
    analogWriteFrequency(GRE_PWM, greenFreq); 
    analogWrite(GRE_PWM,greenPWM);


    if(blueMode){bluePWM = newBlue;} else { blueFreq = newBlue;}
    
    Serial.print("BLUE: PWM(");
    Serial.print(bluePWM);
    Serial.print(") FREQ(");
    Serial.print(blueFreq);
    Serial.println(")");
    
    analogWriteFrequency(BLU_PWM, blueFreq); 
    analogWrite(BLU_PWM, bluePWM);
    
    positionRed = newRed;
    positionGreen = newGreen;
    positionBlue = newBlue;
  }
}
