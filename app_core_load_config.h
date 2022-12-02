#ifndef APP_CORE_LOAD_CONFIG_H_
#define APP_CORE_LOAD_CONFIG_H_

#include <stdlib.h>

#include "./app_core_util.h"
#include "./app_core_def.h"

#define APP_CONFIG_SIZE 128

class App_Load_Config {
protected:
    uint8_t getFieldNum(Zic_File* file)
    {
        char field[4];
        if (!file->read(field, 4)) {
            return 255;
        }
        field[3] = '\0';
        trimToNumeric(field);
        return atoi(field);
    }

    uint8_t getFieldCommand(Zic_File* file)
    {
        char field[4];
        if (!file->read(field, 3)) {
            return 255;
        }
        field[2] = '\0';
        return *(uint8_t*)field;
    }

public:
    uint8_t config[APP_CONFIG_SIZE][3];

    virtual const char* getConfigFile() = 0;

    void load()
    {
        uint8_t i = 0;
        Zic_File file(getConfigFile(), "r");
        if (file.isOpen()) {
            for (; i < APP_CONFIG_SIZE; i++) {
                config[i][0] = getFieldCommand(&file);
                config[i][1] = getFieldNum(&file);
                config[i][2] = getFieldNum(&file);
                if (config[i][0] == 255) {
                    break;
                }
            }

            for (; i < APP_CONFIG_SIZE; i++) {
                config[i][0] = 255;
                config[i][1] = 255;
                config[i][2] = 255;
            }

            file.close();
        }
    }

    void save()
    {
    }

    void logCfg()
    {
        for (uint8_t i = 0; i < APP_CONFIG_SIZE; i++) {
            if (config[i][0] == 255) {
                break;
            }
            APP_LOG("cfg (%d): %d %d %d\n", i, config[i][0], config[i][1], config[i][2]);
        }
    }
};

#endif