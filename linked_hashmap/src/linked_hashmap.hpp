/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
    /**
     * In linked_hashmap, iteration ordering is differ from map,
     * which is the order in which keys were inserted into the map.
     * You should maintain a doubly-linked list running through all
     * of its entries to keep the correct iteration order.
     *
     * Note that insertion order is not affected if a key is re-inserted
     * into the map.
     */

    template<
            class Key,
            class T,
            class Hash = std::hash<Key>,
            class Equal = std::equal_to<Key>
    >
    class linked_hashmap {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::linked_hashmap as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;

        struct NODE {
            value_type *data;
            int hash;
            NODE *next, *order_pre, *order_nxt;

            NODE() : hash(-1), next(nullptr), order_nxt(nullptr), order_pre(nullptr), data(nullptr) {}

            NODE(const NODE &other) : hash(other.hash), next(nullptr), order_nxt(nullptr),
                                      order_pre(nullptr) {
                data = new value_type(other.data);
            }

            NODE(const value_type &_data, int _hash = -1) : hash(_hash), next(nullptr), order_nxt(nullptr),
                                                            order_pre(nullptr) {
                data = new value_type(_data);
            }

            ~NODE() {
                if (data) delete data;
            }
        };

        int capacity, siz; //capacity表示指针数组的容量（表头的长度），而siz是实际元素个数
        double load_factor; //负载因子，当 siz > capacity * factor 时，需要 double_space
        NODE **arr;
        NODE *rear, *head; //用来存储双链表(输入顺序)


        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = linked_hashmap.begin(); --it;
         *       or it = linked_hashmap.end(); ++end();
         */
        class const_iterator;

        class iterator {
            friend class linked_hashmap;

        private:
            NODE *p;
            const linked_hashmap<Key, T, Hash, Equal> *id;

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


            iterator() : p(nullptr), id(nullptr) {}

            iterator(const iterator &other) : p(other.p), id(other.id) {}

            iterator(NODE *_p, const linked_hashmap<Key, T, Hash, Equal> *_id) : p(_p), id(_id) {}

            //TODO iter++
            iterator operator++(int) {
                iterator t(*this);
                ++(*this);
                return t;
            }

            //TODO ++iter
            iterator &operator++() {
                if (*this == id->cend() || p == nullptr) throw invalid_iterator();
                p = p->order_nxt;
                return *this;
            }

            iterator operator--(int) {
                iterator t(*this);
                --(*this);
                return t;
            }

            iterator &operator--() {
                if (*this == id->cbegin() || p == nullptr) throw invalid_iterator();
                p = p->order_pre;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                if (p == nullptr || p == id->head || p == id->rear) throw invalid_iterator();
                return *(p->data);
            }

            bool operator==(const iterator &rhs) const {
                return p == rhs.p && id == rhs.id;
            }

            bool operator==(const const_iterator &rhs) const {
                return p == rhs.p && id == rhs.id;
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
                if (p == nullptr || p == id->head || p == id->rear) throw invalid_iterator();
                return p->data;
            }
        };

        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
            friend class linked_hashmap;

        private:
            NODE *p;
            const linked_hashmap<Key, T, Hash, Equal> *id;
        public:
            const_iterator() : p(nullptr), id(nullptr) {}

            const_iterator(const const_iterator &other) : p(other.p), id(other.id) {}

            const_iterator(const iterator &other) : p(other.p), id(other.id) {}

            const_iterator(NODE *_p, const linked_hashmap<Key, T, Hash, Equal> *_id) : p(_p), id(_id) {}

            const_iterator operator++(int) {
                const_iterator t(*this);
                ++(*this);
                return t;
            }

            const_iterator &operator++() {
                if (*this == id->cend() || p == nullptr) throw invalid_iterator();
                p = p->order_nxt;
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator t(*this);
                --(*this);
                return t;
            }

            const_iterator &operator--() {
                if (*this == id->cbegin() || p == nullptr) throw invalid_iterator();
                p = p->order_pre;
                return *this;
            }

            value_type &operator*() const {
                if (p == nullptr || p == id->head || p == id->rear) throw invalid_iterator();
                return *(p->data);
            }

            bool operator==(const iterator &rhs) const {
                return p == rhs.p && id == rhs.id;
            }

            bool operator==(const const_iterator &rhs) const {
                return p == rhs.p && id == rhs.id;
            }

            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }

            value_type *operator->() const noexcept {
                if (p == nullptr || p == id->head || p == id->rear) throw invalid_iterator();
                return p->data;
            }
        };

        /**---------------------------------------------------linked_hashmap-----------------------------------------------------
         * TODO two constructors
         */

    public:
        //static double Clock;
        linked_hashmap() : capacity(16), load_factor(0.75), siz(0){
            head = new NODE;
            rear = new NODE;
            head->order_nxt = rear;
            head->order_pre = nullptr;
            rear->order_pre = head;
            rear->order_nxt = nullptr;

            arr = new NODE *[capacity];
            std::fill(arr, arr + capacity, nullptr);
        }

        linked_hashmap(const linked_hashmap &other) : load_factor(other.load_factor), capacity(other.capacity), siz(0){
            head = new NODE;
            rear = new NODE;
            head->order_nxt = rear;
            head->order_pre = nullptr;
            rear->order_pre = head;
            rear->order_nxt = nullptr;

            arr = new NODE *[capacity];
            std::fill(arr, arr + capacity, nullptr);

            for (NODE *tp = other.head->order_nxt; tp != other.rear; tp = tp->order_nxt)
                insert(*(tp->data));
        }

        /**
         * TODO assignment operator
         */
        linked_hashmap &operator=(const linked_hashmap &other) {
            if (this == &other) return *this;
            clear();
            for (NODE *tp = other.head->order_nxt; tp != other.rear; tp = tp->order_nxt)
                insert(*(tp->data));
            return *this;
        }

        /**
         * TODO Destructors
         */
        ~linked_hashmap() {
            clear();
            delete[] arr;
            delete head;
            delete rear;
            //std::cout << "Clock : " << Clock << std::endl;
        }

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            iterator it = find(key);
            if (it == end()) {
                throw index_out_of_bound();
            }

            return it.p->data->second;
        }

        const T &at(const Key &key) const {
            const_iterator it = find(key);
            if (it == cend()) {
                throw index_out_of_bound();
            }

            return it.p->data->second;
        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            iterator it = find(key);
            if (it == end())
                return insert(value_type(key, T())).first.p->data->second;
            else
                return it.p->data->second;
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            const_iterator it = find(key);
            if (it == cend())
                throw index_out_of_bound();
            else
                return it.p->data->second;
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator(head->order_nxt, this);
        }

        const_iterator cbegin() const {
            return const_iterator(head->order_nxt, this);
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(rear, this);
        }

        const_iterator cend() const {
            return const_iterator(rear, this);
        }

        bool empty() const {
            return !siz;
        }

        size_t size() const {
            return siz;
        }

        void clear() {
            siz = 0;
            for (NODE *tp = head->order_nxt->order_nxt; tp; tp = tp->order_nxt) {
                delete tp->order_pre;
            }
            for (int i = 0;i < capacity; ++i) {
                if (arr[i]) delete arr[i];
                arr[i] = nullptr;
            }

            head->order_nxt = rear;
            rear->order_pre = head;
        }

        void double_space() { //新开一个->不可取
            //直接改变数据节点的存储方式即可.
            for (int i = 0; i < capacity; ++i){
                if (arr[i]) delete arr[i];
                arr[i] = nullptr;
            }
            delete[] arr;

            capacity = capacity << 1;
            arr = new NODE *[capacity];
            std::fill(arr, arr + capacity, nullptr);

            for (NODE *tp = head->order_nxt; tp != rear; tp = tp->order_nxt) {
                int num = (tp->hash % capacity + capacity) % capacity;
                if (arr[num] == nullptr) arr[num] = new NODE;
                NODE *tp_2;
                for (tp_2 = arr[num]; tp_2->next; tp_2 = tp_2->next);
                tp_2->next = tp;
                tp->next = nullptr;
            }
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            //先判断是否已经存在
            //Clock += clock() / (double)(1e6);
            iterator it = find(value.first);
            if (it != end()) {
                return pair<iterator, bool>(it, false);
            }

            if (siz >= load_factor * capacity) {
                //std::cout << "size = " << siz << " capacity = " << capacity << std::endl;
                double_space();
            }
            ++siz;

            int H = Hash()(value.first), num = (H % capacity + capacity) % capacity;
            //存储的哈希值H不需要取模！
            if (arr[num] == nullptr) arr[num] = new NODE;
            NODE *tmp, *tp;
            for (tp = arr[num]; tp->next; tp = tp->next);
            tmp = new NODE(value, H);
            tmp->next = nullptr;
            tp->next = tmp;

            tmp->order_pre = rear->order_pre;
            rear->order_pre->order_nxt = tmp;
            tmp->order_nxt = rear;
            rear->order_pre = tmp;
            return pair<iterator, bool>(iterator(tmp, this), true);
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos.id != this || pos == end() || pos.p == head ||
                pos.p == nullptr || find(pos.p->data->first) == end() )
                throw invalid_iterator();

            siz--;
            NODE *to_del = pos.p;
            to_del->order_pre->order_nxt = to_del->order_nxt;
            to_del->order_nxt->order_pre = to_del->order_pre;
            int num = (Hash()(to_del->data->first) % capacity + capacity) % capacity;
            if (arr[num] == nullptr || arr[num]->next == nullptr) return;
            for (NODE *tp = arr[num]; tp->next; tp = tp->next) {
                if (Equal()(tp->next->data->first, to_del->data->first)) {
                    tp->next = to_del->next;
                    delete to_del;
                    return;
                }
            }
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         */
        size_t count(const Key &key) const {
            if (find(key) == cend()) return 0;
            else return 1;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            int num = (Hash()(key) % capacity + capacity) % capacity;
            if (arr[num] == nullptr || arr[num]->next == nullptr) return end();
            for (NODE *tp = arr[num]->next; tp; tp = tp->next) {
                if (Equal()(tp->data->first, key)) return iterator(tp, this);
            }

            return end();
        }

        const_iterator find(const Key &key) const {
            int num = (Hash()(key) % capacity + capacity) % capacity;
            if (arr[num] == nullptr || arr[num]->next == nullptr) return cend();
            for (NODE *tp = arr[num]->next; tp; tp = tp->next) {
                if (Equal()(tp->data->first, key)) return const_iterator(tp, this);
            }

            return cend();
        }
    };

}

#endif