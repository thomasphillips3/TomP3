#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>
#include <Bounce2.h> 

#define B_NEXT  A0
#define B_PREV  A1
#define B_PLAY  A2
#define BUTTON_DEBOUNCE_PERIOD 20 //ms

SdFat sd;
SFEMP3Shield MP3player;
boolean playing = false;

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

  pinMode(B_NEXT, INPUT_PULLUP);
  pinMode(B_PREV, INPUT_PULLUP);
  pinMode(B_PLAY, INPUT_PULLUP);

  b_Next.attach(B_NEXT);
  b_Next.interval(BUTTON_DEBOUNCE_PERIOD);
  b_Prev.attach(B_PREV);
  b_Prev.interval(BUTTON_DEBOUNCE_PERIOD);
  b_Play.attach(B_PLAY);
  b_Play.interval(BUTTON_DEBOUNCE_PERIOD);

  if(!sd.begin(9, SPI_HALF_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");

  MP3player.begin();
  MP3player.setVolume(10,10);
  
  Serial.println(F("Listening for button press"));
}

void loop() {

// Below is only needed if not interrupt driven. Safe to remove if not using.
#if defined(USE_MP3_REFILL_MEANS) \
    && ( (USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer) \
    ||   (USE_MP3_REFILL_MEANS == USE_MP3_Polled)      )

  MP3player.available();
#endif

  if (b_Play.update()) {
    if (b_Play.read() == LOW)	{
      if (playing == false) {
        Serial.print(F("PLAY"));
        Serial.println();
        MP3player.playTrack(current_track);
        playing = true;
      } else if (playing == true) {
        Serial.print(F("PAUSE"));
        Serial.println();
        MP3player.pauseMusic();
        playing = false;
      }
    }
  }

  if (b_Prev.update()) {
    if (b_Prev.read() == LOW)	{
      Serial.print(F("PREVIOUS"));
      Serial.println();
      current_track--;
      MP3player.stopTrack();
      MP3player.playTrack(current_track);
    }
  }

  if (b_Next.update()) {
    if (b_Next.read() == LOW)	{
      Serial.print(F("NEXT));
      Serial.println();
      current_track++;
      MP3player.stopTrack();
      MP3player.playTrack(current_track);
    }
  }
}
