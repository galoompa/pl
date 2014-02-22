#ifndef _sensor_input_h_
#define _sensor_input_h_

void sensor_input_setup();
void sensor_input_loop();

// sensors are zero-indexed
#define NUM_DISTANCE_SENSORS 1

bool get_distance( unsigned sensor_number, unsigned & value );
long get_lift_encoder_ticks();

void print_sensor_buffer();

#endif
