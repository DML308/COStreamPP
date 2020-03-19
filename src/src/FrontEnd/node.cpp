#include "node.h"
#include <cstring>
string listToString(list<Node *> list)
{
    int k = 0;
    string str = "";
    for (auto i : list)
        str += (k++ > 0 ? "," : "") + i->toString();
    if (str[str.size() - 1] == ';')
    { // ???去掉分号
        str[str.size() - 1] = ' ';
    }
    return str;
}

void Node::setLoc(YYLTYPE loc)
{
    this->loc->first_line = loc.first_line;
    this->loc->first_column = loc.first_column;
    this->loc->last_line = loc.last_line;
    this->loc->last_column = loc.last_column;
}
Node* Node::copyNode(Node * u) {
    switch (u->type)
    {
        case constant:
            break;
        case Decl:{
        Node *prim = copyNode(static_cast<declareNode *>(u)->prim);
        Node *id = static_cast<declareNode *>(u)->id_list.front();
        declareNode *tmp = new declareNode((primNode *)prim, (idNode *)id);
        for (auto it = ++static_cast<declareNode *>(u)->id_list.begin(); it != static_cast<declareNode *>(u)->id_list.end(); ++it)
            tmp->id_list.push_back((idNode *)copyNode(*it));
        return tmp;
        break;
        }
        case Id:
        {
            idNode *node = static_cast<idNode *>(u);
            idNode *tmp = new idNode(node->name);

            tmp->arg_list = node->arg_list;
            tmp->init = node->init;
            tmp->isArray = node->isArray;
            tmp->isParam = node->isParam;
            tmp->isStream = node->isStream;
            tmp->level = node->level;
            tmp->version = node->version;
            tmp->type = node->type;
            tmp->valType = node->valType;
            //cout << "location:" << node->loc->first_line << " idname= " << node->name << "   " << node << " " << u << endl;
            return tmp;
            break;
        }
        case Binop:
        {
            Node *left = copyNode(static_cast<binopNode *>(u)->left);
            Node *right = copyNode(static_cast<binopNode *>(u)->right);
            binopNode *tmp = new binopNode((expNode *)left, static_cast<binopNode *>(u)->op, (expNode *)right);
            if (right->type == Operator_) {
                if (left->type == Id) {
                    ((operatorNode *)right)->outputs = new list<Node *>({left});
                } else if (left->type == Tuple) {
                    ((operatorNode *)right)->outputs = ((tupleNode *)left)->tupleList;
                } else {
                    assert("wrong output stream");
                }
            } else if (right->type == CompositeCall) {
                if (left->type == Id) {
                    ((compositeCallNode *)right)->outputs = new list<Node *>({left});
                } else if (left->type == Tuple) {
                    ((compositeCallNode *)right)->outputs = ((tupleNode *)left)->tupleList;
                } else {
                    assert("wrong output stream");
                }
            } else if (right->type == SplitJoin) {
                if (left -> type == Id) {
                    ((splitjoinNode *)right)->outputs = new list<Node *>({left});
                } else {
                    assert("wrong output stream");
                }
            } else if (right->type == Pipeline) {
                if(left -> type == Id) {
                    ((pipelineNode *)right)->outputs = new list<Node *>({left});
                } else {
                    assert("wrong output stream");
                }
            }
            return tmp;
            break;
        }
        case Tuple: {
            list<Node *> *tupleList = new list<Node *>();
            for (auto it=(static_cast<tupleNode *>(u)->tupleList)->begin(); it != (static_cast<tupleNode *>(u)->tupleList)->end(); it++) {
                tupleList->push_back(copyNode(*it));
            }
            Node* newTuple = new tupleNode(tupleList);
            return newTuple;
            break;
        }
        case Paren:
        {
            Node *exp = copyNode(static_cast<parenNode *>(u)->exp);
            parenNode *tmp = new parenNode((expNode *)exp);
            return tmp;
            break;
        }
        case Unary:
        {
            Node *exp = copyNode(static_cast<unaryNode *>(u)->exp);
            unaryNode *tmp = new unaryNode(static_cast<unaryNode *>(u)->op, (expNode *)exp);
            return tmp;
            break;
        }
        case Cast:
        {

            copyNode(static_cast<castNode *>(u)->exp);
            break;
        }
        case Ternary:
        {
            Node *first = copyNode(static_cast<ternaryNode *>(u)->first);
            Node *second = copyNode(static_cast<ternaryNode *>(u)->second);
            Node *third = copyNode(static_cast<ternaryNode *>(u)->third);
            ternaryNode *tmp = new ternaryNode((expNode *)first, (expNode *)second, (expNode *)third);
            return tmp;
            break;
        }
        case Initializer:
        case Label:
            break;
        case Switch:
        {
            Node *exp = copyNode(static_cast<switchNode *>(u)->exp);
            Node *stat = copyNode(static_cast<switchNode *>(u)->stat);
            switchNode *tmp = new switchNode((expNode *)exp, stat);
            return tmp;
            break;
        }
        case Case:
        {
            Node *exp = copyNode(static_cast<caseNode *>(u)->exp);
            Node *stmt = copyNode(static_cast<caseNode *>(u)->stmt);
            caseNode *tmp = new caseNode((expNode *)exp, (expNode *)stmt);
            return tmp;
            break;
        }
        case Default:
        {
            return static_cast<defaultNode *>(u);
            break;
        }
        case If:
        {
            Node *exp = copyNode(static_cast<ifNode *>(u)->exp);
            Node *stmt = copyNode(static_cast<ifNode *>(u)->stmt);
            ifNode *tmp = new ifNode((expNode *)exp, stmt);
            return tmp;
            break;
        }
        case IfElse:
        {
            Node *exp = copyNode(static_cast<ifElseNode *>(u)->exp);
            Node *stmt1 = copyNode(static_cast<ifElseNode *>(u)->stmt1);
            Node *stmt2 = copyNode(static_cast<ifElseNode *>(u)->stmt2);
            ifElseNode *tmp = new ifElseNode((expNode *)exp, (expNode *)stmt1, (expNode *)stmt2);
            return tmp;
            break;
        }
        case While:
        {
            copyNode(static_cast<whileNode *>(u)->exp);
            copyNode(static_cast<whileNode *>(u)->stmt);
            break;
        }
        case Do:
        {
            copyNode(static_cast<doNode *>(u)->exp);
            copyNode(static_cast<doNode *>(u)->stmt);
            break;
        }
        case For:
        {
            Node *init = copyNode(static_cast<forNode *>(u)->init);
            Node *cond = copyNode(static_cast<forNode *>(u)->cond);
            Node *next = copyNode(static_cast<forNode *>(u)->next);
            Node *stmt = copyNode(static_cast<forNode *>(u)->stmt);
            forNode *tmp = new forNode(init, (expNode *)cond, (expNode *)next, stmt);
            return tmp;
            break;
        }
        case Continue:
        {
            return static_cast<continueNode *>(u);
            break;
        }
        case Break:
        {
            return static_cast<breakNode *>(u);
            break;
        }
        case Return:
        {
            Node *exp = copyNode(static_cast<returnNode *>(u)->exp);
            returnNode *tmp = new returnNode((expNode *)tmp);
            return tmp;
            break;
        }
        case Block:
        {
            list<Node *> *stmt_list = new list<Node *>();
            list<Node *> *stmts = &static_cast<blockNode *>(u)->stmt_list;
            if (stmts != NULL)
            {
                for (auto it : *stmts)
                {
                    stmt_list->push_back(copyNode(it));
                }
            }
            blockNode *block = new blockNode(stmt_list);
            return block;
            break;
        }
        case primary:
        {
            return static_cast<primNode *>(u);
            break;
        }
        case Array:
        {
            return static_cast<arrayNode *>(u);
            break;
        }
        case Call:
        {
            list<Node *> *ids = new list<Node *>();
            for (auto it : static_cast<callNode *>(u)->arg_list)
            {
                ids->push_back(copyNode(it));
            }
            callNode *tmp = new callNode(static_cast<callNode *>(u)->name, ids);
            return tmp;
            break;
        }
        case Operator_:
        {
            list<Node *> *newOutputs = new list<Node *>();
            list<Node *> *newInputs = new list<Node *>();
            for(auto it: *((operatorNode *)u)->outputs) {
                newOutputs->push_back(copyNode(it));
            }
            for(auto it: *((operatorNode *)u)->inputs) {
                newInputs->push_back(copyNode(it));
            }
            Node* newOperBody = copyNode(((operatorNode *)u)->operBody);
            return new operatorNode(newOutputs, ((operatorNode *)u)->operName, newInputs, (operBodyNode *)newOperBody);
            break;
        }
        case OperBody:
        {
            Node* work = copyNode(((operBodyNode *)u)->work);
            Node* init = NULL;
            if (((operBodyNode *)u)->init != NULL)
                init = copyNode(((operBodyNode *)u)->init);
            Node* win = copyNode(((operBodyNode *)u)->win);
            // 声明的变量是固定的,不需要copy一份
            // list<Node *> *stmt_list = new list<Node *>();
            // for (auto it : *(((operBodyNode *)u)->stmt_list)) {
            //     stmt_list->push_back(copyNode(it));
            // }
            Node* operBody = new operBodyNode(&((operBodyNode *)u)->stmt_list, init, work, (windowNode*)win);
            return operBody;
            break;
        }
        case Pipeline: {
            list<Node *> *outputs = new list<Node *>();
            list<Node *> *inputs = new list<Node *>();
            list<Node *> *body_stmts = new list<Node *>();
            if (static_cast<pipelineNode *>(u)->outputs != NULL)
                for (auto it : *static_cast<pipelineNode *>(u)->outputs)
                    outputs->push_back(copyNode(it));
            if (static_cast<pipelineNode *>(u)->inputs != NULL)
                for (auto it : *static_cast<pipelineNode *>(u)->inputs)
                    inputs->push_back(copyNode(it));
            if (static_cast<pipelineNode *>(u)->body_stmts != NULL)
                for (auto it : *static_cast<pipelineNode *>(u)->body_stmts)
                    body_stmts->push_back(copyNode(it));
            pipelineNode *tmp = new pipelineNode(outputs, body_stmts, inputs);
            tmp->replace_composite = NULL;
            return tmp;
            break;
        }
        case SplitJoin: {
            list<Node *> *outputs = new list<Node *>();
            list<Node *> *inputs = new list<Node *>();

            Node *split = copyNode(static_cast<splitjoinNode *>(u)->split);

            Node *join = copyNode(static_cast<splitjoinNode *>(u)->join);
            list<Node *> *stmt_list = new list<Node *>();
            list<Node *> *body_list = new list<Node *>();
            if (static_cast<splitjoinNode *>(u)->outputs != NULL)
                for (auto it : *static_cast<splitjoinNode *>(u)->outputs)
                    outputs->push_back(copyNode(it));
            if (static_cast<splitjoinNode *>(u)->inputs != NULL)
                for (auto it : *static_cast<splitjoinNode *>(u)->inputs)
                    inputs->push_back(copyNode(it));
            if (static_cast<splitjoinNode *>(u)->stmt_list != NULL)
                for (auto it : *static_cast<splitjoinNode *>(u)->stmt_list)
                    stmt_list->push_back(copyNode(it));
            if (static_cast<splitjoinNode *>(u)->body_stmts != NULL)
                for (auto it : *static_cast<splitjoinNode *>(u)->body_stmts)
                    body_list->push_back(copyNode(it));
            splitjoinNode *tmp = new splitjoinNode(inputs, outputs, (splitNode *)split, stmt_list, body_list, (joinNode *)join);
            tmp->replace_composite = NULL;
            return tmp;
        }
        case  Split: {
            return new splitNode(static_cast<splitNode *>(u)->dup_round);
        }
        case Join: {
            return new joinNode(static_cast<joinNode *>(u)->rdb);
        }
        case Window:
        {
            list<Node *> *win_list = new list<Node *>();
            for(auto it: *(((windowNode *)u)->win_list)) {
                win_list->push_back(copyNode(it));
            }
            Node* window = new windowNode(win_list);
            return window;
        }
        case WindowStmt:
        {
            winStmtNode* old = (winStmtNode *)u;
            // 窗口信息是可以重复使用的
            Node* winStmt = new winStmtNode(old->winName, old->winType);
            return winStmt;
        }
        default:
            break;
    }
    return u;
}

