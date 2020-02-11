# COStreamPP
>重构 COStream 的前端, C => C++
## 运行方法
```bash
git clone git@github.com:DML308/COStreamPP
cd COStreamPP
cd src
make 
./a ../test/wang.cos # -o output 
# 可以用 -o 参数来指定输出文件夹, 若不指定, 则输出至当前目录的 StaticDistCode 文件夹
```
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
  - [ ] 整合其他人工作

