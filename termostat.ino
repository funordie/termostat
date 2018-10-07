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

// The loop function is called in an endless loop
void loop()
{
    static int count = 0;
    char buff[80];
    sprintf(buff, "loop: start %d", count);
    Serial.println(buff);

    delay(60*1000);

    temperature_loop();
    mqtt_loop();
    oled_loop();

    temperature_get_temp(&temp);
    mqtt_publish_temperature(temp);
    sprintf(buff, "loop: return %d", count);
    Serial.println(buff);
    count ++;
}
