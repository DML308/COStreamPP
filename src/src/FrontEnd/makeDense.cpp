#include "unfoldComposite.h"
#include "compositeFlow.h"
#include "symboltableGenerate.h"
#include <string.h>
extern SymbolTable *runningTop,S,*top;
extern list<Node*> *Program;
extern sequentialNode *globalSequential;
extern vector<Node *> compositeCall_list;

compositeNode* UnfoldComposite::makeDenseLayer(layerNode *layer) {
    compositeNode *comp = NULL;
    Node *compHead = NULL, *compBody = NULL, *compInOut = NULL;
    Node *inputDecl = makeStream("in", "double");
    Node *outputDecl = makeStream("out", "double");
    list<Node *> *inputs_decl = new list<Node *>({inputDecl});
    list<Node *> *outputs_decl = new list<Node *>({outputDecl});
    ComInOutNode *inout = new ComInOutNode(inputs_decl, outputs_decl);
    compInOut = new ComInOutNode(inputs_decl, outputs_decl);
    compHead = new compHeadNode("denseLayer_" + to_string(layer->level), (ComInOutNode *)compInOut);
    list<Node *> *inputs_id=new list<Node *>(),*outputs_id=new list<Node *>();
    for(auto it : *outputs_decl){
        outputs_id->push_back(((inOutdeclNode *)it)->id);
    }
    for(auto it : *inputs_decl){
        inputs_id->push_back(((inOutdeclNode *)it)->id);
    }  
    compBody = makeDenseLayerBody((denseLayerNode *)layer, inputs_id, outputs_id);
    comp = new compositeNode((compHeadNode *)compHead, (compBodyNode *)compBody);
    S.InsertCompositeSymbol(comp->compName, comp);
    return comp;
}

Node* UnfoldComposite::makeDenseLayerBody(denseLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id) {
    Node *body = NULL;
    string streamName = "denseStream_" + to_string(layer -> level);
    list<Node *> *inputList = NULL;
    operatorNode *splitOperator = NULL, *joinOperator = NULL;
    // join operator的输入流
    list<Node *> *inputs_join = new list<Node *>();
    list<compositeCallNode *> *comCallList = new list<compositeCallNode *>();
    list<Node *> *compStmtList = new list<Node *>();

    idNode *streamDeclId = new idNode("x");
    primNode *streamType = new primNode("double");
    streamDeclId->valType = streamType->name;
    Node *streamDecl = new strdclNode(streamDeclId);

    splitOperator = makeSpecialSplitOperator(inputs_id->front(), layer -> cols, layer);
    inputList = splitOperator -> outputs;
    auto inputIter = inputList -> begin();
    for(auto input : *inputList) {
        ((strdclNode*)streamDecl)->id_list.push_back((idNode *)input);
    }
    compositeNode *childComp = makeDenseChildComp(layer);
    for (int i = 0; i < layer -> cols; i++) {
        string tempName = streamName + "_" + to_string(i);
        idNode *childOutput = new idNode(tempName);
        ((strdclNode*)streamDecl)->id_list.push_back(childOutput);
        inputs_join->push_back(childOutput);
        // child的输出流
        list<Node *> *call_outputs = new list<Node *>({childOutput});
        Node *arg = new constantNode("long long", (long long)i);
        Node *index = arg;
        list<Node *> *argList = new list<Node *>({arg});
        //compositeCall的输入流
        list<Node *> *call_inputs = new list<Node *>({*inputIter});
        compositeCallNode *call = new compositeCallNode(call_outputs, childComp->compName, argList, call_inputs, childComp);
        call->isOriginal = false;
        comCallList->push_back(call);
        inputIter++;
    }
    joinOperator = makeSpecialJoinOperator(outputs_id->front(), inputs_join);
    compStmtList->push_back(streamDecl);
    compStmtList->push_back(splitOperator);
    for (auto it : *comCallList)
    {
        compStmtList->push_back(it);
    }
    compStmtList->push_back(joinOperator);
    body = new compBodyNode(NULL, compStmtList);
    return body;
}

