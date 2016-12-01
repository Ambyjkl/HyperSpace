#include <OrbitOledChar.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOledGrph.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitOled.h>
#include <Wire.h>
#include <stdlib.h>

void WireRequestArray(int address, uint8_t* buffer, uint8_t amount);
void WireWriteRegister(int address, uint8_t reg, uint8_t value);
void WireWriteByte(int address, uint8_t value);

static float const SensorMaximumReading = 512.0;
static float const SensorMaximumAccel = 9.81 * 4.0;
static uint8_t const SensorAccelerometer = 0x1D;

const uint32_t ButtonCount = 2;
const uint32_t Buttons[ButtonCount] = { PD_2, PE_0 };
const uint32_t Potentiometer = PE_3;
const uint8_t  PRIMARY = 0;
const uint8_t  SECONDARY = 1;

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

void ShakeInit()
{
    WireWriteRegister(SensorAccelerometer, 0x31, 1);
    WireWriteRegister(SensorAccelerometer, 0x2D, 1 << 3);
}
static int n;
void ControlsTick()
{
    for (int i = 0; i < ButtonCount; ++i) {
        // Only look for Rising Edge Signals.
        bool previousState = buttons[i].state;
        buttons[i].state = digitalRead(Buttons[i]);
        buttons[i].isTriggered = (!previousState && buttons[i].state);
    }

    if (buttons[PRIMARY].isTriggered) {
        Serial.println('p');
        return;
    }
    if (!buttons[SECONDARY].isLive) {
        loadingBar();
    } else if (buttons[SECONDARY].isTriggered) {
        n=0;
        buttons[SECONDARY].isLive=false;
        Serial.println('s');
        return;
    }

    size_t const DataLength = 6;
    uint32_t data[DataLength] = { 0 };

    WireWriteByte(SensorAccelerometer, 0x32);
    WireRequestArray(SensorAccelerometer, data, DataLength);

    uint16_t xi = (data[1] << 8) | data[0];
    //uint16_t yi = (data[3] << 8) | data[2];
    //uint16_t zi = (data[5] << 8) | data[4];
    input.x = *(int16_t*)(&xi) / SensorMaximumReading * SensorMaximumAccel;
    //input.y = *(int16_t*)(&yi) / SensorMaximumReading * SensorMaximumAccel;

    if (input.x > 3) {
        Serial,println('l');
        input.previous='l';  //Left
    } else if (input.x < -3) {
        Serial.println('r'); //Right
        input.previous='r';       
    } else {
        Serial.println('c'); //Centre
        input.previous='c';
    } 
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
    OrbitOledClear();
    OrbitOledSetFillPattern(OrbitOledGetStdPattern(iptnSolid));
    OrbitOledMoveTo(0, 0);
    OrbitOledSetCursor(0, 0);
    OrbitOledPutString("gg");
    OrbitOledUpdate();
    ShakeInit();
    buttons[1].isLive=false;
    n = 0;
    srand(5764);
    input.previous='c';
}

void loop()
{
    // put your main code here, to run repeatedly:
    ControlsTick();
}
