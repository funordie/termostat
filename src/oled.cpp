/*
 * oled.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include "OLEDDisplayUi.h"
#include <common.hpp>
#include "oled.hpp"
#include "rtc.hpp"

#include <MenuSystem.h>

const uint8_t activeSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B01000010,
    B01000010,
    B00100100,
    B00011000
};

const uint8_t inactiveSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000
};

////Added by Sloeber
//#pragma once

SSD1306Wire display(0x3c, SSD1306_SDA, SSD1306_SCL);
OLEDDisplayUi ui     ( &display );

/////////////////////  MENU  //////////////////////////////////////
class MyRenderer : public MenuComponentRenderer {
public:
    void render(Menu const& menu) const {
        display.clear();
        //display->set.setCursor(0,0);
        display.drawString(10, 10, menu.get_name());
//        lcd.setCursor(0,1);
        menu.get_current_component()->render(*this);
    }

    void render_menu_item(MenuItem const& menu_item) const {
        display.drawString(10, 10, menu_item.get_name());
    }

    void render_back_menu_item(BackMenuItem const& menu_item) const {
        display.drawString(10, 10, menu_item.get_name());
    }

    void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
        display.drawString(10, 10, menu_item.get_name());
    }

    void render_menu(Menu const& menu) const {
        display.drawString(10, 10, menu.get_name());
    }
}my_renderer;

void on_menu_sp_selected(MenuComponent* p_menu_component) {
//    lcd.setCursor(0,1);

    delay(1500); // so we can look the result on the LCD
}

void on_menu_sp1_selected(MenuComponent* p_menu_component) {
//    lcd.setCursor(0,1);

    delay(1500); // so we can look the result on the LCD
}

MenuSystem ms(my_renderer);
MenuItem mm_mi1("L1 - Item 1", &on_menu_sp_selected);
MenuItem mm_mi2("L2 - Item 1", &on_menu_sp1_selected);

void serial_print_help() {
    Serial.println("***************");
    Serial.println("w: go to previus item (up)");
    Serial.println("s: go to next item (down)");
    Serial.println("a: go back (right)");
    Serial.println("d: select \"selected\" item");
    Serial.println("?: print this help");
    Serial.println("h: print this help");
    Serial.println("***************");
}

void serial_handler() {
    char inChar;
    if ((inChar = Serial.read()) > 0) {
        switch (inChar) {
            case 'w': // Previus item
                ms.prev();
                ms.display();
                break;
            case 's': // Next item
                ms.next();
                ms.display();
                break;
            case 'a': // Back presed
                ms.back();
                ms.display();
                break;
            case 'd': // Select presed
                ms.select();
                ms.display();
                break;
            case '?':
            case 'h': // Display help
                serial_print_help();
                break;
            case '3':
                // Display menu
                ms.display();
                break;
            default:
                break;
        }
    }
}
/////////////////////////////////////////////////////////////////////

oled_display_status_t   dspl_status;
String                  dspl_loading;
////////////////   OVERLAY   ///////////////////////////////////

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(10, 0, GetDateAndTime());
}

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

//////////////////   FRAMES    ////////////////////////////////////

void drawFrameLoading(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//    addToLog(LOG_LEVEL_ERROR, "%s: enter", __FUNCTION__);

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_16);
    display->drawString(20, 20, dspl_loading);
}

void drawFrameStatus(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//    addToLog(LOG_LEVEL_ERROR, "%s: enter", __FUNCTION__);

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_10);

    //OVERLAY ROW
    display->drawString(0, 10, String("Temp: ") + String(dspl_status.temp));
    display->drawString(0, 20, String("Temp SP: ") + String(dspl_status.temp_sp));
    display->drawString(0, 30, String("Mode: ") + String(dspl_status.mode));
    display->drawString(0, 40, String("Relay: ") + String(dspl_status.relay));
    display->drawString(0, 50, dspl_status.info);
}

void drawFrameSettings(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//    addToLog(LOG_LEVEL_ERROR, "%s: enter", __FUNCTION__);

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_10);

}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrameLoading, drawFrameStatus, drawFrameSettings};

// how many frames are there?
int frameCount = 3;

//////////////////////////////////////////////////////////////////////////////


void oled_setup() {
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);

    display.init();

    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

    // The ESP is capable of rendering 60fps in 80Mhz mode
    // but that won't give you much time for anything else
    // run it in 160Mhz mode or just set it to 30 fps
    ui.setTargetFPS(60);

    // Customize the active and inactive symbol
    ui.setActiveSymbol(activeSymbol);
    ui.setInactiveSymbol(inactiveSymbol);

    // You can change this to
    // TOP, LEFT, BOTTOM, RIGHT
    ui.setIndicatorPosition(RIGHT);

    // Add overlays
    ui.setOverlays(overlays, overlaysCount);

    // Add frames
    ui.setFrames(frames, frameCount);

    ui.disableAutoTransition();

    ////////   MENU   /////////////////////////
    ms.get_root_menu().add_item(&mm_mi1);
    ms.get_root_menu().add_item(&mm_mi2);
    ///////////////////////////////////////////
}

void oled_loop() {
//    addToLog(LOG_LEVEL_ERROR, "%s: enter", __FUNCTION__);
    int remainingTimeBudget = ui.update();

    ////////   MENU   /////////////////////////
    serial_handler();
    ///////////////////////////////////////////

}

void oled_switchToFrame(uint8_t frame) {
    ui.switchToFrame(frame);
}
