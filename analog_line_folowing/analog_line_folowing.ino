// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6)
const int pin1 = 36;
const int pin2 = 25;
const int pin3 = 34;
const int pin4 = 35;
const int pin5 = 32;
const int pin6 = 33;
const int pins[6] = {pin1, pin2, pin3, pin4, pin5, pin6};
int sensorDistance[6] = {9, 24, 39, 54, 79, 84};
int totalDistance = 289;
int sensorValues[6];

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
int slowSpeed = 90;
int slowSpeedneg = -100;
int highSpeed = 255;
int highSpeedneg = -255;
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
    getSensorValues();
    int range = findRange();
    if (range < 1500)
    {
        findTheLine();
    }
    int sensorWeightedAverage = WeightedAverage(sensorValues);

    int steeringAngle = ((sensorWeightedAverage - 45) * 3.3 + SWA);
    int speed = 140 - 2.8 * sqrt((sensorWeightedAverage - 45) * (sensorWeightedAverage - 45));
    sendDataToArduino(speed, speed, steeringAngle);
}

void displaySensorValues()
{
    S1 = 5000 - analogRead(pins[0]);
    S2 = 5000 - analogRead(pins[1]);
    S3 = 5000 - analogRead(pins[2]);
    S4 = 5000 - analogRead(pins[3]);
    S5 = 5000 - analogRead(pins[4]);
    S6 = 5000 - analogRead(pins[5]);
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

int WeightedAverage(int *sensorValues)
{
    int sum = 0;
    for (int i = 0; i < 6; i++)
    {
        sum += sensorValues[i] * sensorDistance[i];
    }

    return sum / (Sum(sensorValues[0], sensorValues[1], sensorValues[2], sensorValues[3], sensorValues[4], sensorValues[5]));
}

int *getSensorValues()
{
    for (int i = 0; i < 6; i++)
    {
        sensorValues[i] = 5000 - analogRead(pins[i]);
    }
}

int Sum(int a, int b, int c, int d, int e, int f)
{
    return a + b + c + d + e + f;
}

int checkForLine()
{
    for (int i = 0; i < 6; i++)
    {
        if (sensorValues[i] > 1000)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

int findRange()
{
    int min = sensorValues[0];
    int max = min;
    for (int i = 0; i < 6; i++)
    {
        if (sensorValues[i] < min)
        {
            min = sensorValues[i];
        }
        if (sensorValues[i] > max)
        {
            max = sensorValues[i];
        }
    }
    return (max - min);
}

void findTheLine()
{
    // find the range of the sensorvalues

    sendDataToArduino(slowSpeedneg, slowSpeedneg, SWA);
    delay(300);
    while (findRange() < 1000)
    {
        getSensorValues();
        sendDataToArduino(slowSpeed, slowSpeed, SWA);
    }
}

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