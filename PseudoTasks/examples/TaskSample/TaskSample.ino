 
#include "PseudoTasks.h"

void 
setup(void) 
{
  Serial.begin(115400);
  Serial.println("Starting...");
  
  pinMode(7, INPUT_PULLUP);
  
}

/*
  Example 1 - Blink
*/

void 
Blink()
{
  RUN_EVERY_MILLISECONDS(500);
  
  static bool ledState = false;

  // this will stuff up the serial output on the esp8266, so don't put it in the setup!
  pinMode(4, OUTPUT);    
  
  ledState = not ledState;
  digitalWrite(4, ledState);
  
  Serial.print("Blink:");
  Serial.println(ledState);  
}

/*
  Example 2 - run when a button is pressed. 

  Argument 1: Pin ID - appropriate mode should already be set.
  Argument 2: Debounce timeout in milliseconds
  Argument 3: true for Normally Open, false for Normally closed.
 */

void 
CheckInput()
{
  RUN_ON_PIN(7, 10, true);

  // now do the work!!!
  Serial.println("button pressed");
}

/* 
  Example 3 - a multi step function. 
 
  This is how to structure stuff that uses delays. Set up a switch. Make the first bit of code 'State 1'. each time a 
  delay is found, make it a new state, put the code into the appropriate case, set the delay variable and return.
  
    The old way of writing the following function looks like this...
  
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
 
*/
 
void
MultiStep(void)
{
  static int Step = 0;
  static int Delay = 0;

  RUN_EVERY_MILLISECONDS(Delay);

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

// list all tasks here

void 
loop(void) 
{
  Blink();
  CheckInput();
  MultiStep();
}

