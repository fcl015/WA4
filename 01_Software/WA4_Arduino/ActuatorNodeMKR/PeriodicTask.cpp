#include "PeriodicTask.h"
#include <Arduino.h> 

//---------------------------------------------------------
// PeriodicTask class functions
//---------------------------------------------------------
PeriodicTask::PeriodicTask(unsigned long interval, void (*taskPtr)(void)) {
  this->taskInterval = interval;
  this->task = taskPtr;
}
void PeriodicTask::RunHandler() {

  if (millis() - taskMillis > taskInterval) {
    task();
    taskMillis = millis();
  }
  
}
