#ifndef _DATAFLOW_H_
#define _DATAFLOW_H_

typedef struct {
  bool undefined;
  union {
    unsigned long  bitvector;
    //List *list;
    void *ptr;
  } u;
} FlowValue;

#endif