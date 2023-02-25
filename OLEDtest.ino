/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define PI 3.14159f
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

struct vec3 {
  float x, y, z;
};

struct vec2 {
  float x, y;
};

struct line {
  vec2 points[2];
};

void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  float px = 64.0f,
        py = 32.0f,
        pa = 0.0f;



  line walls[] = {
    {10.0f, 10.0f, 11.0f, 60.0f},
    {25.0f, 30.0f, 60.0f, 5.0f}, 
    {40.0f, 60.0f, 100.0f, 35.0f},
    {120.0f, 30.0f, 100.0f, 15.0f}
  };
  display.clearDisplay();

  while (true) {
    raycast(walls, 4, px, py, pa);
    pa += 30.0f;
  }
}

void raycast(line* walls, int num_walls, int px, int py, float pa) {

  float fov = 40.0f;
  float dov = 60.0f;

  float x3 = px;
  float y3 = py;
  float x4;
  float y4;

  display.clearDisplay();
  for (int w = 0; w < num_walls; w++) {
    bresenham_line((int)walls[w].points[0].x, (int)walls[w].points[0].y, (int)walls[w].points[1].x, (int)walls[w].points[1].y);
  }
  display.display();
  
  // Defines the number of rays
  for (int i = -45; i < 45; i+= 5) {

    x4 = px + dov*sinf((pa + i) * (PI/180));
    
    y4 = py + dov*cosf((pa + i) * (PI/180));

    float dist = 100.0f;
    vec2 pt_final = { NULL, NULL };
    for (int w = 0; w < num_walls; w++) {
      
      float x1 = walls[w].points[0].x;
      float y1 = walls[w].points[0].y;
      float x2 = walls[w].points[1].x;
      float y2 = walls[w].points[1].y;
  
      float denominator = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);
  
      // Lines do not ever intersect
      if (denominator == 0) {
        continue;
      }
  
      float t = ((x1-x3)*(y3-y4)-(y1-y3)*(x3-x4))/denominator;
      float u = -((x1-x2)*(y1-y3)-(y1-y2)*(x1-x3))/denominator;
  
      if (t > 0 && t < 1 && u > 0) {
        
        vec2 pt = { x1 + t * (x2 - x1), y1 + t * (y2 - y1) };
        float ptDist = sqrt(pow(pt.y-py, 2) + pow(pt.x-px, 2));
        dist = (ptDist < dist ? ptDist : dist);
        pt_final = pt;
      }
    }
    if (pt_final.x != NULL) {
      bresenham_line((int)px, (int)py, (int)pt_final.x, (int)pt_final.y); 
      display.display();
    }
  }
  delay(100);
}

void bresenham_line(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    //# Increments
    int sx = (dx > 0 ? 1 : -1), 
        sy = (dy > 0 ? 1 : -1);

    //# Segment length
    dx = abs(dx); 
    dy = abs(dy); 
    int d = (dx > dy ? dx : dy);

    // Remainder count track pixel offset
    double r = d / 2;

    int x = x1;
    int y = y1;
    // Lower octaves (increment x)
    if(dx > dy) {   
        for(int i = 0; i < d; i++) {   
            display.drawPixel(x, y, WHITE);
            x += sx;
            r += dy;
            if (r >= dx) {
                y += sy; 
                r -= dx;
            }
        }
    // Upper octaves (increment y)
    } else {   
        for(int i = 0; i < d; i++) {    
            display.drawPixel(x, y, WHITE);
            y += sy; 
            r += dx;
            if (r >= dy) {    
                x += sx; 
                r -= dy;
            }
        }
    }
}

void loop() {
}