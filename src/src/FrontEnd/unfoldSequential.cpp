#include "unfoldComposite.h"
#include "compositeFlow.h"
#include <string.h>
extern SymbolTable S;
extern list<Node*> *Program;
extern sequentialNode *globalSequential;
extern vector<Node *> compositeCall_list;

compositeNode *UnfoldComposite::UnfoldSequential(sequentialNode *node) {
    globalSequential = node;
    compositeCallFlow(node->body_stmts); // 将通过add加入的层,依次push到compositeCall_list中
    vector<compositeCallNode *> comCallList; // 用于存储展开后的compositeCallNode
    compositeNode *sequential = NULL;
    string streamName = "Sstream";
    string comName = MakeCompositeName("sequential");
    list<Node *> *inputs = node->inputs;
    list<Node *> *outputs = node->outputs;
    list<Node *> *arg_list = node->arg_list;
    list<Node *> *comp_stmts = new list<Node *>();
    cout<<"Unfold sequential"<<endl;
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    compHeadNode *head = new compHeadNode(comName, inout);
    compBodyNode *body = NULL;
    SymbolTable *top = new SymbolTable(NULL);
    // sequential有两个输入流, 分别是是训练集和标签
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
            ((conv2DLayerNode *)*iter)->init(globalSequential);
        }
    }
    if (((layerNode *)(compositeCall_list.back()))->layerName == "conv2D") {
        ((conv2DLayerNode *)(compositeCall_list.back()))->init(globalSequential);
    }
    ((layerNode *)compositeCall_list.back())->level = ++currentLevel;
    Node *weightType = new primNode("double");
    Node *prevDim = new constantNode("integer",((constantNode *)(node->arg_list->front()))->llval);
    // 以全局变量声明参数
    for (auto iter = compositeCall_list.begin(); iter != compositeCall_list.end(); iter++) {
        string weightName = "_weight_" + to_string(((layerNode *)*iter)-> level);
        Node *weight =  new idNode(weightName);
        ((idNode *)weight)->isArray = 1;
        if (((layerNode *)*iter) -> layerName == "dense") {
            // 声明_weight_[prevDim][dim]
            Node *dim = new constantNode("integer", ((constantNode *)(((layerNode *)*iter)->arg_list->front()))->llval);
            Node* arrDecl = new arrayNode((expNode *)prevDim);
            (static_cast<arrayNode *>(arrDecl))->arg_list.push_back((expNode *)dim);
            (static_cast<idNode *>(weight))->arg_list = (static_cast<arrayNode *>(arrDecl))->arg_list;
            Node* weightDecl = new declareNode((primNode*)weightType,(static_cast<idNode*>(weight)));
            Program->push_front(weightDecl);
            prevDim = dim;
        } else if (((layerNode *)*iter) -> layerName == "conv2D") {
            // 声明_weight_[depth][kernelSizeDim0][kernelSizeDim1]
            Node *depth = new constantNode("integer", ((conv2DLayerNode *)*iter) -> depth);
            Node* arrDecl = new arrayNode((expNode *)depth);
            (static_cast<arrayNode *>(arrDecl))->arg_list.push_back((expNode *)(new constantNode("integer", ((conv2DLayerNode *)*iter) -> kernel_size -> at(0))));
            (static_cast<arrayNode *>(arrDecl))->arg_list.push_back((expNode *)(new constantNode("integer", ((conv2DLayerNode *)*iter) -> kernel_size -> at(1))));
            (static_cast<idNode *>(weight))->arg_list = (static_cast<arrayNode *>(arrDecl))->arg_list;
            Node* weightDecl = new declareNode((primNode*)weightType,(static_cast<idNode*>(weight)));
            Program->push_front(weightDecl);
            // 若卷积层后跟全连接层, 需要计算输出的尺寸 filters * size[0] * size[1]
            prevDim = new constantNode("integer", ((conv2DLayerNode *)*iter) -> filters * ((conv2DLayerNode *)*iter) -> size -> at(0) * ((conv2DLayerNode *)*iter) -> size -> at(1));
        }
    }
    // 取得输入到sequential的训练集
    list<Node *> *temp_stream = new list<Node *>({inputs->front()});
    // 取得输入到sequential的标签
    list<Node *> *y_stream = new list<Node *>({inputs->back()});
    // 声明stream stream<double x>...
    idNode *streamDeclId = new idNode("x");
    top->put("x", streamDeclId);
    primNode *streamType = new primNode("double");
    streamDeclId->valType = streamType->name;
    Node *streamDecl = new strdclNode(streamDeclId);
   
    idNode *inputCopy1 = NULL, *inputCopy2 = NULL;
    inputCopy1 = new idNode("sequential_input_copy_1");
    inputCopy2 = new idNode("sequential_input_copy_2");
    ((strdclNode*)streamDecl)->id_list.push_back(inputCopy1);
    ((strdclNode*)streamDecl)->id_list.push_back(inputCopy2);
    list<Node *> *temp_call_inputs, *temp_call_outputs;
    temp_call_inputs = new list<Node *>({temp_stream->front()});
    temp_call_outputs = new list<Node *>({inputCopy1, inputCopy2});
    compositeNode *tempActualComposite = makeInputComposite((layerNode *)(*compositeCall_list.begin()), temp_call_inputs, temp_call_outputs);
    compositeCallNode *tempCall = new compositeCallNode(temp_call_outputs, "copySequentialInput", NULL, temp_call_inputs, tempActualComposite);
    comCallList.push_back(tempCall);
    // 用于存储前向传播给反向传播的数据流
    // 输入sequential的训练集在反向传播中仍然需要
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
    string name = "sequential_loss";
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
    // 生成sequential composite
    // 将声明的数据流加入
    comp_stmts->push_back(streamDecl);
    // 遍历计算节点, 依次加入
    for (auto nd : comCallList)
        comp_stmts->push_back(nd);
    body = new compBodyNode(NULL, comp_stmts);
    sequential = new compositeNode(head, body);
    compositeCall_list.clear();
    return sequential;
}
compositeNode* UnfoldComposite::makeForwardComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    cout << "makeForwardComposite"<<endl;
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    string comName = MakeCompositeName("F" + layer->layerName + to_string(layer-> level));
    compHeadNode *compHead = new compHeadNode(comName, inout);
    compBodyNode *compBody = NULL;
    list<Node *> *comp_stmt_list = new list<Node *>();
    // 声明数据流, 修改makeDenseOperator的传入参数
    if (layer->layerName == "dense") {
        Node *layerOper = makeDenseOperator(layer, inputs, outputs);
        Node* layerExp = new binopNode((expNode*)outputs,"=",(expNode*)layerOper);
        comp_stmt_list->push_back(layerExp);
    } else if (layer->layerName == "conv2D") {
        compositeNode *layerComp = makeConv2DLayer(layer, inputs, outputs);
        // compositeNode *actualLayerComp = compositeCallStreamReplace(layerComp, inputs, outputs);
        Node *call = new compositeCallNode(outputs, "conv2D", NULL, inputs, layerComp);
        Node *layerExp = new binopNode((expNode*)outputs,"=",(expNode*)call);
        comp_stmt_list->push_back(layerExp);
    }
    compBody = new compBodyNode(NULL, comp_stmt_list);
    compositeNode *comp = new compositeNode(compHead, compBody);
    return comp;
 }
