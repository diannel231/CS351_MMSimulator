#ifndef MEMORY_H
#define MEMORY_H
#include "process.h"
#include <cstdlib>
#include <cstdio>

struct Frame
{
  //1 if assigned to process otherwise 0
  int assigned;
  char location[40];
  int processAssign;
  int pageNum;
};

struct FrameList
{
  Frame* frames;
  int numFrames;
  int pageSize;
};

//creates frame list
FrameList* createList(int numFrames, int pageSize)
{
  FrameList *f;

  f = (FrameList*) malloc(sizeof(FrameList));

  f->frames = (Frame*)malloc(sizeof(Frame) * numFrames);
  f->pageSize = pageSize;
  f->numFrames = numFrames;

  for(int i = 0; i < f->numFrames; i++)
  {
    f->frames[i].assigned = 0;
  }

  return f;
}

//checks if the process can fit into memory
int processInMemory(FrameList* list, Process* p)
{
  int numFreeFrames = 0;

  for(int i = 0; i < list->numFrames; i++)
  {
    if(!list->frames[i].assigned)
    {
      numFreeFrames++;
    }
  }

  //if the number of free frames * the page size > the mem req.
  //for the process in question we can fit it in
  return (numFreeFrames * list->pageSize) >= p->memSpace;
}

//fits process into memory
void fitIntoMemory(FrameList* list, Process* p)
{
  int remainingMem, currentPage = 1;

  remainingMem = p->memSpace;

  for(int i = 0; i < list->numFrames; i++)
  {
    //if frame is not assigned
    if(!list->frames[i].assigned)
    {
      list->frames[i].assigned = 1; //assign it
      list->frames[i].pageNum = currentPage; //set the page number
      list->frames[i].processAssign = p->id; //set the process id

      currentPage++;
      remainingMem -= list->pageSize;
    }
  }

  if(remainingMem <= 0)
  {
    exit(1);
  }
}

//prints the frame list
void printFrameList(FrameList* list)
{
  int i, freeBlock = 0; //free blocks in memory page
  int begin;

  printf("\tMemory Map:\n");

  for(i = 0; i < list->numFrames; i++)
  {
    if(!freeBlock && list->frames[i].assigned)
    {
      freeBlock = 1;
      begin = i;
    }
    else if(freeBlock && list->frames[i].assigned)
    {
      freeBlock = 0;
      printf("\t\t%d-%d: Free frame(s)\n", begin * list->pageSize,
              (i * list->pageSize) - 1);
    }

    if(list->frames[i].assigned)
    {
      printf("\t\t%d-%d: Process %d, Page %d\n", i * list->pageSize,
              ((i+1) * list->pageSize) - 1, list->frames[i].processAssign,
              list->frames[i].pageNum);
    }
  }

  if(freeBlock)
  {
    printf("\t\t%d-%d: Free frame(s)\n", begin * list->pageSize,
            ((i) * list->pageSize) - 1);
  }
}

//returns 1 if frame list is empty
int frameListEmpty(FrameList* list)
{
  for(int i = 0; i < list->numFrames; i++)
  {
    if(list->frames[i].assigned)
    {
      return 0;
    }
  }

  return 1;
}

//puts the process in the frames if it is free
void freeMemory(FrameList* list, int id)
{
  Frame* frame;

  for(int i = 0; i < list->numFrames; i++)
  {
    frame = &list->frames[i];

    if(frame->processAssign == id)
    {
      frame->processAssign = 0;
      frame->pageNum = 0;
      frame->assigned = 0;
    }
  }
}

#endif
