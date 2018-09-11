#include <avr/io.h>
#include <avr/interrupt.h>

#include "scheduler/config.h"
#include "schdeuler/scheduler_16.h"

//Scheduler Class File
//Version:		1.0

// TIMER OVERFLOW INTERRUPT
//cuidado, variavel global (foi necessario)
scheduler *SCHEDULER_EVENT_HANDLER = nullptr;
ISR(TIMER1_OVF_vect){
	SCHEDULER_EVENT_HANDLER->timer_overflow();
}

// MAIN+
void scheduler::schedule(void (*subr)(void *), void *param, uint16_t delay){
	interrupt_global_block_enable();
	uint16_t insertion_pos, time_diff;
	if(delay){
		timer_block_enable();
		if(interrupt_timer_check()){
			timer_set_overflow(0);
		}
		insertion_pos = stack_find_pos(delay, &time_diff);
		if(insertion_pos == sctr){
			timer_set_overflow(time_diff);
		}
		stack_lift(insertion_pos);
		data[insertion_pos].stack = subr;
		data[insertion_pos].arg = param;
		data[insertion_pos].time = time_diff;
		if(insertion_pos){
			data[insertion_pos - 1].time -= time_diff;
		}
		timer_block_disable();
	}
	else{
		subr(param);
	}
	interrupt_global_block_disable();
}
scheduler::scheduler(scheduler_variables *space){
	atomic_operation_status = 0;
	interrupt_block_status = 0;
	prescaler = SCHEDULER_PRESCALER;
	sctr = 0;
	data = space;
    power_reduction_enable = 0;
	power_mode_low = nullptr;
	power_mode_normal = nullptr;
	SCHEDULER_EVENT_HANDLER = this;
	timer_setup();
	timer_start();
	interrupt_global_enable();
}
scheduler::scheduler(scheduler_variables *space, uint16_t prsc){
	atomic_operation_status = 0;
	interrupt_block_status = 0;
	timer_block_status = 0;
	prescaler = prsc;
	sctr = 0;
	data = space;
	power_reduction_enable = 0;
	power_mode_low = nullptr;
	power_mode_normal = nullptr;
	SCHEDULER_EVENT_HANDLER = this;
	timer_setup();
	timer_start();
	interrupt_global_enable();
}
scheduler::~scheduler(void){
	
}

// MAIN
void scheduler::run(void){
	if(power_reduction_enable && (power_mode_normal != nullptr) && (power_mode_low != nullptr)){
		power_mode_normal();
		data[sctr].stack( data[sctr].arg );
		power_mode_low();
	}
	else{
		data[sctr].stack( data[sctr].arg );
	}
}
void scheduler::run(uint16_t prog){
	if(power_reduction_enable && (power_mode_normal != nullptr) && (power_mode_low != nullptr)){
		power_mode_normal();
		data[prog].stack( data[prog].arg );
		power_mode_low();
	}
	else{
		data[prog].stack( data[prog].arg );
	}
}
void scheduler::stack_lift(uint16_t pos_stop){
	interrupt_global_block_enable();
	uint16_t pos_lifting = sctr;
	while(pos_lifting > pos_stop){
		data[pos_lifting].stack = data[pos_lifting - 1].stack;
		data[pos_lifting].arg = data[pos_lifting - 1].arg;
		data[pos_lifting].time = data[pos_lifting - 1].time;
		pos_lifting--;
	}
	sctr++;
	interrupt_global_block_disable();
}
void scheduler::timer_overflow(void){
	interrupt_global_block_enable();
	if(sctr){
		do{
			sctr--;
			timer_set_next();
			interrupt_timer_clear();
			run();
		}while(sctr && (!data[(sctr ? sctr - 1 : 0)].time || interrupt_timer_check()));
	}
	interrupt_global_block_disable();
}
uint16_t scheduler::stack_find_pos(uint16_t value, uint16_t *time_diff_addr){
	uint16_t pos_search = sctr;
	uint32_t acctime = 0;
	uint32_t acctime_prev = 0;
 	stack_refresh();
	while( ( acctime <= value ) && pos_search){
		pos_search--;
		acctime_prev = acctime;
		acctime += data[pos_search].time;
	}
	if( pos_search ){
		*time_diff_addr = value - acctime_prev;
		pos_search++;
	}
	else{
		if( acctime <= value ){
			*time_diff_addr = value - acctime;
			return 0;
		}
		else{
			*time_diff_addr = value - acctime_prev;
			return 1;
		}
	}
	return pos_search;
}

// UTILS
void scheduler::stack_refresh(void){
	if(sctr){
		data[sctr - 1].time = (interrupt_timer_check() ? 0 : (time_get() ? timer_overflow_time() : 0 ));
	}
}
inline void scheduler::dump(void){
	sctr = 0;
}
uint16_t scheduler::timer_overflow_time(void){
	return (TIMER_MAX - time_get( )) + 1;
}
uint16_t scheduler::timer_overflow_time(uint16_t time){
	return (TIMER_MAX - time) + 1;
}

// DEPRECATED
void scheduler::push(void (*subr)(void *), void *param){
	interrupt_global_block_enable();
	data[sctr].stack = subr;
	data[sctr].arg = param;
	sctr++;
	interrupt_global_block_disable();
}
void scheduler::pop(void){
	sctr--;
	data[sctr].stack( data[sctr].arg );
}
