#include "serial.h"
#include "motor.h"
#include "sensor_input.h"
#include "ground_map.h"
#include "Arduino.h"
#include "interlock.h"

bool get_param( unsigned & param, unsigned & len );
void process_command( unsigned command, unsigned param );

static unsigned cycles_per_second = 0;

void serial_setup()
{
  Serial.begin( 9600 ); // set up Serial library at 9600 bps
  Serial.println( "Main test" );
  
  pinMode( 9, INPUT_PULLUP );
  pinMode( 10, INPUT_PULLUP );
}

void serial_loop()
{
  // calculate cycles per second
  unsigned long time = millis();
  static unsigned long check_time = 0;
  static unsigned count = 0;
  count++;
  if( time > check_time ) {
    cycles_per_second = count;
    check_time += 1000;
    count = 0;
  }
  
  // receive commands
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
  
  // buttons
  static bool last_right_button = HIGH;
  static bool last_left_button = HIGH;
  
  bool new_right_button = digitalRead( 10 );
  bool new_left_button = digitalRead( 9 );
  if( new_right_button != last_right_button ) {
    last_right_button = new_right_button;
    //Serial.print( "Right " );
    //Serial.println( new_right_button );
    set_lat_action( MOTOR_RIGHT );

  }
  if( new_left_button != last_left_button ) {
    last_left_button = new_left_button;
    //Serial.print( "Left " );
    //Serial.println( new_left_button );
    set_lat_action( MOTOR_LEFT );
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
    case 1: // lat motor move
      switch( param ) {
        case 0:
          set_lat_action( MOTOR_STOP );
          break;
        case 1:
          set_lat_action( MOTOR_RIGHT );
          break;
        case 2:
          set_lat_action( MOTOR_LEFT );
          break;
        default:
          break;
      }
      break;
    case 2: // lat motor speed
      set_lat_speed( param );
      break;
    case 3: // lift motor auto
      switch( param ) {
        case 0:
          set_lift_action_auto( MOTOR_STOP );
          break;
        case 1:
          set_lift_action_auto( MOTOR_UP );
          break;
        case 2:
          set_lift_action_auto( MOTOR_DOWN );
          break;
        default:
          break;
      }
      break;
    case 4: // lift motor move
      switch( param ) {
        case 0:
          set_lift_action( MOTOR_STOP );
          break;
        case 1:
          set_lift_action( MOTOR_UP );
          break;
        case 2:
          set_lift_action( MOTOR_DOWN );
          break;
        default:
          break;
      }
      break;
    case 5: // lift motor speed up
      set_lift_up_speed( param );
      break;
    case 6: // lift motor speed down
      set_lift_down_speed( param );
      break;
    case 7:
      set_K( param );
      break;
    case 8: // overrides
      switch( param ) {
        case 1:
          set_override_sling_raised( true );
          Serial.println( "Sling override on" );
          break;
        case 2:
          set_override_sling_raised( false );
          Serial.println( "Sling override off" );
          break;
        default:
          break;
      }
      break;
    case 9: // misc debug
      switch( param ) {
        case 1:
        case 2:
          //Serial.print( get_lift_encoder_ticks() );
          //Serial.println( " ticks" );
          Serial.print( "Lift " );
          Serial.print( get_lift_cm() );
          Serial.println( " cm" );
          //break;
        
          //Serial.print( get_lat_position_ticks() );
          //Serial.println( " ticks" );
          Serial.print( "Lat  " );
          Serial.print( get_lat_position_cm() );
          Serial.println( " cm" );
          break;
        case 3:
          print_ground_map();
          break;
        case 4:
          print_sensor_buffer();
          break;
        case 5:
          Serial.print( "Cycles/second: " );
          Serial.println( cycles_per_second );
          break;

        case 9:
          Serial.print( "LEFT  " );
          Serial.println( get_distance( LEFT_DISTANCE_SENSOR ) );
          Serial.print( "RIGHT " );
          Serial.println( get_distance( RIGHT_DISTANCE_SENSOR ) );
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
