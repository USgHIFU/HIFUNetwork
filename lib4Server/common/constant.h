#ifndef CONSTANT
#define CONSTANT

//  SONICATION PARAMETERS
#define VOLTAGE 14
#define MS_UNIT 1000
#define PERCENT_UNIT 100
#define TEST_SPOT_COUNT 20
//  FINISH

//  DOCONTROLLER PARAMETERS
#define DEVICE_ID "USB-4751,BID#0"
#define SONICATIONTIME_LL 6
#define SONICATIONTIME_UL 20
#define SONICATIONPERIOD_LL 1
#define SONICATIONPERIOD_UL 1000
#define DUTYCYCLE_LL 1
#define DUTYCYCLE_UL 100
#define COOLINGTIME_LL 1
#define COOLINGTIME_UL 500
#define SONICATIONTIME_DEFAULT 15
#define SONICATIONPERIOD_DEFAULT 500
#define DUTYCYCLE_DEFAULT 100
#define COOLINGTIME_DEFAULT 300
#define PORT_CHANNEL 4
#define PORT_PHASE 5
#define PORT_LOAD 3
#define PORT_ENABLE 3
#define PORT_DISABLE 3
#define TRANSDUCER_COUNT 112
//  FINISH

//  PA PARAMETERS
#define DEV_COUNT_MAX 127
#define VOLT_MAX 18
#define WAITPERIOD 50
//  FINISH

//	SERVER
#define CONFIG_PATH "../lib/config/config.ini"

#endif // CONSTANT

