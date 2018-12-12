#include <list>
#include "node.h"
#include "defines.h"
extern list<Node*> *Program;
extern compositeNode *gMainComposite;

class SemCheck{
public:
    SemCheck(){}
    ~SemCheck(){
        
    }
    void findMainComposite(list<Node*> *program);
    
};