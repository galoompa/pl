#include "serial.h"
#include "motor.h"
#include "sensor_input.h"
#include "Arduino.h"

bool get_param( unsigned & param, unsigned & len );
void process_command( unsigned command, unsigned param );

void serial_setup()
{
  Serial.begin( 9600 ); // set up Serial library at 9600 bps
  Serial.println( "Main test" );
}

void serial_loop()
{
  unsigned command;
  if( Serial.available() > 0 ) {
    delay( 10 );
    unsigned len = Serial.available();
    if( get_param( command, len ) ) {
      unsigned param = 0;
      get_param( param, len );
      process_command( command, param );
    }
    // flush
    while( len > 0 ) {
      Serial.read();
      len--;
    }
  }
}

// Returns true if a parameter is immediately available
bool get_param( unsigned & param, unsigned & len )
{
  param = 0;
  if( len == 0 ) {
    return false;
  }
  char input = Serial.read();
  len--;
  if( input == ' ' ) {
    return false;
  }

  param = (input - 48);

  while( len > 0 ) {
    len--;
    input = Serial.read();
    if( input == ' ' ) {
      return true;
    }
    param = param * 10 + (input - 48);
  }
  return true;
}

void process_command( unsigned command, unsigned param )
{
  Serial.print( "C " );
  Serial.print( command );
  Serial.print( ", " );
  Serial.println( param );
  switch( command ) {
    case 1: // motor move
      switch( param ) {
        case 0:
          set_lat_action( MOTOR_STOP );
          break;
        case 1:
          set_lat_action( MOTOR_LEFT );
          break;
        case 2:
          set_lat_action( MOTOR_RIGHT );
          break;
        default:
          break;
      }
      break;
    case 2: // motor speed
      set_lat_speed( param );
      break;
    case 3:
    { unsigned value;
      read_distance_sensor( 0, value );
      Serial.print( "S1 " );
      Serial.println( value );
    } break;
    default:
      break;
  }
}
