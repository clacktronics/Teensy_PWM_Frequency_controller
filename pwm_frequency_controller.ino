// PWM FREQUENCY CONTROLLER
// ========================

// Written by: Ben Bariwse - CLACKTRONICS.CO.UK
// For Teensy 3.5, written in Arduino 1.8.9

// Desicription: Using the inbuilt PWM perheprials of the Teensy 3.5 to create three PWM
// outputs with independant frequency and PWM controls. The values are controlled by three
// encoders and the values are shown on a 128x64 OLED display.

// Libraries required : encoder (included with Teensy), bounce (included with Teensy), adafruit_SSD1306, adafruit_GFX, SPI, Wire

//******Encoder Setup*****************************

// This optional setting causes Encoder to use more optimized code,
// It must be defined before Encoder.h is included.
#define ENCODER_OPTIMIZE_INTERRUPTS

#include <Encoder.h>

// Change these pin numbers to the pins connected to your encoder.
// Best Performance: both pins have interrupt capability
// Good Performance: only the first pin has interrupt capability
// Low Performance:  neither pin has interrupt capability

// All pins on the teensy 3.5 have interrupt capability - Ben

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

// Teensy 3.5 timers to PWM pins for reference
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
// These store the values of the RGB modes and what the encoder is toggled to
bool redMode, greenMode, blueMode;
long redPWM, greenPWM, bluePWM;
long redFreq, greenFreq, blueFreq;

long positionRed  = 0;
long positionGreen = 0;
long positionBlue = 0;



void setup() {

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  
  Serial.begin(9600);
  Serial.println("PWM controller:");

  // This sets up the PWM resolution, e.g 8 would be range 0 to 255 = 0 to 100% PWM 
  analogWriteResolution(8);
  

  pinMode(RED_PWM, OUTPUT);
  analogWrite(RED_PWM,0);
  pinMode(RED_BT, INPUT_PULLUP);
  
  
  pinMode(GRE_PWM, OUTPUT);
  analogWrite(GRE_PWM,0);
  pinMode(GRE_BT, INPUT_PULLUP);

  pinMode(BLU_PWM, OUTPUT);
  analogWrite(BLU_PWM,0);
  pinMode(BLU_BT, INPUT_PULLUP);

  writeDisplay();

}


// This simply writes current PWM and Frequency values to the screen

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



void loop() {



  if (redbutton.update()) { 
    if (redbutton.fallingEdge()) { 
      writeDisplay();
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
      writeDisplay();
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
      writeDisplay();
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

    if ( redbutton.read() == LOW && redbutton.duration() > 500 ) { 
    knobRed.write(0); 
    redPWM=0; 
    redFreq = 0;    
    analogWriteFrequency(RED_PWM, redFreq); 
    analogWrite(RED_PWM, redPWM); 
    writeDisplay();
    }

    if ( greenbutton.read() == LOW && greenbutton.duration() > 500 ) { 
    knobGreen.write(0); 
    greenPWM=0; 
    greenFreq = 0;    
    analogWriteFrequency(GRE_PWM, greenFreq); 
    analogWrite(GRE_PWM, greenPWM); 
    writeDisplay();
    }

    if ( bluebutton.read() == LOW && bluebutton.duration() > 500 ) { 
    knobBlue.write(0); 
    bluePWM=0; 
    blueFreq = 0;    
    analogWriteFrequency(BLU_PWM, blueFreq); 
    analogWrite(BLU_PWM, bluePWM); 
    writeDisplay();
    }

  
  long newRed, newGreen, newBlue;
  
  if(knobRed.read() >= 0){newRed = knobRed.read()/4;} else { knobRed.write(0); newRed = 0;}
  if(knobGreen.read() >= 0){newGreen = knobGreen.read()/4;} else { knobGreen.write(0); newGreen = 0;}
  if(knobBlue.read() >= 0){newBlue = knobBlue.read()/4;} else { knobBlue.write(0); newBlue = 0;}
  
  if (newRed != positionRed || newGreen != positionGreen || newBlue != positionBlue ) {
    
    if(redMode){redPWM = newRed;} else { redFreq = newRed;}
    
    Serial.print("RED: PWM(");
    Serial.print(redPWM);
    Serial.print(") FREQ(");
    Serial.print(redFreq);
    Serial.print(")  ");
    
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

    writeDisplay();
  }
}
