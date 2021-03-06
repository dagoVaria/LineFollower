/////////////////////////////////////////////////////////////////////////////////////////////
//           Code line follower yang memiliki beberapa case dalam rintangannya             //
//                                                                                         //
//   Mode 0 : Kalibrasi                                                                    //
//     Mode ini digunakan untuk melakukan kalibrasi sensor                                 //
//   Mode 1 : Setting Const PID                                                            //
//     Mode ini digunakan untuk minimal menyelesaikan sampai check point 2 pada lintasan   //
//   Mode 2 : CheckPoint I                                                                 //
//     Mode siap untuk Line Following hingga CheckPoint I dari Start                       //
//   Mode 3 : CheckPoint II                                                                //
//     Mode siap untuk Line Following hingga CheckPoint II dari I                          //
//   Mode 4 : CheckPoint III                                                               //
//     Mode siap untuk Line Following hingga CheckPoint III dari II                        //
//                                                                                         //
//  dibuat oleh ://                                                                        //
//    Calmantara Sumpono Putra - 13215032                                                  // 
//    Muh Fathoni Nurrahman - 13215040                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>
#include <AFMotor.h>

/////////////////////////////////////////////////////////////////////////////////////////////
//                         Initiation for Constanta and Variable                           //
//                                                                                         //
//    ::Indikator-Sensor-PID                                                               //
/////////////////////////////////////////////////////////////////////////////////////////////
//terminal shield motor
AF_DCMotor motor_l(1);
AF_DCMotor motor_r(4);

// Inisiasi Bagian Indikator
int pin_Mode = 36;
int val_Mode = 0; //nilai indikator
int pin_Count_Plus = 34;
int pin_Count_Min = 32;
int pin_Groetzel = 38;
int pin_Ind[4] = {24, 26, 28, 30};
int list_Indikator[16];
int data_Indikator[2][2][2][2]= {0};

// Inisiasi Bagian Sensor
int cal_Black ; //calibrating black line
int read_sens[6] = {0, 0, 0, 0, 0, 0};
int val_sens[6] = {0, 0, 0, 0, 0, 0};
int val_Count = 0;
int counter; // sensor mana yg di pake untuk counter A8 = Kiri || A13 = Kanan
char bacaBagian = 'F'; // metode pembacaan sensor;
int ignorance = 1;

// Inisiasi Bagian PID+Motor
float Kp = 1, Ki = 0, Kd = 0; //Trial and Error
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_Error = 0;
const int initial_Motor_Speed = 200;//initial speed for motor
float speed_l = 0;
float speed_r = 0;

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Deklarasi Procedure Function                                //
//                                                                                         //
//   ::Indikator-Sensor-PID                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////
void edit_data_int(int data);     
void edit_data_float(float data);
// untuk mengedit variable menggunakan Pushbuttin (+) (-)
// int -> +1    float -> +0.1
void baca_Indikator();
// pengaturan mode dengan counter oleh pushbutton
void olah_Indikator();
// pengaturan mode dengan switch case
void set_Indikator(int Nomor, int Kondisi);
// pengesetan indikator ke n dengan kondisi 0/1
void view_Indikator();
// mengatur penampilan indikator ke 16 LED
  void indikatorKalibrasiLine();
  void indikatorSettingPID(float x);
  void indikatorJalan();
  // Mode penampilan indikator;

void baca_Sensor(char bagian);
// untuk input nilai read_sens[i]
void olah_Sensor(); 
// pengolah input val_sens[i] dan mengupdate nilai error
void data_error_default(); 
void count(int counter);
// pengaturan metode counting
void olah_Count();
// penentuan kondisi dengan Case untuk variable "count"

void goPID();
// penghitungan pengolahan data untuk mencari PID
void set_Motor();
// kecepatan motor yang diatur dengan PID

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Main Program                                        //
//                                                                                         //
//   ::Indikator-Sensor-PID                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  pinMode(pin_Mode,INPUT);
  pinMode(pin_Count_Plus,INPUT);
  pinMode(pin_Count_Min,INPUT);
  pinMode(pin_Groetzel,OUTPUT);
  digitalWrite(pin_Groetzel,HIGH);
  
  //pin input analog
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);

  //pin output Indikator
  for (int i = 0; i < 4; i++)
  {
    pinMode(pin_Ind[i],OUTPUT);
  }
  
  //inisiasi sensor
  baca_Sensor('F');	
}

void loop () 
{
  //Pembacaan Sensor
  Serial.print('val_sens[0]'+'val_sens[1]'+'val_sens[2]'+'val_sens[3]'+'val_sens[4]'+'val_sens[5]');
  olah_Sensor();
  olah_Count();
  
  goPID();
  set_Motor();

  olah_Indikator();
  view_Indikator();
  
}