string primNode::toString()
{
    string str = "";
    if (isConst)
        str += "const ";
    return str + name;
}

string declareNode::toString()
{
    string str = "";
    str += prim->toString() + " ";
    str += id_list.front()->toString();
    if (id_list.front()->init != NULL)
        str += "=" + id_list.front()->init->toString();
    for (auto iter = ++id_list.begin(); iter != id_list.end(); ++iter)
    {
        str += ",";
        str += (*iter)->toString();
        if ((*iter)->init != NULL)
            str += "=" + (*iter)->init->toString();
    }
    if (str[str.size() - 1] != ';')
    return str;
}

string constantNode::toString()
{
    if (style == "double")
        return to_string(dval);
    else if (style == "integer")
        return to_string(llval);
    else
        return "\"" + sval + "\"";
    ;
}

string initNode::toString()
{
    if (value.size() == 1)
        return listToString(value);
    string str = "{";
    return str + listToString(value) + "}";
}

string binopNode::toString()
{
    if (op == "=" || op == "+=" || op == "-=" || op == "*=" || op == "/=")
        return left->toString() + op + right->toString();
    if (left->type == Id && ((idNode *)left)->name == "cout")
        return left->toString() + op + right->toString();
    return left->toString() + op + right->toString();
}

string ternaryNode::toString()
{
    assert(first && second && third);
    return first->toString() + "?" + second->toString() + ":" + third->toString();
}

