# COStreamPP
重构 COStream 的前端, C => C++

- [ ] 词法分析, 传递`数字 | 字符串 | 特殊符号`
    - [x]  修改 any_white 的定义后, yylineno 在 repl 模式下能用了
    - [x]  增加了对字符串的识别
- [ ] 语法分析


效果:

![yylineo](https://i.loli.net/2018/11/26/5bfbf72f270ac.png)

## use

在/COStreamPP/src 路径下 ./a ../tests/XXX.cos