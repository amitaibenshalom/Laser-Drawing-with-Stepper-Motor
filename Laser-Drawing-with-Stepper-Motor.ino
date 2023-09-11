#include "Parser.h"

void setup() {
  Serial.begin(BAUDRATE);
  delay (1000);// wait to make sure serial begin 
  Serial.println(F(__FILE__ " " __DATE__ " " __TIME__));
  Serial.println("START");
 
  pinMode(IS_EN_JUMPER,INPUT_PULLUP);// jumper to GND for constant hold/active  motors    
  pinMode(STEPPER_EN_PIN, OUTPUT);// invert !
  pinMode(LED_OUT, OUTPUT);// 
  digitalWrite(LED_OUT, LOW);  //off led
  Blink_Led(START_LED_BLINK_NUMBER,START_LED_BLINK_ON,START_LED_BLINK_OFF);
  digitalWrite(STEPPER_EN_PIN, HIGH);  //disable    
  if (digitalRead(IS_EN_JUMPER)== 0) {digitalWrite(STEPPER_EN_PIN, LOW);}   //enable if jumper set 
  for(uint8_t i=0; i< NUMBER_OF_MOTORS; ++i){
    pinMode(DIR_PIN[i],OUTPUT);
    digitalWrite(DIR_PIN[i],LOW);// initialy set direction to low/0
    pinMode(STEP_PIN[i],OUTPUT);
    digitalWrite(STEP_PIN[i],LOW);// pulse low 
  }
  for (uint8_t i=0; i<NUMBER_OF_MOVES; i++) {
    pinMode(MOVE_PIN[i],INPUT_PULLUP);    
  }
  Homming();
  set_steps(0,0);// set (define) current (steps) position 
  set_position(0, 0);
  update_rates();
  print_help_menu();
  
  curve_index = 0;
  step_index = 0;
  num_of_curves = 0;
}

void loop() {
  
  update_rates();

  if (py_flag && num_of_curves > 0 && Is_destination_done && !drawing_curve) {
    compute_bezier_variable(curve_index);
    set_destination(bezier_point[0],bezier_point[1]);
    print_destination();
//    Serial.println(bezier_point[0]);
//    Serial.println(bezier_point[1]);
    Is_destination_done = false;
    drawing_curve = true;
  }
  if (py_flag && num_of_curves > 0 && Is_destination_done && drawing_curve) {
    if (compute_next_bezier_point()) {
      led_on();
//      Serial.println();
//      Serial.println(bezier_point[0]);
//      Serial.println(bezier_point[1]);
      set_destination(bezier_point[0],bezier_point[1]);
      print_destination();
      Is_destination_done = false;
    }
    else {
      led_off();
      drawing_curve = false;
      curve_index++;
      if (curve_index >= MAX_CURVES) {
        num_of_curves = 0;
        curve_index = 0;
      }
    }   
  }

  if (!Is_destination_done) {
    read_destination();
  }
//  read_pushbuttons();
  if (!py_flag && get_in_command()>0){
    process_in_command();
    in_command ="";
  }

/*  
  if (py_flag && !drawing_curve) {
    num_of_curves = 1;
    curves[0] = 50;
    curves[1] = 50;
    curves[2] = 50;
    curves[3] = 150;
    curves[4] = 100;
    curves[5] = 150;
    curves[6] = 100;
    curves[7] = 50;
    curve_index = 0;
    step_index = 0;
    drawing_curve = false;
  } */

  // to chane !pyflag to pyflag
  if (py_flag && !drawing_curve && Serial.available()) {
    float value;
    Serial.readBytes((char *)&value, sizeof(value)); // Read the float value from serial
//    if (value != 0.00)
//      Serial.println(value);
    if (!start_flag) {
      // if arduino didnt get a starting key, check if it did now
      if (abs(value-starting_key) <= tolerance_float) {
      // arduino got a starting key from the python script
        start_flag = true;
        num_of_curves = 0;
        curve_index = 0;
      }
    }
    else {
      // if already got a starting key, check number of curves (it is negative to distinguish between this and the curve's points)
      if (value < 0 && num_of_curves == 0) {
        num_of_curves = -round(value);
      }
      for (int i = 0; i < points_per_curve * num_of_curves; i++) {
        Serial.readBytes((char *)&value, sizeof(value)); // Read the float value from serial
        curves[i] = value * mm_per_pixel[1-i%2];
        Serial.println(curves[i]);
        if ((i+1) % points_per_curve == 0) {
          Serial.println(next_curve_key);
        }
        delay(TIME_DELAY_ARDUINO);
      }
      Serial.println(end_key);
      Serial.readBytes((char *)&value, sizeof(value)); // Read the float value from serial
      Serial.println(value);
      if (abs(value-end_key) <= tolerance_float) {
        start_flag = false;
        drawing_curve = false;
      }
    }
  }
}
