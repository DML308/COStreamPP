#include "unfoldComposite.h"
#include "compositeFlow.h"
#include <string.h>
extern SymbolTable S;
extern list<Node*> *Program;
extern squentialNode *globalSquential;
extern vector<Node *> compositeCall_list;

compositeNode *UnfoldComposite::UnfoldSquential(squentialNode *node) {
    globalSquential = node;
    compositeCallFlow(node->body_stmts); // 将通过add加入的层,依次push到compositeCall_list中
    vector<compositeCallNode *> comCallList; // 用于存储展开后的compositeCallNode
    compositeNode *squential = NULL;
    string streamName = "Sstream";
    static int num = 0;
    string comName = MakeCompositeName("squential");
    list<Node *> *inputs = node->inputs;
    list<Node *> *outputs = node->outputs;
    list<Node *> *arg_list = node->arg_list;
    list<Node *> *comp_stmts = new list<Node *>();
    cout<<"Unfold squential"<<endl;
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    compHeadNode *head = new compHeadNode(comName, inout);
    compBodyNode *body = NULL;
    SymbolTable *top = new SymbolTable(NULL);
    // squential有两个输入流, 分别是是训练集和标签
    assert(inputs != NULL && outputs != NULL);
    int levelNum = compositeCall_list.size();
    int currentLevel = 0;
    assert(levelNum != 0);
    // assert(((constantNode *)(node->arg_list->front()))->style == "integer");
    // 将层连接起来
    for (auto iter = compositeCall_list.begin(); (iter + 1) != compositeCall_list.end(); iter++) {
        ((layerNode *)*iter)->level = ++currentLevel;
        ((layerNode *)*iter)->nextLayer  = ((layerNode *)*(iter+1));
        ((layerNode *)*(iter+1))->prevLayer  = ((layerNode *)*iter);
        if (((layerNode *)*iter)->layerName == "conv2D") {
            // 计算卷积层输出的尺寸
            ((conv2DLayerNode *)*iter)->init(globalSquential);
        }
    }
    ((layerNode *)compositeCall_list.back())->level = ++currentLevel;
    Node *weightType = new primNode("double");
    Node *prevDim = new constantNode("integer",((constantNode *)(node->arg_list->front()))->llval);
    // 以全局变量声明参数
    for (auto iter = compositeCall_list.begin(); iter != compositeCall_list.end(); iter++) {
        // 声明_weight_[prevDim][dim]
        Node *dim = new constantNode("integer", ((constantNode *)(((layerNode *)*iter)->arg_list->front()))->llval);
        string weightName = "_weight_" + to_string(((layerNode *)*iter)-> level);
        Node *weight =  new idNode(weightName);
        ((idNode *)weight)->isArray = 1;
        Node* arrDecl = new arrayNode((expNode *)prevDim);
        (static_cast<arrayNode *>(arrDecl))->arg_list.push_back((expNode *)dim);
        (static_cast<idNode *>(weight))->arg_list = (static_cast<arrayNode *>(arrDecl))->arg_list;
        Node* weightDecl = new declareNode((primNode*)weightType,(static_cast<idNode*>(weight)));
        Program->push_front(weightDecl);
        prevDim = dim;
    }
    // 取得输入到squential的训练集
    list<Node *> *temp_stream = new list<Node *>({inputs->front()});
    // 取得输入到squential的标签
    list<Node *> *y_stream = new list<Node *>({inputs->back()});
    // 声明stream stream<double x>...
    idNode *streamDeclId = new idNode("x");
    top->put("x", streamDeclId);
    primNode *streamType = new primNode("double");
    streamDeclId->valType = streamType->name;
    // 声明成Node还是strdclNode
    Node *streamDecl = new strdclNode(streamDeclId);
    // streamDecl->id_list.push_back()
   
    idNode *inputCopy1 = NULL, *inputCopy2 = NULL;
    inputCopy1 = new idNode("squential_input_copy_1");
    inputCopy2 = new idNode("squential_input_copy_2");
    ((strdclNode*)streamDecl)->id_list.push_back(inputCopy1);
    ((strdclNode*)streamDecl)->id_list.push_back(inputCopy2);
    list<Node *> *temp_call_inputs, *temp_call_outputs;
    temp_call_inputs = new list<Node *>({temp_stream->front()});
    temp_call_outputs = new list<Node *>({inputCopy1, inputCopy2});
    compositeNode *tempActualComposite = makeInputComposite((layerNode *)(*compositeCall_list.begin()), temp_call_inputs, temp_call_outputs);
    compositeCallNode *tempCall = new compositeCallNode(temp_call_outputs, "copySquentialInput", NULL, temp_call_inputs, tempActualComposite);
    comCallList.push_back(tempCall);
    // 用于存储前向传播给反向传播的数据流
    // 输入squential的训练集在反向传播中仍然需要
    list<list<Node *>*> *temp_stream_list = new list<list<Node *>*>({new list<Node *>({inputCopy2})});
    temp_stream->pop_back();
    temp_stream->push_back(inputCopy1);
    // 展开前向传播composite
    for (auto iter = compositeCall_list.begin(); iter != compositeCall_list.end(); iter++) {
        list<Node *> *call_inputs, *call_outputs;
        string name = ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)->level);
        if (*iter != compositeCall_list.back()) {
            string namePrefix = streamName + "_F" + ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)->level) + "_";
            // 正向传递给下一层的stream名称
            string tempName1 = namePrefix + "F" + ((layerNode *)*iter)->nextLayer->layerName + to_string(((layerNode *)*iter)->nextLayer->level);
            idNode *id1 = new idNode(tempName1);
            // 传递给反向传播中本层的stream名称
            string tempName2 = namePrefix + "B" + ((layerNode *)*iter)->nextLayer->layerName + to_string(((layerNode *)*iter)->nextLayer->level);
            idNode *id2 = new idNode(tempName2);
            // 将反向传播要连接的id存储到符号表中???
            //top->put(tempName2 ,id2);
            // 将数据流声明加入
            ((strdclNode*)streamDecl)->id_list.push_back(id1);
            ((strdclNode*)streamDecl)->id_list.push_back(id2);
            call_inputs = new list<Node *>({temp_stream->front()});
            call_outputs = new list<Node *>({id1, id2});
            // 将暂时只有一个输入流的输入流列表(缺乏传递误差的数据流)加入到临时列表中
            temp_stream_list->push_back(new list<Node *>({id2}));
            // call_outputs = new list<Node *>({id1});
            temp_stream->pop_back();
            temp_stream->push_back(call_outputs->front());
        } else {
            // 对正向传播过程的最后一层要做特别的处理, 他只有一个输出流
            // call_inputs = new list<Node *>({temp_stream->front()});
            // 只有正向传播的时候, output为输出 
            // call_outputs = new list<Node *>({outputs->front()});
            // 带有反向传播
            string tempName =  streamName  + "_F" + ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)->level) + "_B" + ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)-> level);
            idNode *id = new idNode(tempName);
            call_inputs = new list<Node *>({temp_stream->front()});
            call_outputs = new list<Node *>({id});
            temp_stream->pop_back();
            temp_stream->push_back(call_outputs->front());
        }
        // 构造实际的正向传播composite
        compositeNode* actual_composite = makeForwardComposite((layerNode *) *iter, call_inputs, call_outputs);
        compositeCallNode *call = new compositeCallNode(call_outputs, name, NULL, call_inputs, actual_composite);
        ((layerNode *)*iter)->fp_composite = call; 
        comCallList.push_back(call);
    }
    // dl/dy的输入为y, y`
    // 展开反向传播composite, 最后一层的composite的输入为实际预测和期望预测的输入流 也即temp_stream和 与y_stream
    list<Node *> *call_inputs= new list<Node *>({temp_stream->front(), y_stream->front()});
    string lossStreamName = streamName + "_Loss";
    idNode *lossStreamId = new idNode(lossStreamName);
    ((strdclNode*)streamDecl)->id_list.push_back(lossStreamId);
    list<Node *> *call_outputs = new list<Node *>({lossStreamId});
    compositeNode* actual_composite = makeLossComposite((layerNode *)(compositeCall_list.back()), call_inputs, call_outputs);
    string name = "squential_loss";
    comCallList.push_back(new compositeCallNode(call_outputs, name, NULL, call_inputs, actual_composite));
    cout<<"Unfold forward propagation"<<endl;
    temp_stream->clear();
    temp_stream->push_back(call_outputs->front());
    for (auto iter = compositeCall_list.rbegin(); iter != compositeCall_list.rend(); iter++) {
        list<Node *> *call_inputs, *call_outputs;
        temp_stream_list->back()->push_front(temp_stream->front());
        call_inputs = temp_stream_list->back();
        if (*iter != compositeCall_list.front()) {
            string name = "B" + ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)->level);
            string namePrefix = "B" + streamName + "_" + ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)->level) + "_";
            string tempName =  namePrefix + ((layerNode *)*iter)->prevLayer->layerName + to_string(((layerNode *)*iter)->prevLayer->level);
            idNode *id = new idNode(tempName);
            call_outputs = new list<Node *>({id}); 
        } else {
            call_outputs = new list<Node *>({outputs->front()});
        }
        temp_stream->clear();
        temp_stream->push_back(call_outputs->front());
        temp_stream_list->pop_back();
        compositeNode* actual_composite = makeBackComposite((layerNode *) *iter,call_inputs, call_outputs);
        compositeCallNode *call = new compositeCallNode(call_outputs, name, NULL, call_inputs, actual_composite);
        ((layerNode *) *iter)-> bp_composite = call; 
        comCallList.push_back(call);
    }
    cout<<"Unfold back propogation"<<endl;
    // ...
    // 生成squential composite
    // 将声明的数据流加入
    comp_stmts->push_back(streamDecl);
    // 遍历计算节点, 依次加入
    for (auto nd : comCallList)
        comp_stmts->push_back(nd);
    body = new compBodyNode(NULL, comp_stmts);
    squential = new compositeNode(head, body);
    compositeCall_list.clear();
    return squential;
}
compositeNode* UnfoldComposite::makeForwardComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    string comName = MakeCompositeName("F" + layer->layerName + to_string(layer-> level));
    compHeadNode *compHead = new compHeadNode(comName, inout);
    compBodyNode *compBody = NULL;
    list<Node *> *comp_stmt_list = new list<Node *>();
    Node *layerOperator = NULL;
    // 声明数据流, 修改makeDenseOperator的传入参数
    // operatorNode *activationOperator = NULL;
    if (layer->layerName == "dense") layerOperator = makeDenseOperator(layer, inputs, outputs);
    Node* layerExp = new binopNode((expNode*)outputs,"=",(expNode*)layerOperator) ;
    comp_stmt_list->push_back(layerExp);
    compBody = new compBodyNode(NULL, comp_stmt_list);
    compositeNode *comp = new compositeNode(compHead, compBody);
    cout << "makeForwardComposite"<<endl;
    return comp;
 }
