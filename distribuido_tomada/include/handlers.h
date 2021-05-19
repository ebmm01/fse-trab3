#ifndef HANDLERS_H
#define HANDLERS_H

#define ADDED_DEVICE "ADDED_DEVICE"
#define UPDATE_OUTPUT_SENSOR "UPDATE_OUTPUT_SENSOR"
#define RESET_DEVICE "RESET_DEVICE"

extern char roomName[30];

void apply_handler(char* msg);

#endif