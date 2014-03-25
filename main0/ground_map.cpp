#include "ground_map.h"
#include "motor.h"
#include "sensor_input.h"
#include "Arduino.h"

void map_sensor_to_index( unsigned sensor, unsigned index );
unsigned mod_pos( int value, int mod );

// resolution defined in cm
#define MAP_RESOLUTION   1.0  // for 1 cm

// in units of MAP_RESOLUTION (i.e. 1 cm)
#define MAP_SIZE         25
#define SEAT_SIZE        9

#define SAFE_MIN_HEIGHT    9
#define SAFE_MAX_HEIGHT    23
#define SAFE_SURFACE_NOISE 3

static float  ground_map_buffer[MAP_SIZE];
static int    last_pos = 0;

void ground_map_setup()
{
  pinMode( 13, OUTPUT );
}

void ground_map_loop()
{
  // check for new position
  int pos = (int)( get_lat_position_cm() / MAP_RESOLUTION );

  if( pos > last_pos ) {
    //map_sensor_to_index( RIGHT_DISTANCE_SENSOR, mod_pos( pos + MAP_SIZE/2, MAP_SIZE ) );
    ground_map_buffer[mod_pos( pos + MAP_SIZE/2, MAP_SIZE )] = get_distance( RIGHT_DISTANCE_SENSOR );
  } else if( pos < last_pos ) {
    ground_map_buffer[mod_pos( pos - MAP_SIZE/2, MAP_SIZE )] = get_distance( LEFT_DISTANCE_SENSOR );
    //map_sensor_to_index( LEFT_DISTANCE_SENSOR, mod_pos( pos - MAP_SIZE/2, MAP_SIZE ) );
  }
  
  // check if in safe zone
  static bool cur_safe = false;
  
  if( last_pos != pos ) {
    bool new_safe = safe_zone();
    if( new_safe != cur_safe ) {
      cur_safe = new_safe;
      if( new_safe ) {
        Serial.println( "Safe" );
        digitalWrite( 13, HIGH );
        set_lat_action( MOTOR_STOP );
      } else {
        Serial.println( "Not safe" );
        digitalWrite( 13, LOW );
      }
    }
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
  
  // find MIN and MAX for the seat area
  for( int i = 1; i < SEAT_SIZE; i++ ) {
    unsigned pos_i = (start_pos + i)%MAP_SIZE;
    if( ground_map_buffer[pos_i] < min_height ) {
      min_height = ground_map_buffer[pos_i];
    } else if( ground_map_buffer[pos_i] < max_height ) {
      max_height = ground_map_buffer[pos_i];
    }
  }
  
  float ave_height = (max_height + min_height) / 2.0;
  
  if( ((max_height - min_height) < SAFE_SURFACE_NOISE) &&
      (ave_height < SAFE_MAX_HEIGHT) &&
      (ave_height > SAFE_MIN_HEIGHT) )
  {
    return true;
  }
  return false;
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