string unaryNode::toString()
{
    if (op == "PREINC")
        return "++" + exp->toString();
    else if (op == "PREDEC")
        return "--" + exp->toString();
    else if (op == "POSTINC")
        return exp->toString() + "++";
    else if (op == "POSTDEC")
        return exp->toString() + "--";
    else
        return op + exp->toString();
}

string parenNode::toString()
{
    return "(" + exp->toString() + ")";
}

string castNode::toString()
{
    return "(" + prim->toString() + ")" + exp->toString();
}

string callNode::toString()
{
    if (name == "print" || name =="printf")
        return "cout<<" + listToString(arg_list);
    else if (name == "println")
        return "cout<<" + listToString(arg_list) + "<<endl";
    string str = name + '(';
    return str + listToString(arg_list) + ")"; 
}

string operatorNode::toString()
{
    string s = "{ operName: " + operName;
    if (inputs)
        s += ", inputs:[" + listToString(*inputs) + "]";
    if (outputs)
        s += ", outputs:[" + listToString(*outputs) + "]";
    return s + " }";
}
string idNode::toString()
{
    string str = name;
    if (isArray)
    {
        if (arg_list.size() == 0) // int sum(int a[])特殊情况,是数组但是无 arg_list
            str += "[]";
        else
        {
            for (auto i : arg_list)
            {
                str += '[' + i->toString();
                str += "]";
            }
        }
    }

    return str;
}

