#include <CleO.h>
#include <SPI.h>

#pragma region structures
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 480

struct vec2d_t
{
  vec2d_t() {
    x = 0;
    y = 0;
  }
  vec2d_t(double _x, double _y) {
    x = _x;
    y = _y;
  }

  double x, y;
};

struct ball_t
{
  ball_t() {
    direction = vec2d_t(1,1);
    position  = vec2d_t(1,1);
        size  = vec2d_t(1,1);
    velocity  = 1.0;
    colour    = CORNFLOWER_BLUE;
  }

public:
  vec2d_t direction;
  vec2d_t position;
  vec2d_t size;
  double velocity;
  uint32_t colour;
};

struct paddle_t
{
  paddle_t()
  {
    size = vec2d_t(0,0);
    position  = vec2d_t(0,0);
  }
  paddle_t(vec2d_t pos, vec2d_t sz)
  {
    position  = pos;
    size      = sz;
  }

public:
  vec2d_t position;
  vec2d_t size;
};
#pragma endregion

// Game Variables
ball_t ball;
paddle_t paddle;

int score;

void setup() {
  // Put your setup code here, to run once:
  CleO.begin();
  Serial.begin(9600);

  // Setup paddle position
  paddle.position = vec2d_t(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 10);
  paddle.size = vec2d_t(40, 5);
}

void loop() {
  // Put your main code here, to run repeatedly:
  physics();
  frame();
  control();
}

void frame() {
  // Graphics rendering loop
  CleO.Start();

  // Draw paddle
  CleO.RectangleExt(
    paddle.position.x, paddle.position.y,
   paddle.size.x, paddle.size.y,
    CORNFLOWER_BLUE, TL, 0, 0
    );

  // Draw ball
  CleO.RectangleExt(
    ball.position.x, ball.position.y,
   ball.size.x, ball.size.y,
    ball.colour, TL, 0, 0
    );

  // Format score string and render.
  char s0[10];
  sprintf(s0, "Score: %i", score);
  CleO.StringExt(FONT_TINY, 1, 1, LIGHT_GREEN, TL, 0, 0, s0);

  CleO.Show();
}

void control() {
  // Input control loop

  // Get touch co-ordinates.
  int16_t x, y, dur, tag;
  CleO.TouchCoordinates(x, y, dur, tag);

  // If holding press on touch screen move paddle.
  if(dur > 0)
    paddle.position.x = x;
}

bool check_pos_in_bbox(int x, int y, int bx, int by, int bw, int bh)
{
  if((x >= bx && x <= (bx + bw)) && (y >= by && y <= (y + by)))
    return true;

    return false;
}

void physics()
{  
  // Check if ball is touching paddle.
  if(check_pos_in_bbox(ball.position.x, ball.position.y, paddle.position.x, paddle.position.y, paddle.size.x, paddle.size.y)) {
    // Add random factor to ball direction.
    auto factor = (random(0, 9999) / 10000.0);

    // Inverse ball direction and add random factor.
    ball.direction.x = -(ball.direction.x + factor);
    ball.direction.y = -(ball.direction.y + factor);

    score++;
  }

  ball.colour = CORNFLOWER_BLUE;

  if(ball.direction.x > 1 && ball.direction.y > 1) {
    // Random decay factor
    auto factor = (random(0, 999) / 10000.0);
    //Serial.println(factor);

    // Decay ball direction and change colour to show for debug.
    // Probably best to use an exponential or linear curve here.
    ball.direction.x -= factor;
    ball.direction.y -= factor;
    ball.colour = CORAL3;

    // Debug print direction values for testing.
    Serial.print("Dir: "); Serial.print(ball.direction.x); Serial.print(" "); Serial.print(ball.direction.y);
    Serial.println("");
  }

  // Clamp
  if(ball.direction.x >= 10)
      ball.direction.x = 1;

  if(ball.direction.y >= 10)
      ball.direction.y = 1;

  // Hit edge inverse direction.
  if(ball.position.x >= SCREEN_WIDTH || ball.position.x <= 0)
    ball.direction.x = -ball.direction.x;

  if(ball.position.y >= SCREEN_HEIGHT || ball.position.y <= 0)
    ball.direction.y = -ball.direction.y;

  // Failed to hit ball, score reset.
  if(ball.position.y >= SCREEN_HEIGHT)
    score = 0;

  // Update ball position after applying physics.
  ball.position.x += (ball.direction.x * ball.velocity);
  ball.position.y += (ball.direction.y * ball.velocity);
}
