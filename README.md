# COStreamPP

  陈名韬GPU后端整合

GPU后端代码和C++端的区别:
1.将GPU看成一个具有并行能力的CPU单核，作为流水线的一个部分。
2.GPU的计算任务由kenerl完成，COStream的work函数中的内容会翻译成对应的kenerl函数，稳态次数会成为kenerl的globalsize。
3.kenerl中的需要将外部的参数传入，如FFT测试用例中CombineDFTX的wn_r,wn_i变量。
  特别注意数字参数，普通变量值传递即可，数组需要进行在GPU端开辟内存，从主存将数组内存拷贝到显存。
4.假如编译设置有2个GPU，2个CPU线程。生产的线程中0、1线程控制两个GPU，3、4线程控制CPU-GPU的数据传输，2、5线程执行CPUactor。
5.扩大因子保留，会作用于actor的稳态次数，缓冲区大小。可以用于测试。


新后端存在的问题：
1.当前新后端不支持多类型stream变量，因此后端只支持老的benchmark，新文法扩展暂时无法使用

2.新前端已经增加了对operter中有无状态的判断，合并后取消ActorStateDetector中的注释即可使用

3.缺少从语法树获取数组变量名称，类型，维度的方式,
  旧版生成work时会使用SPLnode，新版没有，一些从语法树中获取的信息暂时没有了,旧版代码于CGGlobalVirable、CGdeclList、CGlogicInit调用了SPL2GPU_List(state, 0); 
