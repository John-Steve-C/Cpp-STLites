/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class RedBlackTree {
    public:
        typedef pair<const Key, T> value_type;
        enum COLOR {
            RED, BLACK
        };

        struct NODE {
            value_type data;
            NODE *left, *right, *fa;
            COLOR color;

            NODE() = default;

            NODE(const value_type &_data, NODE *f = nullptr, NODE *lt = nullptr, NODE *rt = nullptr, COLOR c = RED)
                    : data(_data), fa(f), left(lt), right(rt), color(c) {}

            NODE(NODE *other) : data(other->data), fa(other->fa), left(other->left), right(other->right),
                                color(other->color) {}
        };

        NODE *root, *end_node;
        int len;

        RedBlackTree() {
            root = new NODE;
            len = 0;
            end_node = nullptr;
        }

        RedBlackTree(NODE *t = nullptr) : root(t), len(0), end_node(nullptr) {}

        RedBlackTree(const RedBlackTree &other) : len(other.len), end_node(nullptr) {
            clone(root, other.root);
        }

        ~RedBlackTree() { make_empty(root); }

        value_type *find(const Key &x) const {
            if (root->data.first == x) { return root->data; }
            NODE *t = root;
            while (t != nullptr && t->data.first != x) {
                t = (Compare()(x, t->data.first)) ? t->left : t->right;
            }

            if (t == nullptr) return nullptr;
            else return (value_type *) t->data;
        }

        NODE *find_node(const Key &x) const {
            if (root->data.first == x) { return root; }
            NODE *t = root;
            while (t != nullptr && t->data.first != x) {
                t = (Compare()(x, t->data.first)) ? t->left : t->right;
            }

            if (t == nullptr) return nullptr;
            else return t;
        }

        void insert(const value_type &x) {
            if (root == nullptr) {
                root = new NODE(x, nullptr, nullptr, nullptr, BLACK);
                return;
            }
            NODE *t = find(x.first), *parent, *GrandP;
            if (t != nullptr) return; //防止重复插入

            len++;
            t = parent = GrandP = root;
            while (1) {
                //在路径中,自上向下访问
                if (t != nullptr) {
                    //直接修改颜色
                    if (t->left && t->left->color == RED &&
                        t->right && t->right->color == RED) {
                        t->left->color = t->right->color = BLACK;
                        t->color = RED;
                        insert_adjust(GrandP, parent, t);
                    }
                    if (t->fa == nullptr) t->fa = parent;
                    GrandP = parent;
                    parent = t;
                    t = (Compare()(x.first, t->data.first)) ? t->left : t->right;
                } else {
                    //遍历到了叶子节点
                    t = new NODE(x);
                    if (t->fa == nullptr) t->fa = parent;
                    if (Compare()(x.first, parent->data.first)) parent->left = t;
                    else parent->right = t;
                    insert_adjust(GrandP, parent, t);
                    root->color = BLACK;
                    return;
                }
            }
        }

        void remove(const Key &key) {
            NODE *t = find(key), *parent, *t2; //t2是t的兄弟节点
            Key del = key;

            if (t == nullptr || root == nullptr) return;
            len--;
            if (root->data.first == key && root->left == nullptr && root->right == nullptr) {
                delete root;
                root = nullptr;
                return;
            }

            t = parent = t2 = root;
            while (1) {
                remove_adjust(parent, t, t2, del);
                if (t->data.first == del && t->left != nullptr && t->right != nullptr) {
                    NODE *tmp = t->right;
                    while (tmp->left) tmp = tmp->left;
                    t->data = tmp->data;
                    del = tmp->data.first;
                    parent = t;
                    t = t->right;
                    t2 = parent->left;
                    continue;
                }

                if (t->data.first == del) {
                    delete t;
                    if (parent->left == t) parent->left = nullptr;
                    else parent->right == nullptr;
                    root->color = BLACK;
                    return;
                }

                parent = t;
                t = (Compare()(del, parent->data.first)) ? parent->left : parent->right;
                t2 = (t == parent->left) ? parent->right : parent->left;
            }
        }

        void make_empty(NODE *&t) {
            if (t != nullptr) {
                make_empty(t->left);
                make_empty(t->right);
                delete t;
            }
            t = nullptr;
            len = 0;
        }

        //自上向下调整,使gp->p->t这棵子树合法
        void insert_adjust(NODE *gp, NODE *p, NODE *t) {
            //没有连续的红色,不需要调整
            if (p->color == BLACK) return;
            if (p == root) {
                p->color = BLACK;
                return;
            }
            if (gp->left == p) {
                if (p->left == t) LL(gp);
                else LR(gp);
            } else {
                if (p->right == t) RR(gp);
                else RL(gp);
            }
        }

        //t使当前节点,t2是t的兄弟
        void remove_adjust(NODE *p, NODE *t, NODE *t2, Key del) {
            if (t->color == RED) return;
            if (t == root) {
                if (t->left && t->right && t->left->color == t->right->color) {
                    t->color = RED;
                    t->left = t->right = BLACK;
                    return;
                }
            }

            //第一种情况,t有两个黑儿子/1+0
            if ((t->left && t->left->color == BLACK || t->left == nullptr) &&
                (t->right && t->right->color == BLACK || t->right == nullptr)) {
                //1.1: t2
                if ((t2->left && t2->left->color == BLACK || t2->left == nullptr) &&
                    (t2->right && t2->right->color == BLACK || t2->right == nullptr)) {
                    p->color = BLACK;
                    t->color = t2->color = RED;
                    //1.2:
                } else {
                    if (p->left == t2) {
                        if (t2->left != nullptr && t2->left->color == RED) {
                            t2->left->color = BLACK;
                            LL(p);
                            p = t2;
                        } else {
                            LR(p);
                            p = p->right;
                            p->color = BLACK;
                        }
                    } else if (t2->right != nullptr && t2->right->color == RED) {
                        t2->right->color = BLACK;
                        RR(p);
                        p = t2;
                    } else {
                        RL(p);
                        p = p->left;
                        p->color = BLACK;
                    }
                    t->color = RED;
                }
            }
                //第二种,p有红儿子
            else {
                if (t->data.first == del) {
                    if (t->left != nullptr && t->right != nullptr) {
                        if (t->right->color == BLACK) {
                            LL(t);
                            t = t->right;
                        }
                        return;
                    }
                    if (t->left != nullptr) {
                        LL(t);
                        p = t;
                        t = t->right;
                    } else {
                        RR(t);
                        p = t;
                        t = t->left;
                    }
                } else {
                    p = t;
                    t = (Compare()(del, p->data.first)) ? p->left : p->right;
                    t2 = (t == p->left) ? p->right : p->left;
                    if (t->color == BLACK) {
                        if (t2 == p->right) RR(p);
                        else LL(p);
                        p = t2;
                        t2 = (t == p->left) ? p->right : p->left;
                        remove_adjust(p, t, t2, del);
                    }
                }
            }
        }

        void LL(NODE *gp) {
            NODE *p = gp->left, *t = p->left;
            gp->color = RED;
            p->color = BLACK;
            gp->left = p->right;
            p->right = gp;
//            gp = p;
        }

        void RR(NODE *gp) {
            NODE *p = gp->right, *t = p->right;
            gp->color = RED;
            p->color = BLACK;
            gp->right = p->left;
            p->left = gp;
//            gp = p;
        }

        void LR(NODE *gp) {
            NODE *p = gp->left, *t = p->right;
            gp->color = RED;
            t->color = BLACK;
            // == RR(gp->left) == RR(p)
            p->right = t->left;
            t->left = p;
            // == LL(gp)
            gp->left = p->right;
            p->right = gp;
        }

        void RL(NODE *gp) {
            NODE *p = gp->right, *t = p->left;
            gp->color = RED;
            t->color = BLACK;
            // == LL(gp->right) == LL(p)
            p->left = t->right;
            t->right = p;
            // == RR(gp)
            gp->right = p->left;
            p->left = gp;
        }

        value_type *front() {
            NODE *t = root;
            while (t->left) {
                t = t->left;
            }
            return t->data;
        }

        value_type *back() {
            NODE *t = root;
            while (t->right) {
                t = t->right;
            }
            return t->data;
        }

        void clone(NODE *t, const NODE *p) {
            if (p == nullptr) return;
            t = new NODE(p->data);
            clone(t->left, p->left);
            clone(t->right, p->right);
        }
    };

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
        RedBlackTree<Key, T, Compare> mp;

        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;

        class iterator {
            friend class map;

        private:
            value_type *p;
            const map<Key, T, Compare> *id;
        public:
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
//            using difference_type = std::ptrdiff_t;
//            using value_type = T;
//            using pointer = T *;
//            using reference = T &;
//            using iterator_category = std::output_iterator_tag;
            // If you are interested in type_traits, toy_traits_test provides a place to
            // practice. But the method used in that test is old and rarely used, so you
            // may explore on your own.
            // Notice: you may add some code in here and class const_iterator and namespace sjtu to implement toy_traits_test,
            // this part is only for bonus.

            iterator() = default;

            iterator(const iterator &other) = default;

            iterator(value_type *_p, const map<Key, T, Compare> *_id) : p(_p), id(_id) {}

            //iter++指的是访问下一个比key大的位置
            iterator operator++(int) {
                iterator t(*this);
                ++t;
                return *this;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                auto t = id->mp.find_node(p->first);
                //边界特判,如果是最大值,就直接返回end()
                if (t->data == *(id->mp.back())) return id->end();
                //
                if (t->right) return iterator(t->right->data, id);
                else {
                    if (t->fa->left == t)
                        return iterator(t->fa->data, id);
                    else
                        return id->end();
                }
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator t(*this);
                --t;
                return *this;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                auto t = id->mp.find_node(p->first);
                //边界,理论上应该报错?
                if (t->data == *(id->mp.front())) return id->begin();
                //
                if (t->left) return iterator(t->left->data, id);
                else {
                    if (t->fa->right == t)
                        return iterator(t->fa->data, id);
                    else
                        return id->begin();
                }
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                return *p;
            }

            bool operator==(const iterator &rhs) const {
                if (id == rhs.id && p == rhs.p) return true;
                else return false;
            }

            bool operator==(const const_iterator &rhs) const {
                if (id == rhs.id && p == rhs.p) return true;
                else return false;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                return p;
            }
        };

        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
            friend class map;

        private:
            // data members.
        public:
            const_iterator() {
                // TODO
            }

            const_iterator(const const_iterator &other) {
                // TODO
            }

            const_iterator(const iterator &other) {
                // TODO
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
        };

        /**
         * TODO two constructors
         */
        map() = default;

        map(const map &other) {
            mp.clone(mp.root, other.mp.root);
        }

        /**
         * TODO assignment operator
         */
        map &operator=(const map &other) {
            mp.clone(mp.root, other.mp.root);
        }

        /**
         * TODO Destructors
         */
        ~map() {
            clear();
        }

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            if (mp.find(key) == nullptr)
                throw index_out_of_bound();
            return mp.find(key)->second;
        }

        const T &at(const Key &key) const {
            if (mp.find(key) == nullptr)
                throw index_out_of_bound();
            return mp.find(key)->second;
        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            return at(key);
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            return at(key);
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            value_type *t = mp.front();
            return iterator(t, this);
        }

        const_iterator cbegin() const {}

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(mp.end_node->data, this);
        }

        const_iterator cend() const {}

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return !(mp.len);
        }

        /**
         * returns the number of elements.
         */
        size_t size() const {
            return mp.len;
        }

        /**
         * clears the contents
         */
        void clear() {
            mp.make_empty();
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            value_type *t = mp.find(value.first);
            if (t) return std::make_pair(iterator(t, this), false);

            mp.insert(value);
            t = mp.find(value.first);
            return std::make_pair(iterator(t, this), true);
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            value_type *t = mp.find(pos->first);
            if (t == nullptr) throw index_out_of_bound();
            mp.remove(pos->first);
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            if (mp.find(key)) return 1;
            else return 0;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            value_type *t = mp.find(key);
            if (t == nullptr) return end();
            else return iterator(t, this);
        }

        const_iterator find(const Key &key) const {}

    };

}

#endif
