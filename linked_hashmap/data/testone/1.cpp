#include "linked_hashmap.hpp"
#include <iostream>
#include <cassert>
#include <string>
class Integer {
public:
	static int counter;
	int val;
	
	Integer(int val) : val(val) {
		counter++;
	}

	Integer(const Integer &rhs) {
		val = rhs.val;
		counter++;
	}

	Integer& operator = (const Integer &rhs) {
		assert(false);
	}

	~Integer() {
		counter--;
	}
};

int Integer::counter = 0;

class Equal {
public:
	bool operator () (const Integer &lhs, const Integer &rhs) const {
		return lhs.val == rhs.val;
	}
};
class Hash {
public:
	unsigned int operator () (Integer lhs) const {
		int val = lhs.val;
		return std::hash<int>()(val);
	}
};
void tester(void) {
//    std::cout<<clock()/(double)1e6<<'\n';

    //	test: constructor

    sjtu::linked_hashmap<Integer, std::string,Hash,Equal> map;
//    std::cout<<clock()/(double)1e6<<'\n';

    //	test: empty(), size()
	assert(map.empty() && map.size() == 0);
//    std::cout<<clock()/(double)1e6<<'\n';

    //	test: operator[], insert()
	for (int i = 0; i < 100000; ++i) {
		std::string string = "";
		for (int number = i; number; number /= 10) {
			char digit = '0' + number % 10;
			string = digit + string;
		}
		if (i & 1) {
			map[Integer(i)] = string;
			auto result = map.insert(sjtu::pair<Integer, std::string>(Integer(i), string));
			assert(!result.second);
		} else {
			auto result = map.insert(sjtu::pair<Integer, std::string>(Integer(i), string));
			assert(result.second);
		}
	}

//    std::cout<<clock()/(double)1e6<<'\n';

	//	test: count(), find(), erase()
	for (int i = 0; i < 100000; ++i) {
		if (i > 1896 && i <= 2016) {
			continue;
		}
		assert(map.count(Integer(i)) == 1);
		assert(map.find(Integer(i)) != map.end());
		map.erase(map.find(Integer(i)));
	}

//    std::cout<<clock()/(double)1e6<<'\n';

    //	test: constructor, operator=, clear();
	for (int i = 0; i < (int)map.size(); ++i) {
		sjtu::linked_hashmap<Integer, std::string,Hash,Equal> copy(map);
		map.clear();
		std::cout << map.size() << " " << copy.size() << " ";
		map = copy;
		copy.clear();
		std::cout << map.size() << " " << copy.size() << " ";
		copy = map;
		map.clear();
		std::cout << map.size() << " " << copy.size() << " ";
		map = copy;
		copy.clear();
		std::cout << map.size() << " " << copy.size() << " ";
	}
	std::cout << std::endl;

//    std::cout<<clock()/(double)1e6<<'\n';

    //	test: const_iterator, cbegin(), cend(), operator++, at()
	sjtu::linked_hashmap<Integer, std::string,Hash,Equal>::const_iterator const_iterator;
	const_iterator = map.cbegin();
	while (const_iterator != map.cend()) {
		const Integer integer(const_iterator->first);
		const_iterator++;
		std::cout << map.at(integer) << " ";
	}
	std::cout << std::endl;

//    std::cout<<clock()/(double)1e6<<'\n';

    //	test: iterator, operator--, operator->
	sjtu::linked_hashmap<Integer, std::string,Hash,Equal>::iterator iterator;
	iterator = map.end();
	while (true) {
		sjtu::linked_hashmap<Integer, std::string,Hash,Equal>::iterator peek = iterator;
		if (peek == map.begin()) {
			std::cout << std::endl;
			break;
		}
		std::cout << (--iterator)->second << " ";
	}

//    std::cout<<clock()/(double)1e6<<'\n';

    //	test: erase()
	while (map.begin() != map.end()) {
		map.erase(map.begin());
	}
	assert(map.empty() && map.size() == 0);
//    std::cout<<clock()/(double)1e6<<'\n';

    //	test: operator[]
	for (int i = 0; i < 100000; ++i) {
		std::cout << map[Integer(i)];
	}
	std::cout << map.size() << std::endl;
//    std::cout<<clock()/(double)1e6<<'\n';

}

int main(void) {
	tester();
	std::cout << Integer::counter << std::endl;
}
