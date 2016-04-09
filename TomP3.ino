#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>
#include <Bounce2.h> 
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define B_NEXT  A0
#define B_PREV  A1
#define B_PLAY  A2
#define BUTTON_DEBOUNCE_PERIOD 20 //ms
#define SEED_PIN A5
#define I2C_ADDR    0x27
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

SdFat sd;
SdFile file;
SdFile dirFile;
SFEMP3Shield MP3player;
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

Bounce b_Next  = Bounce();
Bounce b_Prev  = Bounce();
Bounce b_Play  = Bounce();

/**
 * Index of the current track playing.
 *
 * Value indicates current playing track, used to populate "x" for playing the 
 * filename of "track00x.mp3" for track000.mp3 through track254.mp3
 */
int8_t current_track = 0;

void setup() {
  Serial.begin(115200);
  lcd.begin (16,2);

  randomSeed(analogRead(SEED_PIN));
  current_track = random(0, 10);

  pinMode(B_NEXT, INPUT_PULLUP);
  pinMode(B_PREV, INPUT_PULLUP);
  pinMode(B_PLAY, INPUT_PULLUP);

  b_Next.attach(B_NEXT);
  b_Next.interval(BUTTON_DEBOUNCE_PERIOD);
  b_Prev.attach(B_PREV);
  b_Prev.interval(BUTTON_DEBOUNCE_PERIOD);
  b_Play.attach(B_PLAY);
  b_Play.interval(BUTTON_DEBOUNCE_PERIOD);

  if(!sd.begin(9, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  }
  
  if (!sd.chdir("/")) {
    sd.errorHalt("sd.chdir");
  } 
  
  MP3player.begin();
  MP3player.setVolume(10,10);
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home ();
  lcd.print("ToMP3");  
  
  Serial.println(F("Listening for button press"));
}

void loop() {
// Below is only needed if not interrupt driven. Safe to remove if not using.
#if defined(USE_MP3_REFILL_MEANS) \
    && ( (USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer) \
    ||   (USE_MP3_REFILL_MEANS == USE_MP3_Polled)      )

  MP3player.available();
#endif

  char title[30];

  if (b_Play.update()) {
    if (b_Play.read() == LOW)	{
      if(MP3player.getState() == playback) {
        Serial.print(F("PAUSE"));
        Serial.println();
        lcd.home();
        lcd.clear();
        lcd.write("Music paused");
        MP3player.pauseMusic();
      } else {
        Serial.print(F("PLAY"));
        Serial.println();
        MP3player.trackTitle((char*)&title);
        Serial.println();
        Serial.write((byte*)&title, 30);
        lcd.home();
        lcd.clear();
        lcd.write("Play");
        MP3player.playTrack(current_track);
        MP3player.resumeMusic();
      }
    }
  }

  if (b_Prev.update()) {
    if (b_Prev.read() == LOW)	{
      if(MP3player.getState() == playback) {
        Serial.print(F("PREVIOUS"));
        Serial.println();
        lcd.home();
        lcd.clear();
        lcd.write("Previous track");
        current_track--;
        MP3player.stopTrack();
        MP3player.playTrack(current_track);
      }
    }
  }

  if (b_Next.update()) {
    if (b_Next.read() == LOW)	{
      Serial.print(F("NEXT"));
      Serial.println();
      lcd.home();
      lcd.clear();
      lcd.write("Next track");
      current_track++;
      MP3player.stopTrack();
      MP3player.playTrack(current_track);
    }
  }
}
