#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define PI 3.14159f
#define COLLISION_DIST 5

enum texture {
  CHECK,
  STRIPE_V,
  STRIPE_H,
  STRIPE_D
};

struct vec2 {
  float x, y;
};

struct wall {
  vec2 points[2];
  texture tex;
};

const int lBtn = 4;
const int fBtn = 3;
const int rBtn = 2;
const int rotSpeed = 5;

void setup() {
  Serial.begin(9600);

  pinMode(lBtn, INPUT_PULLUP);
  pinMode(fBtn, INPUT_PULLUP);
  pinMode(rBtn, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }


  
  vec2 p = { 64.0f, 32.0f };
  float pa = 0.0f;

  wall walls[] = {
    {10.0f, 10.0f, 10.0f, 60.0f, CHECK},
    {10.0f, 60.0f, 120.0f, 60.0f, STRIPE_V}, 
    {120.0f, 60.0f, 120.0f, 10.0f, STRIPE_H},
    {120.0f, 10.0f, 10.0f, 10.0f, STRIPE_D}
  };
  
  display.clearDisplay();

  while (true) {
    int l = digitalRead(lBtn);
    int r = digitalRead(rBtn);
    int f = digitalRead(fBtn);
    int run_cast = 0;
    
    if (l == 0) {
      pa = (pa - rotSpeed < 0 ? pa - rotSpeed + 360 : pa - rotSpeed);
      run_cast = 1;
    }
    
    if (r == 0) {
      pa = (pa + rotSpeed >= 360 ? pa + rotSpeed - 360 : pa + rotSpeed);
      run_cast = 1;
    }

    if (f == 0) {
      p = collision_detection(walls, 4, p.x + 2*cos(pa * (PI/180)), p.y + 2*sin(pa * (PI/180)));
      run_cast = 1;
    }

    if (run_cast) {
      raycast(walls, 4, p, pa);
    }
  }
}

void raycast(wall* walls, int num_walls, vec2 p, float pa) {
  
  float dov = 12.0f;
  float fov = 90.0f;

  float x3 = p.x;
  float y3 = p.y;
  float x4;
  float y4;

  display.clearDisplay();
//  for (int w = 0; w < num_walls; w++) {
//    bresenham_line((int)walls[w].points[0].x, (int)walls[w].points[0].y, (int)walls[w].points[1].x, (int)walls[w].points[1].y);
//  }
//  display.display();

  int counter = 0;
  
  // Defines the number of rays
  for (int i = 0; i < 128; i+=2) {

    float angle = (i*(fov/127.0f)) - (fov/2.0f);

    x4 = p.x + dov*cosf((pa + angle) * (PI/180));
    y4 = p.y + dov*sinf((pa + angle) * (PI/180));

    float dist = 1000.0f;
    vec2 pt_final = { NULL, NULL };
    wall cur_wall;
    
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
        float ptDist = sqrt(pow(pt.y-p.y, 2) + pow(pt.x-p.x, 2));
        if (ptDist < dist) {
          dist = ptDist;
          pt_final = pt;
//          cur_wall = walls[w]; 
        }
      }
    }
    
    if (pt_final.x != NULL) {
      int length = 700.0f/dist;
//      drawVLine(
//        i,
//        SCREEN_HEIGHT / dist - length / 2 + SCREEN_HEIGHT / 2,
//        SCREEN_HEIGHT / dist + length / 2 + SCREEN_HEIGHT / 2,
//        6 - int(dist / dov * 6)
//      );
      
//      if (cur_wall.tex == CHECK) {

        int lim = length/2;
        
        counter = counter > lim ? 0 : counter + 1;
        if (counter >= lim/2) {
          vertical_line(i, length, -8, true);
        } else if (counter >= 0) {
          vertical_line(i, length, -8, false);
        }
        
//      } else if (cur_wall.tex == STRIPE_H) {
//        
//      } else if (cur_wall.tex == STRIPE_V) {
//        
//      } else if (cur_wall.tex == STRIPE_D) {
//  
//      }

    }
  }
  display.display();
}

void vertical_line(int x, int half_length, int offset, boolean phase) {

  int lower = SCREEN_HEIGHT/2 - half_length - offset;
  int upper = SCREEN_HEIGHT/2 + half_length - offset;
  
  for (int i = lower; i < upper; i+=2) {

    if (i > SCREEN_HEIGHT) {break;}
    
    if (phase) {
      if (i == lower || (i >= lower + half_length && i <= lower + 3*half_length/2)) {
        i += half_length/2;
      }
    } else {
      if ((i >= lower + half_length/2 && i <= lower + half_length) || (i >= lower + 3*half_length/2 && i <= upper)) {
        i += half_length/2;
      }
    }
    
    display.drawPixel(x, i, WHITE);
  }
  display.drawPixel(x, SCREEN_HEIGHT/2 - half_length - offset, WHITE);
  display.drawPixel(x, SCREEN_HEIGHT/2 + half_length - offset, WHITE);
}

vec2 collision_detection(wall walls[], int num_walls, float px, float py) {

  vec2 nv;
  nv.x = px;
  nv.y = py;
  
  int dir = 0;
  for (int i = 0; i < num_walls; i++) {

    if (abs(nv.x - walls[i].points->x) < 5) {
      dir = nv.x - walls[i].points->x > 0 ? 1 : -1;
      nv.x = walls[i].points->x + dir*COLLISION_DIST;
    }
    if (abs(nv.y - walls[i].points->y) < 5) {
      dir = nv.y - walls[i].points->y > 0 ? 1 : -1;
      nv.y = walls[i].points->y + dir*COLLISION_DIST;
    }
  }

  return nv;
}

void bresenham_line(int x1, int y1, int x2, int y2)
{
    x1 = (x1 >= SCREEN_WIDTH ? SCREEN_WIDTH - 1 : x1);
    x1 = (x1 < 0 ? 0 : x1);
    y1 = (y1 >= SCREEN_HEIGHT ? SCREEN_HEIGHT - 1 : y1);
    y1 = (y1 < 0 ? 0 : y1);
    x2 = (x2 >= SCREEN_WIDTH ? SCREEN_WIDTH - 1 : x2);
    x2 = (x2 < 0 ? 0 : x2);
    y2 = (y2 >= SCREEN_HEIGHT ? SCREEN_HEIGHT - 1 : y2);
    y2 = (y2 < 0 ? 0 : y2);
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
    // Lower octants (increment x)
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
    // Upper octants (increment y)
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