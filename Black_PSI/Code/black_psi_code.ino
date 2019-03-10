#define ARDUINO_SAMD_ZERO
#include <FastLED.h>

// TOTAL NUMBER OF LEDS ON A PANEL
#define NUM_LEDS 148

#define PSI_LED_TYPE APA102   // THE LED TYPE
#define LEDCOLORS BGR         // THE COLOR ORDER
#define FPSI_DATA_PIN 10      // FRONT PSI DATA PIN
#define FPSI_CLOCK_PIN 9      // FRONT PSI CLOCK PIN
#define RPSI_DATA_PIN 12      // FRONT PSI DATA PIN
#define RPSI_CLOCK_PIN 11     // FRONT PSI CLOCK PIN
#define ROWCOL 14             // THE NUMBER OF ROWS/COLUMNS IN THE VIRTUAL MATRIX

// Define the sticky pins
#define rsticky 7             // FRONT STICKY PIN
#define fsticky 2             // REAR STICKY PIN
int fsticky_state = 0;        // FRONT STICKY BUTTON STATE
int rsticky_state = 0;        // REAR STICKY BUTTON STATE

// Define the array of leds
CRGB fpsi_leds[NUM_LEDS];
CRGB rpsi_leds[NUM_LEDS];

// Define the front and rear PSI color pallettes
DEFINE_GRADIENT_PALETTE( fpsi_gp ) {
  0, 255,   0,   0,      //red
255,   0,   0, 255 };    //blue

DEFINE_GRADIENT_PALETTE( rpsi_gp ) {
  0,   0, 255,   0,      //green
255, 255, 255,   0 };    //yellow  

CRGBPalette16 fpsiPal = fpsi_gp;
CRGBPalette16 rpsiPal = rpsi_gp;

// Set the maximum brightness.
// ==================================================    WARNING!!!    ==================================================
//      Each LED can draw up to 60mA if using white (255, 255, 255) at full brightness (255).
//      Thus, at full brightness white, there is potential to draw up to 17760mA or 17.76A!!! (296 LEDs * 60mA)
//      
//           It is not recommended to exceed a MAXIMUM BRIGHTNESS of 128 when using the default red/blue & yellow green patterns.
//           Even at 128, the yellow board can get very warm.
//           IF you use different colors, be sure to adjust MAXBRIGHTNESS or you run risk of burning out your boards! 
//           I recommend starting at Brightness 32 (low power) and using an current sensor to find out exact amps as well
//           as a thermistor to determine the temperatures. 
//           You have been warned.
float MAXBRIGHTNESS = 96; 

// DEFINE OUR "MATRIX" LAYOUT
// 999 indicates a "dead"/not used area of the n by n matrix
const int PSI_MATRIX[ROWCOL][ROWCOL] = {

  { 999, 999, 999, 999, 999, 59, 73, 87, 101, 999, 999, 999, 999, 999},
  { 999, 999, 999,  33,  45, 58, 72, 86, 100, 113, 125, 999, 999, 999},
  { 999, 999,  21,  32,  44, 57, 71, 85,  99, 112, 124, 135, 999, 999},
  { 999,  11,  20,  31,  43, 56, 70, 84,  98, 111, 123, 134, 143, 999},
  { 999,  10,  19,  30,  42, 55, 69, 83,  97, 110, 122, 133, 142, 999},
  {   3,   9,  18,  29,  41, 54, 68, 82,  96, 109, 121, 132, 141, 147},
  {   2,   8,  17,  28,  40, 53, 67, 81,  95, 108, 120, 131, 140, 146},
  {   1,   7,  16,  27,  39, 52, 66, 80,  94, 107, 119, 130, 139, 145},
  {   0,   6,  15,  26,  38, 51, 65, 79,  93, 106, 118, 129, 138, 144},
  { 999,   5,  14,  25,  37, 50, 64, 78,  92, 105, 117, 128, 137, 999},
  { 999,   4,  13,  24,  36, 49, 63, 77,  91, 104, 116, 127, 136, 999},
  { 999, 999,  12,  23,  35, 48, 62, 76,  90, 103, 115, 126, 999, 999},
  { 999, 999, 999,  22,  34, 47, 61, 75,  89, 102, 114, 999, 999, 999},
  { 999, 999, 999, 999, 999, 46, 60, 74,  88, 999, 999, 999, 999, 999}
  
};

// Setup our potentiometers
int BrightPotPin = 2;     // Brightness Potentiometer Pin
int BrightPotVal = 0;     // Brightness Potentiometer Value
int FastPotPin = 1;       // Transition Speed Pot Pin
int FastPotVal = 0;       // Transition Speed Value
int SwipeDelay = 0;       // A variable swipe delay value

