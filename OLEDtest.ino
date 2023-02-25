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
#define WALL_OFFSET 0
#define NUM_WALLS 4

// Setup button pins
const int lBtn = 4;
const int fBtn = 3;
const int rBtn = 2;

// Doom logo intro screen, stored in PROGMEM to save global section space
const unsigned char PROGMEM doom_logo [] = {
0x7F, 0xFF, 0xFF, 0xFE, 0x03, 0xFF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 0xC3, 0xFF, 0x80, 0xFF, 0xFE,
0x3F, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xFC, 0x3F, 0xFF, 0xFF, 0xF3, 0xFF, 0x81, 0xFF, 0xFC,
0x1F, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xC1, 0xFF, 0xF8,
0x07, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xC1, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xC3, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xE3, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xE3, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xE7, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xF7, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xF7, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xF7, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x0F, 0xFF, 0x7F, 0xF0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x01, 0xFF, 0xDF, 0xFC, 0x07, 0xFF, 0x7F, 0xE0, 0x3F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x03, 0xFF, 0xDF, 0xFE, 0x07, 0xFF, 0x7F, 0xE0, 0x7F, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0,
0x07, 0xFF, 0x07, 0xFF, 0xDF, 0xFF, 0x07, 0xFF, 0x7F, 0xE0, 0xFF, 0xFB, 0xFF, 0xFE, 0xFF, 0xE0,
0x07, 0xFF, 0x1F, 0xFF, 0xDF, 0xFF, 0xC7, 0xFF, 0x7F, 0xE3, 0xFF, 0xFB, 0xFF, 0xBE, 0xFF, 0xE0,
0x07, 0xFF, 0x3F, 0xFF, 0xDF, 0xFF, 0xE7, 0xFF, 0x7F, 0xE7, 0xFF, 0xFB, 0xFF, 0xBE, 0xFF, 0xE0,
0x07, 0xFF, 0x7F, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xBC, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0x9C, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xF3, 0xFF, 0x9C, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xE7, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xE7, 0xFF, 0x9C, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0xC1, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0x83, 0xFF, 0x98, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0x00, 0xFF, 0x88, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFE, 0x00, 0x3F, 0xFF, 0xFF, 0x7F, 0xFF, 0xFC, 0x00, 0x7F, 0x88, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xFC, 0x00, 0x1F, 0xFF, 0xFF, 0x7F, 0xFF, 0xF8, 0x00, 0x1F, 0x80, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xF0, 0x00, 0x0F, 0xFF, 0xFC, 0x3F, 0xFF, 0xF0, 0x00, 0x0F, 0x80, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0xE0, 0x00, 0x03, 0xFF, 0xF8, 0x1F, 0xFF, 0xC0, 0x00, 0x07, 0x80, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0x80, 0x00, 0x01, 0xFF, 0xF0, 0x0F, 0xFF, 0x80, 0x00, 0x01, 0x80, 0xFF, 0xE0,
0x07, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x7F, 0xC0, 0x03, 0xFF, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xE0,
0x07, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x01, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0,
0x07, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0,
0x07, 0xFF, 0xF0, 0x1F, 0x80, 0x3C, 0x04, 0x01, 0xE0, 0x39, 0x02, 0xFC, 0x00, 0x70, 0xFF, 0xE0,
0x07, 0xFF, 0xE0, 0x1F, 0x80, 0x3E, 0x00, 0x01, 0xF0, 0x09, 0x02, 0xFC, 0x00, 0xF8, 0xFF, 0xE0,
0x07, 0xFF, 0x80, 0x10, 0x00, 0x27, 0x00, 0x01, 0x38, 0x09, 0x02, 0x10, 0x01, 0x98, 0xFF, 0xE0,
0x07, 0xFF, 0x00, 0x10, 0x0E, 0x21, 0x00, 0x79, 0x08, 0xF9, 0x02, 0x30, 0xB9, 0x08, 0xFF, 0xE0,
0x07, 0xFC, 0x00, 0x1F, 0x9F, 0x23, 0x00, 0xF9, 0x19, 0xF9, 0x06, 0x30, 0xF9, 0x08, 0x3F, 0xE0,
0x07, 0xF8, 0x00, 0x1F, 0x91, 0x3E, 0x00, 0xD9, 0xF1, 0x19, 0x06, 0x30, 0xC9, 0x08, 0x1F, 0xE0,
0x07, 0xF0, 0x00, 0x10, 0x31, 0x3C, 0x01, 0x89, 0xE3, 0x19, 0x06, 0x30, 0xC9, 0x08, 0x07, 0xE0,
0x07, 0xC0, 0x00, 0x10, 0x31, 0x27, 0x01, 0x89, 0x39, 0x19, 0x8C, 0x30, 0x89, 0x98, 0x03, 0xE0,
0x07, 0x80, 0x00, 0x10, 0x1F, 0x23, 0x80, 0xF9, 0x1D, 0xF8, 0xF8, 0xFC, 0x8D, 0xF8, 0x01, 0xE0,
0x07, 0x00, 0x00, 0x10, 0x0E, 0x21, 0x80, 0x7D, 0x0C, 0xF8, 0x70, 0xFC, 0x88, 0xF0, 0x00, 0x60,
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Defines the texture that a wall will have
enum texture {
  CHECK,
  STRIPE_V,
  STRIPE_H,
  STRIPE_D
};

// Represents a place in 2D space
struct vec2 {
  float x, y;
};

// Represents a wall as a line between two points with a texture
struct wall {
  vec2 points[2];
  texture tex;
};

// Number of degrees per tick
const int rotSpeed = 5;

void setup() {

  // Initializes hardware
  Serial.begin(9600);
  pinMode(lBtn, INPUT_PULLUP);
  pinMode(fBtn, INPUT_PULLUP);
  pinMode(rBtn, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Runs intro sequence
  display.clearDisplay();
  display.drawBitmap(0, 0, doom_logo,  SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  delay(1000);

  // Initializes player state
  vec2 p = { 64.0f, 32.0f };
  float pa = 0.0f;

  // Initializes walls
  wall walls[] = {
    {10.0f, 10.0f, 10.0f, 60.0f, CHECK},
    {10.0f, 60.0f, 120.0f, 60.0f, CHECK},
    {120.0f, 60.0f, 120.0f, 10.0f, CHECK},
    {120.0f, 10.0f, 10.0f, 10.0f, CHECK}
  };

  display.clearDisplay();

  // Waits for inputs continually (main game loop)
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
      p = collision_detection(walls, p.x + 2*cos(pa * (PI/180)), p.y + 2*sin(pa * (PI/180)));
      run_cast = 1;
    }

    // Runs the raycasting function if any input has been detected
    if (run_cast) {
      raycast(walls, p, pa);
    }
  }
}

// Runs a pseudo-3D raycasting algorithm on the environment around the player
void raycast(wall* walls, vec2 p, float pa) {

  // Defines the camera's depth of view and field of view
  float dov = 12.0f;
  float fov = 90.0f;

  float x3 = p.x;
  float y3 = p.y;
  float x4, y4;

  display.clearDisplay();
//  for (int w = 0; w < num_walls; w++) {
//    bresenham_line((int)walls[w].points[0].x, (int)walls[w].points[0].y, (int)walls[w].points[1].x, (int)walls[w].points[1].y);
//  }
//  display.display();

  // Defines the number of rays
  for (int i = 0; i < 128; i+=2) {

    // Calculates the angle at which the ray is projected
    float angle = (i*(fov/127.0f)) - (fov/2.0f);

    // Projects the endpoint of the ray
    x4 = p.x + dov*cosf((pa + angle) * (PI/180));
    y4 = p.y + dov*sinf((pa + angle) * (PI/180));

    float dist = 1000.0f;
    vec2 pt_final = { NULL, NULL };
    wall cur_wall;
    int cur_edge2pt;

    // Checks if the vector from the camera to the ray's endpoint intersects any walls
    for (int w = 0; w < NUM_WALLS; w++) {

      float x1 = walls[w].points[0].x;
      float y1 = walls[w].points[0].y;
      float x2 = walls[w].points[1].x;
      float y2 = walls[w].points[1].y;

      float denominator = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);

      // vectors do not ever intersect
      if (denominator == 0) {
        continue;
      }

      float t = ((x1-x3)*(y3-y4)-(y1-y3)*(x3-x4))/denominator;
      float u = -((x1-x2)*(y1-y3)-(y1-y2)*(x1-x3))/denominator;

      // Case where the vectors intersect
      if (t > 0 && t < 1 && u > 0) {

        vec2 pt = { x1 + t * (x2 - x1), y1 + t * (y2 - y1) };
        float ptDist = sqrt(pow(pt.y-p.y, 2) + pow(pt.x-p.x, 2));

        // Checks if the intersected wall is the closest to the camera
        if (ptDist < dist) {
          dist = ptDist;
          pt_final = pt;
          cur_wall = walls[w];
          cur_edge2pt = (int) sqrt(pow(pt.x - x1, 2) + pow(pt.y - y1, 2));
        }
      }
    }

    if (pt_final.x != NULL) {

      int length = 700.0f/dist;

      // Draws lines at the edges of walls
      if (cur_edge2pt == 0 || cur_edge2pt == sqrt(pow(cur_wall.points[0].x - cur_wall.points[1].x, 2)
          + pow(cur_wall.points[0].y - cur_wall.points[1].y, 2))) {

        vertical_line(i, length);
      }

      if (cur_wall.tex == CHECK) {

        check_line(i, length, cur_edge2pt%10 < 5 ? true : false);

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

void vertical_line(int x, int half_length) {
  for (int i = 0; i < half_length; i+=2) {
    display.drawPixel(x, SCREEN_HEIGHT/2 + WALL_OFFSET + i, WHITE);
    display.drawPixel(x, SCREEN_HEIGHT/2 + WALL_OFFSET - i, WHITE);
  }
}

void check_line(int x, int half_length, boolean phase) {
  int lower = SCREEN_HEIGHT/2 - half_length + WALL_OFFSET;
  int upper = SCREEN_HEIGHT/2 + half_length + WALL_OFFSET;

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
  display.drawPixel(x, SCREEN_HEIGHT/2 - half_length + WALL_OFFSET, WHITE);
  display.drawPixel(x, SCREEN_HEIGHT/2 + half_length + WALL_OFFSET, WHITE);
}

vec2 collision_detection(wall walls[], float px, float py) {

  vec2 nv;
  nv.x = px;
  nv.y = py;

  int dir = 0;
  for (int i = 0; i < NUM_WALLS; i++) {
    if (abs(walls[i].points[0].x - nv.x) < COLLISION_DIST) {
      dir = nv.x - walls[i].points[0].x > 0 ? 1 : -1;
      nv.x = walls[i].points[0].x + dir*COLLISION_DIST;
    }
    if (abs(walls[i].points[0].y - nv.y) < COLLISION_DIST) {
      dir = nv.y - walls[i].points[0].y > 0 ? 1 : -1;
      nv.y = walls[i].points[0].y + dir*COLLISION_DIST;
    }
  }

  return nv;
}