#include <common.hpp>
#include <mqtt.hpp>
#include <temperature.hpp>
#include <oled.hpp>
#include <wifi.hpp>
#include <webserver.hpp>
#include <settings.hpp>

//#define _USE_EXTERNAL_TEMPERATURE_
#define DB 0.5f
static float temperature;
static float temperature_setpoint;
static int   termostat_status;
static int   termostat_mode;

//return 1 if period is not expired
//return 0 if period is expired
int limit_execution_time_sec(long * pValue, long time_sec) {
    long now = millis();
    if (now - *pValue > time_sec*1000 || *pValue == 0) {
        *pValue = now;
        return 0;
    }
    return 1;
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

// The loop function is called in an endless loop
void loop()
{
	int res;

    //TODO: alive message

    //loop all devices
	mqtt_loop();
    temperature_loop();
    oled_loop();
    web_loop();
    settings_loop();

#ifdef _USE_EXTERNAL_TEMPERATURE_
    res = mqtt_get_temperature(&temperature);
#else
    res = temperature_get_temperature(&temperature);
#endif
    if(res) {
    	Serial.print(__FUNCTION__);
    	Serial.print(__LINE__);
    	Serial.println("read temperature error!!!!");
    	return;
    }
//    else {
//    	Serial.print(__FUNCTION__);
//    	Serial.print(__LINE__);
//    	Serial.print("process temperature:");
//    	Serial.println(temperature);
//    }

    res = mqtt_publish_temperature(temperature);
    if(res) {
    	Serial.print(__FUNCTION__);
    	Serial.print(__LINE__);
    	Serial.println("sent temperature error !!!");
    }

    res = mqtt_get_setpoint(&temperature_setpoint);
    if(res) {
    	Serial.print(__FUNCTION__);
    	Serial.print(__LINE__);
    	Serial.println("get setpoint error !!!");
    	return;
    }
//    else {
//    	Serial.print(__FUNCTION__);
//    	Serial.print(__LINE__);
//    	Serial.print("process setpoint:");
//    	Serial.println(temperature_setpoint);
//    }

    res = mqtt_get_mode(&termostat_mode);
    if(res) {
        Serial.print(__FUNCTION__);
        Serial.print(__LINE__);
        Serial.println("get mode error !!!");
        return;
    }
//    else {
//      Serial.print(__FUNCTION__);
//      Serial.print(__LINE__);
//      Serial.print("process mode:");
//      Serial.println(termostat_mode);
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
    oled_display();
}
