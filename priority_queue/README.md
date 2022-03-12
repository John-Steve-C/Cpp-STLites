# STLite-Priority_queue

## 实现细节

最终提交仅需要提交 `priority_queue.hpp` 的内容。

你需要完成的内容：

+ 构造函数（两种）
+ 析构函数
+ 赋值重载
+ 获得队首元素 `top`
+ 元素入队 `push`
+ 队首元素出队 `pop`
+ 队列大小 `size`
+ 判断队列是否为空 `empty`
+ 队列合并 `merge`

具体细节可以查看下发的`priority_queue.hpp`框架。

注意：

- 你能使用的头文件仅限于下发框架中提供的头文件。
- `merge` 复杂度不能超过 $O(\log n)$，这意味着你可能需要写可并堆。
- 有测试内存泄漏的数据点。

## 分数构成

正常情况下，在 OJ 上通过测试数据可以获得 80% 的分数，CR 占 20% 的分数。

如果在CR时，发现有任何违规行为（包括但不限于使用其它头文件、使用非常规方法通过测试点以及`merge`复杂度超过 $O(\log n)$），你最终的得分都有可能为 $0$ 分。

## 截止日期

3月19日（第五周周六）23:00 前。

---

## 调试：

2022.3.10晚，拼命浪费时间，在10点才开始

1. 链表的复制构造，不能直接头指针赋值，还是要重新开一块空间，把之前的每个值一一存进来。否则会在clear后访问非法内存。

2. 要深刻理解底层的原理，不要一会儿用const，一会儿又不用

---

2022.3.11

3. `segmentation fault`，连代码都跑不起来.......Clion 的调试也用不了，只能用输出来判断...

   目前用`Dev-cpp`跑了一下，发现是merge时，对other重复进行了clear操作（实际上临时变量在函数结束时，会自动执行析构函数）但是我操作的对象是引用传递的变量，会自动调用析构函数吗？

4. 事实上，我已经~~按照模板~~写好了两个节点的merge操作，然而我把push写得太复杂。

5. pop时，不能在对`root->left`操作之前，直接写`delete root`。这样会造成leak？

6. 一定要用`linux`命令行调试！通过提示，发现了`heap-use-after-free`和`memory-leak`两大错误

7. Node的默认构造函数要补全，~~不能写data=0，直接省略~~。否则会报错`no matching function for call to guest::guest(int)`。

   最后在钟老师的指导下，发现问题：Node中存储的T类型可能没有默认构造函数，所以不能出现 `new Node`，而应该写成 `new Node(data)`

8. 还会遇到类没有定义<的情况，但是模板中他传入了一个Compare类，所以直接判断`Compare()(x->data, y->data)`就行了

9. 如果在clone时，把第二个参数设定为`Node *&b`，那么会` binding reference of type ‘sjtu::priority_queue<int>::Node*&’ to ‘sjtu::priority_queue<int>::Node* const’ discards qualifiers`，可以写成`const Node *b`，但是不能加上`&`？

10. 第五个点，测试`merge`，然后又是`heap-use-after-free`。len=0不应该放在clear中实现。但是最后Fail？

11. 特判自己赋值给自己。clone和=都要判断

9. 本地命令行没问题，但是mlk。用`valgrind`跑的时候，g++只能写到`-o a`，后面不能有参数。然后`valgrind ./a`

---

2022.3.12

13. 在周神的指导下，发现clone在判断时，不能写两个`Node *`相等就return，而应该在clone之前，就进行自身赋值自己的判断。否则可能会提前析构？

    修改完，不会`memory leak`，只剩下第五个点WA。

14. 把数组调小就对，但是到400000就错了。

    把rand()改成其他数，就Segmentation fault。。。merge到一半会被断开？此时other.root和root.left位置是重合的？所以会导致`double free`。

    所以merge完不需要马上写`clear(other.node)`，应该等到结束时令其自动析构。（实际上*this和other.root有公用的节点）。写成`other.root = nullptr`即可。
