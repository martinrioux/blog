---
layout: post
title: Escape Room
category: 3D Printer
tags: [RepRap, 3D Printer]
---
## Escape Room - Work In Progress

I am currently working with a starting business of "Escape Room" puzzles for which I make all of their electronics and software. They have been in business since a short time (March 2017) and are still all in the process of learning our craft, but we are having a lot of fun doing so!

The two projects I did for them are:
- A [Keypad demo box](./keypad_box) used to bring and show people what are "Escape Room" puzzles.
- [Lost at sea](./lost%20in%20sea) - Their first room that includes electronics.


## Demo Box

This project is a demo box they can bring to events so people can get an idea of what are “Escape Room” puzzles.

It is a simple box with a lock and a keypad on which you must enter the right combination. A piezo buzzer gives the players feedback, as does the red, green and yellow lights. On a successful combination entered, the box open and shows the next clue (or congratulation note).

If a secret combination is entered, the combination can be edited.

The code is available [right here](./demo_box.ino).

### Pictures
![Box 1](./box1.JPG)
![Box 2](./box2.JPG)
![Box 3](./box3.JPG)
![Box 4](./box4.JPG)

## Lost at Sea - Escape Room

In this folder are the different things I made for their room "Lost in Sea", where I worked on all of the electronics and software.
The python script contains a portion of the game is used to send and receive commands to the Arduino. The monitor, which displays information to the players, is powered when needed by the Arduino. Then, the players have to input the right password and username to move on.
It is in fact a room in two episodes. I will be working with them this summer to make the next episode.

The code for the Arduino is available [here](./perdu.ino).
And here is the [code of the game](./perdu_en_mer_ep1.py).

### Picture
Here is the first version of the board. We added a lot more since then!
![First version](./controller_board_v1.jpg)
