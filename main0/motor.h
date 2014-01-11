#ifndef _motor_h_
#define _motor_h_

void motor_setup();
void motor_loop();

enum motor_direction {
  MOTOR_STOP       = 0,
  MOTOR_UP         = 1,
  MOTOR_LEFT       = 1,
  MOTOR_DOWN       = 2,
  MOTOR_RIGHT      = 2,
};

void set_lift_action( motor_direction );
void set_lat_action( motor_direction );
void set_lat_speed( float speed );

#endif
