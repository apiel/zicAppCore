#ifndef APP_SDL2_H_
#define APP_SDL2_H_

#include <SDL2/SDL.h>

#include <stdio.h>

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 44100
#endif

#ifndef APP_CHANNELS
#define APP_CHANNELS 2
#endif

#ifndef APP_AUDIO_FORMAT
// #define APP_AUDIO_FORMAT AUDIO_S16LSB
#define APP_AUDIO_FORMAT AUDIO_F32LSB
#endif

#ifndef APP_AUDIO_CHUNK
// #define APP_AUDIO_CHUNK 1024
#define APP_AUDIO_CHUNK 128
#endif

#include "../app_core_def.h"
#include "display.h"
#include "zicKeyMap.h"

typedef struct {
    bool keysChanged = false;
    uint16_t keys = 0;
} Ui;

Ui ui;

bool handleKeyboard(SDL_KeyboardEvent* event)
{
    uint16_t bit;

    SDL_Log("handleKeyboard %d\n", event->keysym.scancode);
    // SDL_Log("handleKeyboard %d\n", event->repeat);
    switch (event->keysym.scancode) {
    case KEY_UP:
        bit = UI_KEY_UP;
        break;
    case KEY_DOWN:
        bit = UI_KEY_DOWN;
        break;
    case KEY_LEFT:
        bit = UI_KEY_LEFT;
        break;
    case KEY_RIGHT:
        bit = UI_KEY_RIGHT;
        break;
    case KEY_A:
        bit = UI_KEY_EDIT;
        break;
    case KEY_B: // Should be configurable!
        bit = UI_KEY_MENU;
        break;
    case KEY_Y:
        bit = UI_KEY_ACTION;
        break;
    case KEY_MENU:
        return false;
    default:
        break;
    }

    if (event->type == SDL_KEYDOWN) {
        ui.keys |= 1 << bit; // Set bit to 1
    } else {
        ui.keys &= ~(1 << bit); // unset bit (to 0)
    }

    // printf("keys %d\n", ui.keys);

    // we could skip keyChange on A repeat
    ui.keysChanged = true;

    return true;
}

bool handleController(SDL_ControllerButtonEvent* event)
{
    uint16_t bit;

    SDL_Log("handleController btn %d state %d\n", event->button, event->state);
    switch (event->button) {
    case 11:
        bit = UI_KEY_UP;
        break;
    case 12:
        bit = UI_KEY_DOWN;
        break;
    case 13:
        bit = UI_KEY_LEFT;
        break;
    case 14:
        bit = UI_KEY_RIGHT;
        break;
    case 0:
        bit = UI_KEY_EDIT;
        break;
    case 1: // Should be configurable!
        bit = UI_KEY_MENU;
        break;
    // case 20:
    //     bit = UI_KEY_ACTION;
    //     printf("should set Action\n");
    //     break;
    case 9:
        return false;
    default:
        break;
    }

    if (event->state == SDL_PRESSED) {
        ui.keys |= 1 << bit; // Set bit to 1
    } else {
        ui.keys &= ~(1 << bit); // unset bit (to 0)
    }

    // we could skip keyChange on A repeat
    ui.keysChanged = true;

    return true;
}

void initController()
{
    int num = SDL_NumJoysticks();
    // SDL_Log("%d joysticks connected\n", num);
    for (int i = 0; i < num; i++) {
        SDL_JoystickOpen(i);
        // SDL_Joystick* joystick = SDL_JoystickOpen(i);
        // SDL_Log("Joystick %d: %s\n", i, SDL_JoystickName(joystick));
        if (SDL_IsGameController(i)) {
            SDL_GameControllerOpen(i);
            // SDL_GameController* controller = SDL_GameControllerOpen(i);
            // SDL_Log("Game controller %d: %s\n", i, SDL_GameControllerName(controller));
        }
    }
}

bool handleEvent()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        // if (event.type > 0x300 && event.type < 0x800) {
        //     SDL_Log("handleEvent %d\n", event.type);
        // }

        switch (event.type) {
        case SDL_QUIT:
            return false;

        case SDL_KEYUP:
        case SDL_KEYDOWN:
            return handleKeyboard(&event.key);
            // could be useful to simulate a pot
            // case SDL_MOUSEWHEEL:
            //     break;

        // case SDL_JOYDEVICEADDED:
        //     SDL_Log("handleEvent SDL_JOYDEVICEADDED\n");
        //     break;
        case SDL_JOYAXISMOTION:
            SDL_Log("SDL_JOYAXISMOTION\n");
            break;
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            SDL_Log("SDL_JOYBUTTON\n");

        case SDL_CONTROLLERDEVICEADDED:
            // SDL_Log("SDL_CONTROLLERDEVICEADDED\n");
            initController();
            break;
        // case SDL_CONTROLLERAXISMOTION:
        //     SDL_Log("SDL_CONTROLLERAXISMOTION\n");
        //     break;
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            return handleController(&event.cbutton);
        }
    }

    return true;
}

SDL_AudioDeviceID initAudio(SDL_AudioCallback callback)
{
    SDL_AudioSpec spec, aspec;

    SDL_zero(spec);
    spec.freq = SAMPLE_RATE;
    spec.format = APP_AUDIO_FORMAT;
    spec.channels = APP_CHANNELS;
    spec.samples = APP_AUDIO_CHUNK;
    spec.callback = callback;
    spec.userdata = NULL;

    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(NULL, 0, &spec, &aspec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (audioDevice <= 0) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        return false;
    }

    SDL_Log("aspec freq %d channel %d sample %d format %d", aspec.freq, aspec.channels, aspec.samples, aspec.format);

    // Start playing, "unpause"
    SDL_PauseAudioDevice(audioDevice, 0);
    return audioDevice;
}

#endif