#include <AFMotor.h>
#include <AccelStepper.h>
#include "serial.h"
#include "motor.h"
#include "sensor_input.h"
#include "ground_map.h"

void setup() {
  serial_setup();
  motor_setup();
  sensor_input_setup();
  ground_map_setup();
}

void loop() {
  serial_loop();
  motor_loop();
  sensor_input_loop();
  ground_map_loop();
}
