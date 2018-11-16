/*
 * oled.hpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

#ifndef OLED_OLED_HPP_
#define OLED_OLED_HPP_

#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

void oled_setup();
void oled_loop();
void oled_switchToFrame(uint8_t frame);

typedef struct {
    String date;
    String mode;
    float temp;
    float temp_sp;
    int relay;
    String info;
}oled_display_status_t;

#endif /* OLED_OLED_HPP_ */
