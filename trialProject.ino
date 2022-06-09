#include <Wire.h>
#include <Servo.h>

Servo rollServo;
Servo pitchServo;

void setup() {
  Serial.begin(38400);
  Wire.begin();

  Wire.beginTransmission(0x68); //Begin transmission to MPU6050, which uses address 0x68
  Wire.write(0x6B); //0x6B is the PWR_MGMT_1 register
  Wire.write(0x00);
  Wire.endTransmission(true);

  delay(50);
  
  Wire.beginTransmission(0x68);
  Wire.write(0x1C); //Register for accelerometer
  Wire.write(0x00); 
  Wire.endTransmission(true);

  rollServo.attach(9, 600, 2300);
  pitchServo.attach(10, 600, 2300);
}

void loop() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B); //Data from accelerometers is 16 bits per axis, starting at 0x3B
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6, true); //Requesting 6 registers, 2 per axis
  
  int16_t xAxis = (Wire.read() << 8 | Wire.read()); //3b and 3c contain x axis data
  int16_t yAxis = (Wire.read() << 8 | Wire.read()); //3d and 3e contain y axis data
  int16_t zAxis = (Wire.read() << 8 | Wire.read()); //3f and 40 contain z axis data
  
  float xAxisG = (float)xAxis / 16384; //Full scale range was chosen in configuration, so sensitivity is 16384 LSB/g.
  float yAxisG = (float)yAxis / 16384; //Therefore, dividing the 16 bit value from the sensors by 16384 should give data in g's
  float zAxisG = (float)zAxis / 16384;

  Serial.print("X Axis = "); //Outputting accelerometer data in g's
  Serial.print(xAxisG, 2); //The output formatting is sloppy, I know, my bad. This was my first time working with an Arduino so I was more worried about function than form
  Serial.print("g   |   "); //Future project will have a better formatting
  Serial.print("Y Axis = ");
  Serial.print(yAxisG, 2);
  Serial.print("g   |   ");
  Serial.print("Z Axis = ");
  Serial.print(zAxisG, 2);
  Serial.print("g   |   ");

  int roll = -(atan2(xAxisG, sqrt(yAxisG*yAxisG + zAxisG*zAxisG))*180)/M_PI; //Calculates roll and pitch in degrees. Got the equation from one of the MPU6050 library examples 
  int pitch = (atan2(yAxisG, zAxisG)*180)/M_PI;

  rollServo.write(-(roll - 90));
  pitchServo.write(pitch + 90);

  Serial.print(" Pitch = ");
  Serial.print(pitch);
  Serial.print(" Roll = ");
  Serial.print(roll);
  Serial.println();
  
 delay(200);
}
