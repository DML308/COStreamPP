#include "makeFileReader.h"

compositeNode *makeFileReader(compositeCallNode *call) {
    compositeNode *comp;
    idNode *idOut = new idNode("out"), *idX = new idNode("x");
    idX -> valType = "double";
    strdclNode *strDecl = new strdclNode(idX);
    inOutdeclNode *inOutDecl = new inOutdeclNode(strDecl, idOut);
    ComInOutNode *inout = new ComInOutNode(NULL, new list<Node *>({inOutDecl}));

    compHeadNode *head = new compHeadNode(call->compName, inout);

    operatorNode *oper = makeFileReaderOper(call, idOut);
    list<Node *> *stmtList = new list<Node *>({oper});

    idNode *idFilename = new idNode("filename"); // 文件名
    idNode *idNum = new idNode("num"); // 窗口滑动大小
    idFilename->valType = "string";
    idNum->valType = "int";
    list<Node *> *paramList = new list<Node *>({idFilename, idNum});
    paramNode *param = new paramNode(paramList);
    compBodyNode *body = new compBodyNode(param, stmtList);
    comp = new compositeNode(head, body);
    return comp;
}

operatorNode *makeFileReaderOper(compositeCallNode *call, idNode *out) {
    string operName = "fileReader";
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;
    list<Node *> *winStmt = new list<Node *>();

    // 输出窗口
    tumblingNode *tumb = new tumblingNode(new list<Node *>({call -> stream_List -> back()}));
    winStmtNode *win = new winStmtNode(out->name, tumb);
    winStmt->push_back(win);
    window = new windowNode(winStmt);

    // idNode *idFilename = new idNode("filename"); // 文件名
    // idNode *idNum = new idNode("num"); // 窗口滑动大小
    // Node *filenameInit = new initNode(call->stream_List->front());
    // Node *numInit = new initNode(call->stream_List->back());
    // idFilename -> init = filenameInit;
    // idNum -> init = numInit;
    // primNode *primStr = new primNode("string");
    // primNode *primInt = new primNode("int");
    // Node *declStr = new declareNode(primStr, idFilename);
    // Node *declInt = new declareNode(primInt, idNum);
    // list<Node *> *stmtList = new list<Node *>({declStr, declInt});
    work = new blockNode(new list<Node *>());
    body = new operBodyNode(NULL, NULL, work, window);
    return new operatorNode(new list<Node *>({out}), operName, NULL, body);
}