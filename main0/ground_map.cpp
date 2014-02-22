#include "ground_map.h"
#include "motor.h"
#include "sensor_input.h"
#include "Arduino.h"

//void map_to_index( unsigned index );
void map_sensor_to_index( unsigned sensor, unsigned index );
unsigned mod_pos( int value, int mod );

// resolution defined in ticks
#define MAP_RESOLUTION   265
#define MAP_SIZE         7
#define SEAT_SIZE        3

static float ground_map_buffer[MAP_SIZE];

static long next_sample_position_large = 0;
static long next_sample_position_small = 0;

void ground_map_setup()
{
}

static int last_pos = 0;
void ground_map_loop()
{
  long pos = get_lat_position_ticks() / MAP_RESOLUTION;
  
  // same position, do nothing
  if( last_pos == pos ) {
    return;
  }
  
  if( pos > last_pos ) {
    map_sensor_to_index( 0, mod_pos( pos + MAP_SIZE/2, MAP_SIZE ) );
  } else {
    // no sensors on other size...
    // the below function will simply zero the values (since sensor 1 does not exist)
    map_sensor_to_index( 1, mod_pos( pos - MAP_SIZE/2, MAP_SIZE ) );
  }
  last_pos = pos;
}

void map_sensor_to_index( unsigned sensor, unsigned index )
{
  ground_map_buffer[index] = get_distance( sensor );
}

bool safe_zone()
{
  unsigned start_pos = mod_pos( last_pos - SEAT_SIZE/2, MAP_SIZE );
  float min_height = ground_map_buffer[start_pos];
  float max_height = ground_map_buffer[start_pos];
  
  for( int i = 1; i < SEAT_SIZE; i++ ) {
    unsigned pos_i = (start_pos + i)%MAP_SIZE;
    if( ground_map_buffer[pos_i] < min_height ) {
      min_height = ground_map_buffer[pos_i];
    } else if( ground_map_buffer[pos_i] < max_height ) {
      max_height = ground_map_buffer[pos_i];
    }
  }
}

void print_ground_map()
{
  Serial.print( "I: " );
  Serial.println( last_pos );
  for( int i = 0; i < MAP_SIZE; i++ ) {
    Serial.print( " " );
    Serial.println( ground_map_buffer[mod_pos( i + last_pos - MAP_SIZE/2, MAP_SIZE )] );
  }
}

unsigned mod_pos( int value, int mod )
{
  value = value % mod;
  if( value < 0 ) {
    return value + mod;
  }
  return value;
}
