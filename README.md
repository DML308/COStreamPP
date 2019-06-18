# COStreamPP

  陈名韬GPU后端整合

  1.当前新后端不支持多类型stream变量，因此后端只支持老的benchmark，新文法扩展暂时无法使用

  2.缺少从语法树获取数组变量名称，类型，维度的方式,

  3.旧版生成work时会使用SPLnode，新版没有，一些从语法树中获取的信息暂时没有了,旧版代码于CGGlobalVirable、CGdeclList、CGlogicInit调用了SPL2GPU_List(state, 0); 

  4.新前端已经增加了对operter中有无状态的判断，合并后取消注释ActorStateDetector的函数即可使用


