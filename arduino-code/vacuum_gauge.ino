#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1X15.h>
#include <Button.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)

const int button1_pin = 4;
const int button2_pin = 5;
const int pirani_en_pin = 2; 
const int thrm_en_pin = 3; 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_ADS1115 ads;

Button button1(button1_pin); // Connect your button between pin 2 and GND
Button button2(button2_pin); // Connect your button between pin 3 and GND

bool measurement_active = false;

void setup() {

  pinMode(pirani_en_pin, OUTPUT);
  pinMode(thrm_en_pin, OUTPUT);
  //stop the relays (if they were set to 1)
  digitalWrite(pirani_en_pin, LOW);
  digitalWrite(thrm_en_pin, LOW);

  button1.begin();
	button2.begin();

  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Vacuum gauge v1.0");
  display.display(); 

}

void loop() {
  int16_t ads_0_results = 0; //A0-A1 diff
  int16_t ads_1_results = 0; //A2-A3 diff
  float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

  if (measurement_active){
    ads_0_results = ads.readADC_Differential_0_1();
    ads_1_results = ads.readADC_Differential_2_3();
  }

  //display data
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("GBVAC v1.0");
  display.setCursor(0, 10);
  display.println("Pirani pres.:");
  display.setCursor(80, 10);
  display.println(ads_0_results * multiplier);
  display.setCursor(0, 20);
  display.println("Thermocouple:");
  display.setCursor(80, 20);
  display.println(ads_1_results * multiplier);
  display.setCursor(70, 0);
  if (!measurement_active)
    display.println("[Stopped]");
  else
    display.println("[Meas]");
  display.display(); 

  if (button1.pressed()) {
    measurement_active = !measurement_active;
    if (measurement_active){
      digitalWrite(pirani_en_pin, HIGH);
      digitalWrite(thrm_en_pin, HIGH);
    } else {
      digitalWrite(pirani_en_pin, LOW);
      digitalWrite(thrm_en_pin, LOW);
    }
  }

}
