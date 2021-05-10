#ifndef PROCESS_H
#define PROCESS_H

// struct of process information
struct Process
{
  int id;
  int arrivalTime; //time the process is submitted into the system
  int lifeTime;
  int memSpace;

  int timeAdded;
  int active;
  int timeFinished;
};
#endif
