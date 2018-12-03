#ifndef _LIST_H_
#define _LIST_H_

struct liststruct {
	void *element;
	struct liststruct *next;
};
typedef struct liststruct List;


#endif  /* ifndef _LIST_H_ */
