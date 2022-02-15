# STLite_Vector

CS1951 assignment, ACM Class, SJTU

## 实现要求

要求实现与C++标准库`<vector>`类似的数据结构，包括迭代器等。框架接口在已本仓库中给出，只需要实现`.hpp`文件内所要求的内容即可。

请注意，在本作业中，只允许使用`cstdio`，`cstring`，`iostream`，`cmath`四个C++标准库，如需使用其他功能请自行实现。在下发代码中有`exceptions.hpp`和`utility.hpp`两个辅助文件，你需要使用其中的异常处理和`sjtu::pair`完成STLite的实现，但不允许修改其中的任何内容。

本作业中的一个容器在接口和复杂度要求上与STL基本一致

因此，如果对内容有任何疑问，可以参考STL，或是在这里提出issue，谢谢！

最后，我们会检查内存泄漏，如果还不会测内存泄漏，可以看教程[如何检测内存泄漏？](https://github.com/IvyCharon/STLite_ACM/tree/main/tutorials/detect-memory-leak/detect-memory-leak.md)


## 评测方法

测试数据将全部下发（见本仓库），供调试使用。最终测试将会在OJ上进行，并进行code review。

## 分数构成

在 OJ 上通过测试数据可获得 80% 的分数，code review 占 20% 的分数。

## 截止日期

3月2日（第三周周三）19:00前

## 迟交惩罚

迟交第i天(向上取整)，扣除i*10%的分数，扣完为止。