compositeNode* UnfoldComposite::makeBackComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    compHeadNode *compHead = new compHeadNode("B" + layer->layerName + to_string(layer-> level), inout);
    compBodyNode *compBody = NULL;
    list<Node *> *comp_stmt_list = new list<Node *>();
    Node *layerOperator = NULL;
    if (layer->layerName == "dense") layerOperator = makeDDenseOperator(layer, inputs, outputs);
    Node* layerExp = new binopNode((expNode*)outputs,"=",(expNode*)layerOperator);
    comp_stmt_list->push_back(layerExp);
    compBody = new compBodyNode(NULL, comp_stmt_list);
    compositeNode *comp = new compositeNode(compHead, compBody);
    cout << "makeBackComposite"<<endl;
    return comp;
}
operatorNode* UnfoldComposite::makeDenseOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    string operName = "dense";
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;
    Node *row = NULL, *col = NULL;
    list<Node *> *win_stmt = new list<Node *>();
    // In0 sliding(row, row);
    if (layer->prevLayer != NULL) {
        row = layer->prevLayer->arg_list->front();
    } else {
        row = globalSquential->arg_list->front();
    }
    col = layer->arg_list->front();
    
    // 添加输入窗口
    auto iter = inputs->front();
    slidingNode *slid = new slidingNode(new list<Node *>({row, row}));
    winStmtNode *win1 = new winStmtNode(((idNode *)iter)->name, slid);
    win_stmt->push_back(win1);
    // 添加输出窗口
    tumblingNode *tumb = new tumblingNode(new list<Node *>({col}));
    iter = outputs->front();
    winStmtNode *win2 = new winStmtNode(((idNode *)iter)->name, tumb);
    win_stmt->push_back(win2);
    iter = outputs->back();
    winStmtNode *win3 = new winStmtNode(((idNode *)iter)->name, tumb);
    win_stmt->push_back(win3);
    window = new windowNode(win_stmt);
    init = makeDenseInit(layer, inputs, outputs);
    work = makeDenseWork(layer, inputs, outputs);
    // statement.list, operator.selfdefine.body.init, operator.selfdefine.body.work, operator.selfdefine.body.window.list
    body = new operBodyNode(NULL, init, work, window);
    return new operatorNode(outputs, operName, inputs, body);
}
operatorNode* UnfoldComposite::makeActivationOperator(layerNode* layer, list<Node *> *inputs, list<Node *> *outputs) {
    string operName;
    operBodyNode *operBody;
    return new operatorNode(outputs, operName, inputs, operBody);
}
// 全局变量初始化Weight
Node* UnfoldComposite::makeDenseInit(layerNode *layer, list<Node *> *inputs, list<Node*> *outputs) {
    list<Node *> *stmts = new list<Node *>();
    Node *init =  NULL;
    // 获得当前层的权值id
    string weightName = "_weight_" + to_string(layer-> level);
    Node*  weightId = new idNode(weightName);
    ((idNode *)weightId)->isArray = 1;
    // 利用for循环初始化
    constantNode *const_zero = new constantNode("integer", (long long)0);
    initNode *init_i = new initNode(const_zero);
    initNode *init_j = new initNode(const_zero);
    idNode *id_i = new idNode("i"), *id_j = new idNode("j");
    id_i->init = init_i;
    id_j->init = init_j;
    primNode *prim = new primNode("int");
    declareNode *declI = new declareNode(prim, id_i), *declJ = new declareNode(prim, id_j);
    stmts->push_back(declI);
    stmts->push_back(declJ);
    Node *init1 = NULL, *cond1 = NULL, *next_i = NULL, *stmt1 = NULL, *forNode1;
    Node *init2 = NULL, *cond2 = NULL, *next_j = NULL, *stmt2 = NULL, *forNode2;
    init1 = new binopNode((expNode *)id_i, "=", (expNode *)const_zero);
    if (layer->prevLayer == NULL) {
        cond1 = new binopNode((expNode *)id_i, "<", (expNode *)(globalSquential->arg_list->front()));
    } else {
        cond1 = new binopNode((expNode *)id_i, "<", (expNode *)(layer->prevLayer->arg_list->front()));
    }
    next_i = new unaryNode("POSTINC", (expNode *)id_i);
    init2 = new binopNode((expNode *)id_j, "=", (expNode *)const_zero);
    cond2 = new binopNode((expNode *)id_j, "<", (expNode *)(layer->arg_list->front()));
    next_j = new unaryNode("POSTINC", (expNode *)id_j);
    Node *weightArrDec = new arrayNode((expNode *)id_i);
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)id_j);
    (static_cast<idNode*>(weightId))->arg_list = (static_cast<arrayNode*>(weightArrDec))->arg_list;
    stmt2 = new binopNode((expNode *)weightId, "=", (expNode *)const_zero);
    forNode2 = new forNode(init2, (expNode *)cond2, (expNode *)next_j, stmt2);
    stmt1 = forNode2;
    forNode1 = new forNode(init1, (expNode *)cond1, (expNode *)next_i, stmt1);
    stmts->push_back(forNode1);
    init  = new blockNode(stmts);

    return init;
}
// 计算Dense
Node* UnfoldComposite::makeDenseWork(layerNode *layer, list<Node *> *inputs, list<Node*> *outputs) {
    list<Node *> *stmts = new list<Node *>();
    Node *work =  NULL;
    string weightName = "_weight_" + to_string(layer-> level);
    Node*  weightId = new idNode(weightName);
    ((idNode *)weightId)->isArray = 1;

    constantNode *const_zero = new constantNode("integer", (long long)0);
    initNode *init_i = new initNode(const_zero);
    initNode *init_j = new initNode(const_zero);
    idNode *id_i = new idNode("i"), *id_j = new idNode("j");
    id_i->init = init_i;
    id_j->init = init_j;
    primNode *prim = new primNode("int");
    declareNode *declI = new declareNode(prim, id_i), *declJ = new declareNode(prim, id_j);
    stmts->push_back(declI);
    stmts->push_back(declJ);

    idNode *id_temp = new idNode("temp");
    id_temp->init = new initNode(const_zero);
    declareNode *declTemp = new declareNode(prim, id_temp);
    stmts->push_back(declTemp);

    Node *forNode1 = NULL, *init1 = NULL, *cond1 = NULL, *next_j = NULL, *stmt1 = NULL;
    init1 = new binopNode((expNode *)id_j, "=", (expNode *)const_zero);
    cond1 = new binopNode((expNode *)id_j, "<", (expNode *)(layer->arg_list->front()));
    next_j = new unaryNode("POSTINC", (expNode *)id_j);
    list<Node *> *stmts1 = new list<Node *>();
    stmts1->push_back(new binopNode((expNode *)id_temp, "=", (expNode *)const_zero));

    Node *forNode2 = NULL, *init2 = NULL, *cond2 = NULL, *next_i = NULL, *stmt2 = NULL, *block2 = NULL;
    init2 = new binopNode((expNode *)id_i, "=", (expNode *)const_zero);
    cond2 = new binopNode((expNode *)id_i, "<", (expNode *)(* (layer->arg_list->begin()++)));
    next_i = new unaryNode("POSTINC", (expNode *)id_i);
    list<Node *> *stmts2 = new list<Node *>();
    // 取得_weight_level[i][j]
    Node *weightArrDec = NULL;
    weightArrDec =  new arrayNode((expNode *)(id_i));
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)(id_j));
    (static_cast<idNode*>(weightId))->arg_list = (static_cast<arrayNode*>(weightArrDec))->arg_list;
    // 取得In1[i].x
    idNode* x = new idNode("x");
    idNode* input1 = new idNode(static_cast<idNode *>(inputs->front())->name);
    input1->isArray = 1;
    input1->arg_list.push_back(id_i);
    Node* In1 = new binopNode((expNode *)input1, ".", (expNode *)x);
    // temp+=In1[i].x*_weight_level[i][j];
    Node* midRes = new binopNode((expNode *)id_temp, "+=", (expNode *)(new binopNode((expNode *)In1, "*", (expNode *)weightId)));
    stmts2->push_back(midRes);
    block2 = new blockNode(stmts2);
    // for(i = 0; i < arg:row; i++) {temp+=In0[i].x*weight1[i][j];}
    forNode2 = new forNode(init2, (expNode *)cond2, (expNode *)next_i, block2);
    stmts1->push_back(forNode2);

    // 取得Out1[j].x, Out2[j].x
    idNode* output1 = new idNode(static_cast<idNode *>(outputs->front())->name);
    output1->isArray = 1;
    output1->arg_list.push_back(id_j);
    Node* out1X = new binopNode((expNode *)output1, ".", (expNode *)x);
    Node* res1 = new binopNode((expNode *)out1X, "=", (expNode *)id_temp);
    stmts1->push_back(res1);
    idNode* output2 = new idNode(static_cast<idNode *>(outputs->back())->name);
    output2->isArray = 1;
    output2->arg_list.push_back(id_j);
    Node* out2X = new binopNode((expNode *)output2, ".", (expNode *)x);
    Node* res2 = new binopNode((expNode *)out2X, "=", (expNode *)id_temp);
    stmts1->push_back(res2);
    Node* block1 = new blockNode(stmts1);
    forNode1 = new forNode(init1, (expNode *)cond1, (expNode *)next_j, block1);
    stmts->push_back(forNode1);
    
    work = new blockNode(stmts);
    return work;
}

