# JVS IO board

This repository contains the source for the JVS I/O board I've built using a Teensy4.1 to run inside my Vewlix cabinet, connected to a Naomi. If this setup doesn't seem too bespoke to you and is something you're interested in replicating, setup the IO->button mapping in `io.cc` to your liking, connect a RS485 transceiver to Serial4, SENSE pin to 41, and you're good to go!

Now with support for JVS' (JVS-dash) and the exA-Arcadia game menu button.

## Building

This project uses [PlatformIO](https://platformio.org/). Run `pio run -t upload` to build the project and upload it to the microcontroller.

## I/O board presentation

Currently the IO board presents as the following:

- 2 player 12 buttons (6 ea)
- 2 coin slots
- 8 analog inputs
- 6 digital outputs

## Quirks

When you control the firmware for your I/O board, you can do some fun things:

- Pressing all the player buttons on one side presses the respective Service button to insert a credit
- Digital joystick inputs also update the analog inputs (emulating jerky movements of an analog joystick). The early levels of Monkey Ball are surprisingly playable!
- The I/O board can present to the NAOMI as many in a chain. It takes a good while for the NAOMI to enumerate the max allowed! As-is in the repo it only presents as one board, but this can be adjusted via `jvs_count` in main.cc. There is currently no difference in functionality implemented for additional virtual boards, so they all report the same capabilities.
