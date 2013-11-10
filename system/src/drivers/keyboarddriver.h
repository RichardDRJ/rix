#ifndef KEYBOARDDRIVER_H
#define KEYBOARDDRIVER_H

#include "interrupts.h"

#define CAPSLOCK 50
#define LCTRL 29
#define LALT 56
#define LWIN 125
#define RWIN 126
#define RALT 100
#define RSHIFT 54
#define LSHIFT 42
#define ESC 1
#define UP 103
#define LEFT 105
#define RIGHT 106
#define DOWN 108

void handleKeyPress();

#endif
