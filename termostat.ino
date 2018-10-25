#include <common.hpp>

#include <mqtt.hpp>
#include <temperature.hpp>
#include <oled.hpp>
#include <wifi.hpp>
#include <webserver.hpp>
#include <settings.hpp>
#include <rtc.hpp>

//#define _USE_EXTERNAL_TEMPERATURE_
#define DB 0.5f
static float temperature;
static float temperature_setpoint;
static int   termostat_status;
static int   termostat_mode;

uint32_t uptime;
Settings_t Settings;

void TermostatRun() {
    int res;

    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);

#ifdef _USE_EXTERNAL_TEMPERATURE_
    res = mqtt_get_temperature(&temperature);
#else
    res = temperature_get_temperature(&temperature);
#endif
    if(res) {
        addToLog(LOG_LEVEL_ERROR, "read temperature error!!!!");
        return;
    }
//    else {
//      addToLog(LOG_LEVEL_ERROR, "process temperature: %f", temperature);
//    }

    res = mqtt_publish_temperature(temperature);
    if(res) {
        addToLog(LOG_LEVEL_ERROR, "sent temperature error !!!");
    }

    res = mqtt_get_setpoint(&temperature_setpoint);
    if(res) {
        addToLog(LOG_LEVEL_ERROR, "get setpoint error !!!");
        return;
    }
//    else {
//      addToLog(LOG_LEVEL_ERROR, "process setpoint: %f", temperature_setpoint);
//    }

    res = mqtt_get_mode(&termostat_mode);
    if(res) {
        addToLog(LOG_LEVEL_ERROR, "get mode error !!!");
        return;
    }
//    else {
//      addToLog(LOG_LEVEL_ERROR, "process mode: %d", termostat_mode);
//    }

    if(temperature + DB > temperature_setpoint) {
        termostat_status = 0;
    }
    else if(temperature - DB < temperature_setpoint) {
        termostat_status = 1;
    }

    oled_clear();
    res = oled_print(0, 0, String("Temperature: ") + String(temperature));
    res = oled_print(0, 10, String("Temperature SP: ") + String(temperature_setpoint));
    res = oled_print(0, 20, String("Mode: ") + String(termostat_mode));
    res = oled_print(0, 30, String("Status: ") + String(termostat_status));
    res = oled_print(0, 30, String("Time: ") + GetDateAndTime());
    oled_display();
}
//The setup function is called once at startup of the sketch
void setup()
{
    Serial.begin(115200);

    wifi_setup();
	mqtt_setup();
    tempetarure_setup();
    oled_setup();
    web_setup();
    settings_setup();
}

void PerformEverySecond()
{
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);

    static int tele_period = 1;                        // Tele period timer
    static int check_period = 1;                       // Check period timer

    uptime++;

    if (Settings.tele_period) {
        tele_period++;
        if (tele_period >= Settings.tele_period) {
            tele_period = 0;

            //run function
            temperature_loop();
        }
    }
    if (Settings.check_period) {
        check_period++;
        if (check_period >= Settings.check_period) {
            check_period = 0;

            //run function
            wifi_check();
        }
    }

    //every second
    TermostatRun();
    oled_loop();
}

/*********************************************************************************************\
 * State loops
\*********************************************************************************************/
/*-------------------------------------------------------------------------------------------*\
 * Every 0.1 second
\*-------------------------------------------------------------------------------------------*/

void Every100mSeconds()
{
  // As the max amount of sleep = 250 mSec this loop will shift in time...
}

/*-------------------------------------------------------------------------------------------*\
 * Every 0.25 second
\*-------------------------------------------------------------------------------------------*/

void Every250mSeconds()
{
// As the max amount of sleep = 250 mSec this loop should always be taken...

  static uint8_t state_250mS = 0;                    // State 250msecond per second flag

  state_250mS++;
  state_250mS &= 0x3;

/*-------------------------------------------------------------------------------------------*\
 * Every second at 0.25 second interval
\*-------------------------------------------------------------------------------------------*/

  switch (state_250mS) {
  case 0:                                                 // Every x.0 second
    PerformEverySecond();
    break;
  case 1:                                                 // Every x.25 second
    break;
  case 2:                                                 // Every x.5 second
    break;
  case 3:                                                 // Every x.75 second
    break;
  }
}

// The loop function is called in an endless loop
void loop()
{
    //TODO: alive message

    static unsigned long state_50msecond = 0;          // State 50msecond timer
    static unsigned long state_100msecond = 0;         // State 100msecond timer
    static unsigned long state_250msecond = 0;         // State 250msecond timer

    if (TimeReached(state_50msecond)) {
      SetNextTimeInterval(state_50msecond, 50);
//      XdrvCall(FUNC_EVERY_50_MSECOND);
//      XsnsCall(FUNC_EVERY_50_MSECOND);
    }
    if (TimeReached(state_100msecond)) {
      SetNextTimeInterval(state_100msecond, 100);
      Every100mSeconds();
//      XdrvCall(FUNC_EVERY_100_MSECOND);
//      XsnsCall(FUNC_EVERY_100_MSECOND);
    }
    if (TimeReached(state_250msecond)) {
      SetNextTimeInterval(state_250msecond, 250);
      Every250mSeconds();
//      XdrvCall(FUNC_EVERY_250_MSECOND);
//      XsnsCall(FUNC_EVERY_250_MSECOND);
    }

    mqtt_loop();
    web_loop();
    settings_loop();

//    addToLog(LOG_LEVEL_ERROR, "\n\n\n");
}
