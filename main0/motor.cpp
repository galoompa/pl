#include <AFMotor.h>
#include <AccelStepper.h>
#include "motor.h"

static float RAW_LAT_SPEED = 100;        // ticks/sec
static float RAW_LIFT_UP_SPEED = 90;     // of 255 PWM
static float RAW_LIFT_DOWN_SPEED = 20;   // of 255 PWM

AF_DCMotor lift_motor( 1, MOTOR12_64KHZ ); // create motor #1, 64KHz pwm
AF_Stepper lat_motor( 200, 2 );            // 200 steps per rev, port 2 - channel 3&4

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
  if( lat_action != MOTOR_STOP ) {
    lat_stepper.runSpeed();
  }
}

void set_lift_action( motor_direction dir )
{
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

void set_lift_up_speed( float speed )
{
  RAW_LIFT_UP_SPEED = speed;
  // apply speed
  set_lift_action( lift_action );
}

void set_lift_down_speed( float speed )
{
  RAW_LIFT_DOWN_SPEED = speed;
  // apply speed
  set_lift_action( lift_action );
}

void set_lat_action( motor_direction dir )
{
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
