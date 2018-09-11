#include <avr/io.h>
#include <avr/interrupt.h>

#include "scheduler/config.h"
#include "scheduler/scheduler_16.h"

//Scheduler Hardware Link file
//MCU:			Atmega328p
//Version:		1.0

// HARDWARE
void scheduler::timer_setup(void){
	//set timer1 mode = 0
	TCCR1B &= ~((1<<WGM12) | (1<<WGM13));
	TCCR1A &= ~((1<<WGM11) | (1<<WGM10));
	//set prescaler
	TCCR1B &= ~((1<<CS12)|(1<<CS10)|(1<<CS11));
	if(prescaler == 256){
		TCCR1B |= (1<<CS12);
	}
	else if(prescaler == 64){
		TCCR1B |= ((1<<CS11)|(1<<CS10));
	}
	else if(prescaler == 8){
		TCCR1B |= (1<<CS11);
	}
	else if(prescaler == 1){
		TCCR1B |= (1<<CS10);
	}
	else{
		TCCR1B |= ((1<<CS12)|(1<<CS10));
	}
	interrupt_timer_enable();
}
void scheduler::timer_start(void){
	TCCR1B &= ~((1<<CS12)|(1<<CS10)|(1<<CS11));
	if(prescaler == 256){
		TCCR1B |= (1<<CS12);
	}
	else if(prescaler == 64){
		TCCR1B |= ((1<<CS11)|(1<<CS10));
	}
	else if(prescaler == 8){
		TCCR1B |= (1<<CS11);
	}
	else if(prescaler == 1){
		TCCR1B |= (1<<CS10);
	}
	else{
		TCCR1B |= ((1<<CS12)|(1<<CS10));
	}
}
void scheduler::timer_stop(void){
	TCCR1B &= ~((1<<CS10) | (1<<CS11) | (1<<CS12));
}
void scheduler::timer_set_next(void){
	interrupt_global_block_enable();
	if(data[sctr - 1].time){
		TCNT1H = uint8_t(timer_overflow_time(data[sctr - 1].time) >> ENTIRE_BYTE);
		TCNT1L = uint8_t(timer_overflow_time(data[sctr - 1].time) & 0x00FF);
	}
	else{
		TCNT1H = 0x00;
		TCNT1L = 0x00;
	}
	interrupt_global_block_disable();
}
void scheduler::timer_set_overflow(uint16_t value){
	interrupt_global_block_enable();
	if(value){
		TCNT1H = uint8_t(timer_overflow_time(value) >> ENTIRE_BYTE);
		TCNT1L = uint8_t(timer_overflow_time(value) & 0x00FF);
	}
	else{
		TCNT1H = 0x00;
		TCNT1L = 0x00;
	}
	interrupt_global_block_disable();
}
void scheduler::timer_block_enable(void){
	if(!timer_block_status){
		timer_block_data = TCCR1B;
	}
	timer_stop();
	timer_block_status++;
}
void scheduler::timer_block_disable(void){
	if(timer_block_status > 1){
		timer_block_status--;
	}
	else if(timer_block_status){
		TCCR1B = (TCCR1B & ~((1<<CS12)|(1<<CS11)|(1<<CS10))) | (timer_block_data & ((1<<CS12)|(1<<CS11)|(1<<CS10)));
		timer_block_status--;
	}
}
uint16_t scheduler::time_get(void){
	uint16_t reading;
	reading = uint16_t(TCNT1L);
	reading |= (uint16_t(TCNT1H) << ENTIRE_BYTE);
	return reading;
}

// INTERRUPTS
void scheduler::interrupt_global_enable(void){
	SREG |= (1 << 7);
}
void scheduler::interrupt_global_disable(void){
	SREG &= ~(1 << 7);
}
void scheduler::interrupt_global_block_enable(void){
	if(!interrupt_block_status){
		interrupt_block_data = SREG;
	}
	SREG &= ~(1 << 7);
	interrupt_block_status++;
}
void scheduler::interrupt_global_block_disable(void){
	if(interrupt_block_status > 1){
		interrupt_block_status--;
	}
	else if(interrupt_block_status){
		SREG = (SREG & 0x7F) | (interrupt_block_data & 0x80);
		interrupt_block_status--;
	}
}
void scheduler::interrupt_timer_clear(void){
	TIFR1 |= (1 << TOV1);
}
void scheduler::interrupt_timer_enable(void){
	TIMSK1 |= (1 << TOIE1);
}
void scheduler::interrupt_timer_disable(void){
	TIMSK1 &= ~(1 << TOIE1);
}
uint8_t scheduler::interrupt_timer_check(void){
	return TIFR1 & (1 << TOV1);
}

// DEPRECATED
inline void scheduler::atomic_operation_begin(void){
	if(!atomic_operation_status){
		temp_status_register = SREG;
	}
	SREG &= ~(1 << 7);
	atomic_operation_status = 1;
}
inline void scheduler::atomic_operation_end(void){
	if(atomic_operation_status){
		SREG = temp_status_register;
	}
	atomic_operation_status = 0;
}
