#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstring>

#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 */
    template<class TYPE>
    void swap(TYPE &a, TYPE &b) {
        TYPE p = a;
        a = b;
        b = p;
    }

    //要写大根堆,用斜堆实现
    template<typename T, class Compare = std::less<T> >
    class priority_queue {

        struct Node {
            T data;
            Node *left, *right;

            //默认构造
            Node(T item, Node *l = nullptr, Node *r = nullptr) : left(l), right(r), data(item) {}

            //赋值构造
            Node(const Node &p) : left(p.left), right(p.right), data(p.data) {}

            ~Node() {}
        };

    private:
        Node *root;
        int len;

    public:
        /**
         * TODO constructors
         */
        priority_queue() : root(nullptr), len(0) {}

        priority_queue(const priority_queue &other) {
            //if (this == &other) return;
            clone(root, other.root);
            len = other.len;
        }

        /**
         * TODO deconstructor
         */
        ~priority_queue() {
            clear(root);
            len = 0;
        }

        /**
         * TODO Assignment operator
         */
        priority_queue &operator=(const priority_queue &other) {
            if (this == &other) return *this;
            clear(root);
//            len = 0;
            clone(root, other.root);
            len = other.len;
            return *this;
        }

        /**
         * get the top of the queue.
         * @return a reference of the top element.
         * throw container_is_empty if empty() returns true;
         */
        const T &top() const {
            if (empty()) throw container_is_empty();
            return root->data;
        }

        /**
         * TODO
         * push new element to the priority queue.
         */
        void push(const T &e) {
//            std::cout << e << std::endl;
            Node *cur = new Node(e);
            root = merge_node(root, cur);
            len++;
        }

        /**
         * TODO
         * delete the top element.
         * throw container_is_empty if empty() returns true;
         */
        void pop() {
            if (empty()) throw container_is_empty();
            Node *cur = root;
            root = merge_node(root->left, root->right);
            delete cur;
            len--;
        }

        /**
         * return the number of the elements.
         */
        size_t size() const {
            return len;
        }

        /**
         * check if the container has at least an element.
         * @return true if it is empty, false if it has at least an element.
         */
        bool empty() const {
            return !len;
        }

        /**
         * merge two priority_queues with at least O(logn) complexity.
         * clear the other priority_queue.
         */
        void merge(priority_queue &other) {
            root = merge_node(root, other.root);
            len += other.len;
//            clear(other.root);
            //不能提前clear,other在结束时会自动析构
            //因为实际上*this和other.root有公用的节点
            other.len = 0;
            other.root = nullptr;
        }

        //把x和y合并(并没有新建空间,所以之前需要new操作)
        Node *merge_node(Node *&x, Node *&y) {
            if (x == nullptr) return y;
            if (y == nullptr) return x;

            //保证x是较大的那个根(如果要实现小根堆,就改成大于号)
            if (Compare()(x->data, y->data)) swap<Node *>(x, y);

            //交换左右子树
            x->right = merge_node(x->right, y);
            swap<Node *>(x->left, x->right);

            return x;
        }

        //利用递归实现
        void clear(Node *&t) {
            if (t == nullptr) return;
            clear(t->left);
            clear(t->right);
            delete t;
            t = nullptr;
        }

        void clone(Node *&t, const Node *p) {
            if (p == nullptr) return;

            t = new Node(p->data);
            clone(t->left, p->left);
            clone(t->right, p->right);
        }

    };

}

#endif
