//Codigo oficial 
/**********************************************

Project Name: BLDC controller
Author: Elyos team assfasfas
Date: 2022-03-03
Microcontroller: ESP 32
Description: A BLDC controller version 3 

*************************************************/
 
 
 /*
  Pinout:
  - GPIO 21: Hall A
  - GPIO 19: Hall B
  - GPIO 13: Hall C
  - GPIO 27: PWM de la fase A
  - GPIO 32: PWM de la fase B
  - GPIO 26: PWM de la fase C
  - GPIO 12: PWM de la fase AA
  - GPIO 33: PWM de la fase BB
  - GPIO 26: PWM de la fase CC
  - GPIO 4: Pin del potenciómetro
 */

const int pot_pin = 4;              // Potentiometer pin
const float pwm_min = 0.0;           // Minimum PWM duty cycle
const float pwm_max = 255.0;         // Maximum PWM duty cycle
const float pwm_frequency = 20000;  // PWM frequency in Hz
const int num_poles = 8;              // Number of poles in the motor
const int num_comm_steps = 6;         // Number of commutation steps
const int hall_a_pin = 21;            // Hall sensor A pin
const int hall_b_pin = 19;            // Hall sensor B pin
const int hall_c_pin = 13;            // Hall sensor C pin
const int pwm_pin_c = 26;             // PWM pin for phase A
const int pwm_pin_b = 32;             // PWM pin for phase B
const int pwm_pin_a = 27;             // PWM pin for phase C

//Fases prueba
const int pwm_pin_cc = 25;             // PWM pin for phase A
const int pwm_pin_bb = 33;             // PWM pin for phase B
const int pwm_pin_aa = 12;             // PWM pin for phase C


int AH, AL, BH, BL, CH, CL;
float phase_a_duty, phase_b_duty, phase_c_duty;

/*
  Tabla de conmutación

  Ciclos de trabajo para cada fase en cada paso de conmutación.
  Cada fila de la tabla representa un paso de conmutación, y cada columna representa una fase
  del motor.
*/

//Fases prueba nuestro gate driver
const int commutation_table[num_comm_steps][7] = {
  {0, 0, 2, 1, 0, 1},
  {2, 0, 0, 1, 0, 1},
  {2, 0, 0, 1, 1, 0},
  {0, 2, 0, 1, 1, 0},
  {0, 2, 0, 0, 1, 1},
  {0, 0, 2, 0, 1, 1},
  {0, 0, 0, 0, 0, 0}
};


void setup() {
  //Serial.begin(9600);
  
  pinMode(hall_a_pin, INPUT);
  pinMode(hall_b_pin, INPUT);
  pinMode(hall_c_pin, INPUT);
  pinMode(pwm_pin_a, OUTPUT);
  pinMode(pwm_pin_b, OUTPUT);
  pinMode(pwm_pin_c, OUTPUT);

  //.....Fases prueba
  pinMode(pwm_pin_aa, OUTPUT);
  pinMode(pwm_pin_bb, OUTPUT);
  pinMode(pwm_pin_cc, OUTPUT);

  ledcSetup(0, pwm_frequency, 8);
  ledcSetup(1, pwm_frequency, 8);
  ledcSetup(2, pwm_frequency, 8);
  ledcAttachPin(pwm_pin_a, 0);
  ledcAttachPin(pwm_pin_b, 1);
  ledcAttachPin(pwm_pin_c, 2);
}

/*
  Decteccion de fase por medio de sensores hall y asignacion de paso siguiente del motor
*/

void loop() {
  int hall_a_val = !digitalRead(hall_a_pin);
  int hall_b_val = !digitalRead(hall_b_pin);
  int hall_c_val = !digitalRead(hall_c_pin);


  int hall_state = (hall_a_val << 2) | (hall_b_val << 1) | hall_c_val;
  int commutation_step = 0;
  switch (hall_state) {
    case 0b101:
      commutation_step = 1;
      break;
    case 0b001:
      commutation_step = 2;
      break;
    case 0b011:
      commutation_step = 3;
      break;
    case 0b010:
      commutation_step = 4;
      break;
    case 0b110:
      commutation_step = 5;
      break;
    case 0b100:
      commutation_step = 0;
      break;
    default:
      commutation_step = 6;
    break;
  }

/*
  Mapeo de la señal analogica del potenciometro (acelerador) a un valor dentro de la resolucion de pwm 
  como el valor del duty cycle 

  Rango de lectura para el potenciometro de 0 - 4095 (12 bits)
  Rango de convercion 0 - 255 (pwm_min - pwm_max) 
*/

  int pot_val = analogRead(pot_pin);
  float pwm_duty = map(pot_val, 160, 900, pwm_min, pwm_max);

  if(pot_val < 60){
      commutation_step = 6;
    }

  AH = commutation_table[commutation_step][0];
  AL = commutation_table[commutation_step][3];
  BH = commutation_table[commutation_step][1];
  BL = commutation_table[commutation_step][4];
  CH = commutation_table[commutation_step][2];
  CL = commutation_table[commutation_step][5];

  int phase_a_duty = AH * pwm_duty /2;
  int phase_b_duty = BH * pwm_duty /2;
  int phase_c_duty = CH * pwm_duty /2;
  int phase_aa_duty = AL;
  int phase_bb_duty = BL;
  int phase_cc_duty = CL;


  ledcWrite(0, phase_a_duty);
  ledcWrite(1, phase_b_duty);
  ledcWrite(2, phase_c_duty);

  digitalWrite(pwm_pin_aa, phase_aa_duty);
  digitalWrite(pwm_pin_bb, phase_bb_duty);
  digitalWrite(pwm_pin_cc, phase_cc_duty);


}
