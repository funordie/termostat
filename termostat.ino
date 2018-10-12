#include <gpio_define.hpp>
#include <mqtt.hpp>
#include <temperature.hpp>
#include <oled.hpp>
#include <wifi.hpp>

//#define _USE_EXTERNAL_TEMPERATURE_
#define DB 0.5f
static float temperature;
static float temperature_setpoint;
static int   termostat_status;
static int   termostat_mode;
//The setup function is called once at startup of the sketch
void setup()
{
    Serial.begin(115200);

    wifi_setup();
	mqtt_setup();
    tempetarure_setup();
    oled_setup();
}

// The loop function is called in an endless loop
void loop()
{
	int res;
    static int count = 0;

    long now = millis();
    static long lastMsg = 0;
    if (now - lastMsg > 5000) {
        lastMsg = now;
    }
    else {
        mqtt_loop();
        return;
    }

    //TODO: alive message

    //loop all devices
    temperature_loop();
    oled_loop();

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

    count ++;
}
