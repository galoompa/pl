#include "sensor_input.h"
#include "Arduino.h"

bool get_distance_sensor_port( unsigned sensor_number, unsigned & port );

#define BUFFER_SIZE 3

static const unsigned          time_between_samples = 500;
static unsigned long           next_sample_time = 0;
static unsigned                next_distance_sensor = 0;
static unsigned                next_distance_buffer = 0;

unsigned                       distance_raw_buffer[NUM_DISTANCE_SENSORS][BUFFER_SIZE];
//unsigned                       distance_raw_buffer[2][2];
void sensor_input_setup()
{
  //distance_raw_buffer = new unsigned;
}

void sensor_input_loop()
{
  unsigned long time = millis();
  if( time > next_sample_time ) {
    next_sample_time = time + time_between_samples;
    unsigned port;
    get_distance_sensor_port( next_distance_sensor, port );
    distance_raw_buffer[next_distance_sensor][next_distance_buffer] =
        analogRead( port );
    
    // next sensor / buffer
    next_distance_sensor++;
    if( next_distance_sensor >= NUM_DISTANCE_SENSORS ) {
      next_distance_sensor = 0;
      next_distance_buffer = (next_distance_buffer + 1) % BUFFER_SIZE;
    }
  }
}

bool read_distance_sensor( unsigned sensor_number, unsigned & value )
{
  if( sensor_number >= NUM_DISTANCE_SENSORS ) {
    return false;
  }
  unsigned long total = 0;
  for( unsigned i = 0; i < BUFFER_SIZE; i++ ) {
    total += distance_raw_buffer[sensor_number][i];
  }
  value = (float)total / BUFFER_SIZE;
  return true;
}

bool get_distance_sensor_port( unsigned sensor_number, unsigned & port )
{
  switch( sensor_number ) {
    case 0:
      port = 0;
      break;
    default:
      return false;
  }
  return true;
}
