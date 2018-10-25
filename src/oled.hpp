/*
 * oled.hpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

#ifndef OLED_OLED_HPP_
#define OLED_OLED_HPP_

void oled_setup();
void oled_loop();
int oled_clear();
int oled_print(int16_t xMove, int16_t yMove, String strUser);
int oled_display();

#endif /* OLED_OLED_HPP_ */
