#include <cstring>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstdbool>
#include <iostream>
#include "process.h"
#include "queue.h"
#include "memory.h"
#include <string>
using namespace std;

/* in1 file format:
  8           *integer value N that tells you how many processes are in the file
  1           *unique process id
  0 1000      *time it is submitted into the system(Arrival time) / lifetime in Memory
  1 400       *Address Space; first int gives the # of pieces of memory on the line
              Denotes the total size of the address space of the process
              Overall space req. = sum of the integers
  2
  0 2000
  2 200 400
  .
  .
  .
*/

//global variables
const int TIME_MAX = 100000; //max time
int numProcesses = 0;
int lastAnnouncement = -1;
Process* processList;
InputQueue* q;
FrameList* framelist;

char* getAnnouncement(int currentTime)
{
  char* resultTime;
  resultTime = (char*) malloc(20 * sizeof(char));

  if(lastAnnouncement == currentTime)
  {
    sprintf(resultTime, "\t");
  }
  else{
    sprintf(resultTime, "t = %d: ", currentTime);
  }

  lastAnnouncement = currentTime;

  return resultTime;
}

void enqueueNewProcess(int currentTime)
{
  Process* p;

  for(int i = 0; i < numProcesses; i++)
  {
    p = &processList[i];

    if(p->arrivalTime == currentTime)
    {
      printf("%sProcess %d arrives\n", getAnnouncement(currentTime),
              p->id);

      enqueueProcess(q, p);
      printFrameList(framelist);
    }
  }
}

void removeProcess(int currentTime)
{
  int timeSpent; //time spent in Memory
  Process* p;

  for(int i = 0; i < numProcesses; i++)
  {
    p = &processList[i];
    timeSpent = currentTime - p->timeAdded;

    if(p->active && (timeSpent >= p->lifeTime))
    {
      printf("%sProcess %d completes\n", getAnnouncement(currentTime),
              p->id);
      p->active = 0;
      p->timeFinished = currentTime;

      freeMemory(framelist, p->id);
      printFrameList(framelist);
    }
  }
}

void assignAvailableMem(int currentTime)
{
  int index, limit;
  Process* p;

  //inserts any processes that can be put in the Memory
  for(int i = 0; i < q->size; i++)
  {
    index = iterateQueueIndex(q, i);
    p = q->elements[index];

    if(processInMemory(framelist, p))
    {
      printf("%sMM moves Process %d to memory\n", getAnnouncement(currentTime),
              p->id);
      fitIntoMemory(framelist, p);

      p->active = 1;
      p->timeAdded = currentTime;

      dequeueAtIndex(q, i);
      printQ(q); //prints the input queue
      printFrameList(framelist);
    }
  }
}

void printTurnTime()
{
  float total = 0;

  for(int i = 0; i < numProcesses; i++)
  {
    total += processList[i].timeFinished - processList[i].arrivalTime;
  }

  printf("Average Turnaround Time: %2.2f\n", total / numProcesses);
}


void clearChars(char* c)
{
  if(c[strlen(c) - 1] != '\n')
  {
    int ch;
    while(((ch = getchar()) != '\n') && (ch != EOF));
  }
}


// stores the process info into a process array
Process* assignProcessList(string filename)
{

  int numSpace, nM1, nM2;
  int temp;
  int counter = 0;
  int totalSpace = 0;

  Process* processList = (Process*) malloc(numSpace * sizeof(Process));
  Process p;

  ifstream inFile;
  inFile.open(filename);

  if(!inFile.is_open())
  {
    cout << "Cannot open file";
    exit(1);
  }

  int n; //number of processes and memeory space
  //int totalnM;
  while(inFile >> n){

    cout << "The number of processes: " << n << endl;

    for(int i = 0; i < n; i++){

      // stores process info
      while(inFile >> processList[i].id >> processList[i].arrivalTime >> processList[i].lifeTime >> numSpace){

        std::cout << "Proc ID: " << processList[i].id << endl;
        std::cout << "Arrival Time: " << processList[i].arrivalTime << endl;
        std::cout << "Life Time: " << processList[i].lifeTime << endl;

      // get total memory requirements for process
        if(numSpace == 2){
          inFile >> nM1 >> nM2;

          totalSpace = nM1 + nM2;

          processList[i].memSpace = totalSpace;

        }

        if(numSpace == 1){

          inFile >> processList[i].memSpace;

        }

        std::cout << "Memory Space: " << processList[i].memSpace << endl;

        processList[i].active = 0;
        processList[i].timeAdded = -1;
        processList[i].timeFinished = -1;

      }

    }
  }

  inFile.close();

  return processList;
}

void promptFile(string filename)
{
  char c[100];
  FILE* f;

  printf("Input file: ");
  std::cin >> filename;

  assignProcessList(filename);

}

// prompts for memory size and page size
void getUserInput(int memSize, int pageSize, string filename)
{
  while(true)
  {
      printf("Memory size: ");
      std::cin >> memSize;
      if(memSize % 100 != 0)
      {
        printf("Error: Memory size is not a multiple of 100\n");
      }
      //*memSize = processNumericInput("Memory size: ", memSizeNum);
      std::cout << "Page size (1: 100, 2: 200, 3: 400)";
      std::cin >> pageSize;
      //*pageSize = processNumericInput("Page size (1: 100, 2: 200, 3: 400)", pageInputNum);

      switch (pageSize)
      {
        case 1: pageSize = 100;
          break;
        case 2: pageSize = 200;
          break;
        case 3: pageSize = 400;
          break;
      }

      if ((memSize) % (pageSize) == 0)
      {
        break;
      }

      printf("Error: Memory size must be a multiple of the page!");
      printf(" %d is not a multiple of %d, try again\n", memSize, pageSize);
    }

    promptFile(filename);
}


void mainLoop()
{
  long currentTime = 0;

  while(true)
  {
    //insert any processes that arrive in the queue
    enqueueNewProcess(currentTime);
    //remove any completed processes
    removeProcess(currentTime);
    //assign available memory to processes
    assignAvailableMem(currentTime);

    currentTime++;

    //checks if the current time is greater than the max time
    if(currentTime > TIME_MAX)
    {
      printf("Deadlock: max time is reached\n");
      break;
    }
    //checks if the queue size and frame list are empty
    if(q->size == 0 && frameListEmpty(framelist))
    {
      break;
    }
  }

  printTurnTime();
}

int main()
{

  int memSize = 0;
  int pageSize = 0;

  char* filename = (char*) malloc(100 * sizeof(char));

  //gets the user inputs
  getUserInput(memSize, pageSize, filename);
  //reads the values from the input file
  processList = assignProcessList(filename);
  //create a shared queue with the size of the number of processes
  q = createQueue(numProcesses);
  //create shared frame list
  framelist = createList(memSize / pageSize, pageSize);

  mainLoop();

  return 0;
}
