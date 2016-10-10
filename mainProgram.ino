/////////////////////////////////////////////////////////////////////////////////////////////
//           Code line follower yang memiliki beberapa case dalam rintangannya             //
//                                                                                         //
//   case 1 : default                                                                      //
//    case ini digunakan untuk minimal menyelesaikan sampai check point 1 pada lintasan    //
//   case 2 : indikator 1                                                                  //
//     case ini digunakan untuk minimal menyelesaikan sampai check point 2 pada lintasan   //
//   case 3 : indikator 2                                                                  //
//     case ini digunakan untuk minimal menyelesaikan sampai check point 3 pada lintasan   //
//   case 4 : indikator 3                                                                  //
//     case ini digunakan untuk minimal menyelesaikan sampai check point 4 pada lintasan   //
//                                                                                         //
//  dibuat oleh ://                                                                        //
//    Calmantara Sumpono Putra - 13215032                                                  // 
//    Muh Fathoni Nurrahman - 13215040                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>
#include <AFMotor.h>

/////////////////////////////////////////////////////////////////////////////////////////////
//                         Initiation for Constanta and Variable                           //
/////////////////////////////////////////////////////////////////////////////////////////////
//terminal shield motor
AF_DCMotor motor_l(1);
AF_DCMotor motor_r(4);

//inisiasi motor dan PID
const float Kp = 1, Ki = 0, Kd = 0; //Trial and Error
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0;
const int initial_motor_speed = 200;//initial speed for motor
int sens[6] = {0, 0, 0, 0, 0, 0};

//Inisiasi indikator case
int pin_Indicator = 36;
int ind_Value = 0; //nilai indikator
int ind[6] = {24, 26, 28, 30, 32, 34};

//inisiasi variabel lain
int pin_Groetzel = 38;
int cal_black ; //calibrating black line
int button_Ind = 0; //inisiasi untuk push button
float speed_l = 0;
float speed_r = 0;
int bacaBagian = 0; // metode pembacaan sensor;
int count = 0; //untuk case yg perlu counter
int counter; // sensor mana yg di pake untuk counter A8 = Kiri || A13 = Kanan

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Deklarasi Procedure Function                                //
/////////////////////////////////////////////////////////////////////////////////////////////
void bacaSensor();
//  untuk input nilai sens[i]
void olahSensor(); 
//  pengolah input sens[i] dan mengupdate nilai error
void indicator_in();
//  pengaturan mode dengan counter oleh pushbutton
void default_program();
//    
void mode_1();
void mode_2();
void mode_3();
void PID();
//  Mode yang digunakan dalam lomba
void motor_speed_default(int f);
//  pengatur gerak motor (PWM) dari PID
void motor_speed1();
void motor_speed2();
void motor_speed3();
//  speed control program 1  

///////////////////////////////////////////////////////////////////////////////////////////
//                                     Main Program                                      //
///////////////////////////////////////////////////////////////////////////////////////////
void setup () 
{
  Serial.begin(9600);
  pinMode(pin_Indicator,INPUT);
  pinMode(pin_Groetzel,OUTPUT);
  
  //pin input analog
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);

  //pin output Indicator
  for (int i = 0; i < 6; i++)
  {
    pinMode(ind[i],OUTPUT);
  }
  
  //inisiasi sensor
  bacaSensor(0);	
}

void loop () 
{
  //Pembacaan Sensor
  Serial.print('sens[0]'+'sens[1]'+'sens[2]'+'sens[3]'+'sens[4]'+'sens[5]');
  
  indicator_in();

}


///////////////////////////////////////////////////////////////////////////////////////////
//                            Algoritma Procedure Function                               //
///////////////////////////////////////////////////////////////////////////////////////////
void bacaSensor(int bagian)
{
  //all sensor is active
  if (bagian == 0)
  {
    sens[0] = analogRead(A8);
    sens[1] = analogRead(A9);
    sens[2] = analogRead(A10);
    sens[3] = analogRead(A11);
    sens[4] = analogRead(A12);
    sens[5] = analogRead(A13);
  }
  //left sensor is active
  if (bagian == 2){
    sens[0] = analogRead(A8);
    sens[1] = analogRead(A9);
    sens[2] = analogRead(A10);
    sens[3] = analogRead(A11);
    sens[4] = 0;
    sens[5] = 0;
  }
  //right sensor is active
  if (bagian == 1){
    sens[0] = 0;
    sens[1] = 0;
    sens[2] = analogRead(A10);
    sens[3] = analogRead(A11);
    sens[4] = analogRead(A12);
    sens[5] = analogRead(A13);
  }
  // untuk pengisian/pembacaan nilai sens[i]
}

