
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <pgmspace.h>
#include "FS.h"

#include <settings.hpp>
#include <rtc.hpp>

#include <common.hpp>

extern Settings_t Settings;

#define FILE_CONFIG       "config.dat"

// an abstract class used as a means to proide a unique pointer type
// but really has no body
class __FlashStringHelper;
#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#define F(string_literal) (FPSTR(PSTR(string_literal)))

/********************************************************************************************\
Mount FS and check config.dat
\*********************************************************************************************/
void fileSystemCheck()
{
    if (SPIFFS.begin())
    {
        fs::FSInfo fs_info;
        SPIFFS.info(fs_info);
        String log = F("FS   : Mount successful, used ");
        log=log+fs_info.usedBytes;
        log=log+F(" bytes of ");
        log=log+fs_info.totalBytes;
        addToLog(LOG_LEVEL_INFO, log + "\n");

        fs::File f = SPIFFS.open(FILE_CONFIG, "r");
        if (!f)
        {
            //				ResetFactory();
        }
        f.close();
    }
    else
    {
        String log = F("FS   : Mount failed");
        addToLog(LOG_LEVEL_ERROR, "log:%s\n", log.c_str());
        //			ResetFactory();
    }
}

/********************************************************************************************\
  Save settings to SPIFFS
  \*********************************************************************************************/
size_t SaveSettings(void)
{
    size_t res = -1;

    Settings.StructSize = sizeof(struct SettingsStruct);

    Settings.validate();

    fs::File f = SPIFFS.open(FILE_CONFIG, "r");
    if (!f)
    {
        res = f.write((const uint8_t *)&Settings, sizeof(struct SettingsStruct));
        if(!res) {
            addToLog(LOG_LEVEL_ERROR,"write config file error\n");
        }
    }
    f.close();
    return (res);
}

/********************************************************************************************\
  Load settings from SPIFFS
  \*********************************************************************************************/
int LoadSettings()
{
    size_t res = -1;

    fs::File f = SPIFFS.open(FILE_CONFIG, "r");
    if (!f)
    {
        res = f.read((uint8_t *)&Settings, sizeof(struct SettingsStruct));
        if(!res) {
            addToLog(LOG_LEVEL_ERROR,"read config file error\n");
        }
    }
    f.close();

    Settings.validate();
    return(res);
}

void settings_setup() {
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter\n", __FUNCTION__);
    RtcInit();
}

void settings_loop() {
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter\n", __FUNCTION__);
}
