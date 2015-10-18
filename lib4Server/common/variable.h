#ifndef VARIABLE
#define VARIABLE

#include "macro.h"

enum Header
{
    COMMAND = 1,
    PLAN,
    STATUS
};

enum cmdType
{
    START = 1,
    STOP,
    PAUSE,
    RESUME
};

struct Plane2DCoordinate
{
    Coordinate x;
    Coordinate y;
};

struct Spot3DCoordinate
{
    Coordinate x;
    Coordinate y;
    Coordinate z;
};

struct SpotSonicationParameter
{
    VOLT volt;
    int totalTime;
    int period;
    int dutyCycle;
    int coolingTime;
};

struct SessionRecorder
{
    int spotIndex;
    int periodIndex;
};

struct SessionParam
{
    int spotCount;
    int periodCount;
    int dutyOn;
    int dutyOff;
    int coolingTime;
};

#define PI 3.14159

#endif // VARIABLE

