#ifndef scheduler_16
#define scheduler_16

//Scheduler Header File
//Version:		1.0

#include "scheduler/config.h"

#define TIMER_MAX 0xFFFF
#define SCHEDULER_CLOCKS_PER_SEC uint16_t(F_CPU/SCHEDULER_PRESCALER)

typedef void (*scheduler_stack_t)(void *);

struct scheduler_variables{
	scheduler_stack_t stack;									//define o ponteiro para onde vai ser salva a função
	uint16_t time;												//hora de executar
	void *arg;													//define o ponteiro onde serão salvos os argumentos
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
	uint8_t temp_sreg;											//salva o SREG inteiro
	uint8_t atomic_operation_status;							//verifica se ja está em operação atomica
	uint8_t interrupt_block_data;								//salva o SREG.I
	uint8_t timer_block_data;									//salva o TCCR1B
	uint8_t power_reduction_enable;								//permite chamar uma função para reduzir o consumo de enrgia
	uint16_t prescaler;											//valor do prescaler (1024, 512...) (padrão 1024)
	uint16_t sctr;												//contador do stack
	uint16_t interrupt_block_status;							//salva se ja está em modo de bloqueio
	uint16_t timer_block_status;								//salva se o timer ja está bloqueado
	void (*power_mode_low)(void);								//função chamada quando power_reduction_enable está setado
	void (*power_mode_normal)(void);							//função chamada quando power_reduction_enable está setado
	scheduler_variables *data;									//ponteiro pra onde ficam salvos os dados

//	MAIN+
	void schedule(void (*)(void *), void *, uint16_t);			//programa execucao da funcao
	scheduler(scheduler_variables *);							//construtor
	scheduler(scheduler_variables *, uint16_t);					//construtor
	~scheduler(void);											//destrutor

//	MAIN
	void run(void);												//executa a funcao sem variar o sctr
	void run(uint16_t);											//executa uma funcao da pilha
	void stack_lift(uint16_t);									//levanta a pilha pra inserir a funcao nela
	void timer_overflow(void);									//funcao chamada na interrupção do timer
	uint16_t stack_find_pos(uint16_t, uint16_t *);				//funcao para encontrar o lugar de inserir a funcao na pilha e devolve tambem a diferença de tempo na pilha

//	UTILS
	void dump(void);											//zera o ponteiro da pilha
	void stack_refresh(void);									//atualiza o tempo do TOS
	uint16_t timer_overflow_time(void);							//retorna o tempo para overflow a partir de agora
	uint16_t timer_overflow_time(uint16_t);						//retorna o tempo para overflow a partir do argumento

//	HARDWARE
	void timer_setup(void);										//configura o timer1
	void timer_start(void);										//descongela o timer
	void timer_stop(void);										//congela o timer
	void timer_set(uint16_t);									//carrega o timer com o argumento
	void timer_set_next(void);									//configura o timer pra proxima execucao
	void timer_block_enable(void);								//aumenta um bloqueio do timer
	void timer_block_disable(void);								//tira um dos bloqueios do timer
	uint16_t time_get(void);									//retorna o valor do timer

//	INTERRUPTS
	void interrupt_global_enable(void);							//habilita interrupções globais
	void interrupt_global_disable(void);						//desabilita interrupções globais
	void interrupt_global_block_enable(void);					//bloqueia interrupções salvando o estado de SREG.I
	void interrupt_global_block_disable(void);					//desbloqueia interrupções, e devolve o estado de SREG.I
	void interrupt_timer_clear(void);							//clears the overflow interrupt flag
	void interrupt_timer_enable(void);							//habilita a interrupção por overflow do timer1
	void interrupt_timer_disable(void);							//desabilita a interrupção por overflow do timer1
	uint8_t interrupt_timer_check(void);						//retorna se o timer1 estourou

//	DEPRECATED
	void push(void (*)(void *), void *);						//adiciona uma função à pilha
	void pop(void);												//força a execução da função do topo da pilha e tira ela de lá
	void atomic_operation_begin(void);							//desabilita interrupção pra azer operação atomica
	void atomic_operation_end(void);							//habilita interrupção pra fazer operação atomica
};

#endif