/////////////////////////////////////////////////////////////////////////////////////////////
//                             Algoritma Procedure Function                                //
//                                                                                         //
//   ::Indikator-Sensor-PID                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////
void edit_data_int(int data)
{
  if (digitalRead(pin_Count_Plus) == 0){
    if (digitalRead(pin_Count_Plus) == 1){
      data++;
    }
  }if (digitalRead(pin_Count_Min) == 0){
    if (digitalRead(pin_Count_Min) == 1){
      data--;
    }
  }
}
void edit_data_float(float data)
{
  if (digitalRead(pin_Count_Plus) == 0){
    if (digitalRead(pin_Count_Plus) == 1){
      data += 0.1;
    }
  }if (digitalRead(pin_Count_Min) == 0){
    if (digitalRead(pin_Count_Min) == 1){
      data -= 0.1;
    }
  }
}
void baca_Indikator()  //  pengaturan mode dengan counter oleh pushbutton
{
  if (digitalRead(pin_Mode) == 0){
    if (digitalRead(pin_Mode) == 1){
      val_Mode++;
      if (val_Mode == 3) { 
        val_Mode = 0;
      }
    }
  }
}
void olah_Indikator()  // pengaturan mode dengan switch case
{
  float ubah = 0.0;
  baca_Indikator();
  switch (val_Mode){
    case 0:  // Mode Kalibrasi
      indikatorKalibrasiLine();
      edit_data_int(cal_Black);
    break;
    case 1:
      indikatorSettingPID(ubah);
      edit_data_float(ubah);
      Kp += ubah;
    break;
    case 2:
      indikatorJalan();
      edit_data_int(val_Count);
    break;
  }
}
void set_Indikator(int Nomor, int Kondisi) // pengesetan indikator ke n dengan kondisi 0/1
{
  list_Indikator[Nomor] = Kondisi;
  int i = 0;
  for (int D = 0; D < 2; D++){
    for (int C = 0; C < 2; C++){
      for (int B = 0; B < 2; B++){
        for (int A = 0; A < 2; A++){
          if (i == Nomor){
            data_Indikator[D][C][B][A] = 1;
          }
          i++;
        }
      }
    }
  }
}
void view_Indikator() // mengatur penampilan indikator ke 16 LED
{
  for (int D = 0; D < 2; D++){
    for (int C = 0; C < 2; C++){
      for (int B = 0; B < 2; B++){
        for (int A = 0; A < 2; A++){
          if (data_Indikator[D][C][B][A] == 1){
            digitalWrite(pin_Ind[0], A);
            digitalWrite(pin_Ind[1], B);
            digitalWrite(pin_Ind[2], C);
            digitalWrite(pin_Ind[3], D);
          }
        }
      }
    }
  }
}
void indikatorKalibrasiLine()
{
  //inisiasi agar 0 semua
  for (int i = 1; i <= 16; i++){
    set_Indikator(i, 0);
  }
  for (int i = 0; i < 6; i++){
    set_Indikator((i+1), val_sens[i]);
  }
  set_Indikator(16,1);
  set_Indikator(15,0);
}
void indikatorSettingPID(float x)
{
  //inisiasi agar 0 semua
  for (int i = 1; i <= 16; i++){
    set_Indikator(i, 0);
  }
  for (int i = 1; i <= 10*x; i++){
    set_Indikator(i, 1);
  }
  set_Indikator(16,0);
  set_Indikator(15,1);
}
void indikatorJalan()
{
  //inisiasi agar 0 semua
  for (int i = 1; i <= 16; i++){
    set_Indikator(i, 0);
  }
  for (int i = 1; i <= val_Count; i++){
    set_Indikator(i, 1);
  }
  set_Indikator(16,1);
  set_Indikator(15,1);
}

void baca_Sensor(char bagian)  // untuk input nilai val_sens[i]
{
  //all sensor is active
  if (bagian == 'F')
  {
    read_sens[0] = analogRead(A8);
    read_sens[1] = analogRead(A9);
    read_sens[2] = analogRead(A10);
    read_sens[3] = analogRead(A11);
    read_sens[4] = analogRead(A12);
    read_sens[5] = analogRead(A13);
  }
  //left sensor is active
  if (bagian == 'L'){
    read_sens[0] = analogRead(A8);
    read_sens[1] = analogRead(A9);
    read_sens[2] = analogRead(A10);
    read_sens[3] = analogRead(A11);
    read_sens[4] = 0;
    read_sens[5] = 0;
  }
  //right sensor is active
  if (bagian == 'R'){
    read_sens[0] = 0;
    read_sens[1] = 0;
    read_sens[2] = analogRead(A10);
    read_sens[3] = analogRead(A11);
    read_sens[4] = analogRead(A12);
    read_sens[5] = analogRead(A13);
  }

  for(int i = 0; i < 6; i++)
  {
    if(read_sens[i] < cal_Black) 
    {
      val_sens[i] = 0;
    }
    else 
    {
      val_sens[i] = 1;
    }
  }
  /* untuk pembacaan yg lebih sederhana
  string line;
  for (int i = 0; i< 6; i++){
    line[i] = val_sens[i];
  }
  /*Serial.println(line);*/
}

