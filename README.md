# Scheduler

This is a simple scheduler for embedded systems.  
  
All the scheduler logic goes into the **Core** folder,  
while all the hardware that supports it goes into the **Devices** folder.  
  
To use the scheduler, you should place in the same folder the files:  
(Core/) scheduler_*xx*.h  
(Core/) scheduler_*xx*.cpp  
(Devices/"your_device"/) config.h  
(Devices/"your_device"/) hardware.cpp  
Where *xx* is the size (in bits) of the hardware timer.  
  
You should `#include` the `scheduler_xx.h` header.  
In your main, create an array of `scheduler_variables`, then create the `scheduler` by calling its constructor with the first argument being the (address of the) array of `scheduler_variables` you just created.  
  
You can change the timer prescaler by `#define SCHEDULER_PRESCALER` (**before** including `scheduler_xx.h`), and writing the value as the second argument to the `scheduler` constructor.  
If you are not using the *deafult* clock frequency on your CPU, you need to `#define F_CPU` (also **before** including `scheduler_xx.h`).  
  
To `schedule` some task, *wrap* it in a `void function(void *)`.  
Then `schedule( the function, with the (void *) arguments to the function, and the delay);`  
  
You may create a `struct` with all the arguments a function uses, and when `schedule`ing it, pass the `(void *) &of_the_struct` as the second argument to the `schedule` function.  In the wrapper, re-cast your argument struct address (from `void *` to its original format (pointer)).  
  
To make your life easier, you can use the macro `SCHEDULER_CLOCKS_PER_SEC` to transform seconds to the delay value used as the third argument when `schedule`ing some task.