compositeNode* UnfoldComposite::makeLossComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    string comName = MakeCompositeName("Loss");
    compHeadNode *compHead = new compHeadNode(comName, inout);
    compBodyNode *compBody = NULL;
    list<Node *> *comp_stmt_list = new list<Node*>();
    Node *lossOperator = makeLossOperator(layer, inputs, outputs);
    Node *lossExp = new binopNode((expNode*)outputs, "=", (expNode*)lossOperator); 
    comp_stmt_list->push_back(lossExp);
    compBody = new compBodyNode(NULL, comp_stmt_list);
    compositeNode *comp = new compositeNode(compHead, compBody);
    cout << "Make loss composite"<<endl;
    return comp;
}

operatorNode* UnfoldComposite::makeLossOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    string operName = "loss";
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;

    // window
    list<Node *> *winStmt = new list<Node *>();
    Node* num = layer->arg_list->front();
    slidingNode *slid = new slidingNode(new list<Node *>({num, num}));
    winStmtNode *win1 = new winStmtNode(((idNode *)(inputs->front()))->name, slid);
    winStmtNode *win2 = new winStmtNode(((idNode *)(inputs->back()))->name, slid);
    winStmt->push_back(win1);
    winStmt->push_back(win2);
    tumblingNode *tumb = new tumblingNode(new list<Node *>({num}));
    winStmtNode *win3 = new winStmtNode(((idNode *)(outputs->front()))->name, tumb);
    winStmt->push_back(win3);
    window = new windowNode(winStmt);

    // workNode
    list<Node *> *stmts = new list<Node *>();
    constantNode *const_zero = new  constantNode("integer", (long long)0);
    initNode *init_i = new initNode(const_zero);
    idNode *id_i = new idNode("i");
    id_i->init = init_i;
    primNode *prim = new primNode("int");
    declareNode *declI = new declareNode(prim, id_i);
    stmts->push_back(declI);
    Node *forNode1 = NULL, *forInitI = NULL, *forCondI = NULL, *forNextI = NULL;
    forInitI = new binopNode((expNode *)id_i, "=", (expNode *)const_zero);
    forCondI = new binopNode((expNode *)id_i, "<", (expNode *)(* (layer->arg_list->begin()++)));
    forNextI = new unaryNode("POSTINC", (expNode *)id_i);
    list<Node *> *forStmts = new list<Node *>();
    // 取得In1[i].x, In2[i].x
    idNode* x = new idNode("x");
    // 预测
    idNode* y = new idNode(static_cast<idNode *>(inputs->front())->name);
    // 标签
    idNode* y_ = new idNode(static_cast<idNode *>(inputs->back())->name);
    y->isArray = 1;
    y->arg_list.push_back(id_i);
    y_->isArray = 1;
    y_->arg_list.push_back(id_i);
    Node* Y = new binopNode((expNode *)y, ".", (expNode *)x);
    Node* Y_ = new binopNode((expNode *)y_, ".", (expNode *)x);
    idNode* dl_dy = new idNode(static_cast<idNode *>(outputs->front())->name);
    dl_dy->isArray = 1;
    dl_dy->arg_list.push_back(id_i);
    Node* Out = new binopNode((expNode *)dl_dy, ".", (expNode *)x);
    Node* res = new binopNode((expNode *)Out, "=", (expNode *)(new binopNode((expNode *)Y, "-", (expNode *)Y_)));
    forStmts->push_back(res);
    Node* forBlock = new blockNode(forStmts);
    forNode1 = new forNode(forInitI, (expNode *)forCondI, (expNode *)forNextI, forBlock);
    stmts->push_back(forNode1);
    work = new blockNode(stmts);
    
    body = new operBodyNode(NULL, init, work, window);
    return new operatorNode(outputs, operName, inputs, body);
}

