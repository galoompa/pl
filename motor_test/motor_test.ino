#include <AFMotor.h>

AF_DCMotor motor(1, MOTOR12_64KHZ); // create motor #1, 64KHz pwm
AF_Stepper smotor(200, 2);

int inByte = 0;         // incoming serial byte
int motor_speed = 0;

void setup() {
  Serial.begin(9600); // set up Serial library at 9600 bps
  Serial.println("Motor test! 0");
  motor.setSpeed(10); // set the speed to 100/255
  smotor.setSpeed(10); // set speed to 10 RPM
}

void loop() {
  if (Serial.available() > 0) {
    inByte = Serial.read() - 48;
    Serial.print( "received " );
    Serial.print( inByte );
    Serial.println( " ." );

    switch( inByte ) {
    case 0:
      motor.run( RELEASE );
      break;
    case 1:
      while( Serial.available() <= 0 ) {
      }
      motor_speed = (Serial.read() - 48) * 10;
      while( Serial.available() <= 0 ) {
      }
      motor_speed += Serial.read() - 48;
      if( motor_speed > 100 ) motor_speed = 100;
      else if( motor_speed < 0 ) motor_speed = 0;

      Serial.print( "Speed " );
      Serial.print( motor_speed );
      Serial.println( "." );

      motor.setSpeed( motor_speed );
      motor.run( FORWARD );
      break;
    case 2:
      smotor.step(100, FORWARD, MICROSTEP); 
      break;
    default:
      Serial.println( "?" );
      break;
    }
  }
}

