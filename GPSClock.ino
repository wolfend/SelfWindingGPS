// GPS clock using Adafruit GPS modules using MTK3329/MTK3339 driver 
//and ST7735 128x128 TFT display. Outputs a 250msec pulse on D2 at the top
//of every hour that is used to synch Self Winding Clock

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

//pins for tft display (pin 11:SDA and pin 13:SCL assumed spi pins)
#define TFT_CS        10
#define TFT_RST        9 
#define TFT_DC        0

#define TIME_PULSE 2     //pin 2 connected to relay
#define TIME_ZONE 3      // pin 3 connected to time zone switch

#define PulseLength   250  //length of pulse sent in ms

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


void setup()
{
  pinMode(TIME_ZONE,INPUT_PULLUP); 
  pinMode(TIME_PULSE, OUTPUT);
  digitalWrite(TIME_PULSE,LOW);
  tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_GREEN);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  
   // 9600 NMEA is the default baud rate for Adafruit MTK GPS's
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
 // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1
  delay(1000); //wait before starting main loop
 }

uint8_t scroll = 4;
uint8_t currentHour = 25;  //init to invalid hour- always emit synch pulse on startup
int displayHour;
uint8_t timezone;

void loop()                     // main loop
{
  char c = GPS.read();
 // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
    if (GPS.hour != currentHour){
      currentHour=GPS.hour;
      digitalWrite(TIME_PULSE, HIGH); //start output synch pulse
      tft.fillScreen(ST77XX_BLACK);
      tft.fillRoundRect(25, 10, 78, 55, 8, ST77XX_RED);
      tft.fillCircle(64, 35, 20, ST77XX_YELLOW);
      tft.drawLine(64, 35, 64, 15, ST77XX_BLACK);
      tft.fillTriangle(64,15,69,20, 59,20, ST77XX_BLACK);
      tft.drawLine(84, 35, 80, 35, ST77XX_BLACK);
      tft.drawLine(44, 35, 48, 35, ST77XX_BLACK);
      tft.drawLine(64, 55, 64, 51, ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.setTextColor(ST77XX_CYAN);
      tft.setTextSize(2);
      tft.print("\n\n\n\n\n :00 Time\nPulse Sent");
      delay(PulseLength);  //output synch pulse
      digitalWrite(TIME_PULSE, LOW); //turn off synch pulse
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      scroll=4;
      tft.setTextColor(ST77XX_GREEN);
      return;
    }
    if (scroll ==0){
            scroll=4;
            tft.fillScreen(ST77XX_BLACK);
            tft.setCursor(0, 0);
    }
    timezone=digitalRead(TIME_ZONE);
    if(timezone)
       displayHour = GPS.hour;
      else{     //MST calcs
        displayHour = GPS.hour-7;
        if(displayHour<0)
          displayHour=displayHour+24;
      }
    
    if (displayHour < 10) { tft.print('0'); }
    tft.print(displayHour, DEC); tft.print(':');
    if (GPS.minute < 10) { tft.print('0'); }
    tft.print(GPS.minute, DEC); tft.print(':');
    if (GPS.seconds < 10) { tft.print('0'); }
    tft.print(GPS.seconds, DEC); 
    tft.setTextSize(1);
    if(timezone){
      tft.setTextColor(ST77XX_YELLOW);
      tft.print(" UTC");
    }
    else{
      tft.setTextColor(ST77XX_CYAN);
      tft.print(" MST");
    }
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_GREEN);
    tft.println("\n");
   
    scroll--;
  }
} //end of main loop
