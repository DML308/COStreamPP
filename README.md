# COStreamPP
重构 COStream 的前端, C => C++

- [x] 词法分析, 传递`数字 | 字符串 | 特殊符号`
    - [x]  修改 any_white 的定义后, yylineno 在 repl 模式下能用了
    - [x]  增加了对字符串和 identifier 的识别,返回`std::string`类型
    - [x]  十进制整数|八进制整数|十六进制整数 返回`integerConstant`,数值类型通过`stoll`存为`longlong`
    - [x]  浮点数返回`doubleConstant`,数值类型通过`strtod`存为`double`
- [x] 语法分析
    - [x]  摘抄移植 COStream 文法,修改的地方记录在了[docs/7.delete-yacc.md](https://github.com/DML308/COStreamPP/blob/master/docs/7.delete-yacc.md)中
    - [x]  能 make 通过了
    - [x]  调试 bug 完成
      - [x] `tests/wang.cos`      文法测试通过
      - [x] `tests/jepg.cos`      文法测试通过
      - [x] `VocoderTopLevel.cos` 文法测试通过
- 下一步工作
    - [ ] `Li`: `handle_opt`
    - [ ] `Yang`: `Node` 构建

效果:

![yylineo](https://i.loli.net/2018/12/03/5c0529c0079fc.jpg)
