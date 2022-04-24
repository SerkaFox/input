#ifndef VIGEM_DEFS_H
#define VIGEM_DEFS_H

#include <QObject>

namespace Vigem {

typedef struct {
    unsigned short wButtons;
    unsigned char bLeftTrigger;
    unsigned char bRightTrigger;
    short sThumbLX;
    short sThumbLY;
    short sThumbRX;
    short sThumbRY;
} XInputGamepad;

/* Vigem control operation */
enum VigemOperation {
    ButtonA = 0,
    ButtonB = 1,
    ButtonX = 2,
    ButtonY = 3,
    ButtonLeft = 4,
    ButtonRight = 5,
    ButtonUp = 6,
    ButtonDown = 7,
    ButtonStart = 8,
    ButtonBack = 9,
    ButtonLThumb = 10,
    ButtonRThumb = 11,
    ButtonLShoulder = 12,
    ButtonRShoulder = 13,
    ButtonLTrigger = 14,
    ButtonRTrigger = 15,
    ButtonLThumbLeft = 16,
    ButtonLThumbRight = 17,
    ButtonLThumbUp = 18,
    ButtonLThumbDown = 19,
    ButtonRThumbLeft = 20,
    ButtonRThumbRight = 21,
    ButtonRThumbUp = 22,
    ButtonRThumbDown = 23,

    ButtonStartup = 24,
    ButtonShutdown = 25,

    ButtonInvalid = 26
};

}

#endif // VIGEM_DEFS_H
