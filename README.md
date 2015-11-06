trinity
=======

Source code for my autonomous mobile robot that participates in the annual Trinity College Fire Fighting Home Robot Contest.

Results so far:
2013: software was not ready
2014: placed 5th out of 50+ in the senior division
2015: placed 7th out of 30+ in the senior division

This repository contains code that run on the robot itself ("robot" folder) as well as code that runs on a pc for teleoperation / debuggin / testing mode ("pc" folder).


General robot platform in a nutshell (pretty much the same so far over the years):
* Pololu Orangutaon SVP 1284 controller
* 2 gear motors 2/ encoders
* several Sharp IR range sensors for wall following
* several sonars for some situations where the IR is unreliable
* look-down line sensors to detect the entracne into a room
* Bluetooth module for 2-way communication with a PC for remote control and debugging
* microphone for detecting the 3.8Khz sound start signal

Specfic to the 2015 contest:
* water pupmp with nozzle attached to a pan-tilt mechanism - used to extinguish the flame
* omni-directional and directional flame sensors

Software:
* Uses the cocoOS cooperative O/S
* Logic is implemented as several concurrently running finite state machines, plus a PID loop for wall following
* All of the robot code (including the cocoOS) also runs on win32 and talks to a V-REP simulator

https://www.youtube.com/playlist?list=PLKz57Za9efT8gHFnLunfcnyxTgr257nPJ


