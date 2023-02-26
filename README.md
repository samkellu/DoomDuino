# DoomDuino
This is a basic interpretation of the original Doom 3D, written for the Arduino/ATMega328p, using a SSD1306 128x32px OLED screen.

In order to set this up you will need:
 - 1 x Arduino Uno (or a bare 328p if you know what youre doing)
 - 4 x Buttons
 - SSD1306 128x32px OLED screen
 
The wiring (as I have it setup) is as follows:

 - Left Button    -> Digital In p5
 - Forward Button -> Digital In p4
 - Right Button   -> Digital In p3
 - Shoot Button   -> Digital In p6
 
 - Screen SCL     -> Analogue In p5
 - Screen SDA     -> Analogue In p4
 
![DoomSetup](https://user-images.githubusercontent.com/34088495/221393432-3a28edbb-ca72-4aab-ba64-c33f711a304f.PNG)
 
This project also requires the Adafruit SSD1306 library to run:

  - https://github.com/adafruit/Adafruit_SSD1306
  
  
Currently Implemented:
  
  - Basic character controller
  - Raycasting engine
  - 2D to pseudo-3D conversion engine
  - Tiled wall textures
  - Intro, character and gun textures
  - Score and time GUI
 