operatorNode* UnfoldComposite::makeDDenseOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    string operName = "dDense";
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;
    Node *row = NULL, *col = NULL;
    list<Node *> *win_stmt = new list<Node *>();
    if (layer->prevLayer != NULL) {
        row = layer->prevLayer->arg_list->front();
    } else {
        row = globalSquential->arg_list->front();
    }
    col = layer->arg_list->front();

    // 添加输入窗口
    auto iter = inputs->front();
    slidingNode *slid1 = new slidingNode(new list<Node *>({col, col}));
    winStmtNode *dIn = new winStmtNode(((idNode *)iter)->name, slid1);
    win_stmt->push_back(dIn);
    iter = inputs->back();
    slidingNode *slid2 = new slidingNode(new list<Node *>({row, row}));
    winStmtNode *in = new winStmtNode(((idNode *)iter)->name, slid2);
    win_stmt->push_back(in);

    // 添加输出窗口
    iter = outputs->front();
    tumblingNode *tumb = new tumblingNode(new list<Node *>({row}));
    winStmtNode *dOut = new winStmtNode(((idNode *)iter)->name, tumb);
    win_stmt->push_back(dOut);
    window = new windowNode(win_stmt);
    init = NULL;
    work = makeDDenseWork(layer, inputs, outputs);
    body = new operBodyNode(NULL, init, work, window);
    return new operatorNode(outputs, operName, inputs, body);
}
// MakeDDenseInit 初始化误差矩阵
// 初始化学习率为0.1
Node* UnfoldComposite::makeDDenseWork(layerNode *layer, list<Node *> *inputs, list<Node*> *outputs) {
    list<Node *> *stmts = new list<Node *>();
    Node *work =  NULL;
    string weightName = "_weight_" + to_string(layer-> level);
    Node*  weightId = new idNode(weightName);
    ((idNode *)weightId)->isArray = 1;
    Node *row = NULL, *col = NULL;
    if (layer->prevLayer != NULL) {
        row = layer->prevLayer->arg_list->front();
    } else {
        row = globalSquential->arg_list->front();
    }
    col = layer->arg_list->front();
    // 循环修改w, 循环传递误差
    constantNode *const_zero = new  constantNode("integer", (long long)0);
    constantNode *const_i = new constantNode("integer", (long long)0);
    constantNode *const_j = new constantNode("integer", (long long)0);
    initNode *init_i = new initNode(const_i);
    initNode *init_j = new initNode(const_j);
    idNode *id_i = new idNode("i"), *id_j = new idNode("j");
    id_i->init = init_i;
    id_j->init = init_j;
    primNode *primInt = new primNode("int");
    declareNode *declI = new declareNode(primInt, id_i), *declJ = new declareNode(primInt, id_j);
    stmts->push_back(declI);
    stmts->push_back(declJ);
    primNode *primDouble = new primNode("double");
    idNode *id_temp = new idNode("temp");
    id_temp->init = new initNode(const_zero);
    declareNode *declTemp = new declareNode(primDouble, id_temp);
    stmts->push_back(declTemp);

    // 计算传播误差
    Node *forInitI = NULL, *forCondI = NULL, *forNextI = NULL, *forInitJ = NULL, *forCondJ = NULL, *forNextJ = NULL;
    forInitI = new binopNode((expNode *)id_i, "=", (expNode *)const_zero);
    forCondI = new binopNode((expNode *)id_i, "<", (expNode *)(row));
    forNextI = new unaryNode("POSTINC", (expNode *)id_i);
    forInitJ = new binopNode((expNode *)id_j, "=", (expNode *)const_zero);
    forCondJ = new binopNode((expNode *)id_j, "<", (expNode *)(col));
    forNextJ = new unaryNode("POSTINC", (expNode *)id_j);
    // 取得 weight_level[i][j]
    Node *weightArrDec = NULL;
    weightArrDec =  new arrayNode((expNode *)(id_i));
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)(id_j));
    (static_cast<idNode*>(weightId))->arg_list = (static_cast<arrayNode*>(weightArrDec))->arg_list;
    
    idNode* x = new idNode("x");
    // 取得dIn[j].x
    idNode* dIn = new idNode(static_cast<idNode *>(inputs->front())->name);
    dIn->isArray = 1;
    dIn->arg_list.push_back(id_j);
    Node* dInX = new binopNode((expNode *)dIn, ".", (expNode *)x);
    // 取得in[i].x
    idNode* in = new idNode(static_cast<idNode *>(inputs->back())->name);
    in->isArray = 1;
    in->arg_list.push_back(id_i);
    Node* inX = new binopNode((expNode *)in, ".", (expNode *)x);

    Node *forNode1 = NULL, *stmt1 = NULL, *block1 = NULL;
    list<Node *> *stmts1 = new list<Node *>();
    stmts1->push_back(new binopNode((expNode *)id_temp, "=", (expNode *)const_zero));

    Node *forNode2 = NULL, *stmt2 = NULL, *block2 = NULL;
    list<Node *> *stmts2 = new list<Node *>();
    
    Node* midRes = new binopNode((expNode *)id_temp, "+=", (expNode *)(new binopNode((expNode *)dInX, "*", (expNode *)weightId)));
    stmts2->push_back(midRes);
    block2 = new blockNode(stmts2);
    forNode2 = new forNode(forInitJ, (expNode *)forCondJ, (expNode *)forNextJ, block2);
    stmts1->push_back(forNode2);
    // 取得Out1[j].x
    idNode* dOut = new idNode(static_cast<idNode *>(outputs->front())->name);
    dOut->isArray = 1;
    dOut->arg_list.push_back(id_j);
    Node* Out1 = new binopNode((expNode *)dOut, ".", (expNode *)x);
    Node* res = new binopNode((expNode *)Out1, "=", (expNode *)id_temp);
    stmts1->push_back(res);
    block1 = new blockNode(stmts1);
    forNode1 = new forNode(forInitI, (expNode *) forCondI, (expNode *) forNextI, block1);
    stmts->push_back(forNode1);

    Node *forNode3 = NULL, *stmt3 = NULL, *block3 = NULL;
    list<Node *> *stmts3 = new list<Node *>();
    Node *forNode4 = NULL, *stmt4 = NULL, *block4 = NULL;
    list<Node *> *stmts4 = new list<Node *>();
    // 暂时 学习率
    constantNode *constLr = new constantNode("double", (double)0.1);
    initNode *initLr = new initNode(constLr);
    idNode *idLr = new idNode("lr");
    idLr->init = initLr;
    primNode *prim = new primNode("double");
    declareNode *declLr = new declareNode(primDouble, idLr);
    stmts->push_back(declLr);
    // weight_level[i][j] = weight_level[i][j] + lr * dIn[j].x * in[i].x;
    Node * change = new binopNode((expNode *)weightId, "=", (expNode *)(new binopNode((expNode *) weightId, "+", (expNode *)(new binopNode((expNode *)dInX, "*", (expNode *)(new binopNode((expNode *) inX, "*", (expNode *) idLr)))))));
    stmts4->push_back(change);
    block4 = new blockNode(stmts4);
    forNode4 = new forNode(forInitJ, (expNode *)forCondJ, (expNode *)forNextJ, block4);
    stmts3->push_back(forNode4);
    block3 = new blockNode(stmts3);
    forNode3 = new forNode(forInitI, (expNode *) forCondI, (expNode *) forNextI, block3); 
    stmts->push_back(forNode3);
    work = new blockNode(stmts);
    return work;
}
// 暂时,优化可与makeLossComposite合并
compositeNode* UnfoldComposite::makeInputComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    string comName = MakeCompositeName("SquentialInput");
    compHeadNode *compHead = new compHeadNode(comName, inout);
    compBodyNode *compBody = NULL;
    list<Node *> *comp_stmt_list = new list<Node*>();
    Node *inputOperator = makeInputOperator(layer, inputs, outputs);
    Node *inputExp = new binopNode((expNode*)outputs, "=", (expNode*)inputOperator); 
    comp_stmt_list->push_back(inputExp);
    compBody = new compBodyNode(NULL, comp_stmt_list);
    compositeNode *comp = new compositeNode(compHead, compBody);
    cout << "Make input composite"<<endl;
    return comp;
}
// 暂时保留layerNode,每次只复制一条数据
operatorNode* UnfoldComposite::makeInputOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    string operName = "squential_input";
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;
    // window
    list<Node *> *winStmt = new list<Node *>();
    Node* num = globalSquential->arg_list->front();
    slidingNode *slid = new slidingNode(new list<Node *>({num, num}));
    winStmtNode *win1 = new winStmtNode(((idNode *)(inputs->front()))->name, slid);
    winStmt->push_back(win1);
    tumblingNode *tumb = new tumblingNode(new list<Node *>({num}));
    winStmtNode *win2 = new winStmtNode(((idNode *)(outputs->front()))->name, tumb);
    
    winStmt->push_back(win2);
    winStmtNode *win3 = new winStmtNode(((idNode *)(outputs->back()))->name, tumb);
    winStmt->push_back(win3);
    window = new windowNode(winStmt);

    // workNode
    list<Node *> *stmts = new list<Node *>();
    Node* const_zero = new constantNode("integer", (long long)0);

    idNode* x = new idNode("x");
    // Node *forInitI = NULL, *forCondI = NULL, *forNextI = NULL;
    idNode* input = new idNode(static_cast<idNode *>(inputs->front())->name);
    idNode* copy1 = new idNode(static_cast<idNode *>(outputs->front())->name);
    idNode* copy2 = new idNode(static_cast<idNode *>(outputs->back())->name);
    input->isArray = 1;
    copy1->isArray = 1;
    copy2->isArray = 1;
    input->arg_list.push_back(const_zero);
    copy1->arg_list.push_back(const_zero);
    copy2->arg_list.push_back(const_zero);
    Node* res1 = new binopNode((expNode *) copy1, "=", (expNode *) input);
    Node* res2 = new binopNode((expNode *) copy2, "=", (expNode *) input);
    stmts->push_back(res1);
    stmts->push_back(res2);

    work = new blockNode(stmts);

    body = new operBodyNode(NULL, init, work, window);
    return new operatorNode(outputs, operName, inputs, body);
}

