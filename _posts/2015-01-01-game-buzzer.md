---
layout: post
title: Game Show Buzzer
category: Games
tags: [Game, Hardware]
---
A friend commisionned me on building him a game show buzzer for him and his friends.

Here is the result:
[![buzzer1](/public/img/buzzer1.jpg)](/public/img/buzzer1.jpg)

<!-- more -->
I used an Arduino Nano for the code, an I2C 7 segment display and the **biggest** buttons with LED that I could find.


<center>The inside of the main box</center>
[![buzzer2](/public/img/buzzer2.jpg)](/public/img/buzzer2.jpg)

The buttons can be connected via the connector on the right.

An IR remote controller was used to allow the "Game Host" to confirm or reject the player's answers and edit the scores should it be needed.

An Adafruit Audio FX Sound Board (not shown here) is used to generate a custom sound effect for each team.

I've included all code below for those who could be interested!

```c++
#include "Adafruit_Soundboard.h"
#define SFX_RST 2
Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial, NULL, SFX_RST);

#define RECV_PIN 9;
#include <IRremote.h>
#define IR_RECEIVER  9
IRrecv irrecv(IR_RECEIVER);
decode_results results;
#define IRR_UP 0xFF629D
#define IRR_MIDDLE 0xFF02FD
#define IRR_DOWN 0xFFA857
#define IRR_LEFT 0xFF22DD
#define IRR_RIGHT 0xFFC23D
#define IRR_STAR 0xFF42BD
#define IRR_1 0xFF6897
#define IRR_4 0xFF30CF

#include "LedControl.h"
LedControl lc=LedControl(12,11,10,2);

#define BUTTON1   3
#define BUTTON1_LED   A4
#define BUTTON2   4
#define BUTTON2_LED   A3
#define BUTTON3   5
#define BUTTON3_LED   A2
#define BUTTON4   6
#define BUTTON4_LED   A1
#define BUTTON5   7
#define BUTTON5_LED   A0

#define ARRAY_SIZE(x) ( sizeof(x)/sizeof(x[0]) )


enum SOUND_LIST {
  SOUND_CLICK,
  SOUND_GOOD,
  SOUND_BAD,
  SOUND_WRONG,
  SOUND_TEAM0,
  SOUND_TEAM1,
  SOUND_TEAM2,
  SOUND_TEAM3,
  SOUND_TEAM4
};
uint8_t team_sound_list[] = {
  SOUND_TEAM0,
  SOUND_TEAM1,
  SOUND_TEAM2,
  SOUND_TEAM3,
  SOUND_TEAM4
};

const uint8_t button_list[][2] = {
  {BUTTON1,BUTTON1_LED},
  {BUTTON2,BUTTON2_LED},
  {BUTTON3,BUTTON3_LED},
  {BUTTON4,BUTTON4_LED},
  {BUTTON5,BUTTON5_LED}
};
int scores[] = {0,0,0,0,0};
struct display_pos  { uint8_t address; uint8_t pos; };
struct display_pos display_pos_list[] = {
  {0, 6}, // TEAM0
  {0, 3}, // TEAM1
  {0, 0}, // TEAM2
  {1, 6}, // TEAM3
  {1, 3}, // TEAM4
};



void update_score()
{
  lc.clearDisplay(0);
  lc.clearDisplay(1);

  for (int i = 0; i < ARRAY_SIZE(display_pos_list); ++i){
    lc.setDigit(display_pos_list[i].address, display_pos_list[i].pos,
        scores[i] % 10, false);
    lc.setDigit(display_pos_list[i].address, display_pos_list[i].pos + 1,
        scores[i] / 10, false);
  }
}

void play_sound(uint8_t sound_id)
{
  sfx.playTrack((uint8_t)sound_id);
//  sound_id = 0;
//  char track[20] = "";
//  sprintf(track, "T%i%iOGG",(sound_id/10)%10, sound_id%10);
////  Serial.println(track);
//        if (! sfx.playTrack(track) ) {
////        Serial.println("Failed to play track?");
//      }
}

void blink_score(int player_id)
{
//    Serial.println(millis() % 1000);
    if (millis() % 1000 >= 500) {
//      Serial.println("Blink score");
      lc.setDigit(display_pos_list[player_id].address,
          display_pos_list[player_id].pos,
          scores[player_id] % 10, false);
      lc.setDigit(display_pos_list[player_id].address,
          display_pos_list[player_id].pos + 1,
          scores[player_id] / 10, false);
    } else {
//      Serial.println("Blink 88");
      lc.setDigit(display_pos_list[player_id].address,
          display_pos_list[player_id].pos,
          scores[player_id] % 10, true);
      lc.setDigit(display_pos_list[player_id].address,
          display_pos_list[player_id].pos + 1,
          scores[player_id] / 10, true);
    }
}

void edit_scores()
{
  uint8_t i = 0;
  while(i<5) {
    lc.setDigit(0,0,'e',false);
    blink_score(i);
    if (irrecv.decode(&results)) {
	switch(results.value) {
		case IRR_UP:
			scores[i]++;
       			play_sound(SOUND_CLICK);
 			update_score();
			break;
		case IRR_MIDDLE:
			i++;
        		play_sound(SOUND_CLICK);
  			update_score();
			break;
		case IRR_DOWN:
			if (scores[i] > 0)
				scores[i]--;
        		play_sound(SOUND_CLICK);
  			update_score();
			break;
	}
    	irrecv.resume(); // Receive the next value
    }
  }
  update_score();
}

void lock_mode(uint8_t player_id)
{
  uint8_t waiting = 1;
  while(waiting){
    blink_score(player_id);
    if (irrecv.decode(&results)) {
//      Serial.println(results.value, HEX);

      switch(results.value) {
      case IRR_UP:
        scores[player_id]++;
        play_sound(SOUND_GOOD);
        waiting = 0;
        break;
      case IRR_MIDDLE:
        play_sound(SOUND_BAD);
        waiting = 0;
        break;
      case IRR_DOWN:
        if (scores[player_id] > 0)
          scores[player_id]--;
        play_sound(SOUND_WRONG);
        waiting = 0;
        break;
      }
      irrecv.resume(); // Receive the next value
    }
  }
  update_score();
}


void setup()
{
  Serial.begin(9600);

  /* Setup Buttons */
  for (int i = 0; i < ARRAY_SIZE(button_list); i++) {
    pinMode(button_list[i][0], INPUT_PULLUP);
    pinMode(button_list[i][1], OUTPUT);
    digitalWrite(button_list[i][1], HIGH);
  }

  /* Setup IR Receiver */
  irrecv.enableIRIn(); // Start the receiver

  /* Setup LedControl */

  delay(2000);
  int devices=lc.getDeviceCount();
  // we have to init all devices in a loop
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,8);
    /* and clear the display */
    lc.clearDisplay(address);
    delay(500);
  }
  update_score();
  sfx.reset();
}

void loop()
{
  if (irrecv.decode(&results)) {
//    Serial.println(results.value, HEX);
    if (results.value == IRR_STAR){
      play_sound(SOUND_CLICK);
      edit_scores();
    } else if (results.value == IRR_1){
      sfx.volUp();
      play_sound(SOUND_CLICK);
    } else if (results.value == IRR_4){
      sfx.volDown();
      play_sound(SOUND_CLICK);
    }
    irrecv.resume(); // Receive the next value
  }

  for (int i = 0; i < ARRAY_SIZE(button_list); ++i){
    if (!digitalRead(button_list[i][0])){
      play_sound(team_sound_list[i]);
      digitalWrite(button_list[i][1], LOW);
      lock_mode(i);
      digitalWrite(button_list[i][1], HIGH);
    }
  }
}
```
{: .scrollable}
