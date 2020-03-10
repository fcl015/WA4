#ifndef PERIODIC_TASK_H
#define  PERIODIC_TASK_H

class PeriodicTask {

    unsigned long taskInterval;
    unsigned long taskMillis;
    void (*task)(void);

  public:
    PeriodicTask(unsigned long, void (*taskPtr)(void));
    void RunHandler(void);

};

#endif
