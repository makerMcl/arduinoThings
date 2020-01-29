# Bounce Analyzer

This is a jitter analyzator for bouncing of buttons or switches.

Attach a button to test to BUTTON_PIN, press and get its jitter behaviour analyzed.


## Usage Hints
I would like to know the bouncing behaviour the buttons to use. And was curious if I could use my ATmega328P/Arduino Nano for that.

You might use this program to test bouncing behaviour of your buttons, e.g. to optimize parameters for your debouncing algorithm, 
e.g. [OneButton library from mathertel](https://github.com/mathertel/OneButton).

IF YOU USE IT, REMEMBER ADDING MARGINS FOR SECURITY!


Tested with Arduino Nano.

## How it works 
Records bouncing of buttons/switches from the perspective of the microcontroller and prints the result to Serial (57600 Baud).

Button-down and button-up are measured independently of each other since press and release behaviour will be different in most cases.

## Experiences
There are also buttons with no bouncing at all. 
The worst I found was 95ms from a simple push buttons being pressed slow and soft.

## Background
For a more detailed analysis of jitter and debouncing see [http://www.ganssle.com/debouncing.pdf](http://www.ganssle.com/debouncing.pdf)


## TODO
* Test with ESP8266
* Test with ESP-32
