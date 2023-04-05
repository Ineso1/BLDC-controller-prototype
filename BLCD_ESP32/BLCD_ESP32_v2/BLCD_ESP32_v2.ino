/**********************************************

Project Name: BLDC controller
Author: Elyos team assfasfas
Date: 2022-03-03
Microcontroller: ESP 32
Description: A BLDC controller xd

*************************************************/
 
 
 /*
  Pinout:
  - GPIO 12: Hall A
  - GPIO 14: Hall B
  - GPIO 27: Hall C
  - GPIO 26: PWM de la fase A
  - GPIO 25: PWM de la fase B
  - GPIO 33: PWM de la fase C
  - GPIO 32: PWM de la fase AA
  - GPIO 35: PWM de la fase BB
  - GPIO 34: PWM de la fase CC
  - GPIO 13: Pin del potenciómetro
 */

const int pot_pin = 22;              // Potentiometer pin
const float pwm_min = 0.0;           // Minimum PWM duty cycle
const float pwm_max = 255.0;         // Maximum PWM duty cycle
const float pwm_frequency = 1000;  // PWM frequency in Hz
const int num_poles = 8;              // Number of poles in the motor
const int num_comm_steps = 6;         // Number of commutation steps
const int hall_a_pin = 21;            // Hall sensor A pin
const int hall_b_pin = 19;            // Hall sensor B pin
const int hall_c_pin = 18;            // Hall sensor C pin
const int pwm_pin_c = 26;             // PWM pin for phase A
const int pwm_pin_b = 32;             // PWM pin for phase B
const int pwm_pin_a = 34;             // PWM pin for phase C

//Fasess prueba
const int pwm_pin_cc = 25;             // PWM pin for phase A
const int pwm_pin_bb = 33;             // PWM pin for phase B
const int pwm_pin_aa = 35;             // PWM pin for phase C


/*
  Tabla de conmutación

  Ciclos de trabajo para cada fase en cada paso de conmutación.
  Cada fila de la tabla representa un paso de conmutación, y cada columna representa una fase
  del motor.
*/


/*
const int commutation_table[num_comm_steps][3] = {
  {1, 0, 0},
  {1, 0, 1},
  {0, 0, 1},
  {0, 1, 1},
  {0, 1, 0},
  {1, 1, 0}
};
*/


//Fases prueba
const int commutation_table_decode[num_comm_steps][6] = {
  {0, 0, 1, 1, 0, 0},
  {0, 1, 0, 1, 0, 0},
  {0, 1, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 1, 0},
  {0, 0, 1, 0, 1, 0}
};

//Fases prueba nuestro gate driver
const int commutation_table[num_comm_steps][6] = {
  {0, 1, 1, 0, 0, 1},
  {1, 0, 1, 0, 0, 1},
  {1, 0, 1, 1, 0, 0},
  {1, 1, 0, 1, 0, 0},
  {1, 1, 0, 0, 1, 0},
  {0, 1, 1, 0, 1, 0}
  
};


//Alternativa para conmutar fases con valores diferentes por fase de conmutacion
/*
const float commutation_table[num_comm_steps][3] = {
  {1.0, 0.0, 0.5},
  {1.0, 0.0, 0.0},
  {1.0, 0.5, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 1.0, 0.5},
  {0.0, 0.0, 1.0}
};
*/

void setup() {
  Serial.begin(9600);
  
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

  /* version esp32 */
  ledcSetup(0, pwm_frequency, 12);
  ledcSetup(1, pwm_frequency, 12);
  ledcSetup(2, pwm_frequency, 12);
  ledcAttachPin(pwm_pin_a, 0);
  ledcAttachPin(pwm_pin_b, 1);
  ledcAttachPin(pwm_pin_c, 2);

  ledcSetup(3, pwm_frequency, 12);
  ledcSetup(4, pwm_frequency, 12);
  ledcSetup(5, pwm_frequency, 12);
  ledcAttachPin(pwm_pin_aa, 3);
  ledcAttachPin(pwm_pin_bb, 4);
  ledcAttachPin(pwm_pin_cc, 5);
  /*............................*/

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
    case 0b110:
      commutation_step = 0;
      break;
    case 0b010:
      commutation_step = 1;
      break;
    case 0b011:
      commutation_step = 2;
      break;
    case 0b001:
      commutation_step = 3;
      break;
    case 0b101:
      commutation_step = 4;
      break;
    case 0b100:
      commutation_step = 5;
      break;
  }