void setup() {
  // setup the Sticky Pins
  pinMode(fsticky, INPUT);
  pinMode(rsticky, INPUT);
  
  // Setup the LEDS
  LEDS.addLeds<PSI_LED_TYPE, FPSI_DATA_PIN, FPSI_CLOCK_PIN, LEDCOLORS>(fpsi_leds, NUM_LEDS);
  LEDS.addLeds<PSI_LED_TYPE, RPSI_DATA_PIN, RPSI_CLOCK_PIN, LEDCOLORS>(rpsi_leds, NUM_LEDS);
  for (int n = 0; n < NUM_LEDS; n++) {
    fpsi_leds[n] = CRGB::Red;
    rpsi_leds[n] = CRGB::Green;
  }
  FastLED.setBrightness(MAXBRIGHTNESS);
}

void loop() {
  // Get the sticky pin
  fsticky_state = digitalRead(fsticky);
  rsticky_state = digitalRead(rsticky);

  // SWIPE RIGHT (front = Red; rear = Yellow)
  for (int col = (ROWCOL - 1); col > -1; col --) {
    for (int row = (ROWCOL - 1); row > -1; row --) {
      if (PSI_MATRIX[col][row] != 999) {    
        fsticky_state = digitalRead(fsticky);
        rsticky_state = digitalRead(rsticky);
       
        if ((fsticky_state == HIGH) && (col > (ROWCOL / 2)-1)) {
          fpsi_leds[PSI_MATRIX[col][row]] = ColorFromPalette( fpsiPal, 255, 255, NOBLEND);
        } else {
          fpsi_leds[PSI_MATRIX[col][row]] = ColorFromPalette( fpsiPal, 0, 255, NOBLEND);
        }

        if ((rsticky_state == HIGH) && (col > (ROWCOL / 2)-1)) {
          rpsi_leds[PSI_MATRIX[col][row]] = ColorFromPalette( rpsiPal, 255, 255, NOBLEND);
        } else {
          rpsi_leds[PSI_MATRIX[col][row]] = ColorFromPalette( rpsiPal, 0, 255, NOBLEND);
        }
        
        //rpsi_leds[PSI_MATRIX[col][row]] = ColorFromPalette( rpsiPal, 0, 255, NOBLEND);
      }
    }
    BrightPotVal = analogRead(BrightPotPin);
    BrightPotVal = map(BrightPotVal, 0, 1023, 25, MAXBRIGHTNESS);
    FastLED.setBrightness(BrightPotVal);
    LEDS.show();
    FastPotVal = analogRead(FastPotPin);
    FastPotVal = map(FastPotVal, 0, 1023, 5, 75);
    delay(FastPotVal);
  }
  SwipeDelay = random(1,50);
  delay(FastPotVal*SwipeDelay);

  // SWIPE LEFT (front = Blue; rear = green)
  for (int col = 0; col < ROWCOL; col ++) {
    for (int row = 0; row < ROWCOL; row ++) {
      if (PSI_MATRIX[col][row] != 999) {
        fsticky_state = digitalRead(fsticky);
        rsticky_state = digitalRead(rsticky);
        
        if ((fsticky_state == HIGH) && (col < (ROWCOL / 2))) { 
          fpsi_leds[PSI_MATRIX[col][row]] = ColorFromPalette( fpsiPal, 0, 255, NOBLEND);
        } else {
          fpsi_leds[PSI_MATRIX[col][row]] = ColorFromPalette( fpsiPal, 255, 255, NOBLEND);
        }

        if ((rsticky_state == HIGH) && (col < (ROWCOL / 2))) { 
          rpsi_leds[PSI_MATRIX[col][row]] = ColorFromPalette( rpsiPal, 0, 255, NOBLEND);
        } else {
          rpsi_leds[PSI_MATRIX[col][row]] = ColorFromPalette( rpsiPal, 255, 255, NOBLEND);
        }
        
        //rpsi_leds[PSI_MATRIX[col][row]] = ColorFromPalette( rpsiPal, 255, 255, NOBLEND);
      }
    }
    BrightPotVal = analogRead(BrightPotPin);
    BrightPotVal = map(BrightPotVal, 0, 1023, 25, MAXBRIGHTNESS);
    FastLED.setBrightness(BrightPotVal);
    LEDS.show();
    FastPotVal = analogRead(FastPotPin);
    FastPotVal = map(FastPotVal, 0, 1023, 5, 75);
    delay(FastPotVal);
  }
  SwipeDelay = random(1,50);
  delay(FastPotVal*SwipeDelay);
  
}
