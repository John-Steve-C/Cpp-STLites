# STLite-Vector

## 实现细节

Vector的基本实现要求在总项目文档中已经给出，最终提交仅需要提交`vector.hpp`的内容。

可能需要注意的细节：

- 在测试点中，有一些类并不具有默认构造函数，所以直接使用`T* p=new T[...];`可能会出现问题。
- 你的程序将会受到一定程度的鲁棒性检测

## 分数构成

OJ测试部分占比：80%，code review部分占比：20%

## 截止日期

3月5日（第三周周六）23:00前

---
# 我的经历

- 助教给的模板并不是完整的，还需要自己补充很多东西
> 比如说`iterator`的构造函数，`resize`函数来重新调整内存空间
- 因为模板类T没有默认构造函数，所以`vector`不能直接`new T`，要用 `malloc`
- 小细节，末尾和边界
- 判断是否属于同一个vector，添加一个指向`vector`的指针
- 为什么复制构造函数中的`malloc`会报错？
> STLite_ACM: malloc.c:2379: sysmalloc: Assertion `(old_top == initial_top (av) && old_size == 0) || ((unsigned long) (old_size) >= MINSIZE && prev_inuse (old_top) && ((unsigned long) old_end & (pagesize - 1)) == 0)' failed.

> Signal: SIGABRT (Aborted)

> 因为初始化`cur_len = 0`，所以`push_back`应该是`data[cur_len++] = value`...

喜欢犯低级错误...别人都过了

- for循环写错了，`pos(iterator) - data(T*) is undefined!`。改成友元即可
- `const_iterator`的构造函数无法识别？调用cbegin就报错
   
  因为构造函数中有`this`指针，而指向的vector是会变化的?

  问题似乎是，const的构造函数要用 **列表** 初始化

  再用g++跑了以下，` error: invalid conversion from const sjtu::vector<int>* to sjtu::vector<int>*`

  最后发现，`const_iterator`中的指向vector的`id`，应该定义为`const vector<T>* id`
- 爆空间？内存越界，一个变量打错了
- 当T类型，没有默认构造函数时，不能直接写　`data[i] = other.data[i]`，而要写成`new (data + i) T(other.data[i])`，称为`placement_new`
  
  在`push_back`中，vector的默认构造函数，不会调用T的默认构造函数，生成的data[0]是乱码，不能直接用=赋值，所以要改成new的写法？而`insert`时，中间的空间已经初始化过了，所以不用new
- `MLK`是因为要先每一项调用T的析构函数，再对vector总的free
  `delete`会调用指向的类的析构函数,但是`free`不会
- 参考资料：[placement_new](https://blog.csdn.net/linuxheik/article/details/80449059) 。[malloc/free](https://www.cnblogs.com/limera/p/new_malloc.html)
- 哪个位置要删除？开头和末尾是0~cur_len-1，注意！不要犯低级错误。

---
## code review (2022.3.13)

1. 可以再增加一个`half_space`（空间减半）的操作，当`4 * cur_len < max_size`时调用，删除多余空间
2. `const_iterator`只能遍历`const vector`
3. 为什么数组的初始长度为10？
4. `double_space`操作后，原本的`iterator`会失效。因为实际上是复制到新的位置，`iterator` 指的位置不是原本的位置。
5. `malloc`和`new`的区别
6. 迭代器中的 `id` 是为了实现两 `iterator` 相减的操作