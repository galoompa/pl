#include "sensor_input.h"
#include "Arduino.h"

#define BUFFER_SIZE      67
#define LIFT_ENCODER_A   2
#define LIFT_ENCODER_B   3
#define SENSOR_HEIGHT_CM 84

static const unsigned          time_between_samples = 5; // ms
static unsigned long           next_sample_time = 0;
static unsigned                next_distance_sensor = 0;
static unsigned                next_distance_buffer = 0;

static bool                    last_lift_encoder_a = LOW;
static bool                    last_lift_encoder_b = LOW;
static long                    lift_encoder_ticks = 0;

unsigned                       distance_raw_buffer[NUM_DISTANCE_SENSORS][BUFFER_SIZE];

#define FILTER_SAMPLES      3
#define FILTER_BUFFER_SIZE  10
#define NOISE_LEVEL         3

class DistanceSensorFilter
{
  private:
    unsigned  sample;
    unsigned  filter_buffer[FILTER_BUFFER_SIZE];
    unsigned  sample_index;
    unsigned  filter_index;
  public:
    DistanceSensorFilter() : sample_index(0), filter_index(0) {}
    void process_input( unsigned input )
    {
      if( sample_index == 0 ) {
        sample = input;
        sample_index = 1;
      } else if( abs( sample - input ) < NOISE_LEVEL ) {
        sample_index++;
        if( sample_index >= FILTER_SAMPLES ) {
          filter_buffer[filter_index] = sample;
          filter_index = (filter_index + 1) % FILTER_BUFFER_SIZE;
          sample_index = 0;
        }
      } else {
        sample_index = 1;
        sample = input;
      }
    }
    float get_average()
    {
      unsigned total = 0;
      for( unsigned i = 0; i < FILTER_BUFFER_SIZE; i++ ) {
        total += filter_buffer[i];
      }
      return (float)total / FILTER_BUFFER_SIZE;
    }
    void print_buffer()
    {
      for( unsigned i = 0; i < FILTER_BUFFER_SIZE; i++ ) {
        Serial.print( " " );
        Serial.println( filter_buffer[i] );
      }
    }
};

static DistanceSensorFilter sensor_right;

void sensor_input_setup()
{
}

void sensor_input_loop()
{
  // Distance sensor inputs:
  unsigned long time = millis();
  if( time > next_sample_time ) {
    next_sample_time = time + time_between_samples;
    
    sensor_right.process_input( analogRead( 0 ) );
  }
  
  // Lift encoder input
  bool lift_encoder_a = digitalRead( LIFT_ENCODER_A );
  bool lift_encoder_b = digitalRead( LIFT_ENCODER_B );
  if( last_lift_encoder_a != lift_encoder_a ) {
    last_lift_encoder_a = lift_encoder_a;
    if( lift_encoder_a == lift_encoder_b ) {
      lift_encoder_ticks--;
    } else {
      lift_encoder_ticks++;
    }
  } else if( last_lift_encoder_b != lift_encoder_b ) {
    last_lift_encoder_b = lift_encoder_b;
    if( lift_encoder_b == lift_encoder_a ) {
      lift_encoder_ticks++;
    } else {
      lift_encoder_ticks--;
    }
  }
}

float get_distance( unsigned sensor_number )
{
  switch( sensor_number ) {
    case 0:
      return SENSOR_HEIGHT_CM - 7831*pow( sensor_right.get_average(), -1.04 );
      break;
    default:
      break;
  }
  return 0;
}

long get_lift_encoder_ticks()
{
  return lift_encoder_ticks;
}

void print_sensor_buffer()
{
  sensor_right.print_buffer();
}