compositeNode* UnfoldComposite::makeBackComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    string compName = MakeCompositeName("B" + layer->layerName + to_string(layer-> level));
    compHeadNode *compHead = new compHeadNode(compName, inout);
    compBodyNode *compBody = NULL;
    list<Node *> *comp_stmt_list = new list<Node *>();
    if (layer->layerName == "dense") {
        Node *layerOper = makeDDenseOperator(layer, inputs, outputs);
        Node* layerExp = new binopNode((expNode*)outputs,"=",(expNode*)layerOper);
        comp_stmt_list->push_back(layerExp);
    } else if (layer->layerName == "conv2D") {
        compositeNode *layerComp = makeDConv2DLayer(layer, inputs, outputs);
        Node *call = new compositeCallNode(outputs, "dConv2D", NULL, inputs, layerComp);
        Node *layerExp = new binopNode((expNode*)outputs,"=",(expNode*)call);
        comp_stmt_list->push_back(layerExp);
    }
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
        row = globalSequential->arg_list->front();
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
        cond1 = new binopNode((expNode *)id_i, "<", (expNode *)(globalSequential->arg_list->front()));
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
        row = globalSequential->arg_list->front();
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
        row = globalSequential->arg_list->front();
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
    string comName = MakeCompositeName("SequentialInput");
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
    string operName = "sequential_input";
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;
    // window
    list<Node *> *winStmt = new list<Node *>();
    Node* num = globalSequential->arg_list->front();
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

