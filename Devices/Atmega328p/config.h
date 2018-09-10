//Scheduler File Format
//MCU:			Atmega328p
//Type:			Config File
//Version:		1.0

#ifndef F_CPU
#define F_CPU 16000000
#endif

#ifndef SCHEDULER_PRESCALER
#define SCHEDULER_PRESCALER (uint16_t)1024
#endif

#ifndef ENTIRE_BYTE
#define ENTIRE_BYTE 8
#endif