/*
  Mapeo de la señal analogica del potenciometro (acelerador) a un valor dentro de la resolucion de pwm 
  como el valor del duty cycle 

  Rango de lectura para el potenciometro de 0 - 4095 (12 bits)
  Rango de convercion 0 - 255 (pwm_min - pwm_max) 
*/

  int pot_val = analogRead(pot_pin);
  float pwm_duty = map(pot_val, 0, 4095, pwm_min, pwm_max);
  float phase_a_duty = commutation_table[commutation_step][0] * pwm_duty;
  float phase_b_duty = commutation_table[commutation_step][1] * pwm_duty;
  float phase_c_duty = commutation_table[commutation_step][2] * pwm_duty;

  int phase_aa_duty = commutation_table[commutation_step][3] * pwm_max;
  int phase_bb_duty = commutation_table[commutation_step][4] * pwm_max;
  int phase_cc_duty = commutation_table[commutation_step][5] * pwm_max;


  /*  version esp32 */
  ledcWrite(0, phase_a_duty);
  ledcWrite(1, phase_b_duty);
  ledcWrite(2, phase_c_duty);
  ledcWrite(3, phase_aa_duty);
  ledcWrite(4, phase_bb_duty);
  ledcWrite(5, phase_cc_duty);
  /*............................*/


  /*version arduino*/
  /*
  //HIGH
  analogWrite(pwm_pin_a, phase_a_duty);
  analogWrite(pwm_pin_b, phase_b_duty);
  analogWrite(pwm_pin_c, phase_c_duty);
  //LOW
  digitalWrite(pwm_pin_aa, phase_aa_duty);
  digitalWrite(pwm_pin_bb, phase_bb_duty);
  digitalWrite(pwm_pin_cc, phase_cc_duty);
  /*............................*/



  Serial.print(hall_state, BIN);
  Serial.print("\t---");
  Serial.print(phase_a_duty);
  Serial.print(" - ");
  Serial.print(phase_b_duty);
  Serial.print(" - ");
  Serial.print(phase_c_duty);

  //........Fases prueba
  Serial.print(" ---- ");
  Serial.print(phase_aa_duty);
  Serial.print(" - ");
  Serial.print(phase_bb_duty);
  Serial.print(" - ");
  Serial.println(phase_cc_duty);
  delay(10);


/*
  Serial.print("\t---");
  Serial.print(hall_a_val);
  Serial.print(" - ");
  Serial.print(hall_b_val);
  Serial.print(" - ");
  Serial.print(hall_c_val);
  Serial.print("\t---");
  
  Serial.print(hall_state, BIN);
  Serial.print("\t---");
  Serial.print(commutation_table_decode[commutation_step][0]);
  Serial.print(" - ");
  Serial.print(commutation_table_decode[commutation_step][1]);
  Serial.print(" - ");
  Serial.print(commutation_table_decode[commutation_step][2]);

  //........Fases prueba
  Serial.print(" ---- ");
  Serial.print(commutation_table_decode[commutation_step][3]);
  Serial.print(" - ");
  Serial.print(commutation_table_decode[commutation_step][4]);
  Serial.print(" - ");
  Serial.print(commutation_table_decode[commutation_step][5]);

  Serial.print("\t|||\t");
  Serial.print(phase_a_duty);
  Serial.print(" - ");
  Serial.print(phase_b_duty);
  Serial.print(" - ");
  Serial.print(phase_c_duty);

  //........Fases prueba
  Serial.print(" ---- ");
  Serial.print(phase_aa_duty);
  Serial.print(" - ");
  Serial.print(phase_bb_duty);
  Serial.print(" - ");
  Serial.println(phase_cc_duty);
  delay(10);
  */
}