void olahSensor(int bagian)
{
  bacaSensor(bagian);
  for(int i = 0; i < 6; i++)
  {
    if(sens[i] < cal_black) 
    {
      sens[i] = 0;
    }
    else 
    {
      sens[i] = 1;
    }
  }
  
  // Pembacaan sensor pada Robot
  if( (sens[0] = 0) && (sens[1] = 0) && (sens[2] = 0) && (sens[3] = 0) && (sens[4] = 0) && (sens[5] = 1) ) 
  { // sensor 6 terkena garis
    error = 5;
  }
  
  if( (sens[0] = 0) && (sens[1] = 0) && (sens[2] = 0) && (sens[3] = 0) && (sens[4] = 1) && (sens[5] = 1) ) 
  { // sensor 6 dan 5 terkena garis
    error = 4;
  }
  
  if( (sens[0] = 0) && (sens[1] = 0) && (sens[2] = 0) && (sens[3] = 0) && (sens[4] = 1) && (sens[5] = 0) ) 
  { // sensor 5 terkena garis
    error = 3;
  }
  
  if( (sens[0] = 0) && (sens[1] = 0) && (sens[2] = 0) && (sens[3] = 1) && (sens[4] = 1) && (sens[5] = 0) ) 
  { // sensor 4 dan 5 terkena garis
    error = 2;
  }
  
  if( (sens[0] = 0) && (sens[1] = 0) && (sens[2] = 0) && (sens[3] = 1) && (sens[4] = 0) && (sens[5] = 0) ) 
  { // sensor 4 terkena garis
    error = 1;
  }
  
  if( (sens[0] = 0) && (sens[1] = 0) && (sens[2] = 1) && (sens[3] = 1) && (sens[4] = 0) && (sens[5] = 0) ) 
  { // keadaan robot normal
    error = 0; 
  }
  
  if( (sens[0] = 0) && (sens[1] = 0) && (sens[2] = 1) && (sens[3] = 0) && (sens[4] = 0) && (sens[5] = 0) ) 
  { // sensor 3 terkena garis
    error = -1;
  }
  
  if( (sens[0] = 0) && (sens[1] = 1) && (sens[2] = 1) && (sens[3] = 0) && (sens[4] = 0) && (sens[5] = 0) ) 
  { // sensor 2 dan 3 terkena garis
    error = -2;
  }
  
  if( (sens[0] = 0) && (sens[1] = 1) && (sens[2] = 0) && (sens[3] = 0) && (sens[4] = 0) && (sens[5] = 0) ) 
  { // sensor 2 terkena garis
    error = -3;
  }
  
  if( (sens[0] = 1) && (sens[1] = 1) && (sens[2] = 0) && (sens[3] = 0) && (sens[4] = 0) && (sens[5] = 0) ) 
  { // sensor 1 dan 2 terkena garis
    error = -4;
  }
  
  if( (sens[0] = 1) && (sens[1] = 0) && (sens[2] = 0) && (sens[3] = 0) && (sens[4] = 0) && (sens[5] = 0) ) 
  { // sensor 1 terkena garis
    error = -5;
  }
}

void default_program(){
  // pengaktifan mode saat menyentuh lintasan T
  olahSensor(bacaBagian);
  if ( (sens[0] == 0) && (sens[5] == 0) )
  {
    if ( (sens[0] == 1) && (sens[5] == 1) )
    {
      count = 0;
      bacaBagian = 1;
      counter = A13;    
      /*motor_l.run(FORWARD);
      motor_l.setSpeed(220);
      motor_r.run(FORWARD);
      motor_r.setSpeed(220);
      delay(100);*/
    }
  }
  if (analogRead(counter) == 0)
  {
    if (analogRead(counter) == 1)
    {
      count++;
    }
  }
  switch (count) 
  {
    case 1 :
      bacaBagian = 2;
      counter = A8;
      
    break;
    case 2 : 
      bacaBagian = 1;
      counter = A13;
    break;  
    case 3 :
      bacaBagian = 0;
      count = 0;
    break;
  }
}
//Prosedur untuk indikator case
void indicator_in()
{
  //indikator button
  button_Ind = digitalRead(pin_Indicator);
  if (button_Ind == HIGH)
  {
    if (button_Ind == LOW)
    {
      ind_Value++;
    }
  }
  
  if (ind_Value == 3)
  {
    ind_Value = 0;
  }

  //default 
  if (ind_Value == 0)
  {
    digitalWrite(ind[0], LOW);
    digitalWrite(ind[1], LOW);
    digitalWrite(ind[2], LOW);
    default_program();
    calculate_PID();  
  }
  //Masuk ke mode 1
  if (ind_Value == 1) 
  {
    digitalWrite(ind[0], HIGH);
    digitalWrite(ind[1], LOW);
    digitalWrite(ind[2], LOW);
    // mode_1();
  }
  //Masuk ke mode 2
  if (ind_Value == 2) 
  {
    digitalWrite(ind[0], HIGH);
    digitalWrite(ind[1], HIGH);
    digitalWrite(ind[2], LOW);
    // mode_2();
  }
  //Masuk ke mode 3
  if (ind_Value == 3) 
  {
    digitalWrite(ind[0], HIGH);
    digitalWrite(ind[1], HIGH);
    digitalWrite(ind[2], HIGH);
    // mode_3();
  }
}

//Prosedur untuk perhitungan PID
void calculate_PID()
{
  P = error;
  I += error;
  D = error - previous_error;

  PID_value = (Kp * P) + (Ki * I) + (Kd * D);
  previous_error = error;
}
//procedure for motor speed
void motor_speed_default(int f)
{
  //inisiasi nilai PID
  if (PID_value > 255)
  {
    PID_value = 255;
  }
  if (PID_value < -255 )
  {
    PID_value = -255;
  }
 
  //speed motor
  speed_l = initial_motor_speed + PID_value;
  speed_r = initial_motor_speed - PID_value;
  
  //keadaan motor
  motor_l.run(FORWARD);
  motor_l.setSpeed(speed_l);
  motor_r.run(FORWARD);
  motor_r.setSpeed(speed_r);
}