compositeNode* UnfoldComposite::makeDenseChildComp(denseLayerNode *layer) {
    compositeNode *comp = NULL;
    Node *compHead = NULL, *compBody = NULL, *compInOut = NULL;
    list<Node *> *inputs = new list<Node *>(), *outputs = new list<Node *>();
    Node* inputDecl = makeStream("input", "double");
    Node* inputId = ((inOutdeclNode *)inputDecl) -> id;
    inputs->push_back(inputDecl);
    Node* outputDecl = makeStream("output", "double");
    Node* outputId = ((inOutdeclNode *)outputDecl) -> id;
    outputs->push_back(outputDecl);
    compInOut = new ComInOutNode(inputs, outputs);
    list<Node *> *inputs_id=new list<Node *>(),*outputs_id=new list<Node *>();
    for(auto it : *outputs){
        outputs_id->push_back(((inOutdeclNode *)it)->id);
    }
    for(auto it : *inputs){
        inputs_id->push_back(((inOutdeclNode *)it)->id);
    } 
    string compName = "denseChildComp_" + to_string(layer->level);
    compHead = new compHeadNode(compName, (ComInOutNode *)compInOut);
    compBody = makeDenseChildCompBody(layer, inputs_id, outputs_id);
    comp = new compositeNode((compHeadNode *)compHead, (compBodyNode *)compBody);
    S.InsertCompositeSymbol(compName, comp);
    return comp;
}

Node* UnfoldComposite::makeDenseChildCompBody(denseLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id) {
    Node* body = NULL;
    list<Node *> *stmtList = new list<Node *>(), *paramList = new list<Node *>();
    Node *childIndex = new idNode("childIndex");
    ((idNode *)childIndex) -> valType = "int";
    paramList -> push_back(childIndex);
    paramNode *param = new paramNode(paramList);
    operatorNode *oper  = makeDenseChildOper(layer, inputs_id, outputs_id);
    stmtList -> push_back(oper);
    body = new compBodyNode(param, stmtList);
    return body;
}

operatorNode* UnfoldComposite::makeDenseChildOper(denseLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id) {
    string operName = "dense_" + to_string(layer -> level);
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;
    list<Node *> *winStmt = new list<Node *>();
    // 输入窗口
    auto inputIter = inputs_id->begin();
    Node *pop = new constantNode("long long", layer->rows);
    slidingNode *slid = new slidingNode(new list<Node *>({pop, pop}));
    winStmtNode *win1 = new winStmtNode(((idNode *)*inputIter)->name, slid);
    winStmt->push_back(win1);

    // 输出窗口
    auto outputIter = outputs_id->begin();
    Node *push = new constantNode("long long", (long long)1);
    tumblingNode *tumb = new tumblingNode(new list<Node *>({push}));
    winStmtNode *win2 = new winStmtNode(((idNode *)(*outputIter))->name, tumb);
    winStmt->push_back(win2);
    window = new windowNode(winStmt);

    idNode *idIterations = new idNode("iterations"); // 当前批次内的迭代次数
    idNode *idFlag = new idNode("flag"); // 标记访问哪一组权值矩阵
    constantNode *constZero = new constantNode("long long", (long long)0);
    Node *iterationsInit = new initNode(constZero);
    Node *flagInit = new initNode(constZero);
    idIterations -> init = iterationsInit;
    idFlag -> init = flagInit;
    primNode *primInt = new primNode("int");
    Node *decl = new declareNode(primInt, idIterations);
    ((declareNode *)decl)->id_list.push_back(idFlag);
    list<Node *> *stmtList = new list<Node *>({decl});

    init = makeDenseChildOperInit(layer);
    work = makeDenseChildOperWork(layer, inputs_id, outputs_id);
    body = new operBodyNode(stmtList, init, work, window);
    return new operatorNode(outputs_id, operName, inputs_id, body);
}

