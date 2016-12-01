#include <OrbitOledChar.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOledGrph.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitOled.h>
#include <Wire.h>
#include <stdlib.h>
#include <stdio.h>

void WireRequestArray(int address, uint8_t* buffer, uint8_t amount);
void WireWriteRegister(int address, uint8_t reg, uint8_t value);
void WireWriteByte(int address, uint8_t value);

static float const SensorMaximumReading = 512.0;
static float const SensorMaximumAccel = 9.81 * 4.0;
static uint8_t const SensorAccelerometer = 0x1D;
static uint8_t const TempSensor = 0x4F;

bool isGameOver = true;
const uint32_t ButtonCount = 2;
const uint32_t Buttons[ButtonCount] = { PD_2, PE_0 };
const uint32_t Potentiometer = PE_3;
const uint8_t  PRIMARY = 0;
const uint8_t  SECONDARY = 1;
const float SensorThreshold = 1.5;

struct ButtonState {
    bool state;
    bool isTriggered;
    bool isLive;
} buttons[2];

static struct InputState {
    struct ButtonState buttons[2];
    float x, y;
    char previous;
} input;

void SensorInit()
{
    WireWriteRegister(SensorAccelerometer, 0x31, 1);
    WireWriteRegister(SensorAccelerometer, 0x2D, 1 << 3);
    WireWriteRegister(TempSensor, 0x41,1);
    WireWriteRegister(TempSensor, 0x3D,1 << 3);
}
static int n;

void SecondaryFireTick()
{
    bool previousState = buttons[SECONDARY].state;
    buttons[SECONDARY].state = digitalRead(Buttons[SECONDARY]);
    buttons[SECONDARY].isTriggered = (!previousState && buttons[SECONDARY].state);
}

void ControlsTick()
{
    if (buttons[PRIMARY].state==LOW&&digitalRead(Buttons[PRIMARY])==HIGH) {
        Serial.println('p');
        buttons[PRIMARY].state=HIGH;
        return;
    }
    if (buttons[PRIMARY].state==HIGH&&digitalRead(Buttons[PRIMARY])==LOW) {
        Serial.println('o');
        buttons[PRIMARY].state=LOW;
        return;
    }

    if (!buttons[SECONDARY].isLive) {
        loadingBar();
    } else {
        SecondaryFireTick();
        if (buttons[SECONDARY].isTriggered) {
            n=0;
            buttons[SECONDARY].isLive=false;
            Serial.println('s');
            return;
        }
    }

    size_t const DataLength = 6;
    uint32_t data[DataLength] = { 0 };

    WireWriteByte(SensorAccelerometer, 0x32);
    WireRequestArray(SensorAccelerometer, data, DataLength);

    uint16_t xi = (data[1] << 8) | data[0];
    
    input.x = *(int16_t*)(&xi) / SensorMaximumReading * SensorMaximumAccel;

    if (input.x > SensorThreshold) {
        if (input.previous!='l') {
            Serial.println('l'); //Left
            input.previous='l';
        }
    } else if (input.x < -1 * SensorThreshold) {
        if (input.previous!='r') {
            Serial.println('r'); //Right
            input.previous='r';
        }
    } else {
        if (input.previous!='c') {
            Serial.println('c'); //Centre
            input.previous='c';
        }
    }
}