operatorNode* UnfoldComposite::makeConv2DKernelOper(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    string operName = "conv2D";
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;
    Node *inRow = NULL, *inCol = NULL;
    list<Node *> *winStmt = new list<Node *>(), *bodyStmtList = new list<Node *>();
    // 输入窗口
    auto inputIter = inputs->begin();
    long long popVal = 1;
    if (layer->prevLayer != NULL) {
        layerNode *prevLayer = layer -> prevLayer;
        vector<long long> *inputSize = ((conv2DLayerNode *)prevLayer)->size;
        // depth * width * height
        popVal = inputSize -> at(0)* inputSize -> at(1) * ((conv2DLayerNode *)layer)->depth;
    } else {
        auto iter = globalSequential -> arg_list -> begin();
        for (int i = 0; i < 3; i++) {
            popVal *= ((constantNode *)(*iter)) -> llval;
            iter++;
        }
    }
    cout << "倦極核輸入尺寸" << popVal << endl;
    Node *pop = new constantNode("integer", popVal);
    slidingNode *slid = new slidingNode(new list<Node *>({pop, pop}));
    winStmtNode *win1 = new winStmtNode(((idNode *)(* inputIter))->name, slid);
    winStmt->push_back(win1);

    // 输出窗口
    auto outputIter = outputs->begin();
    long long pushVal = ((conv2DLayerNode *)layer)->size->at(0) * ((conv2DLayerNode *)layer)->size->at(1);
    cout << "倦極核輸出尺寸" << pushVal << endl;
    Node *push = new constantNode("integer", pushVal);
    tumblingNode *tumb = new tumblingNode(new list<Node *>({push}));
    winStmtNode *win2 = new winStmtNode(((idNode *)(*outputIter))->name, tumb);
    winStmt->push_back(win2);
    window = new windowNode(winStmt);
    primNode *primInt = new primNode("int"),
             *primDouble = new primNode("double");
    idNode *idI = new idNode("i"),
           *idJ = new idNode("j"),
           *idN = new idNode("n"),
           *idM = new idNode("m"),
           *idK = new idNode("k"),
           *idPushIndex = new idNode("pushIndex"),
           *idTemp = new idNode("temp");
    // top->put(static_cast<idNode*>($2)->name,static_cast<idNode*>($2)); ???
    Node *declInt = new declareNode(primInt, idI),
        //  *declJ = new declareNode(primInt, idJ),
        //  *declN = new declareNode(primInt, idN),
        //  *declM = new declareNode(primInt, idM),
        //  *declK = new declareNode(primInt, idK),
        //  *declPushIndex = new declareNode(primInt, idPushIndex),
         *declDouble = new declareNode(primDouble, idTemp);
    ((declareNode *)declInt )-> id_list.push_back(idJ);
    ((declareNode *)declInt )-> id_list.push_back(idN);
    ((declareNode *)declInt )-> id_list.push_back(idM);
    ((declareNode *)declInt )-> id_list.push_back(idK);
    ((declareNode *)declInt )-> id_list.push_back(idPushIndex);
    bodyStmtList -> push_back(declInt);
    bodyStmtList -> push_back(declDouble);
    init = makeConv2DKernelOperInit(layer);
    work = makeConv2DKernelOperWork(layer, inputs, outputs);
    body = new operBodyNode(bodyStmtList, init, work, window);
    return new operatorNode(outputs, operName, inputs, body);
}
// 用于获取某一维度的值
Node* getInputDimVal (layerNode* layer, int dim) {
    Node *val = NULL;
    if (layer->prevLayer == NULL) {
        auto iter = globalSequential->arg_list->begin();
        iter++;
        for (int i = 0; i < dim; i++) {
            iter++;
        }
        val = *iter;
    } else {
        val = new constantNode("integer", ((conv2DLayerNode *)(layer->prevLayer))->size->at(dim));
    }
    return val;
}
Node* UnfoldComposite::makeConv2DKernelOperInit(layerNode *layer) {
    list<Node *> *stmts = new list<Node *>();
    Node *init = NULL;
    string weightName = "_weight_" + to_string(layer-> level);
    Node*  weightId = new idNode(weightName);
    ((idNode *)weightId)->isArray = 1;
    Node *kernelIndex = new idNode("kernelIndex");
    constantNode *const_zero = new constantNode("integer", (long long)0);
    idNode *id_j = new idNode("j"), *id_n = new idNode("n"), *id_m = new idNode("m");
    primNode *prim = new primNode("int");
    declareNode 
                *decl_j = new declareNode(prim, id_j),
                *decl_n = new declareNode(prim, id_n),
                *decl_m = new declareNode(prim, id_m);
    stmts->push_back(decl_j);
    stmts->push_back(decl_n);
    stmts->push_back(decl_m);
    Node *init1 = NULL, *cond1 = NULL, *next_j = NULL, *stmt1 = NULL, *forNode1;
    Node *init2 = NULL, *cond2 = NULL, *next_n = NULL, *stmt2 = NULL, *forNode2;
    Node *init3 = NULL, *cond3 = NULL, *next_m = NULL, *stmt3 = NULL, *forNode3;
    Node *weightArrDec = NULL;
    auto argListIter = layer->arg_list->begin();
    // 预处理filter_index个kernel的权值矩阵
    weightArrDec = new arrayNode((expNode *)kernelIndex);
    // 第1层循环遍历核的每一层,对应上一层输出的深度
    init1 = new binopNode((expNode *)id_j, "=", (expNode *)const_zero);
    Node *depth = layer -> prevLayer == NULL ? globalSequential -> arg_list -> front() : new constantNode("integer", ((conv2DLayerNode *)layer) -> depth);
    cond1 = new binopNode((expNode *)id_j, "<", (expNode *)(depth));
    next_j = new unaryNode("POSTINC", (expNode *)id_j);
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)id_j);
    // 第2层循环遍历核中层的行
    init2 = new binopNode((expNode *)id_n, "=", (expNode *)const_zero);
    Node *dim0 = new constantNode("integer", ((conv2DLayerNode *)layer) -> kernel_size -> at(0));
    cond2 = new binopNode((expNode *)id_n, "<", (expNode *)(dim0));
    next_n = new unaryNode("POSTINC", (expNode *)id_n);
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)id_n);
    // 第3层循环遍历核中层的行的每一列
    init3 = new binopNode((expNode *)id_m, "=", (expNode *)const_zero);
    Node *dim1 = new constantNode("integer", ((conv2DLayerNode *)layer) -> kernel_size -> at(1));
    cond3 = new binopNode((expNode *)id_m, "<", (expNode *)(dim1));
    next_m = new unaryNode("POSTINC", (expNode *)id_m);
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)id_m);
    // 为 weight[kernelIndex][j][n][m]赋值
    stmt3 = new binopNode((expNode *)weightId, "=", (expNode *)const_zero);
    forNode3 = new forNode(init3, (expNode *)cond3, (expNode *)next_m, stmt3);
    stmt2 = forNode3;
    forNode2 = new forNode(init2, (expNode *)cond2, (expNode *)next_n, stmt2);
    stmt1 = forNode2;
    forNode1 = new forNode(init1, (expNode *)cond1, (expNode *)next_j, stmt1);
    stmts->push_back(forNode1);
    init = new blockNode(stmts);
    return init;
}
// input符合[h][w][depth]
Node* UnfoldComposite::makeConv2DKernelOperWork(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    Node* work = NULL;
    list<Node *> *stmtList = new list<Node *>();
    // 5层循环实现卷积操作
    Node *idPushIndex = new idNode("pushIndex"), *idTemp = new idNode("temp");
    Node *constZero = new constantNode("integer", (long long)0);
    Node *pushIndexInit = new binopNode((expNode *)idPushIndex, "=", (expNode *) constZero);
    stmtList -> push_back(pushIndexInit);
    Node *init1 = NULL, *cond1 = NULL, *nextM = NULL, *stmt1 = NULL, *forNode1 = NULL;
    Node *init2 = NULL, *cond2 = NULL, *nextN = NULL, *stmt2 = NULL, *forNode2 = NULL;
    Node *init3 = NULL, *cond3 = NULL, *nextD = NULL, *stmt3 = NULL, *forNode3 = NULL;
    Node *init4 = NULL, *cond4 = NULL, *nextI = NULL, *stmt4 = NULL, *forNode4 = NULL;
    Node *init5 = NULL, *cond5 = NULL, *nextJ = NULL, *stmt5 = NULL, *forNode5 = NULL;
    Node *idI = new idNode("i"),
         *idJ = new idNode("j"),
         *idN = new idNode("n"),
         *idM = new idNode("m"),
         *idD = new idNode("d"),
         *idX = new idNode("x");
    Node *kernelDim0 = new constantNode("integer", ((conv2DLayerNode *)layer) -> kernel_size -> at(0));
    Node *kernelDim1 = new constantNode("integer", ((conv2DLayerNode *)layer) -> kernel_size -> at(1));
    Node *kernelDepth = new constantNode("integer", ((conv2DLayerNode *)layer) -> depth);
    Node *outputHeight = new constantNode("integer", ((conv2DLayerNode *)layer) -> size->at(0));
    Node *outputWidth = new constantNode("integer", ((conv2DLayerNode *)layer) -> size->at(1));
    Node *stepHeight = new constantNode("integer", ((conv2DLayerNode *)layer) -> strides -> at(0));
    Node *stepWidth = new constantNode("integer", ((conv2DLayerNode *)layer) -> strides -> at(1));
    Node *inputDim0 = getInputDimVal(layer, 0);
    Node *inputDim1 = getInputDimVal(layer, 1);

    // output height
    init1 = new binopNode((expNode *)idM, "=", (expNode *)constZero);
    cond1 = new binopNode((expNode *)idM, "<", (expNode *)outputHeight);
    Node *nextMExp = new binopNode((expNode *)idM, "+", (expNode *)stepHeight);
    nextM = new binopNode((expNode *)idM, "=", (expNode *)nextMExp);
    // output width
    init2 = new binopNode((expNode *)idN, "=", (expNode *)constZero);
    cond2 = new binopNode((expNode *)idN, "<", (expNode *)outputWidth);
    Node *nextKExp = new binopNode((expNode *)idN, "+", (expNode *)stepWidth);
    nextN = new binopNode((expNode *)idN, "=", (expNode *)nextKExp);
    // kernel depth
    init3 = new binopNode((expNode *)idD, "=", (expNode *)constZero);
    cond3 = new binopNode((expNode *)idD, "<", (expNode *)kernelDepth);
    nextD = new unaryNode("POSTINC", (expNode *)idD);
    // kernel height
    init4 = new binopNode((expNode *)idI, "=", (expNode *)constZero);
    cond4 = new binopNode((expNode *)idI, "<", (expNode *)kernelDim0);
    nextI = new unaryNode("POSTINC", (expNode *)idI);
    // kernel width
    init5 = new binopNode((expNode *)idJ, "=", (expNode *)constZero);
    cond5 = new binopNode((expNode *)idJ, "<", (expNode *)kernelDim1);
    nextJ = new unaryNode("POSTINC", (expNode *)idJ);
    // weight[kernelIndex][n][i][j] * in[d * sizeH * sizeW + (m * strideH + i) * sizeW + n * stridesW + j]????
    Node *output = new idNode(static_cast<idNode *>(outputs -> front()) -> name),
         *input = new idNode(static_cast<idNode *>(inputs -> front()) -> name);
    Node *idNum = new idNode("num");
    ((idNode *)output) -> isArray = 1;
    ((idNode *)input) -> isArray = 1;

    Node *stepY = new binopNode((expNode *)(new binopNode((expNode *)idM, "*", (expNode *)stepHeight)), "+", (expNode *)idI);
    Node *offsetY =  new binopNode((expNode *)stepY, "*", (expNode *)(new binopNode((expNode *)inputDim1, "*", (expNode *)kernelDepth)));
    Node *stepX = new binopNode((expNode *)(new binopNode((expNode *)idN, "*", (expNode *)stepWidth)), "+", (expNode *)idJ);
    Node *offsetX = new binopNode((expNode *)stepY, "*", (expNode *)kernelDepth);
    Node *inputIndex = new binopNode((expNode *)(new binopNode((expNode *)idD, "+", (expNode *)offsetX)), "+", (expNode *)offsetY);
    ((idNode *)input) -> arg_list.push_back(inputIndex);
    Node *inputX = new binopNode((expNode *)input, ".", (expNode *)idX);
    // weight[kernelIndex][d][i][j]
    Node *kernelIndex = new idNode("kernelIndex");
    Node *weightArrDec = new arrayNode((expNode *)kernelIndex);
    ((arrayNode *)weightArrDec) -> arg_list.push_back((expNode *)idD);
    ((arrayNode *)weightArrDec) -> arg_list.push_back((expNode *)idI);
    ((arrayNode *)weightArrDec) -> arg_list.push_back((expNode *)idJ);
    Node *weightId = new idNode("_weight" + layer -> level);
    ((idNode *)weightId) -> arg_list = ((arrayNode *)weightArrDec) -> arg_list;
    Node* midRes = new binopNode((expNode *)idTemp, "+=", (expNode *)(new binopNode((expNode *)inputX, "*", (expNode *)weightId)));
    Node *block5 = new blockNode(new list<Node *>({midRes}));
    forNode5 = new forNode(init5, (expNode *)cond5, (expNode *)nextJ, block5);
    Node *block4 = new blockNode(new list<Node *>({forNode5}));
    forNode4 = new forNode(init4, (expNode *)cond4, (expNode *)nextI, block4);
    Node *block3 = new blockNode(new list<Node *>({forNode4}));
    forNode3 = new forNode(init3, (expNode *)cond3, (expNode *)nextD, block3);
    Node *tempInit = new binopNode((expNode *)idTemp, "=", (expNode *) constZero);
    list<Node *> *stmtList2 = new list<Node *>();
    stmtList2 -> push_back(tempInit);
    stmtList2 -> push_back(forNode3);
    ((idNode *)output) -> arg_list.push_back(idNum);
    Node *outputX = new binopNode((expNode *)output, ".", (expNode *)idX);
    Node *assign = new binopNode((expNode *)outputX, "=", (expNode *)idTemp);
    stmtList2 -> push_back(assign);
    Node *nextNum = new unaryNode("POSTINC", (expNode *)idNum);
    stmtList2 -> push_back(nextNum);
    Node *block2  = new blockNode(stmtList2);
    forNode2 = new forNode(init2, (expNode *)cond2, (expNode *)nextN, block2);
    Node *block1 = new blockNode(new list<Node *>({forNode2}));
    forNode1 = new forNode(init1, (expNode *)cond1, (expNode *)nextN, block1);
    Node *numInit = new binopNode((expNode *)idNum, "=", (expNode *)constZero);
    stmtList -> push_back(forNode1);
    work = new blockNode(stmtList);
    return work;
}