operatorNode* UnfoldComposite::makeConv2DOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    string operName = "conv2D";
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;
    Node *inRow = NULL, *inCol = NULL;
    list<Node *> *win_stmt = new list<Node *>();
    // 输入窗口
    auto inputIter = inputs->begin();
    auto inputSize = layer->prevLayer != NULL ? layer->prevLayer->arg_list->front() : globalSquential->arg_list->front();
    if (inputSize -> type == constant) {
        inRow = inputSize;
        inCol = inputSize;
    } else {
        inRow = ((tupleNode *)inputSize)->tupleList->front();
        inCol = ((tupleNode *)inputSize)->tupleList->back();
    }
    long long popVal = ((constantNode *)inRow)->llval * ((constantNode *)inCol)->llval;
    Node *pop = new constantNode("integer", popVal);
    slidingNode *slid = new slidingNode(new list<Node *>({pop, pop}));
    winStmtNode *win1 = new winStmtNode(((idNode *)(* inputIter))->name, slid);
    win_stmt->push_back(win1);

    // 输出窗口
    auto outputIter = inputs->begin();
    long long pushVal = ((conv2DLayerNode *)layer)->size->at(0) * ((conv2DLayerNode *)layer)->size->at(1);
    Node *push = new constantNode("integer", pushVal);
    tumblingNode *tumb = new tumblingNode(new list<Node *>({push}));
    winStmtNode *win2 = new winStmtNode(((idNode *)(*outputIter))->name, tumb);
    win_stmt->push_back(win2);
    winStmtNode *win3 = new winStmtNode(((idNode *)(*(++outputIter)))->name, tumb);
    win_stmt->push_back(win3);
    window = new windowNode(win_stmt);
    init = makeConv2DInit(layer, inputs, outputs);
    work = makeConv2DWork(layer, inputs, outputs);
    body = new operBodyNode(NULL, init, work, window);
    return new operatorNode(outputs, operName, inputs, body);
}

