# STLite-LinkedHashMap

## 实现要求


   -    实现的LinkedHashMap要求与HashMap基本相同，但是在HashMap的基础上要求实现一些新的功能
   


  -    实现向数据结构插入形如(Key,Value) 的元素对 (HashMap的基本功能)
  


  -    实现在数据结构中查询某个特定的 Key 所对应的 Value (HashMap的基本功能)
  


  -   实现按照插入顺序访问插入的元素 (新增的功能)
  


   -    所有实现的功能都要求期望复杂度是 O(1)，所以不能在Map的基础上实现本次大作业 (我们会进行压力测试来阻止这种行为，如果通过测试的代码在Code Review阶段被发现存在这种行为将会被酌情扣分)
   


  -    具体需要实现的接口见下发文件中的 `linked_hashmap.hpp`



##  分数构成

- 80% OJ分数 + 20% Code Review



##  截止日期

- 4.24晚 18:30

---

## 编程记录

- 链表的基本操作都有点忘记了
- 自己瞎写了半天，发现自己把哈希表都写错了，原来用哈希值去找数组，被我写成了 $O(n)$ 遍历。
- 还有 `loadFactor` ，不理解还是可以参考 [代码](https://www.cnblogs.com/chengxiao/p/6059914.html#t2) 的啊...
- 学会总结，写的四个库。
  - 对于复制构造，`vector`是直接按顺序复制，树是调用了递归的`clone`函数，而链表应该对每个元素执行`insert`操作
- `clear`时，直接按照双链表进行操作即可
- 不会写`double_space`，实际上可以先`delete [] arr`，然后再按照`head`去访问节点。同时还要重新计算哈希值
- 感觉 `NODE` 里面的data没必要写成指针？
> 不行，因为担心`value_type`太大？而且写成指针才能避免调用data的默认构造函数（data没有默认构造）。和当初的map同理
- 为什么`find`中不能修改原本类中的p？抛弃原本单链表的写法
- `find() == end()` 会报错？等于未定义？要改成cend
- 调用形式：`arr[num]->next`，不能用 .
- double_space怎么写？直接新开一个对象临时存储，删除原对象后赋值即可
> 会T飞！因为我的insert写的不好...

> 怎么改？
- 重载等于号的自我赋值！`if (this == &other) return *this;`
- 如果用一个指向b的迭代器来删除a中的元素，那么是不合法的
- clion能跑，但是g++跑不完？clion优化太好了，但是实际上评测是用g++...
> 把前面的内容注释掉，直接跑这个 test，就出锅了

> 发现是clear时，只修改了head,rear的前后继，还要修改每个表头arr[i]的next
- 先判断是否需要 `double_space` 再 `++siz`
- =号的重载要记得return本身
- load_factor设为0.75时，第一个点会T？
- 如何统计运行时间？`std::cout<<clock()/(double)1e6<<'\n';`
 
  或者g++中调用 `time ./a`
  - 从5秒到1.6秒：等于号重载，不需要先delete再new，直接clear+insert即可
  - =号修改，不用clear,加快了0.4s
  - 构造函数，初始时不需要$O(n)$的for new,只需要`arr = new NODE*[]`加上fill为nullptr。
  > 每个表头的new在插入时特判即可
  - 改完不会T了，但是MLK+RE...

---
## 2022.4.18（过了一个周末，才重新开始调试，之前实在是心力交瘁）

- 修改了clear，之前没有删除`arr[i]`
- `double_space`后，hash值不变，但是存储的位置会改变?因为重新取模了
> 哈希值不用取模，但是在求存储的位置=hash mod capacity

> 再修改：用它的Hash算出来的值可能为负数，所以 `num = (Hash % capacity + capacity) % capacity`

- `malloc_consolidate(): invalid chunk size`：内存分配（new）失败
  - 在at操作时，会莫名其妙地调用double_space?
  - 发现是上述hash后，num为负数，`arr[num]`发生越界
  - 很奇怪的错误。之前写的时候根本没遇到，其他同学也没有，只有我要修改...
> 还是欠缺调试技巧！第11个check函数出问题，那就把前面无关的函数先 **注释** 掉，才能保证用Clion的 **调试** 能够跑到真正错误的地方（而不是double_space）

## 2022.4.24 Code Review

- double_space的复杂度：一次是 $O(n)$
  - 对于多次double_space的总复杂度，实质上相当于等比求和： $O(n/2 + n/4 ...)=O(n)$，总复杂度仍然是 $O(n)$
- 如何减少hash的冲突？扩大表头的规模 / size改为 **质数**
- 一开始表头不能开太大，否则在线性遍历的时候会T。需要在后来进行double_space
- 有自己额外的优化吗？没有
  - 周老师的独特技巧：用位运算和异或实现快速取模操作