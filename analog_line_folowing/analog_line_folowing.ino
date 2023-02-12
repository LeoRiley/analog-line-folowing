// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6)
const int pin1 = 36;
const int pin2 = 37;
const int pin3 = 34;
const int pin4 = 35;
const int pin5 = 32;
const int pin6 = 33;
const int pins[6] = {pin1, pin2, pin3, pin4, pin5, pin6};

// variable for storing the potentiometer value
int S1 = 0;
int S2 = 0;
int S3 = 0;
int S4 = 0;
int S5 = 0;
int S6 = 0;

// setup for car controller
#include <Wire.h>
#include <EasyUltrasonic.h>

#include <MPU9250_asukiaaa.h>
// vehicle speed constants
int slowSpeed = 80;
int slowSpeedneg = -120;
int highSpeed = 255;
int highSpeedneg = -255;
// value for strait steering angle
#define SWA 85

// arduino address
#define I2C_SLAVE_ADDR 0x04 // 4 in hexadecimal

void setup()
{
    Serial.begin(115200);
    delay(1000);
    // allows communication with mpu250 and arduino
    Wire.begin();
}

void loop()
{
    // gets up to date values for the 2 sensors
    S3 = analogRead(pins[2]);
    S4 = analogRead(pins[3]);
    // works out the difference between the 2 sensors and divides it by 100
    int difference = (S3 - S4) / 100;
    int steeringAngle = 0;
    steeringAngle = SWA + difference;
    sendDataToArduino(slowSpeed, slowSpeed, steeringAngle);
}

// function for debugging sensor values
void displaySensorValues()
{
    S1 = analogRead(pins[0]);
    S2 = analogRead(pins[1]);
    S3 = analogRead(pins[2]);
    S4 = analogRead(pins[3]);
    S5 = analogRead(pins[4]);
    S6 = analogRead(pins[5]);
    Serial.print(S1);
    Serial.print(" ");
    Serial.print(S2);
    Serial.print(" ");
    Serial.print(S3);
    Serial.print(" ");
    Serial.print(S4);
    Serial.print(" ");
    Serial.print(S5);
    Serial.print(" ");
    Serial.print(S6);
    Serial.println();
}

// function for sending data to arduino
void sendDataToArduino(int leftMotor, int rightMotor, int steeringAngle)
{
    if (leftMotor > 255)
        leftMotor = 255;
    if (rightMotor > 255)
        rightMotor = 255;
    Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device #4
    Wire.write((byte)((leftMotor & 0x0000FF00) >> 8));
    Wire.write((byte)(leftMotor & 0x000000FF));
    Wire.write((byte)((rightMotor & 0x0000FF00) >> 8));
    Wire.write((byte)(rightMotor & 0x000000FF));
    Wire.write((byte)((steeringAngle & 0x0000FF00) >> 8));
    Wire.write((byte)(steeringAngle & 0x000000FF));
    Wire.endTransmission(); // stop transmitting
    delay(100);
}