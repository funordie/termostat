#include <mqtt.hpp>
#include <temperature.hpp>
#include <oled.hpp>

static float temp = 0;
//The setup function is called once at startup of the sketch
void setup()
{
    Serial.begin(115200);

    tempetarure_setup();
    mqtt_setup();
    oled_setup();
}

static float temperature_setpoint;
// The loop function is called in an endless loop
void loop()
{
	int res;
    static int count = 0;
    printf("loop: start %d\n", count);

    delay(60*1000);

    temperature_loop();
    mqtt_loop();
    oled_loop();

    res = temperature_get_temp(&temp);
    if(res) {
    	printf("read temperature error!!!!\n");
    	return;
    }
    else {
    	printf("process temperature: %f\n", temp);
    }

    res = mqtt_publish_temperature(temp);
    if(res) {
    	printf("sent temperature error!!!!\n");
    }

    res = mqtt_get_setpoint(&temperature_setpoint);
    if(res) {
    	printf("get setpoint error\n");
    	return;
    }
    else {
    	printf("process setpoint: %f\n", temperature_setpoint);
    }

    printf("loop: return %d\n", count);
    count ++;
}