Node* UnfoldComposite::makeConv2DKernelBody(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    Node* body = NULL;
    list<Node *> *paramList = new list<Node *>(), *stmtList = new list<Node *>();
    Node *kernelIndex = new idNode("kernelIndex");
    // top->put(static_cast<idNode*>($2)->name,static_cast<idNode*>($2));???
    ((idNode *)kernelIndex) -> valType = "integer";
    paramList -> push_back(kernelIndex);
    operatorNode *oper  = makeConv2DKernelOper(layer, inputs, outputs);
    // 调用kernel operator
    Node *binop = new binopNode((expNode *)(outputs -> front()), "=", (expNode *)oper) ;
    stmtList -> push_back(binop);
    paramNode *param = new paramNode(paramList);
    body = new compBodyNode(param, stmtList);
    return body;
}
// 生成名为"conv2DKernel_" + level 的卷积层计算节点 单输入单输出
compositeNode* UnfoldComposite::makeConv2DKernel(layerNode *layer) {
    compositeNode *comp = NULL;
    Node *compHead = NULL, *compBody = NULL, *compInOut = NULL;
    list<Node *> *inputs = new list<Node *>(), *outputs = new list<Node *>();
    Node* input = makeStream("input", "double");
    inputs->push_back(input);
    Node* output = makeStream("output", "double");
    outputs->push_back(output);
    compInOut = new ComInOutNode(inputs, outputs);
    string compName = "conv2DKernel_" + layer->level;
    compHead = new compHeadNode(compName, (ComInOutNode *)compInOut);
    compBody = makeConv2DKernelBody((conv2DLayerNode *)layer, inputs, outputs);
    comp = new compositeNode((compHeadNode *)compHead, (compBodyNode *)compBody);
    S.InsertCompositeSymbol(compName, comp);
    return comp;
}
// 生成名为"conv2DLayer_" + level 的卷积层计算节点
compositeNode* UnfoldComposite::makeConv2DLayer(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    compositeNode *comp = NULL;
    Node *compHead = NULL, *compBody = NULL, *compInOut = NULL;
    // composite.head.inout.member.list:
    compInOut = new ComInOutNode(inputs, outputs);
    compHead = new compHeadNode("conv2DLayer_" + layer->level, (ComInOutNode *)compInOut);
    compBody = makeConv2DLayerBody(layer, inputs, outputs);
    comp = new compositeNode((compHeadNode *)compHead, (compBodyNode *)compBody);
    return comp;
}

Node* UnfoldComposite::makeConv2DLayerBody(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    Node *body = NULL;
    list<Node *> *stmtList = new list<Node *>(), *splitjoinStmtList = new list<Node *>(), *splitjoinBodyStmts = new list<Node *>();
    Node *output = outputs -> front();
    idNode *streamDeclId = new idNode("x");
    primNode *streamType = new primNode("double");
    streamDeclId->valType = streamType->name;
    Node *streamDecl = new strdclNode(streamDeclId);
    Node *res = new idNode("result");
    ((strdclNode *)streamDecl)->id_list.push_back((idNode *)res);
    list<Node *> *tempStream = new list<Node *>({res});
    idNode *idI = new idNode("i");
    primNode *prim = new primNode("int");
    declareNode *declI = new declareNode(prim, idI);
    splitjoinStmtList->push_back(declI);
    // arg?
    Node *constOne = new constantNode("integer", (long long)1);
    Node *duplicate = new duplicateNode(NULL);
    Node *split = new splitNode(duplicate);                                                                            
    // roundrobinNode *roundrobin = new roundrobinNode(new list<Node *>({constOne}));
    roundrobinNode *roundrobin = new roundrobinNode(NULL);
    constantNode *constZero = new constantNode("integer", (long long)0);
    constantNode *constFilters = new constantNode("integer", (long long)(((conv2DLayerNode *)layer)->filters));
    Node *init = new binopNode((expNode *)idI, "=", (expNode *)constZero);
    Node *cond = new binopNode((expNode *)idI, "<", (expNode *)(constFilters));
    Node *next = new unaryNode("POSTINC", (expNode *)idI);
    list<Node *> *blockStmtList = new list<Node *>();
    string compName = "conv2DKernel_" + layer -> level;
    list<Node *> *argList = new list<Node *>({idI});
    // 根据layer生成相应的composite
    compositeNode *actual_comp = makeConv2DKernel(layer);
    Node *kernel = new compositeCallNode(NULL, compName, argList, NULL, actual_comp);
    Node *add = new addNode(kernel);
    blockStmtList -> push_back(add);
    Node *block = new blockNode(blockStmtList);
    Node *splitjoinFor = new forNode(init, (expNode *)cond, (expNode *)next, block);
    splitjoinBodyStmts -> push_back(splitjoinFor);
    joinNode *join = new joinNode(roundrobin);
    splitjoinNode *splitjoin = new splitjoinNode(inputs, tempStream, (splitNode *)split, splitjoinStmtList, splitjoinBodyStmts,(joinNode *)join);
    Node* conv = new binopNode((expNode *)((idNode *)res), "=", (expNode *)splitjoin);
    stmtList->push_back(conv);
    compositeNode *copyComp = makeCopyComp(tempStream, outputs);
    compositeCallNode *callCopy = new compositeCallNode(outputs, "copy", NULL, tempStream, copyComp);
    Node* copy = new binopNode((expNode *)((inOutdeclNode *)output) -> id, "=", (expNode *)callCopy);
    stmtList->push_back(copy);
    body = new compBodyNode(NULL, stmtList);
    return body;
}

