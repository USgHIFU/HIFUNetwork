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

typedef struct Plane2DCoordinate
{
    Coordinate x;
    Coordinate y;
}_2DCor;

typedef struct Spot3DCoordinate
{
    Coordinate x;
    Coordinate y;
    Coordinate z;
}_3DCor;

typedef struct SpotSonicationParameter
{
    VOLT volt;
    int totalTime;
    int period;
    int dutyCycle;
    int coolingTime;
}_SoniParam;

typedef struct SessionRecorder
{
    int spotIndex;
    int periodIndex;
}_SesRec;

typedef struct SessionParameter
{
    int spotCount;
    int periodCount;
    int dutyOn;
    int dutyOff;
    int coolingTime;
}_SesParam;

#endif // VARIABLE

