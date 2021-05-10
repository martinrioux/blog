---
layout: post
title: Remote Serial/TCP controller for Arduino
category: Software
tags: [Software, Python, Qt]
---

For the Engineering Games of 2017, I’ve been asked to make their control software. They had to remote control a robot hat threw balls, and wanted it to have a camera that allows to driver to aim. I researched and finally came to the conclusion that a good way to achieve this goal would be to have a laptop for the control and a RaspberryPi + Arduino + PiCamera in the robot. <!-- more --> The RaspberryPi act as a Wi-Fi host, bridge the serial communication from the Arduino to a socket (with tcp_serial_redirect.py) and open a video bridge on another socket. The control laptop can then use video software such as VLC to display the video and the [remote_control.py](./remote_control.py) software is used to transmit keyboard input to the remote Arduino.

The main software I made was [remote_control.py](./remote_control.py), using Qt as user interface. The software can either be used locally with a serial communication to the Arduino, or remotely via a TCP socket. The file can be edited to map keyboard keys to different actions and once connected, display all incoming and outgoing transmission to the Arduino.
The main considerations while making this software was reliability and latency, since it was used in real time for a competition involving speed. After a lot of work, I came with something that works really well.
{: .text-justify}

<center>Screenshots</center>
Serial mode
![serial](/public/img/remote_serial/serial.png)
Socket mode
![socket](/public/img/remote_serial/socket.png)