Node* UnfoldComposite::makeStream(string name, string type) {
    idNode *val = new idNode("x");
    primNode *valType = new primNode(type);
    val->valType = valType->name;
    strdclNode *strdcl = new strdclNode(val);
    idNode *inputId = new idNode(name);
    return new inOutdeclNode(strdcl, inputId);
}

compositeNode* UnfoldComposite::makeCopyComp(list<Node *>  *inputs, list<Node *> *outputs) {
    compositeNode *comp = NULL;
    Node *compHead = NULL, *compBody = NULL, *compInOut = NULL;
    compInOut = new ComInOutNode(inputs, outputs);
    string compName = "copy";
    compHead = new compHeadNode(compName, (ComInOutNode *)compInOut);
    list<Node *> *compBodyStmtList = new list<Node *>();
    list<Node *> *workStmtList = new list<Node *>();
    Node *constZero = new constantNode("integer", (long long)0);
    idNode *input = (idNode *)(inputs -> front());
    idNode *output1 = (idNode *)(outputs -> front());
    idNode *output2 = (idNode *)(outputs -> back());
    Node *idX = new idNode("x");    
    input -> isArray = 1;
    input -> arg_list.push_back(constZero);
    Node *inputX = new binopNode((expNode *)input, ".", (expNode *)idX);
    output1 -> isArray = 1;
    output1 -> arg_list.push_back(constZero);
    Node *output1X = new binopNode((expNode *)output1, ".", (expNode *)idX);
    output2 -> isArray = 1;
    output2 -> arg_list.push_back(constZero);
    Node *output2X = new binopNode((expNode *)output2, ".", (expNode *)idX);
    Node *exp1 = new binopNode((expNode *) output1X, "=", (expNode *)inputX);
    Node *exp2 = new binopNode((expNode *) output2X, "=", (expNode *) inputX);
    workStmtList -> push_back(exp1);
    workStmtList -> push_back(exp2);
    Node *work = new blockNode(workStmtList);
    list<Node *> *winList = new list<Node *>();
    constantNode *constOne = new constantNode("integer", (long long)1);
    tumblingNode *tum = new tumblingNode(new list<Node *>({constOne}));
    for (auto output : *outputs) {
        winStmtNode *winStmt = new winStmtNode(((idNode *)output) -> name, tum);
        winList -> push_back(winStmt);
    }
    slidingNode *slid = new slidingNode(new list<Node *>({constOne, constOne}));
    winStmtNode *winStmt = new winStmtNode(((idNode *)(inputs -> front())) -> name, slid);
    winList -> push_back(winStmt);
    windowNode *win = new windowNode(winList);
    operBodyNode *operBody = new operBodyNode(NULL, NULL, work, win);
    Node* oper = new operatorNode(outputs, "copy", inputs, operBody);
    Node* exp = new binopNode((expNode *)new tupleNode(outputs), "=", (expNode *)oper);
    compBodyStmtList->push_back(exp);
    compBody = new compBodyNode(NULL, compBodyStmtList);
    comp = new compositeNode((compHeadNode *)compHead, (compBodyNode *)compBody);
    return comp;
}
// 生成名为"conv2DLayer_" + level 的卷积层反向传播计算节点
compositeNode* UnfoldComposite::makeDConv2DLayer(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    compositeNode *comp = NULL;
    Node *compHead = NULL, *compBody = NULL, *compInOut = NULL;
    Node *error = makeStream("input2", "double");
    compInOut = new ComInOutNode(inputs, outputs);
    compHead = new compHeadNode("dConv2DLayer_" + layer->level, (ComInOutNode *)compInOut);
    compBody = makeDConv2DLayerBody(layer, inputs, outputs);
    comp = new compositeNode((compHeadNode *)compHead, (compBodyNode *)compBody);
    return comp;
}

Node* UnfoldComposite::makeDConv2DLayerBody(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    compositeNode *res = NULL;
    Node *compHead = NULL, *compBody = NULL, *compInOut = NULL;
    string streamName = "DConv2dStream_" + to_string(layer -> level);
    string compName = "DConv2D_" + to_string(layer -> level);
    list<Node *> *errorList = NULL, *fpInputList = NULL;
    // splitOperator1 将误差duplicate成filters份, splitOperator2 将传入正向传播的输入再次传入到反向传播中,并duplicate成多份
    operatorNode *splitOperator1 = NULL, *splitOperator2 = NULL, *joinOperator = NULL;
    // join operator的输入流
    list<Node *> *inputs_join = new list<Node *>();
    list<compositeCallNode *> *comCallList = new list<compositeCallNode *>();
    list<Node *> *compStmtList = new list<Node *>();

    auto inputIter = inputs -> begin();
    splitOperator1 = makeConv2DSplitOperator(*inputIter, layer);
    cout<<((idNode *)(*inputIter))->name << endl;
    splitOperator2 = makeConv2DSplitOperator(*(++inputIter), layer);
    cout<<((idNode *)(*inputIter))->name << endl;
    errorList = splitOperator1 -> outputs;
    fpInputList = splitOperator2 -> outputs;
    auto errorIter = errorList -> begin();
    auto fpInputIter = fpInputList -> begin();
    idNode *streamDeclId = new idNode("x");
    primNode *streamType = new primNode("double");
    streamDeclId->valType = streamType->name;
    Node *streamDecl = new strdclNode(streamDeclId);
    for(auto error : *errorList) {
        ((strdclNode*)streamDecl)->id_list.push_back((idNode *)error);
    }
    for(auto fpInput : *fpInputList) {
        ((strdclNode*)streamDecl)->id_list.push_back((idNode *)fpInput);
    }
    for (int i = 0; i < ((conv2DLayerNode *)layer) -> filters; i++) {
        string tempName = streamName + "_" + to_string(i);
        idNode *kernelOutput = new idNode(tempName);
        ((strdclNode*)streamDecl)->id_list.push_back(kernelOutput);
        //compositeCall的输出流是join节点的输入流
        inputs_join->push_back(kernelOutput);
        // kernel的输出流
        list<Node *> *call_outputs = new list<Node *>({kernelOutput});
        Node *arg = new constantNode("integer", (long long)i);
        list<Node *> *argList = new list<Node *>({arg});
        //compositeCall的输入流
        list<Node *> *call_inputs = new list<Node *>({*errorIter, *fpInputIter});
        compositeNode *dKernelComp = makeDConv2DKernel(layer, call_inputs, call_outputs);
        compositeCallNode *call = new compositeCallNode(call_outputs, tempName, argList, call_inputs, dKernelComp);
        comCallList->push_back(call);
        errorIter++;
        fpInputIter++;
    }

    joinOperator = makeConv2DJoinOperator(outputs->front(), inputs_join, layer);
    compStmtList->push_back(streamDecl);
    compStmtList->push_back(splitOperator1);
    compStmtList->push_back(splitOperator2);
    for (auto it : *comCallList)
    {
        compStmtList->push_back(it);
    }
    compStmtList->push_back(joinOperator);
    compBody = new compBodyNode(NULL, compStmtList);
    return compBody;
}
// 由于反向传播每一层有两个输入流 不可以直接使用splitjoin结构
// 将输入的数据复制成layer->filters份
operatorNode *UnfoldComposite::makeConv2DSplitOperator(Node *input, layerNode *layer) {
    operatorNode *res = NULL;
    Node *work = NULL;
    windowNode *window = NULL;
    operBodyNode *body = NULL;
    string operName = "dConv2D_Duplicate";
    string streamName = "dConv2dDup";
    list<Node *> *outputs = new list<Node *>();
    list<Node *> *inputs = new list<Node *>({input});
    list<Node *> *winStmt = new list<Node *>();
    static int index = 0;
    index++;
    string inputName = ((idNode *)input) -> name;
    Node *constOne = new constantNode("intager", (long long)1);
    Node *constZero = new constantNode("intager", (long long)0);
    // 输出窗口
    for(int i = 0; i < ((conv2DLayerNode *)layer) -> filters; i++) {
        string tempName = streamName + "_" + to_string(index) + "_" + to_string(layer->level) + "_" + to_string(i);
        idNode *id = new idNode(tempName);
        outputs -> push_back(id);
        tumblingNode *tum = new tumblingNode(new list<Node *>({constOne}));
        winStmtNode *win = new winStmtNode(tempName, tum);
        winStmt->push_back(win);
    }
    // 输入窗口
    slidingNode *slid = new slidingNode(new list<Node *>({constOne, constOne}));
    winStmtNode *win = new winStmtNode(inputName, slid);
    // work
    list<Node *> *stmts = new list<Node *>();
    // 遍历outputs, 并赋值
    for(auto output : *outputs) {
        idNode *left = new idNode(((idNode *)output)->name);
        left->isArray = 1;
        left->arg_list.push_back(constZero);
        idNode *right = new idNode(static_cast<idNode *>(input)->name);
        right->isArray = 1;
        right->arg_list.push_back(constZero);
        Node *stmt = new binopNode((expNode *)left, "=", (expNode *)right);
        stmts -> push_back(stmt);
    }
    work = new blockNode(stmts);
    winStmt->push_back(win);
    window = new windowNode(winStmt);
    body = new operBodyNode(NULL, NULL, work, window);
    res = new operatorNode(outputs, operName, inputs, body);
    return res;
}

