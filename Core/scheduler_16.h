#ifndef scheduler_16
#define scheduler_16

//Scheduler Header File
//Version:		1.0

#include "scheduler/config.h"

#define TIMER_MAX 0xFFFF
#define SCHEDULER_CLOCKS_PER_SEC uint16_t(F_CPU/SCHEDULER_PRESCALER)

typedef void (*scheduler_stack_t)(void *);

struct scheduler_variables{
	scheduler_stack_t stack;									//stack level. Stores the function to call on timer overflow
	uint16_t time;												//stores the relative time to overflow
	void *arg;													//pointer to arguments used by function stored on stack
};

/* Function definiiron file:
 * MAIN+			scheduler_16.cpp
 * MAIN				scheduler_16.cpp
 * UTILS			scheduler_16.cpp
 * HARDWARE			hardware.cpp
 * INTERRUPTS		hardware.cpp
 * DEPRECATED		scheduler_16.cpp / hardware.cpp
 */

class scheduler{
public:
//	VARIABLES
	uint8_t temp_status_register;								//DEPRECATED stores status register, to use with atomig operation
	uint8_t atomic_operation_status;							//DEPRECATED indicates if an atomic operation in running
	uint8_t interrupt_block_data;								//store global interrupt status (enabled / disabled)
	uint8_t timer_block_data;									//store timer and prescaler control register
	uint8_t power_reduction_enable;								//enables power reduction mode
	uint16_t prescaler;											//prescaler value (1024, 512...) (default 1024)
	uint16_t sctr;												//stack counter
	uint16_t interrupt_block_status;							//counts the blocks to global interrupts
	uint16_t timer_block_status;								//counts the blocks to the timer
	void (*power_mode_low)(void);								//function to call when power reduction mode is enabled
	void (*power_mode_normal)(void);							//function to call when power reduction mode is enabled
	scheduler_variables *data;									//pointer to stack

//	MAIN+
	void schedule(void (*)(void *), void *, uint16_t);			//schedule function call
	scheduler(scheduler_variables *);							//constructor
	scheduler(scheduler_variables *, uint16_t);					//constructor
	~scheduler(void);											//destructor

//	MAIN
	void run(void);												//call TOS function
	void run(uint16_t);											//call function stored on stack
	void stack_lift(uint16_t);									//lifts stack to insert function
	void timer_overflow(void);									//function which handles timer overflow
	uint16_t stack_find_pos(uint16_t, uint16_t *);				//return stack position based on first argument, and moves to pointer target the time difference

//	UTILS
	void dump(void);											//clear the stack
	void stack_refresh(void);									//refresh TOS time
	uint16_t timer_overflow_time(void);							//returns time to overflow
	uint16_t timer_overflow_time(uint16_t);						//returns time to overflow starting from argument value

//	HARDWARE
	void timer_setup(void);										//timer setup
	void timer_start(void);										//starts the timer
	void timer_stop(void);										//stops the timer
	void timer_set_next(void);									//loads the timer with the next stack time
	void timer_set_overflow(uint16_t);							//set timer to overflow in "argument" units
	void timer_block_enable(void);								//adds a block to the timer
	void timer_block_disable(void);								//removes a block to the timer and, if no blocks remain, restore timer state
	uint16_t time_get(void);									//returns raw timer value

//	INTERRUPTS
	void interrupt_global_enable(void);							//enable global interrupts
	void interrupt_global_disable(void);						//disable global interrupts
	void interrupt_global_block_enable(void);					//adds a block to global interrupts
	void interrupt_global_block_disable(void);					//removes a block to global interrupts and, if no blocks remain, restore global interrupts value
	void interrupt_timer_clear(void);							//clears the overflow interrupt flag
	void interrupt_timer_enable(void);							//enables timer overflow interrupt
	void interrupt_timer_disable(void);							//disables timer overflow interrupt
	uint8_t interrupt_timer_check(void);						//returns 0 if timer overflow flag is low

//	DEPRECATED
	void push(void (*)(void *), void *);						//push a function into stack
	void pop(void);												//run the TOS function and remove it from the stack
	void atomic_operation_begin(void);							//starts an atomic operation. Use interrupt_global_block_enable instead.
	void atomic_operation_end(void);							//ends an atomic operation. Use interrupt_global_block_disable instead.
};

#endif
