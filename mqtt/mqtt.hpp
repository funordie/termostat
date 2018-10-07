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
void mqtt_publish_temperature(float temperature);

#endif /* MQTT_MQTT_HPP_ */
