#include "motor.h"
#include "Arduino.h"
#include "sensor_input.h"
#include "ground_map.h"

#define CHECK_COLLISION_LOOP_MS   100
#define COLLISION_HEIGHT          25  //cm

static bool right_collision = false;
static bool left_collision = false;

void interlock_setup()
{
}

void interlock_loop()
{
  // check for collisions
  unsigned long time = millis();
  static long next_collision_check = 0;
  if( next_collision_check == 0 ) {
    next_collision_check = time;
  }
  if( time > next_collision_check ) {
    next_collision_check += CHECK_COLLISION_LOOP_MS;
    int lat_direction = get_lat_action();
    right_collision = get_distance( RIGHT_DISTANCE_SENSOR ) > COLLISION_HEIGHT;
    left_collision = get_distance( LEFT_DISTANCE_SENSOR ) > COLLISION_HEIGHT;
  }
    
}

bool interlocked_left()
{
  bool moving_up_down = get_lift_action() != MOTOR_STOP;
  return right_collision || moving_up_down;
}

bool interlocked_right()
{
  bool moving_up_down = get_lift_action() != MOTOR_STOP;
  return right_collision || moving_up_down;
}

bool interlocked_up()
{
  bool moving_left_right = get_lat_action() != MOTOR_STOP;
  return moving_left_right;
}

bool interlocked_down()
{
  bool moving_left_right = get_lat_action() != MOTOR_STOP;
  return moving_left_right || !safe_zone();
}