Node* UnfoldComposite::makeDenseChildOperInit(denseLayerNode *layer) {
    list<Node *> *stmts = new list<Node *>();
    Node *init =  NULL;
    Node *rows = new constantNode("long long", ((denseLayerNode *)layer) -> rows),
         *cols = new constantNode("long long", ((denseLayerNode *)layer) -> cols);
    // param id
    Node *childIndex = new idNode("childIndex");
    // 获得当前层的权值id
    string weightName = "_weight_" + to_string(layer-> level);
    Node*  weight0Id = new idNode(weightName + "_" + to_string(0));
    Node*  weight1Id = new idNode(weightName + "_" + to_string(1));
    ((idNode *)weight0Id)->isArray = 1;
    ((idNode *)weight1Id)->isArray = 1;
    // 利用for循环初始化
    constantNode *constZero = new constantNode("long long", (long long)0);
    idNode *idI = new idNode("i");
    
    primNode *prim = new primNode("int");
    declareNode *declI = new declareNode(prim, idI);
    stmts->push_back(declI);
    Node *initI = NULL, *condI = NULL, *nextI = NULL;
    
    initI = new binopNode((expNode *)idI, "=", (expNode *)constZero);
    condI = new binopNode((expNode *)idI, "<", (expNode *)(rows));
    nextI = new unaryNode("POSTINC", (expNode *)idI);

    Node *weightArrDec = new arrayNode((expNode *)idI);
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)childIndex);

    // 获取weight_level_flag[i][j]
    (static_cast<idNode*>(weight0Id))->arg_list = (static_cast<arrayNode*>(weightArrDec))->arg_list;
    (static_cast<idNode*>(weight1Id))->arg_list = (static_cast<arrayNode*>(weightArrDec))->arg_list;
    
    //初始化weight0
    Node *stmt1 = new binopNode((expNode *)weight0Id, "=", (expNode *)globalSequential->getInitializer());
    Node *forNode1 = new forNode(initI, (expNode *)condI, (expNode *)nextI, stmt1);
    stmts->push_back(forNode1);
    //初始化weight1
    Node *stmt2 = new binopNode((expNode *)weight1Id, "=", (expNode *)globalSequential->getInitializer());
    Node *forNode2 = new forNode(initI, (expNode *)condI, (expNode *)nextI, stmt2);
    stmts->push_back(forNode2);

    // 初始化bias
    if (((denseLayerNode *)layer) -> use_bias) {
        string biasName = "_bias_" + to_string(layer-> level);
        Node*  bias0Id = new idNode(biasName + "_" + to_string(0));
        Node*  bias1Id = new idNode(biasName + "_" + to_string(1));
        ((idNode *)bias0Id) -> isArray = 1;
        ((idNode *)bias1Id) -> isArray = 1;
        ((idNode *)bias0Id) -> arg_list = *(new list<Node *>({childIndex}));
        ((idNode *)bias1Id) -> arg_list = *(new list<Node *>({childIndex}));
        Node *biasInit0 = new binopNode((expNode *)bias0Id, "=", (expNode *)globalSequential->getInitializer());
        Node *biasInit1 = new binopNode((expNode *)bias1Id, "=", (expNode *)globalSequential->getInitializer());
        stmts->push_back(biasInit0);
        stmts->push_back(biasInit1);
    }
    
    init  = new blockNode(stmts);
    return init;
}

