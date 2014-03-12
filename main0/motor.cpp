#include <AFMotor.h>
#include <AccelStepper.h>
#include "motor.h"
#include "sensor_input.h"
#include "interlock.h"

#define LAT_TICKS_PER_CM 85.3

#define LIFT_CONTROL_LOOP_MS       100     // ten times per second
#define LIFT_CONTROL_CM_PER_LOOP   0.2     

static float RAW_LAT_SPEED = LAT_TICKS_PER_CM;        // ticks/sec
static float RAW_LIFT_UP_SPEED = 90;                  // of 255 PWM
static float RAW_LIFT_DOWN_SPEED = 70;                // of 255 PWM

AF_DCMotor lift_motor( 1, MOTOR12_64KHZ ); // create motor #1, 64KHz pwm
AF_Stepper lat_motor( 200, 2 );            // 200 steps per rev, port 2 - channel 3&4

#define LIFT_LOWER_LIMIT  0
#define LIFT_UPPER_LIMIT  15

static bool lift_motor_manual = false;
static float lift_motor_reference = 0;

static float Kp = 20;

void forwardstep() {  
  lat_motor.onestep( FORWARD, INTERLEAVE );
}
void backwardstep() {  
  lat_motor.onestep( BACKWARD, INTERLEAVE );
}
AccelStepper lat_stepper( forwardstep, backwardstep );

motor_direction lift_action;
motor_direction lat_action;

void motor_setup()
{
  lift_motor.setSpeed( RAW_LIFT_DOWN_SPEED );
  lat_stepper.setSpeed( RAW_LAT_SPEED );
  lift_action = MOTOR_STOP;
  lat_action = MOTOR_STOP;
}

void motor_loop()
{
  // lateral motor run
  if( lat_action == MOTOR_RIGHT && interlocked_right() ||
      lat_action == MOTOR_LEFT  && interlocked_left() )
  {
    set_lat_action( MOTOR_STOP );
  }
  if( lat_action != MOTOR_STOP ) {
    lat_stepper.runSpeed();
  }
  
  // lift motor run (if auto)
  // time stuff:
  unsigned long time = millis();
  static long next_sample_time = 0;
  if( next_sample_time == 0 ) {
    next_sample_time = time;
  }
  // control loop
  if( time > next_sample_time ) {
    next_sample_time += LIFT_CONTROL_LOOP_MS;
    if( !lift_motor_manual ) {
      float error;
      switch( lift_action ) {
        case MOTOR_UP:
          if( get_lift_cm() > LIFT_UPPER_LIMIT || interlocked_up() ) {
            lift_action = MOTOR_STOP;
            lift_motor.run( RELEASE );
            break;
          }
          lift_motor.run( FORWARD );
          lift_motor_reference += LIFT_CONTROL_CM_PER_LOOP;
          error = lift_motor_reference - get_lift_cm();
          if( error > 0 ) {
            lift_motor.setSpeed( Kp * error );
          } else {
            lift_motor.setSpeed( 0 );
          }
          break;
        case MOTOR_DOWN:
          if( get_lift_cm() < LIFT_LOWER_LIMIT || interlocked_down() ) {
            lift_action = MOTOR_STOP;
            lift_motor.run( RELEASE );
            break;
          }
          lift_motor.run( BACKWARD );
          lift_motor_reference -= LIFT_CONTROL_CM_PER_LOOP;
          error = lift_motor_reference - get_lift_cm();
          if( error < 0 ) {
            lift_motor.setSpeed( -Kp * error );
          } else {
            lift_motor.setSpeed( 0 );
          }
          break;
        case MOTOR_STOP:
          //lift_motor.run( RELEASE );
          break;
      }
    }
  }
}

void set_lift_action( motor_direction dir )
{
  // override interlock on non-auto
  /*if( interlocked_up() || interlocked_down() ) {
    return;
  }*/
  lift_motor_manual = true;
  lift_action = dir;
  switch( dir ) {
    case MOTOR_UP:
      lift_motor.run( FORWARD );
      lift_motor.setSpeed( RAW_LIFT_UP_SPEED );
      break;
    case MOTOR_DOWN:
      lift_motor.run( BACKWARD );
      lift_motor.setSpeed( RAW_LIFT_DOWN_SPEED );
      break;
    case MOTOR_STOP:
    default:
      lift_motor.run( RELEASE );
      break;
  }
}

int get_lift_action()
{
  return lift_action;
}

void set_lift_action_auto( motor_direction dir )
{
  if( interlocked_up() || interlocked_down() ) {
    return;
  }
  lift_motor_manual = false;
  lift_action = dir;
  lift_motor_reference = get_lift_cm();
  switch( dir ) {
    case MOTOR_UP:
      //lift_motor.run( FORWARD );
      //lift_motor.setSpeed( RAW_LIFT_UP_SPEED );
      break;
    case MOTOR_DOWN:
      //lift_motor.run( BACKWARD );
      //lift_motor.setSpeed( RAW_LIFT_DOWN_SPEED );
      break;
    case MOTOR_STOP:
    default:
      lift_motor.run( RELEASE );
      break;
  }
}

void set_lift_up_speed( float speed )
{
  RAW_LIFT_UP_SPEED = speed;
  // apply speed
  if( lift_motor_manual ) {
    set_lift_action( lift_action );
  }
}

void set_lift_down_speed( float speed )
{
  RAW_LIFT_DOWN_SPEED = speed;
  // apply speed
  if( lift_motor_manual ) {
    set_lift_action( lift_action );
  }
}

void set_lat_action( motor_direction dir )
{
  if( interlocked_left() || interlocked_right() ) {
    return;
  }
  float lat_speed = 0;
  lat_action = dir;
  switch( dir ) {
    case MOTOR_RIGHT:
      lat_speed = RAW_LAT_SPEED;
      break;
    case MOTOR_LEFT:
      lat_speed = -RAW_LAT_SPEED;
      break;
    case MOTOR_STOP:
    default:
      break;
  }
  lat_stepper.setSpeed( lat_speed ); // steps / sec
}

void set_lat_speed( float speed )
{
  RAW_LAT_SPEED = speed;
  set_lat_action( lat_action );
}

long get_lat_position_ticks()
{
  return lat_stepper.currentPosition();
}

float get_lat_position_cm()
{
  return lat_stepper.currentPosition() / LAT_TICKS_PER_CM;
}

void set_K( float new_K )
{
  Kp = new_K;
}

int get_lat_action()
{
  return lat_action;
}
