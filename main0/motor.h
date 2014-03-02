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
void set_lift_up_speed( float speed );
void set_lift_down_speed( float speed );
void set_lift_action_auto( motor_direction );

void set_lat_action( motor_direction );
void set_lat_speed( float speed );        // in ticks/sec (200 ticks/rev)
long get_lat_position_ticks();
float get_lat_position_cm();

void set_K( float new_K );

#endif
