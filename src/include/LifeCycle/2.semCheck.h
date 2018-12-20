#include "node.h"
#include "defines.h"
extern list<Node*> *Program;
extern compositeNode *gMainComposite;

class SemCheck{
public:
    SemCheck(){}
    ~SemCheck(){

    }
    static void findMainComposite(list<Node*> *program);
    
};