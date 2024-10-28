# The why, what and how of PseudoTasks


## 1. Why?

When programs grow in complexity and a level of concurrency is required, preemptive multitasking kernels (such as RTOS) are often seen as a solution but Preemptive Multitasking is fraught with issues.
  - code needs to be re-entrant
  - resources need to be protected from contention
  - race conditions need to be avoided
  - priorities need to be managed
  - debugging becomes very difficult
  - there is an addition time and space (and financial) cost

If you don't understand the implications of all the above, for your own sake,
please don't use preemptive multitasking!
 
Arduino code is simple enough (compared to (say) MS Windows) that we can avoid
using preemptive multitasking in most cases through sensible program structure
and co-routines.


# 2. What

PseudoTasks are implemented as a set of #defined 'gate' macros in PseudoTasks.h.
(PseudoTasks.cpp just includes PseudoTasks.h). Gate macros are placed at the
start of a task and determine when to run the task.

**Gate Macros:**

**RUN_ALWAYS**                                      
>the task is always run

**RUN_NEVER**                                       
>the task is never run (useful for disabling a task while leaving the code in place

**RUN_IF(Expr)**                                    
>the task is run if the expression is true

**RUN_EVERY_MILLISECONDS(MilliSeconds)**            
>the task is run if at least MilliSeconds has elapsed. NOTE: elapsed time since 
the last time the task was started, not time is was completed.
 
**RUN_EVERY_MICROSECONDS(MicroSeconds)**            
>same as for RUN_EVERY_MILLISECONDS, except the parameters is microseconds 
rather then milliseconds.
 
**RUN_ON_PIN(InputPin, DebounceDelay, Inverted)**
>the task is run depending on the state of InputPin. High if inverted is false, 
Low if inverted is true. For a simple check of the pin, set
DebounceDelay to 0. If the DebounceDelay is greater then 0, the pin is
read a twice and the task only run if the value is unchanged. This filters
out simple switch bouncing but might not be adequate in all cases.


# 3A. How to do short tasks

1. Write each of your 'tasks' as a function.
2. Call each of your functions from loop() like this:

```C++
    void loop()
    {
        task1();
        task2();
        task3();
        task4();
        task5();
    }
``` 

3. For each task, use the gate macros to determine if the task is ready to run.

```C++
void task1()
{
  RUN_ALWAYS();
  Serial.println("Task 1 always runs");
}

void task2()
{
  RUN_NEVER();
  Serial.println("Task 2 never runs");
}

void task3()
{
  RUN_EVERY_MILLISECONDS(500);
  Serial.println("Task 3 runs every half a second");
}

void task4()
{
  RUN_EVERY_MICROSECONDS(50);
  Serial.println("Task 4 runs lots but there is a delay "
    "of 50 microseconds between runs");
}

void task5()
{
  RUN_ON_PIN(D5, 0, true)   
  Serial.println("Task 5 runs if Pin D5 is low");
}
```

# 3B. How to do long tasks

Long tasks need to be structured so that they don't block the other tasks. Long
tasks are usually long because they have embedded delays. One way to manage 
this situation is to use co-routines.


From google:

>A coroutine is a special type of function that can suspend and resume its 
execution at specific points, allowing it to yield control to other 
coroutines or the main program. This cooperative multitasking approach 
enables efficient and lightweight concurrency, making coroutines a popular 
choice for implementing asynchronous programming, event-driven systems, and 
concurrent computations.

Co-routines can be implemented as a state transition engine using a switch 
statement. This is easier to do than to describe but here goes...

Consider the following function:

```C++
void
MultiStep(void)
{
  Serial.println("Step 0");
  delay(100);
  Serial.println("Step 1"); 
  delay(1000);     
  Serial.println("Step 2");
  delay(5000);     
  Serial.println("Step 3");
  delay(50);     
}
```

This function will take at least 6.6 seconds to run and will block all other 
tasks for this duration. To implement this as a co-routine:

1. Identify each delay and think of the code between delays as a transition 
   between states.
2. Create and set a state variable to 0. We change this value as we change 
   states.
3. Create and set a delay vairalbe to 0. We delay by this amount each time we 
   re-enter the function.
3. Create a switch statement on the state variable with a case for each state.
4. Put the transition code in each case block. Set the next step appropriately. 
   Calls to delay() are replaced by code to set the desired delay and a return.
   This allows other tasks to run other tasks while we are delayed.

New code:

```C++
void
MultiStep(void)
{
  static int Step = 0;
  static int Delay = 0;

  RUN_EVERY(Delay);

  switch (Step) {
    case 0:
        Serial.println("Step 0");
        CHANGE_STATE(1, 100);
    case 1:
        Serial.println("Step 1"); 
        CHANGE_STATE(2, 1000);        
    case 2:
        Serial.println("Step 2");
        CHANGE_STATE(3, 5000);                
    case 3:
        Serial.println("Step 3");
        CHANGE_STATE(1, 50);                        
  }
}
```

I hope this has been of some help. 
```