operatorNode *UnfoldComposite::makeConv2DJoinOperator(Node *output, list<Node *> *inputs, layerNode *layer)
{
    Node *work = NULL;
    operatorNode *res = NULL;
    windowNode *window = NULL;
    operBodyNode *body = NULL;
    string operName = "join";
    assert(output->type == Id);
    string output_name = ((idNode *)output)->name;
    int len = compositeCall_list.size();
    long long sum = ((conv2DLayerNode *)layer) -> filters;
    list<Node *> *outputs = new list<Node *>({output});
    list<Node *> *win_stmt = new list<Node *>();
    list<Node *> *arguments = new list<Node *>();
    Node *constOne = new constantNode("integer", (long long)1);
    auto iter = inputs->begin();
    for (int i = 0; i < sum; i++) {
        arguments->push_back(constOne);
        slidingNode *slid = new slidingNode(new list<Node *>({constOne, constOne}));
        winStmtNode *win = new winStmtNode(((idNode *)*iter)->name, slid);
        win_stmt->push_back(win);
        iter++;
    }
    /*添加join节点输出窗口*/
    constantNode *constNode = new constantNode("integer", sum);
    tumblingNode *tum = new tumblingNode(new list<Node *>({constNode}));
    winStmtNode *win = new winStmtNode(output_name, tum);
    win_stmt->push_back(win);
    /*end*/
    work = MakeJoinWork(inputs, arguments, outputs);
    window = new windowNode(win_stmt);
    body = new operBodyNode(NULL, NULL, work, window);
    res = new operatorNode(outputs, operName, inputs, body);
    //cout << "-----------------join end---------------------" << endl;
    return res;
}

compositeNode* UnfoldComposite::makeDConv2DKernel(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    compositeNode *comp = NULL;
    Node *compHead = NULL, *compBody = NULL, *compInOut = NULL;
    compInOut = new ComInOutNode(inputs, outputs);
    string compName = "dConv2DKernel_" + layer->level;
    compHead = new compHeadNode(compName, (ComInOutNode *)compInOut);
    compBody = makeDConv2DKernelBody((conv2DLayerNode *)layer, inputs, outputs);
    comp = new compositeNode((compHeadNode *)compHead, (compBodyNode *)compBody);
    return comp;
}

Node* UnfoldComposite::makeDConv2DKernelBody(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    Node* body = NULL;
    list<Node *> *paramList = new list<Node *>(), *stmtList = new list<Node *>();
    Node *kernelIndex = new idNode("kernelIndex");
    ((idNode *)kernelIndex) -> valType = "integer";
    paramList -> push_back(kernelIndex);
    operatorNode *oper  = makeDConv2DKernelOper(layer, inputs, outputs);
    Node *binop = new binopNode((expNode *)(outputs -> front()), "=", (expNode *)oper) ;
    stmtList -> push_back(binop);
    paramNode *param = new paramNode(paramList);
    body = new compBodyNode(param, stmtList);
    return body;
}
// 
// input1为经扩展和膨胀后的误差矩阵
// 尺寸为[newHeight][newWidth][kernel_number]
// newHeight = (layer -> size[0] - 1) * stride + 1;
// [i][j][d] => [padding_y + i * stride_y][padding_x + j * stride_x][d]
// ?需要判断上一层的类型,现在假设是卷积层
operatorNode* UnfoldComposite::makeDConv2DKernelOper(layerNode* layer, list<Node *> *inputs, list<Node *> *outputs) {
    string operName = "dConv2D";
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;
    list<Node *> *winStmt = new list<Node *>(), *bodyStmtList = new list<Node *>();
    // *****输入窗口*****
    //[dim0][dim1]
    long long errInputDim0 = ((conv2DLayerNode *)layer) -> errorSize -> at(0);
    long long errInputDim1 = ((conv2DLayerNode *)layer) -> errorSize -> at(1);
    // 经反向传播计算逐层传递的误差
    auto inputIter = inputs->begin();
    long long errPopVal = errInputDim0 * errInputDim1 * ((conv2DLayerNode *)layer) -> filters;
    Node *errPop = new constantNode("integer", errPopVal);
    slidingNode *slid1 = new slidingNode(new list<Node *>({errPop, errPop}));
    winStmtNode *win1 = new winStmtNode(((idNode *)(* inputIter))->name, slid1);
    winStmt->push_back(win1);

    // 经正向传播传入该层的输入
    inputIter++;
    Node *fpInputDim0, *fpInputDim1;
    auto inputSize = layer->prevLayer != NULL ? layer->prevLayer->arg_list->front() : globalSequential->arg_list->front();
    if (inputSize -> type == constant) {
        fpInputDim0 = inputSize;
        fpInputDim1 = inputSize;
    } else {
        fpInputDim0 = ((tupleNode *)inputSize)->tupleList->front();
        fpInputDim1 = ((tupleNode *)inputSize)->tupleList->back();
    }
    long long fpInputPopVal = ((constantNode *)fpInputDim0)->llval * ((constantNode *)fpInputDim1)->llval * ((conv2DLayerNode *)layer)->depth;
    Node *fpInputPop = new constantNode("integer", fpInputPopVal);
    slidingNode *slid2 = new slidingNode(new list<Node *>({fpInputPop, fpInputPop}));
    winStmtNode *win2 = new winStmtNode(((idNode *)(* inputIter))->name, slid2);
    winStmt->push_back(win2);

    // *****输出窗口*****
    auto outputIter = outputs->begin();
    long long pushVal;
    if (layer -> prevLayer) {
        conv2DLayerNode *prevLayer = (conv2DLayerNode *)(layer -> prevLayer);
        // 上一层是卷积层,需要对计算的误差做膨胀和扩展
        if (prevLayer -> layerName == "conv2D") {
            pushVal = prevLayer -> errorSize -> at(0) * prevLayer -> errorSize -> at(1) * prevLayer -> filters;
        }
    } else {
        // 当前到达第一层
        pushVal = ((conv2DLayerNode *)layer) -> size -> at(0) * ((conv2DLayerNode *)layer) -> size -> at(1) * ((conv2DLayerNode *)layer) -> depth;
    }
    Node *push = new constantNode("integer", pushVal);
    tumblingNode *tumb = new tumblingNode(new list<Node *>({push}));
    winStmtNode *win3 = new winStmtNode(((idNode *)(*outputIter))->name, tumb);
    winStmt->push_back(win3);
    window = new windowNode(winStmt);

    primNode *primInt = new primNode("int"),
             *primDouble = new primNode("double");
    idNode *idI = new idNode("i"),
           *idJ = new idNode("j"),
           *idN = new idNode("n"),
           *idM = new idNode("m"),
           *idK = new idNode("k"),
           *idPushIndex = new idNode("pushIndex"),
           *idTemp = new idNode("temp");
    // top->put(static_cast<idNode*>($2)->name,static_cast<idNode*>($2)); ???
    Node *declInt = new declareNode(primInt, idI),
        //  *declJ = new declareNode(primInt, idJ),
        //  *declN = new declareNode(primInt, idN),
        //  *declM = new declareNode(primInt, idM),
        //  *declK = new declareNode(primInt, idK),
        //  *declPushIndex = new declareNode(primInt, idPushIndex),
         *declDouble = new declareNode(primDouble, idTemp);
    ((declareNode *)declInt )-> id_list.push_back(idJ);
    ((declareNode *)declInt )-> id_list.push_back(idN);
    ((declareNode *)declInt )-> id_list.push_back(idM);
    ((declareNode *)declInt )-> id_list.push_back(idK);
    ((declareNode *)declInt )-> id_list.push_back(idPushIndex);
    bodyStmtList -> push_back(declInt);
    bodyStmtList -> push_back(declDouble);
    init = NULL;
    work = makeDConv2DKernelOperWork(layer, inputs, outputs);
    body = new operBodyNode(bodyStmtList, init, work, window);
    return new operatorNode(outputs, operName, inputs, body);
}

