#include "MotorhatDriver.h"

//Write to Register for all Engines
void MotorhatDriver::setAllPWM(int i2c, int on, int off) {
    wiringPiI2CWriteReg8(i2c, PWM_ALL_LED_ON_L, on & 0xFF); //Write Low Bits from variable on to register on
    wiringPiI2CWriteReg8(i2c, PWM_ALL_LED_ON_H, on >> 8); //Write High Bits from variable on to register on
    wiringPiI2CWriteReg8(i2c, PWM_ALL_LED_OFF_L, off & 0xFF); //Write High Bits from variable off to register off
    wiringPiI2CWriteReg8(i2c, PWM_ALL_LED_OFF_H, off >> 8); //Write High Bits from variable off to register off
}
//Write to Register for specific Engine
void MotorhatDriver::setPWM(int i2c, int pin, int on, int off) {
    wiringPiI2CWriteReg8(i2c, PWM_LED0_ON_L + 4 * pin, on & 0xFF); //Write Low bits of variable on to pin position
    wiringPiI2CWriteReg8(i2c, PWM_LED0_ON_H + 4 * pin, on >> 8); // "" High "" 
    wiringPiI2CWriteReg8(i2c, PWM_LED0_OFF_L + 4 * pin, off & 0xFF);
    wiringPiI2CWriteReg8(i2c, PWM_LED0_OFF_H + 4 * pin, off >> 8);
}
//Write Value to Pin ==> Function will be used for choosing Driving Direction in runMotor
void MotorhatDriver::setPin(int i2c, int pin, int value) {
    //Ungültiger Pin übergeben
    if (pin < 0 || pin > 15) {
        system("echo PWM pin must be between 0 and 15 inclusive.");
        return;
    }

    switch (value) {
        //Turn off
    case 0:
        setPWM(i2c, pin, 0, 4096);
        break;
        //Turn on
    case 1:
        setPWM(i2c, pin, 4096, 0);
        break;
    default:
        system("echo PWM pin value must be 0 or 1.");
        return;
    }
}
//Set Rotating Direction
void MotorhatDriver::runMotor(int i2c, int motor, int command) {
    int in1, in2;
    //Choose which Motor to run by given parameter and set in1 and in2 variable
    switch (motor) {
    case 1:
        in1 = PWM_M1_IN1;
        in2 = PWM_M1_IN2;
        break;
    case 2:
        in1 = PWM_M2_IN1;
        in2 = PWM_M2_IN2;
        break;
    case 3:
        in1 = PWM_M3_IN1;
        in2 = PWM_M3_IN2;
        break;
    case 4:
        in1 = PWM_M4_IN1;
        in2 = PWM_M4_IN2;
        break;
    default:
        system("echo Invalid motor number.");
        return;
    }
    //Set Value to Pin, depending on given command
    switch (command) {
    case MOTOR_FORWARD:
        setPin(i2c, in1, 1);
        setPin(i2c, in2, 0);
        break;
    case MOTOR_BACK:
        setPin(i2c, in1, 0);
        setPin(i2c, in2, 1);
        break;
    case MOTOR_RELEASE:
        setPin(i2c, in1, 0);
        setPin(i2c, in2, 0);
        break;
    default:
        system("echo Unsupported command.");
        return;
    }
}
//Set Speed of Engine
void MotorhatDriver::setSpeed(int i2c, int motor, int speed) {
    
    if (speed < 0 || speed > 255) {
        system("echo Speed must be between 0 and 255 inclusive.  Received.");
        return;
    }

    int pwm;
    //Choose which Motor to configure by given parameter
    switch (motor) {
    case 1:
        pwm = PWM_M1_PWM;
        break;
    case 2:
        pwm = PWM_M2_PWM;
        break;
    case 3:
        pwm = PWM_M3_PWM;
        break;
    case 4:
        pwm = PWM_M4_PWM;
        break;
    default:
        system("echo Unsupported motor.");
        break;
    }
    //Set PWM Motor Speed
    setPWM(i2c, pwm, 0, speed * 16); 
    //Max Speed value is 4080, with limiting speed between 0 and 255, we get a better grading in speed settings

}
//Setup whole Driver
int MotorhatDriver::init() {
    //Setup I2C - i2c == fd
    int i2c = wiringPiI2CSetup(ADAFRUIT_MOTORHAT);

    //Setup PWM
    setAllPWM(i2c, 0, 0);
    wiringPiI2CWriteReg8(i2c, PWM_MODE2, PWM_OUTDRV);
    wiringPiI2CWriteReg8(i2c, PWM_MODE1, PWM_ALLCALL);
    delay(5);
    int mode1 = wiringPiI2CReadReg8(i2c, PWM_MODE1) & ~PWM_SLEEP;
    wiringPiI2CWriteReg8(i2c, PWM_MODE1, mode1);
    delay(5);

    //Set PWM frequency
    int prescale = (int)(25000000.0 / 4096.0 / PWM_FREQUENCY - 1.0);
    int oldmode = wiringPiI2CReadReg8(i2c, PWM_MODE1);
    int newmode = oldmode & 0x7F | 0x10;
    wiringPiI2CWriteReg8(i2c, PWM_MODE1, newmode);
    wiringPiI2CWriteReg8(i2c, PWM_PRESCALE, prescale);
    wiringPiI2CWriteReg8(i2c, PWM_MODE1, oldmode);
    delay(5);
    wiringPiI2CWriteReg8(i2c, PWM_MODE1, oldmode | 0x80);

    return i2c;
}
//Setup Motor
void MotorhatDriver::initMotor(int i2c, int motor) {
    runMotor(i2c, motor, MOTOR_RELEASE);
    setSpeed(i2c, motor, 100);
}