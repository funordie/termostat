/*
 * mqtt.h
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

#ifndef MQTT_MQTT_HPP_
#define MQTT_MQTT_HPP_


void mqtt_loop();
void mqtt_setup();
int mqtt_publish_temperature(float temperature);
int mqtt_get_setpoint(float * value);
int mqtt_get_temperature(float * value);
int mqtt_get_temperature_out(float * value);
int mqtt_get_mode(int * value);
int mqtt_check();

#endif /* MQTT_MQTT_HPP_ */
