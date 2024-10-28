#ifndef Tasks_h 
#define Tasks_H

// internal helper macros
#define TIMES_UP_MILLIS(LastTime, Delay) (millis() - LastTime >= (unsigned long)(Delay))

#define TIMES_UP_MICROS(LastTime, Delay) (micros() - LastTime >= (unsigned long)(Delay))

// gate macros
#define RUN_ALWAYS() { }

#define RUN_NEVER() { return; }

#define RUN_IF(Expr) { if (not Expr) return; }

#define RUN_EVERY_MILLISECONDS(MilliSeconds) \
  { \
  static unsigned long prevTime = 0; \
  if (not TIMES_UP_MILLIS(prevTime, MilliSeconds)) return; \
  prevTime =  millis(); \
  }
  
#define RUN_EVERY_MICROSECONDS(MicroSeconds) \
  { \
  static unsigned long prevTime = 0; \
  if (not TIMES_UP_MICROS(prevTime, MicroSeconds)) return; \
  prevTime =  micros(); \
  }  

// RUN_ON_PIN states:
// 1 - waiting for initial pin condition
// 2 - waiting for debounce timeout
// 3 - ok to run the task

#define RUN_ON_PIN(InputPin, DebounceDelay, Inverted) \
{ \
  static int State = 1; \
  bool Value; \
  static int Delay = 0; \
  static unsigned long DebounceStartTime = millis(); \
  if (State == 2) \
    if (not TIMES_UP_MILLIS(DebounceStartTime, Delay)) \
      return; \
  Value = digitalRead(InputPin); \
  switch (State) { \
    case 1: \
      if (Value == Inverted) \
      { \
        State = 2; \
        Delay = DebounceDelay; \
        DebounceStartTime = millis();   \
      }   \
      return; \
    case 2: \
       Delay = 0; \
      if (Value == Inverted) \
      { \
        State = 3; \
        break; \
      } \
      State = 1; \
      return; \
    case 3: \
      if (Value == Inverted) \
        return; \
      Delay = 0; \
      State = 1; \
      return; \
  } \
}

// external helper macros
// transition from state to state in a co-routine
#define CHANGE_STATE(_Step, _Delay) {Step=_Step; Delay=_Delay; return;}

#endif