void olah_Sensor() // pengolah input val_sens[i] dan mengupdate nilai error
{
  baca_Sensor(bacaBagian);
  if (ignorance = 0){
    if ( ( val_sens[2] == 1 ) && ( val_sens[3] == 1) ){
      // Pembacaan sensor pada Robot
      if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[4] == 0) && (val_sens[5] == 1) ) 
      { // sensor 6 terkena garis
        error = 5;
      }
      
      if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[4] == 1) && (val_sens[5] == 1) ) 
      { // sensor 6 dan 5 terkena garis
        error = 4;
      }
      
      if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[4] == 1) && (val_sens[5] == 0) ) 
      { // sensor 5 terkena garis
        error = 3;
      }
            
      if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
      { // keadaan robot normal
        error = 0; 
      }
      
      if( (val_sens[0] == 0) && (val_sens[1] == 1) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
      { // sensor 2 terkena garis
        error = -3;
      }
      
      if( (val_sens[0] == 1) && (val_sens[1] == 1) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
      { // sensor 1 dan 2 terkena garis
        error = -4;
      }
      
      if( (val_sens[0] == 1) && (val_sens[1] == 0) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
      { // sensor 1 terkena garis
        error = -5;
      }
    }
    else
    {
      data_error_default();
    }
  }
  else if (ignorance = 1)
  {
    data_error_default();
  }
}

void data_error_default()
{
  // Pembacaan sensor pada Robot
  if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[2] == 0) && (val_sens[3] == 0) && (val_sens[4] == 0) && (val_sens[5] == 1) ) 
  { // sensor 6 terkena garis
    error = 5;
  }
  
  if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[2] == 0) && (val_sens[3] == 0) && (val_sens[4] == 1) && (val_sens[5] == 1) ) 
  { // sensor 6 dan 5 terkena garis
    error = 4;
  }
  
  if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[2] == 0) && (val_sens[3] == 0) && (val_sens[4] == 1) && (val_sens[5] == 0) ) 
  { // sensor 5 terkena garis
    error = 3;
  }
  
  if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[2] == 0) && (val_sens[3] == 1) && (val_sens[4] == 1) && (val_sens[5] == 0) ) 
  { // sensor 4 dan 5 terkena garis
    error = 2;
  }
  
  if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[2] == 0) && (val_sens[3] == 1) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
  { // sensor 4 terkena garis
    error = 1;
  }
  
  if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[2] == 1) && (val_sens[3] == 1) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
  { // keadaan robot normal
    error = 0; 
  }
  
  if( (val_sens[0] == 0) && (val_sens[1] == 0) && (val_sens[2] == 1) && (val_sens[3] == 0) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
  { // sensor 3 terkena garis
    error = -1;
  }
  
  if( (val_sens[0] == 0) && (val_sens[1] == 1) && (val_sens[2] == 1) && (val_sens[3] == 0) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
  { // sensor 2 dan 3 terkena garis
    error = -2;
  }
  
  if( (val_sens[0] == 0) && (val_sens[1] == 1) && (val_sens[2] == 0) && (val_sens[3] == 0) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
  { // sensor 2 terkena garis
    error = -3;
  }
  
  if( (val_sens[0] == 1) && (val_sens[1] == 1) && (val_sens[2] == 0) && (val_sens[3] == 0) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
  { // sensor 1 dan 2 terkena garis
    error = -4;
  }
  
  if( (val_sens[0] == 1) && (val_sens[1] == 0) && (val_sens[2] == 0) && (val_sens[3] == 0) && (val_sens[4] == 0) && (val_sens[5] == 0) ) 
  { // sensor 1 terkena garis
    error = -5;
  }
}

void count(int counter) // pengaturan metode counting
{
  if (analogRead(counter) == 0)
  {
    if (analogRead(counter) == 1)
    {
      val_Count++;
    }
  }
}

void olah_Count() // penentuan kondisi dengan Case untuk variable "count"
{
  count(counter);
  switch (val_Count) 
  {
    case 0 :
      bacaBagian = 'L';
      counter = A13;    
    break;
    case 1 :
      bacaBagian = 'R';
      counter = A8;
    break;
    case 2 : 
      bacaBagian = 'L';
      counter = A13;
    break;  
    case 3 :
      bacaBagian = 'F';
      
    break;
  }
  
}

void goPID() // penghitungan pengolahan data untuk mencari PID
{
  P = error;
  I += error;
  D = error - previous_Error;

  PID_value = (Kp * P) + (Ki * I) + (Kd * D);
  previous_Error = error;  
}
void set_Motor() // kecepatan motor yang diatur dengan PID
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
  speed_l = initial_Motor_Speed + PID_value;
  speed_r = initial_Motor_Speed - PID_value;
  
  //keadaan motor
  motor_l.run(FORWARD);
  motor_l.setSpeed(speed_l);
  motor_r.run(FORWARD);
  motor_r.setSpeed(speed_r);  
}