void mainMenu()
{
    Serial.println("gg");
    OrbitOledClear();
    OrbitOledMoveTo( 12, 0);    //    H
    OrbitOledLineTo(  0,15);    //    H
    OrbitOledMoveTo( 21, 0);    //    H
    OrbitOledLineTo(  9,15);    //    H
    OrbitOledMoveTo(  6, 7);    //    H
    OrbitOledLineTo( 18, 7);    //    H
    OrbitOledMoveTo( 32, 0);    //    Y
    OrbitOledLineTo( 20,15);    //    Y
    OrbitOledMoveTo( 24, 0);    //    Y
    OrbitOledLineTo( 24,10);    //    Y
    OrbitOledMoveTo( 36, 0);    //    P
    OrbitOledLineTo( 24,15);    //    P
    OrbitOledMoveTo( 36, 0);    //    P
    OrbitOledLineTo( 41, 0);    //    P
    OrbitOledMoveTo( 31, 7);    //    P
    OrbitOledLineTo( 36, 7);    //    P
    OrbitOledMoveTo( 41, 0);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 42, 1);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 36, 7);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 37, 6);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 38, 6);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 39, 5);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 40, 5);    //    P
    OrbitOledDrawPix el(  );    //    P
    OrbitOledMoveTo( 41, 4);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 42, 3);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 42, 2);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 48, 0);    //    E
    OrbitOledLineTo( 36,15);    //    E
    OrbitOledMoveTo( 48, 0);    //    E
    OrbitOledLineTo( 57, 0);    //    E
    OrbitOledMoveTo( 42, 7);    //    E
    OrbitOledLineTo( 51, 7);    //    E
    OrbitOledMoveTo( 37,14);    //    E
    OrbitOledLineTo( 46,14);    //    E
    OrbitOledMoveTo( 60, 0);    //    R
    OrbitOledLineTo( 48,15);    //    R
    OrbitOledMoveTo( 54, 7);    //    R
    OrbitOledLineTo( 57,15);    //    R
    OrbitOledMoveTo( 60, 0);    //    R
    OrbitOledLineTo( 65, 0);    //    R
    OrbitOledMoveTo( 55, 7);    //    R
    OrbitOledLineTo( 60, 7);    //    R
    OrbitOledMoveTo( 65, 0);    //    R
    OrbitOledDrawPixel(   );    //    R
    OrbitOledMoveTo( 66, 1);    //    R
    OrbitOledDrawPixel(   );    //    R
    OrbitOledMoveTo( 60, 7);    //    R
    OrbitOledDrawPixel(   );    //    R
    OrbitOledMoveTo( 61, 6);    //    R
    OrbitOledDrawPixel(   );    //    R
    OrbitOledMoveTo( 62, 6);    //    R
    OrbitOledDrawPixel(   );    //    R
    OrbitOledMoveTo( 63, 5);    //    R
    OrbitOledDrawPixel(   );    //    R
    OrbitOledMoveTo( 64, 5);    //    R
    OrbitOledDrawPixel(   );    //    R
    OrbitOledMoveTo( 65, 4);    //    R
    OrbitOledDrawPixel(   );    //    R
    OrbitOledMoveTo( 66, 3);    //    R
    OrbitOledDrawPixel(   );    //    R
    OrbitOledMoveTo( 66, 2);    //    R
    OrbitOledDrawPixel(   );    //    R
    OrbitOledMoveTo( 81, 0);    //    S
    OrbitOledLineTo( 66, 7);    //    S
    OrbitOledLineTo( 75, 7);    //    S
    OrbitOledLineTo( 61,15);    //    S
    OrbitOledMoveTo( 84, 0);    //    P
    OrbitOledLineTo( 72,15);    //    P
    OrbitOledMoveTo( 84, 0);    //    P
    OrbitOledLineTo( 89, 0);    //    P
    OrbitOledMoveTo( 79, 7);    //    P
    OrbitOledLineTo( 84, 7);    //    P
    OrbitOledMoveTo( 89, 0);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 90, 1);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 84, 7);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 85, 6);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 86, 6);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 87, 5);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 88, 5);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 89, 4);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 90, 3);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 90, 2);    //    P
    OrbitOledDrawPixel(   );    //    P
    OrbitOledMoveTo( 84,15);    //    A
    OrbitOledLineTo( 99, 0);    //    A
    OrbitOledLineTo( 93,15);    //    A
    OrbitOledMoveTo( 92, 7);    //    A
    OrbitOledLineTo( 96, 7);    //    A
    OrbitOledMoveTo(108, 0);    //    C
    OrbitOledLineTo( 96,15);    //    C
    OrbitOledMoveTo(108, 0);    //    C
    OrbitOledLineTo(117, 0);    //    C
    OrbitOledMoveTo( 97,14);    //    C
    OrbitOledLineTo(106,14);    //    C
    OrbitOledMoveTo(120, 0);    //    E
    OrbitOledLineTo(108,15);    //    E
    OrbitOledMoveTo(120, 0);    //    E
    OrbitOledLineTo(127, 0);    //    E
    OrbitOledMoveTo(114, 7);    //    E
    OrbitOledLineTo(123, 7);    //    E
    OrbitOledMoveTo(109,14);    //    E
    OrbitOledLineTo(118,14);    //    E
    OrbitOledSetCursor(1,2);
    OrbitOledPutString("BTN-2 to Start");
    char highScore[50] = { 0 };
    while (1) {
        Serial.readBytesUntil('\n', highScore, 50);
        if (highScore[0]!='\0') break;
    }
    OrbitOledSetCursor(1,3);
    OrbitOledPutString("HIGH: ");
    OrbitOledPutString(highScore);
    OrbitOledUpdate();
    do {
        SecondaryFireTick();
    } while (!buttons[SECONDARY].isTriggered);
    Serial.println("start");
}
void checkGameOver()
{
    char checkForGameOver[2] = { 0 };
    Serial.readBytesUntil('\n',checkForGameOver, 2);
    if (checkForGameOver[0]=='w'&&checkForGameOver[1]=='p')
        isGameOver = true;
}
void loadingBar()
{
    if (n==128) {
      buttons[1].isLive=true;
      return;
    }
    if (n==0) {
      OrbitOledClear();
      OrbitOledSetCursor(6, 0);
      OrbitOledPutString("POWER");
      OrbitOledMoveTo(48,11);
      OrbitOledLineTo(87,11);
    }
    OrbitOledMoveTo(n,16);
    OrbitOledFillRect(n,32);
    int luck = rand();
    if (luck%23==0)
      ++n;
    OrbitOledUpdate();
}

void setup()
{
    // put your setup code here, to run once:
    WireInit();
    Serial.begin(9600);
    for (int i = 0; i < ButtonCount; ++i) {
        pinMode(Buttons[i], INPUT);
    }
    OrbitOledInit();
    OrbitOledSetFillPattern(OrbitOledGetStdPattern(iptnSolid));
    SensorInit();
    buttons[1].isLive=false;
    n = 0;
    srand((2232+analogRead(Potentiometer)) * 5764);
    input.previous='c';
}

void loop()
{
    // put your main code here, to run repeatedly:
    if (isGameOver) {
        mainMenu();
        isGameOver=false;
    }
    ControlsTick();
}