string strdclNode::toString()
{
    auto top = id_list.front();
    string str = top->valType + ' ' + top->toString();
    for (auto iter = ++id_list.begin(); iter != id_list.end(); ++iter)
    {
        str += (*iter)->valType + ' ' + (*iter)->toString() + ',';
    }
    return str;
}

string funcDclNode::toString()
{
    string str = prim->toString() + " ";
    str += name + "(";
    if (param_list.size() != 0)
    {
        auto top = param_list.front();
        str += ((idNode *)top)->valType + " " + ((idNode *)top)->toString();
        for (auto iter = ++param_list.begin(); iter != param_list.end(); ++iter)
        {
            str += "," + ((idNode *)(*iter))->valType + " " + (*iter)->toString();
        }
    }

    str += ")";
    return str;
}

string caseNode::toString()
{
    string str = "case " + exp->toString() + ":\n";
    str += "\t" + stmt->toString();
    return str;
}
string defaultNode::toString()
{
    string str = "default:\n\t";
    str += stmt->toString();
    return str;
}

string compositeCallNode::toString()
{
    return "compositeCallNode";
}

string ifNode::toString()
{
    string str = "if(";
    str += exp->toString();
    str += ")";
    str += "\t" + stmt->toString();
    return str;
}
string ifElseNode::toString()
{
    string str = "if(";
    str += exp->toString();
    str += ")";
    str += "\t" + stmt1->toString();
    str += "else";
    str += "\t" + stmt2->toString();
    return str;
}
string forNode::toString()
{
    string str = "for(";
    str += init->toString()+";";
    str += cond->toString()+";";
    str += next->toString();
    str += ")";
    str += "\t\t" + stmt->toString();
    return str;
}

string blockNode::toString()
{
    string str = "\t{\n";
    for (auto stmt : stmt_list)
    {
        str += "\t\t" + stmt->toString() ;
        if(str[str.size()-1] != '}')   str+=";";  
        str += "\n";
    }
    str += "\t}";
    return str;
}