Node* getDimVal (layerNode* layer, int dim) {
    Node *val = NULL;
    if (layer->prevLayer == NULL) {
        auto iter = ((tupleNode *)(globalSquential->arg_list->front()))->tupleList->begin();
        for (int i = 0; i < dim; i++) {
            iter++;
        }
        val = *iter;
    } else {
        val = new constantNode("integer", ((conv2DLayerNode *)layer)->size->at(dim));
    }
    return val;
}

Node* UnfoldComposite::makeConv2DInit(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    list<Node *> *stmts = new list<Node *>();
    Node *init = NULL;
    string weightName = "_weight_" + to_string(layer-> level);
    Node*  weightId = new idNode(weightName);
    ((idNode *)weightId)->isArray = 1;

    constantNode *const_zero = new constantNode("integer", (long long)0);
    idNode *id_i = new idNode("i"), *id_j = new idNode("j"), *id_n = new idNode("n"), *id_m = new idNode("m");
    primNode *prim = new primNode("int");
    declareNode *decl_i = new declareNode(prim, id_i), 
                *decl_j = new declareNode(prim, id_j),
                *decl_n = new declareNode(prim, id_n),
                *decl_m = new declareNode(prim, id_m);
    stmts->push_back(decl_i);
    stmts->push_back(decl_j);
    stmts->push_back(decl_n);
    stmts->push_back(decl_m);
    Node *init1 = NULL, *cond1 = NULL, *next_i = NULL, *stmt1 = NULL, *forNode1;
    Node *init2 = NULL, *cond2 = NULL, *next_j = NULL, *stmt2 = NULL, *forNode2;
    Node *init3 = NULL, *cond3 = NULL, *next_n = NULL, *stmt3 = NULL, *forNode3;
    Node *init4 = NULL, *cond4 = NULL, *next_m = NULL, *stmt4 = NULL, *forNode4;
    Node *weightArrDec = NULL;
    auto argListIter = layer->arg_list->begin();
    // 第一层循环遍历该卷积层所有核, 即filters
    init1 = new binopNode((expNode *)id_i, "=", (expNode *)const_zero);
    cond1 = new binopNode((expNode *)id_i, "<", (expNode *)(*argListIter));
    next_i = new unaryNode("POSTINC", (expNode *)id_i);
    weightArrDec = new arrayNode((expNode *)id_i);
    // 第二层循环遍历核的每一层,对应上一层输出的深度
    init2 = new binopNode((expNode *)id_j, "=", (expNode *)const_zero);
    Node *depth = getDimVal(layer, 0);
    cond2 = new binopNode((expNode *)id_j, "<", (expNode *)(depth));
    next_j = new unaryNode("POSTINC", (expNode *)id_j);
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)id_j);
    // 第三层循环遍历核中层的行
    init3 = new binopNode((expNode *)id_n, "=", (expNode *)const_zero);
    Node *rows = getDimVal(layer, 1);
    cond3 = new binopNode((expNode *)id_n, "<", (expNode *)(rows));
    next_n = new unaryNode("POSTINC", (expNode *)id_n);
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)id_n);
    // 第四层循环遍历核中层的行的每一列
    init4 = new binopNode((expNode *)id_m, "=", (expNode *)const_zero);
    Node *cols = getDimVal(layer, 2);
    cond4 = new binopNode((expNode *)id_m, "<", (expNode *)(cols));
    next_m = new unaryNode("POSTINC", (expNode *)id_m);
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)id_m);
    // 为 weight[i][j][n][m]赋值
    stmt4 = new binopNode((expNode *)weightId, "=", (expNode *)const_zero);
    forNode4 = new forNode(init4, (expNode *)cond4, (expNode *)next_m, stmt4);
    stmt3 = forNode4;
    forNode3 = new forNode(init3, (expNode *)cond3, (expNode *)next_n, stmt3);
    stmt2 = forNode3;
    forNode2 = new forNode(init2, (expNode *)cond2, (expNode *)next_j, stmt2);
    stmt1 = forNode2;
    forNode1 = new forNode(init1, (expNode *)cond1, (expNode *)next_i, stmt1);
    stmts->push_back(forNode1);
    init = new blockNode(stmts);
    return init;
}