Node* UnfoldComposite::makeDConv2DKernelOperWork(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    Node *work = NULL;
    list<Node *> *stmtList = new list<Node *>();
    Node *idPushIndex = new idNode("pushIndex"), *idTemp = new idNode("temp");
    Node *constZero = new constantNode("integer", (long long)0);
    Node *constOne = new constantNode("integer", (long long)1);
    Node *pushIndexInit = new binopNode((expNode *)idPushIndex, "=", (expNode *) constZero);
    stmtList -> push_back(pushIndexInit);
    Node *init1 = NULL, *cond1 = NULL, *nextM = NULL, *stmt1 = NULL, *forNode1 = NULL;
    Node *init2 = NULL, *cond2 = NULL, *nextN = NULL, *stmt2 = NULL, *forNode2 = NULL;
    Node *init3 = NULL, *cond3 = NULL, *nextD = NULL, *stmt3 = NULL, *forNode3 = NULL;
    Node *init4 = NULL, *cond4 = NULL, *nextI = NULL, *stmt4 = NULL, *forNode4 = NULL;
    Node *init5 = NULL, *cond5 = NULL, *nextJ = NULL, *stmt5 = NULL, *forNode5 = NULL;
    Node *idI = new idNode("i"),
         *idJ = new idNode("j"),
         *idN = new idNode("n"),
         *idM = new idNode("m"),
         *idD = new idNode("d"),
         *idX = new idNode("x");
    Node *kernelDim0 = new constantNode("integer", ((conv2DLayerNode *)layer) -> kernel_size -> at(0));
    Node *kernelDim1 = new constantNode("integer", ((conv2DLayerNode *)layer) -> kernel_size -> at(1));
    Node *kernelDepth = new constantNode("integer", ((conv2DLayerNode *)layer) -> depth);
    Node *inputDim0Val = new constantNode("integer", ((conv2DLayerNode *)layer) -> errorSize -> at(0));
    Node *inputDim1Val = new constantNode("integer", ((conv2DLayerNode *)layer) -> errorSize -> at(1));
    Node *resDim0Val = new constantNode("integer", ((conv2DLayerNode *)layer) -> size -> at(0));
    Node *resDim1Val = new constantNode("integer", ((conv2DLayerNode *)layer) -> size -> at(1));
    Node *outputDim0Val = NULL,
         *outputDim1Val = NULL,
         *prevPaddingDim0 = NULL,
         *prevPaddingDim1 = NULL,
         *prevStrideDim0 = NULL,
         *prevStrideDim1 = NULL;
    if (layer -> prevLayer != NULL) {
        outputDim0Val = new constantNode("integer", ((conv2DLayerNode *)(layer -> prevLayer)) -> errorSize -> at(0));
        outputDim1Val = new constantNode("integer", ((conv2DLayerNode *)(layer -> prevLayer)) -> errorSize -> at(1));
        prevPaddingDim0 = new constantNode("integer", ((conv2DLayerNode *)(layer -> prevLayer)) -> paddings -> at(0));
        prevPaddingDim1 = new constantNode("integer", ((conv2DLayerNode *)(layer -> prevLayer)) -> paddings -> at(1));
        prevStrideDim0 = new constantNode("integer", ((conv2DLayerNode *)(layer -> prevLayer)) -> strides -> at(0));
        prevStrideDim1 = new constantNode("integer", ((conv2DLayerNode *)(layer -> prevLayer)) -> strides -> at(1));
    } else {
        outputDim0Val = new constantNode("integer", ((conv2DLayerNode *)layer) -> size -> at(0));
        outputDim1Val = new constantNode("integer", ((conv2DLayerNode *)layer) -> size -> at(1));
        prevPaddingDim0 = constZero;
        prevPaddingDim1 = constZero;
        prevStrideDim0 = constOne;
        prevStrideDim1 = constOne;
    }
    // 判断上一层的类型, 若是Conv2D则按照变换后的尺寸输出 ??? 前面一层可以是全连接层吗
    // if (layer -> prevLayer -> layerName == "conv2D") {
    //     // ...
    // }
    // 经扩展膨胀变换后的输入误差矩阵 与翻转后的权重矩阵 做卷积运算
    // 最终输出可以直接用于下一层计算输入误差经过变换的误差矩阵
    // (m, n, d) => (padding0 + m * stride0, padding1 + n * stride1, d)
    // output height
    init1 = new binopNode((expNode *)idM, "=", (expNode *)constZero);
    cond1 = new binopNode((expNode *)idM, "<", (expNode *)resDim0Val);
    Node *nextMExp = new binopNode((expNode *)idM, "+", (expNode *)constOne);
    nextM = new binopNode((expNode *)idM, "=", (expNode *)nextMExp);
    // output width
    init2 = new binopNode((expNode *)idN, "=", (expNode *)constZero);
    cond2 = new binopNode((expNode *)idN, "<", (expNode *)resDim1Val);
    Node *nextKExp = new binopNode((expNode *)idN, "+", (expNode *)constOne);
    nextN = new binopNode((expNode *)idN, "=", (expNode *)nextKExp);
    // kernel depth
    init3 = new binopNode((expNode *)idD, "=", (expNode *)constZero);
    cond3 = new binopNode((expNode *)idD, "<", (expNode *)kernelDepth);
    nextD = new unaryNode("POSTINC", (expNode *)idD);
    // kernel height
    init4 = new binopNode((expNode *)idI, "=", (expNode *)constZero);
    cond4 = new binopNode((expNode *)idI, "<", (expNode *)kernelDim0);
    nextI = new unaryNode("POSTINC", (expNode *)idI);
    // kernel width
    init5 = new binopNode((expNode *)idJ, "=", (expNode *)constZero);
    cond5 = new binopNode((expNode *)idJ, "<", (expNode *)kernelDim1);
    nextJ = new unaryNode("POSTINC", (expNode *)idJ);

    Node *output = new idNode(static_cast<idNode *>(outputs -> front()) -> name),
         *inputError = new idNode(static_cast<idNode *>(inputs -> front()) -> name);
    Node *idNum = new idNode("num");
    ((idNode *)output) -> isArray = 1;
    ((idNode *)inputError) -> isArray = 1;
    // weight[kernelIndex][d][kernelDim0 - i][kernelDim1 - j]
    Node *kernelIndex = new idNode("kernelIndex");
    Node *weightArrDec = new arrayNode((expNode *)kernelIndex);
    ((arrayNode *)weightArrDec) -> arg_list.push_back((expNode *)idD);
    ((arrayNode *)weightArrDec) -> arg_list.push_back((expNode *)(new binopNode((expNode *)(new constantNode("integer", ((conv2DLayerNode *)layer) -> kernel_size -> at(0))), "-", (expNode *)idI)));
    ((arrayNode *)weightArrDec) -> arg_list.push_back((expNode *)(new binopNode((expNode *)(new constantNode("integer", ((conv2DLayerNode *)layer) -> kernel_size -> at(1))), "-", (expNode *)idJ)));
    Node *weightId = new idNode("_weight" + layer -> level);
    ((idNode *)weightId) -> arg_list = ((arrayNode *)weightArrDec) -> arg_list;
    // inputError[inputIndex] juanji weight = output[(padding0 + m * stride0, padding1 + n * stride1, d)]
    // 求inputErrorIndex 和 outputsIndex
    
    // inputErrorIndex [m+i][n+j][d]
    Node *stepDim0 = new binopNode((expNode *)idM, "+", (expNode *)idI);
    Node *offsetDim0 = new binopNode((expNode *)stepDim0, "*", (expNode *)(new binopNode((expNode *)inputDim1Val, "*", (expNode *)kernelDepth)));
    Node *stepDim1 = new binopNode((expNode *)idN, "+", (expNode *)idJ);
    Node *offsetDim1 = new binopNode((expNode *)stepDim1, "*", (expNode *)kernelDepth);
    Node *inputErrorIndex = new binopNode((expNode *)offsetDim0, "+", (expNode *)(new binopNode((expNode *)offsetDim1, "+", (expNode *)idD)));
    ((idNode *)inputError) -> arg_list.push_back(inputErrorIndex);
    Node *inputErrorX = new binopNode((expNode *)inputError, ".", (expNode *)idX);
    // outputIndex[padding0 + m * stride0][padding1 + n * stride1][d]
    Node *outputDim0 = new binopNode((expNode *)prevPaddingDim0, "+", (expNode *)(new binopNode((expNode *)idM, "*", (expNode *)prevStrideDim0)));
    Node *outputOffsetDim0 = new binopNode((expNode *)outputDim0, "*", (expNode *)(new binopNode((expNode *)outputDim0Val, "*", (expNode *)kernelDepth)));    
    Node *outputDim1 = new binopNode((expNode *)prevPaddingDim1, "+", (expNode *)(new binopNode((expNode *)idN, "*", (expNode *)prevStrideDim1)));
    Node *outputOffsetDim1 = new binopNode((expNode *)outputDim1, "*", (expNode *)kernelDepth);
    Node *outputIndex = new binopNode((expNode *)outputOffsetDim0, "+", (expNode *)(new binopNode((expNode *)outputOffsetDim1, "+", (expNode *)idD)));
    ((idNode *)output) -> arg_list.push_back(outputIndex);

    // 计算 output[outputIndex] = inputError 卷积 kernel
    /*
        for(m = 0 ; m < outputDim0; m++) {
            for (n = 0; n < outputDim1; n++) {
                temp = 0;
                for (d = 0; d < depth; d++) {
                    for (i = 0; i < kernelDim0; i++) {
                        for (j = 0; j < kernelDim1; j++) {
                            temp += (input[inputIndex] * weight[kernelIndex][d][i][j])
                        }
                    }
                }
                output[outIndex] = temp;
            }
        }
     */
    Node* midRes = new binopNode((expNode *)idTemp, "+=", (expNode *)(new binopNode((expNode *)inputErrorX, "*", (expNode *)weightId)));
    Node *block5 = new blockNode(new list<Node *>({midRes}));
    forNode5 = new forNode(init5, (expNode *)cond5, (expNode *)nextJ, block5);
    Node *block4 = new blockNode(new list<Node *>({forNode5}));
    forNode4 = new forNode(init4, (expNode *)cond4, (expNode *)nextI, block4);
    Node *block3 = new blockNode(new list<Node *>({forNode4}));
    forNode3 = new forNode(init3, (expNode *)cond3, (expNode *)nextD, block3);
    Node *tempInit = new binopNode((expNode *)idTemp, "=", (expNode *) constZero);
    list<Node *> *stmtList2 = new list<Node *>();
    stmtList2 -> push_back(tempInit);
    stmtList2 -> push_back(forNode3);
    ((idNode *)output) -> arg_list.push_back(outputIndex);
    Node *outputX = new binopNode((expNode *)output, ".", (expNode *)idX);
    Node *assign = new binopNode((expNode *)outputX, "=", (expNode *)idTemp);
    stmtList2 -> push_back(assign);
    Node *block2  = new blockNode(stmtList2);
    forNode2 = new forNode(init2, (expNode *)cond2, (expNode *)nextN, block2);
    Node *block1 = new blockNode(new list<Node *>({forNode2}));
    forNode1 = new forNode(init1, (expNode *)cond1, (expNode *)nextN, block1);
    stmtList -> push_back(forNode1);
    // ??? 继续
    // 计算损失函数关于权值的偏导数
    // 膨胀后的误差 卷积 正向传播中传入本层的输入
    // i_ = 
    /* for (m = 0; m < kernelDim0; m++) {
        for (n = 0; n < kernelDim1; n++) {
            for (d = 0; d < depth; d++) {
                temp = 0;
                for (i = 0; i < ; i++) {
                    for (j = 0; j < ; j++) {
                        temp += 
                    }
                }
                dWeight[d][m][n] = temp;
            }
        }
    }

    */

    work = new blockNode(stmtList);
    return work;
}