Node* UnfoldComposite::makeDenseChildOperWork(denseLayerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    /* 
        if (flag) {
            for (i = 0; i< rows; i++) {
                temp += In0[i].x * weight_level_1[i][childIndex];
            }
            Out0[childIndex].x = temp [+ bias_level_1 ];
        } else {
            for (i = 0; i< rows; i++) {
                temp += In0[i].x * weight_level_0[i][childIndex];
            }
            Out0[childIndex].x = temp [ + bias_level_1 ];
        }
        iterations++;
        if( iterations == globalSequntial->batch) {
            flag != flag;
            iterations = 0;
        }
    */
    Node* work = NULL;
    list<Node *> *stmtList = new list<Node *>();
    primNode *primInt = new primNode("int"),
             *primDouble = new primNode("double");
             // param id
    Node *childIndex = new idNode("childIndex");
    idNode *idI = new idNode("i"),
           *idTemp = new idNode("temp");
    Node *declInt = new declareNode(primInt, idI),
         *declDouble = new declareNode(primDouble, idTemp);
    stmtList -> push_back(declInt);
    stmtList -> push_back(declDouble);
    Node *constZero = new constantNode("long long", (long long)0);
    Node *tempInit = new binopNode((expNode *)idTemp, "=", (expNode *)constZero);
    stmtList -> push_back(tempInit);

    Node *rows = new constantNode("long long", ((denseLayerNode *)layer) -> rows);

    string weightName = "_weight_" + to_string(layer-> level);
    Node*  weight0Id = new idNode(weightName + "_" + to_string(0));
    Node*  weight1Id = new idNode(weightName + "_" + to_string(1));
    ((idNode *)weight0Id)->isArray = 1;
    ((idNode *)weight1Id)->isArray = 1;

    Node *initI = new binopNode((expNode *)idI, "=", (expNode *)constZero);
    Node *condI = new binopNode((expNode *)idI, "<", (expNode *)rows);
    Node *nextI = new unaryNode("POSTINC", (expNode *)idI);

    // 取得_weight_level_flag[i][j]
    Node *weightArrDec = NULL;
    weightArrDec =  new arrayNode((expNode *)(idI));
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)(childIndex));
    (static_cast<idNode*>(weight0Id))->arg_list = (static_cast<arrayNode*>(weightArrDec))->arg_list;
    (static_cast<idNode*>(weight1Id))->arg_list = (static_cast<arrayNode*>(weightArrDec))->arg_list;

    // 取得In1[i].x
    idNode* x = new idNode("x");
    idNode* input1 = new idNode(static_cast<idNode *>(inputs->front())->name);
    input1->isArray = 1;
    input1->arg_list.push_back(idI);
    Node* In1 = new binopNode((expNode *)input1, ".", (expNode *)x);

    // 取得Out[j].x
    idNode* output = new idNode(static_cast<idNode *>(outputs->front())->name);
    output->isArray = 1;
    output->arg_list.push_back(constZero);
    Node *outX = new binopNode((expNode *)output, ".", (expNode *)x);
    Node *res0, *res1;

    if (((conv2DLayerNode *)layer) -> use_bias) {
        string biasName = "_bias_" + to_string(layer -> level);
        Node *bias0 = new idNode(biasName + "_" + to_string(0));
        Node *bias1 = new idNode(biasName + "_" + to_string(1));
        res0 = new binopNode((expNode *)outX, "=", (expNode *)(new binopNode((expNode *)idTemp, "+", (expNode *)bias0)));
        res1 = new binopNode((expNode *)outX, "=", (expNode *)(new binopNode((expNode *)idTemp, "+", (expNode *)bias1)));
    } else {
        Node *res = new binopNode((expNode *)outX, "=", (expNode *)idTemp);
        res0 = res;
        res1 = res;
    }

    // 取得temp+=In1[i].x*_weight_level_flag[i][j];
    Node* midRes0 = new binopNode((expNode *)idTemp, "+=", (expNode *)(new binopNode((expNode *)In1, "*", (expNode *)weight0Id)));
    Node* midRes1 = new binopNode((expNode *)idTemp, "+=", (expNode *)(new binopNode((expNode *)In1, "*", (expNode *)weight1Id)));

    Node *block1 = new blockNode(new list<Node *>({midRes1}));
    Node *forNode1 = new forNode(initI, (expNode *)condI, (expNode *)nextI, block1);
    
    Node *block0 = new blockNode(new list<Node *>({midRes0}));
    Node *forNode0 = new forNode(initI, (expNode *)condI, (expNode *)nextI, block0);
    
    Node *idFlag = new idNode("flag");
    Node *ifBlock = new blockNode(new list<Node *>({forNode1, res1}));
    Node *elseBlcok = new blockNode(new list<Node *>({forNode0, res0}));
    Node *ifElse = new ifElseNode((expNode *)idFlag, ifBlock, elseBlcok);
    stmtList->push_back(ifElse);

    Node *idIterations = new idNode("iterations");
    Node *iterationsInc = new unaryNode("POSTINC", (expNode *)idIterations);
    stmtList->push_back(iterationsInc);
    
    Node *compare = new binopNode((expNode *)idIterations, "==", ((expNode *)globalSequential->batch));
    Node *charge = new binopNode((expNode *)idFlag, "!=", (expNode *)idFlag);
    Node *reset = new binopNode((expNode *)idIterations, "=", (expNode *)constZero);
    Node *iterationsIfBlock = new blockNode(new list<Node *>({charge, reset}));
    Node *iterationsIf = new ifNode((expNode *)compare, iterationsIfBlock);
    stmtList->push_back(iterationsIf);
    
    work = new blockNode(stmtList);
    return work;
}