string returnNode::toString()
{
    string str = "return ";
    str += exp->toString();
    return str;
}

string switchNode::toString()
{
    string str = "switch";
    str += "(" + exp->toString() + ")\n";
    str += "{\n" + stat->toString() + "\n}";
    return str;
}

string whileNode::toString()
{
    string str = "while";
    str += "(" + exp->toString() + ")\n";
    str += "{\n" + stmt->toString() + "\n}";
    return str;
}

string doNode::toString()
{
    string str = "do\n";
    str += stmt->toString() + "\n";
    str += "while(" + exp->toString() + ")";
    return str;
}

string funcBodyNode::toString()
{
    string str = "";
    if (stmt_list != NULL)
    {
        for (auto it : *stmt_list)
            str += it->toString() + "\n";
    }
    return str;
}

vector<long long>* layerNode::getInputSize(sequentialNode *sequential) {
    layerNode *prevLayer = this -> prevLayer;
    if (prevLayer != NULL) {
        switch (prevLayer -> layerType)
        {
            case Dense:
                return new vector<long long>({1, ((denseLayerNode *)prevLayer)->cols, 1});
            case Conv2D: {
                return ((conv2DLayerNode *)prevLayer) -> outputFeatureMapSize;
                }
            case MaxPooling2D: {
                return ((maxPooling2DLayerNode *)prevLayer) -> outputPooledSize;
                }
            case AveragePooling2D: {
                return ((averagePooling2DLayerNode *)prevLayer) -> outputPooledSize;
            }
            default:
                return NULL;
        }
    } else {
        Node *firstArg = sequential -> arg_list -> front();
        switch ( firstArg -> type) {
            case Tuple: {
                vector<long long> *res = new vector<long long>();
                 for (auto dim : *(((tupleNode *)firstArg) -> tupleList)) {
                     res -> push_back(((constantNode *)dim)->llval);
                 }
                 return res;
            }
            default:
                return new vector<long long>({1, ((constantNode *)firstArg)->llval, 1});
        }
    }
}
// 根据上一层初始化本层输出特征图的尺寸和输入空间的维度
void conv2DLayerNode::init (sequentialNode* sequential) {
    this -> outputFeatureMapSize = new vector<long long>();
    // 本层反向传播过程中 传入误差的尺寸`
    this -> inputErrorSize = new vector<long long>();
   // 按照arg_list爲傳入整個sequential結構的參數列表((depth, rows, cols), ...)
    this->inputSize = this -> getInputSize(sequential);
    for(int i = 0; i < this->dimension; i++) {
        this->outputFeatureMapSize->push_back((this->inputSize->at(i) + 2 * this->paddings->at(i) - this->kernel_size->at(i)) / this->strides->at(i) + 1);
    }
    this->outputFeatureMapSize->push_back(this->filters);
    for(int i = 0; i < this->dimension; i++) {
        // 2 * (kernel_size - 1) + (outputFeaureMapSize - 1)* stride + 1
        this -> inputErrorSize -> push_back(2 * (this -> kernel_size -> at(i) - 1) + (this -> outputFeatureMapSize -> at(i) - 1) * this -> strides -> at(i) + 1);
    }
}

void denseLayerNode::init (sequentialNode * sequential) {
    this -> inputSize = this->getInputSize(sequential);
    long long temp = 1;
    for(auto dim: *(this->inputSize)) {
        temp*=dim;
    }
    this->rows = temp;
}

void maxPooling2DLayerNode::init(sequentialNode *sequential) {
    this -> inputSize = this -> getInputSize(sequential);
    this->outputPooledSize = new vector<long long>();
    for(int i = 0; i < 2; i++) {
        // 是否加1????
        this->outputPooledSize->push_back(inputSize->at(i) / this->pool_size);
    }
    this->outputPooledSize->push_back(inputSize->back());
}

void averagePooling2DLayerNode::init(sequentialNode *sequential) {
    this -> inputSize = this -> getInputSize(sequential);
    this->outputPooledSize = new vector<long long>();
    for(int i = 0; i < 2; i++) {
        // 是否加1????
        this->outputPooledSize->push_back(inputSize->at(i) / this->pool_size);
    }
    this->outputPooledSize->push_back(inputSize->back());
}