#include "motor.h"
#include "Arduino.h"
#include "sensor_input.h"
#include "ground_map.h"

#define CHECK_COLLISION_LOOP_MS   100
#define COLLISION_HEIGHT          30  //cm

static bool right_collision = false;
static bool left_collision = false;

static bool override_sling_raised = false;

void interlock_setup()
{
}

void interlock_loop()
{
  //static long print_sensor_buffer_time = 0;
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
    if( right_collision ) {
      /*Serial.println( "Right collision" );
      if( time > print_sensor_buffer_time ) {
        print_sensor_buffer();
        print_sensor_buffer_time = time + 2000;
      }*/
    }
    if( left_collision ) {
      /*Serial.println( "Left collision" );
      if( time > print_sensor_buffer_time ) {
        print_sensor_buffer();
        print_sensor_buffer_time = time + 2000;
      }*/
    }
  }
    
}

bool interlocked_left()
{
  bool moving_up_down = get_lift_action() != MOTOR_STOP;
  bool lift_low = get_lift_position() != LIFT_UP;
  return left_collision || moving_up_down || (lift_low && !override_sling_raised);
}

bool interlocked_right()
{
  bool moving_up_down = get_lift_action() != MOTOR_STOP;
  bool lift_low = get_lift_position() != LIFT_UP;
  return right_collision || moving_up_down || (lift_low && !override_sling_raised);
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

void set_override_sling_raised( bool override )
{
  override_sling_raised = override;
}
