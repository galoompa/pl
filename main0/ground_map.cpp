#include "ground_map.h"
#include "motor.h"
#include "sensor_input.h"
#include "Arduino.h"

void map_to_index( unsigned index );

// resolution defined in ticks
#define MAP_RESOLUTION   265
#define MAP_SIZE         7
#define SEAT_SIZE        3

//static const long MAX_SAMPLE = (long)MAP_RESOLUTION * MAP_SIZE;
//static const long MIN_SAMPLE = -1;

static float ground_map_buffer[NUM_DISTANCE_SENSORS][MAP_SIZE];

static long next_sample_position_large = 0;
static long next_sample_position_small = 0;

void ground_map_setup()
{
}

static long last_pos = 0;
void ground_map_loop()
{
  long pos = get_lat_position_ticks() / MAP_RESOLUTION;
  
  // same position, do nothing
  if( last_pos == pos ) {
    return;
  }
  
  if( pos > last_pos ) {
    map_to_index( (pos + MAP_SIZE/2) % MAP_SIZE );
  } else {
    // no sensors on other size...
  }
  last_pos = pos;

  //static bool inbound = true;

  /*if( ticks > MAX_SAMPLE || ticks < MIN_SAMPLE ) {
    if( inbound ) {
      Serial.println( "oob" );
      inbound = false;
    }
  } else {
    if( !inbound ) {
      Serial.println( "ib" );
      inbound = true;
    }*//*
    if( ticks > next_sample_position_large ) {
      index = ticks / MAP_RESOLUTION;
      map_to_index( index );
      next_sample_position_small = next_sample_position_large;
      next_sample_position_large += MAP_RESOLUTION;
    } else if( ticks < next_sample_position_small ) {
      index = ticks / MAP_RESOLUTION + 1;
      map_to_index( index );
      next_sample_position_large = next_sample_position_small;
      next_sample_position_small -= MAP_RESOLUTION;
    }*/
  //}
}

void map_to_index( unsigned index )
{
  /*Serial.print( "i " );
  Serial.println( index );*/
  for( unsigned sensor = 0; sensor < NUM_DISTANCE_SENSORS; sensor++ ) {
    unsigned value;
    get_distance( sensor, value );
    /*Serial.print( "s " );
    Serial.print( sensor );
    Serial.print( " " );
    Serial.println( value );*/

    ground_map_buffer[sensor][index] = value;
  }
}

bool safe_zone()
{
  unsigned start_pos = (last_pos - SEAT_SIZE/2)%MAP_SIZE;
  float min_height = ground_map_buffer[0][start_pos];
  float max_height = ground_map_buffer[0][start_pos];
  
  for( int i = 1; i < SEAT_SIZE; i++ ) {
    unsigned pos_i = (start_pos + i)%MAP_SIZE;
    for( unsigned sensor = 0; sensor < NUM_DISTANCE_SENSORS; sensor++ ) {
      if( ground_map_buffer[sensor][pos_i] < min_height ) {
        min_height = ground_map_buffer[sensor][pos_i];
      } else if( ground_map_buffer[sensor][pos_i] < max_height ) {
        max_height = ground_map_buffer[sensor][pos_i];
      }
    }
  }
}

void print_ground_map()
{
  Serial.print( "I: " );
  Serial.println( last_pos );
  for( int i = 0; i < MAP_SIZE; i++ ) {
    Serial.print( " " );
    Serial.print( ground_map_buffer[0][i] );
    Serial.print( " " );
    Serial.println( ground_map_buffer[0][(last_pos + i - MAP_SIZE/2)%MAP_SIZE] );
  }
}