Node* UnfoldComposite::makeConv2DWork(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    Node* work = NULL;
}

Node* UnfoldComposite::makeConv2DComp(layerNode *layer) {
    Node *compHead = NULL, *compBody = NULL, *comp = NULL, *compInOut = NULL;
    // composite.head.inout.member.list:
    list<Node *> *inputs = new list<Node *>(), *outputs = new list<Node *>();
    idNode *streamId = new idNode("x");
    primNode *valTypeDouble = new primNode("double");
    streamId->valType = valTypeDouble->name;
    strdclNode *strDcl  = new strdclNode(streamId);
    // top->put(static_cast<idNode*>($2)->name,static_cast<idNode*>($2)); ???
    idNode *inputId = new idNode("input");
    // top->put(*($2),id); ??? 符号表
    Node* input = new inOutdeclNode(strDcl, inputId);
    inputs->push_back(input);
    idNode *outputId1 = new idNode("output1");
    Node* output1 = new inOutdeclNode(strDcl, outputId1);
    outputs->push_back(output1);
    idNode *outputId2 = new idNode("output2");
    Node* output2 = new inOutdeclNode(strDcl, outputId2);
    outputs->push_back(output2);
    compInOut = new ComInOutNode(inputs, outputs);
    compHead = new compHeadNode("conv2D_" + layer->level, (ComInOutNode *)compInOut);
    // compBody = makeConv2DCompBody(layer);
    comp = new compositeNode((compHeadNode *)compHead, (compBodyNode *)compBody);
    // S.InsertCompositeSymbol(((compositeNode*)$$)->compName,(compositeNode*)$$); ???
    return comp;
}

// Node* UnfoldComposite::makeConv2DCompBody(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
//     Node *body = NULL;
//     list<Node *> *stmt_list = new list<Node *>(), *splitjoin_stmt_list = new list<Node *>();
//     idNode *output = new idNode("output");
//     idNode *id_i = new idNode("i");
//     primNode *prim = new primNode("int");
//     declareNode *decl_i = new declareNode(prim, id_i);
//     splitjoin_stmt_list->push_back(decl_i);
//     Node* splitjoin = NULL;
//     duplicateNode *duplicate = new duplicateNode(NULL);
//     splitNode *split = new splitNode(duplicate);
//     roundrobinNode *roundrobin = new roundrobinNode(NULL);
//     joinNode *join = new joinNode(roundrobin);
//     splitjoinNode *splitjoin = new splitjoinNode(inputs, )
//     return body;
// }

// Node* UnfoldComposite::makeStream(string name, string type) {
//     idNode *x = new idNode("x");
//     primNode *prim = new primNode(type);
//     x->valType = prim->name;
//     strdclNode *strdcl = new strdclNode(x);
// }