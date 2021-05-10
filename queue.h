#ifndef QUEUE_H
#define QUEUE_H
#include "process.h"
#include <cstdlib>
#include <cstdio>

struct InputQueue
{
  int capacity;
  int size;
  int front;
  int rear;
  Process **elements;
};

//creates queue
InputQueue* createQueue(int length)
{
  InputQueue *q;
  q = (InputQueue*) malloc(sizeof(InputQueue));

  q->elements = (Process**) malloc(sizeof(Process) * length);
  q->capacity = length;
  q->size = 0;
  q->front = 0;
  q->rear = -1;

  return q;
}

//inserts an process into queue
void enqueueProcess(InputQueue* q, Process *p)
{
  if(q->size == q->capacity)
  {
    printf("Error: queue is full\n");
    exit(2);
  }

  q->size++;
  q->rear = q->rear + 1;
  if(q->rear == q->capacity)
  {
    q->rear = 0;
  }

  q->elements[q->rear] = p;
}

//gets next process in queue
int queueNext(InputQueue* q)
{
  //if queue size is 0, return 0, else return 1
  return q->size == 0 ? 0 : 1; //conditional expression
}

//gets the first process in the queue
Process* peekQueue(InputQueue* q)
{
  if(!queueNext(q))
  {
    printf("Error: queue is empty, can't get the next element\n");
    exit(2);
  }

  return q->elements[q->front];
}

//gets the process at a certain index of the queue
Process* peekQueueAtIndex(InputQueue* q, int index)
{
  return q->elements[index];
}

//remove process from the queue
void dequeueProcess(InputQueue* q)
{
  if(!queueNext(q))
  {
    printf("Error: queue is empty, can't dequeue anything\n");
    exit(2);
  }

  q->size--;
  q->front++;

  if(q->front == q->capacity)
  {
    q->front = 0;
  }
}

//loops through queue at an index
int iterateQueueIndex(InputQueue* q, int index)
{
  return q->front + index;
}

//removes process at a certain index in queue
void dequeueAtIndex(InputQueue* q, int index)
{
  int prev = 0;

  for(int i = 0; i < q->size; i++)
  {
    if(i > index)
    {
      q->elements[prev] = q->elements[i];
    }

    prev = i;
  }

  q->size--;
  q->rear--;
}

//displays the input queue
void printQ(InputQueue* q)
{
  Process* p;
  printf("\tInput queue:[ ");
  for(int i = 0; i < q->size; i++)
  {
    p = peekQueueAtIndex(q, iterateQueueIndex(q, i));

    printf("%d ", p->id);
  }
  printf("]\n");
}

#endif
