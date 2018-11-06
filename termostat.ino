#include <common.hpp>

#include <mqtt.hpp>
#include <temperature.hpp>
#include <oled.hpp>
#include <wifi.hpp>
#include <webserver.hpp>
#include <settings.hpp>
#include <rtc.hpp>

typedef enum {
    TERMOSTAT_OK = 0,
    TERMOSTAT_MQTT_ERROR,
    TERMOSTAT_WIFI_ERROR
}termostat_status_t;

//#define _USE_EXTERNAL_TEMPERATURE_
#define DB 0.5f
static float temperature;
static float temperature_setpoint;
static int   termostat_mode;

uint32_t uptime;
Settings_t Settings;

extern struct TIME_T {
  uint8_t       second;
  uint8_t       minute;
  uint8_t       hour;
  uint8_t       day_of_week;               // sunday is day 1
  uint8_t       day_of_month;
  uint8_t       month;
  char          name_of_month[4];
  uint16_t      day_of_year;
  uint16_t      year;
  unsigned long days;
  unsigned long valid;
} RtcTime;

void TermostatRun() {
    int res;
    String status;
    static int relay_status = 0;
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);

#ifdef _USE_EXTERNAL_TEMPERATURE_
    res = mqtt_get_temperature(&temperature);
#else
    res = temperature_get_temperature(&temperature);
#endif
    if(res) {
        status = "read temperature error!!!!";
        goto ERROR_1;
    }

    addToLog(LOG_LEVEL_DEBUG, "process temperature: %f", temperature);

    res = mqtt_get_setpoint(&temperature_setpoint);
    if(res) {
        status = "get setpoint error !!!";
        goto ERROR_1;
    }

    addToLog(LOG_LEVEL_DEBUG, "process setpoint: %f", temperature_setpoint);

    res = mqtt_get_mode(&termostat_mode);
    if(res) {
        status = "get mode error !!!";
        goto ERROR_1;
    }

    addToLog(LOG_LEVEL_DEBUG, "process mode: %d", termostat_mode);

    if(temperature + DB > temperature_setpoint) {
        relay_status = 0;
    }
    else if(temperature - DB < temperature_setpoint) {
        relay_status = 1;
    }

    res = oled_print(0, 10, String("Temperature: ") + String(temperature));
    res = oled_print(0, 20, String("Temperature SP: ") + String(temperature_setpoint));
    res = oled_print(0, 30, String("Mode: ") + String(termostat_mode));
    res = oled_print(0, 40, String("Relay: ") + String(relay_status));

    return;

ERROR_1:
    addToLog(LOG_LEVEL_ERROR, "%s: error: %s", __FUNCTION__, status.c_str());
    res = oled_print(0, 10, status);
}

//The setup function is called once at startup of the sketch
void setup() {
    Serial.begin(115200);

    oled_setup();

    oled_clear();
    oled_print(0, 0, "Setup WIFI");
    oled_display();

    wifi_setup();

    oled_clear();
    oled_print(0, 0, "Setup MQTT");
    oled_display();

	mqtt_setup();

    oled_clear();
    oled_print(0, 0, "Setup Temperature");
    oled_display();

    tempetarure_setup();

    oled_clear();
    oled_print(0, 0, "Setup WEB");
    oled_display();

    web_setup();

    oled_clear();
    oled_print(0, 0, "Setup SETTINGS");
    oled_display();

    settings_setup();

    oled_clear();
    oled_print(0, 0, "Setup DONE");
    oled_display();
}

void PerformEverySecond() {
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);

    static termostat_status_t  termostat_status = TERMOSTAT_OK;

    static int tele_period = Settings.tele_period;                        // Tele period timer
    static int check_period = Settings.check_period;                       // Check period timer

    uptime++;

    if (Settings.tele_period && (tele_period == Settings.tele_period)) {
        tele_period = 0;
        float fTemp;

        temperature_read();
        temperature_get_temperature(&fTemp);
        int res = mqtt_publish_temperature(fTemp);
        if(res) {
            addToLog(LOG_LEVEL_ERROR, "sent temperature error !!!");
        }
    }
    tele_period++;

    if (Settings.check_period && (check_period == Settings.check_period)) {
        termostat_status = TERMOSTAT_OK;
        check_period = 0;
        if(wifi_check()) {
            termostat_status = TERMOSTAT_WIFI_ERROR;
        }
        if(mqtt_check()) {
            termostat_status = TERMOSTAT_MQTT_ERROR;
        }
    }
    check_period++;

    oled_clear();
    oled_print(0, 0, GetDateAndTime());

    addToLog(LOG_LEVEL_DEBUG, "1s: termostat_status:%d", termostat_status);
    //check for termostat status
    if(termostat_status == TERMOSTAT_OK) {
        //termostat is running
        TermostatRun();
    }
    else if(termostat_status == TERMOSTAT_WIFI_ERROR) {
        //WIFI error
        oled_print(0, 10, "WIFI_ERROR");
        //TODO: handle this error
    }
    else if(termostat_status == TERMOSTAT_MQTT_ERROR) {
        //MQTT Error
        oled_print(0, 10, "MQTT_ERROR");
        //TODO: handle this error
    }
    oled_display();
}

/*********************************************************************************************\
 * State loops
\*********************************************************************************************/
/*-------------------------------------------------------------------------------------------*\
 * Every 0.1 second
\*-------------------------------------------------------------------------------------------*/

void Every100mSeconds() {
  // As the max amount of sleep = 250 mSec this loop will shift in time...
}

/*-------------------------------------------------------------------------------------------*\
 * Every 0.25 second
\*-------------------------------------------------------------------------------------------*/

void Every250mSeconds() {
// As the max amount of sleep = 250 mSec this loop should always be taken...

  static uint8_t state_250mS = 0;                    // State 250msecond per second flag

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

  state_250mS++;
  state_250mS &= 0x3;
}

// The loop function is called in an endless loop
void loop() {
    //TODO: alive message
    static uint32_t count = 0;
    addToLog(LOG_LEVEL_DEBUG_MORE, "loop", count++);

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

    addToLog(LOG_LEVEL_DEBUG_MORE, "");
    addToLog(LOG_LEVEL_DEBUG_MORE, "");
    addToLog(LOG_LEVEL_DEBUG_MORE, "");
}
