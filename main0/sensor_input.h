#ifndef _sensor_input_h_
#define _sensor_input_h_

void sensor_input_setup();
void sensor_input_loop();

// sensors are zero-indexed
#define NUM_DISTANCE_SENSORS   2
#define LEFT_DISTANCE_SENSOR   0
#define RIGHT_DISTANCE_SENSOR  1

float get_distance( unsigned sensor_number );
long get_lift_encoder_ticks();
float get_lift_cm();

void print_sensor_buffer();

#endif
