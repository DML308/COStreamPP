#ifndef _MAKE_FILE_READER_H
#define _MAKE_FILE_READER_H
#include "node.h"
compositeNode *makeFileReader(compositeCallNode *call);
operatorNode *makeFileReaderOper(compositeCallNode *call, idNode *out);
#endif