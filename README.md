# COStreamPP
>重构 COStream 的前端, C => C++
## 文件夹目录说明
- `docs` : 代码重构时的一些想法纪录在这里,包含对一些文件的简单说明
- `tests` : 测试用的代码
- `src`
  - `config` : 词法分析和语法分析脚本文件
  - `include` : 头文件
  - `src` : 和头文件对应的 `.cpp` 文件
      - `main.cpp` 程序入口
      - `FrontEnd` 前端
      - `BackEnd`  后端
- `.editorconfig` 编辑器代码风格配置文件
- `.gitignore` git 忽略文件目录
- `README.md` 说明文件
## 任务进度
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
    - [x] 出错误时能显示源文件的对应行内容了
- [ ] 生命周期
  - [x] `handle_options` 对命令行输入预处理
  - [x] `firstScan` 第一遍扫描 `\t 转4个空格`,函数和`composite`变量名识别
  - [x] `yyparse()`按既定文法生成语法树完成
  - [ ] 打印抽象语法树没做
  - [ ] `semCheck` 语义检查没做
  - [x] `AST2FlatStaticStreamGraph` 语法树到平面图转换
  - [x] `WorkEstimate` 对静态数据流图各节点进行工作量估计
  - [ ] 后端部分待测试
- 下一步工作
  - [ ] 测试,加注释
  - [ ] 整合其他人工作



  陈名韬GPU后端整合
  1.当前新后端不支持多类型stream变量，因此后端只支持老的benchmark，新文法扩展暂时无法使用

  2.缺少从语法树获取数组变量名称，类型，维度的方式

  3.旧版生成work时会使用SPLnode，新版没有，一些从语法树中获取的信息暂时没有了
  CGGlobalVirable
  CGdeclList  	SPL2GPU_List(state, 0);
  CGlogicInit

  4.新前端已经增加了对operter中有无状态的判断，合并后取消注释ActorStateDetector 的函数即可使用

global.h que dong xi