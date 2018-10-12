#include <gpio_define.hpp>
#include <mqtt.hpp>
#include <temperature.hpp>
#include <oled.hpp>
#include <wifi.hpp>

//#define _USE_EXTERNAL_TEMPERATURE_

static float temperature = 0;
static float temperature_setpoint;

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
    else {
    	Serial.print(__FUNCTION__);
    	Serial.print(__LINE__);
    	Serial.print("process temperature:");
    	Serial.println(temperature);
    }

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
    else {
    	Serial.print(__FUNCTION__);
    	Serial.print(__LINE__);
    	Serial.print("process setpoint:");
    	Serial.println(temperature_setpoint);
    }

    oled_clear();
    res = oled_print(0, 0, String("Temperature: ") + String(temperature));
    res = oled_print(0, 10, String("Temperature SP: ") + String(temperature_setpoint));
    oled_display();

    count ++;
}
