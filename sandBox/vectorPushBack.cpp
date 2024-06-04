#include <iostream>
#include <vector>

void add(std::vector<int>* v) {
	int i = 1;

	v->push_back(i);
	std::cout << &i << std::endl;
}
int main() {
	std::vector<int> v;
	add(&v);

	std::cout << v.size() << std::endl;
	std::cout << v[0] << std::endl;
	std::cout << &(v[0]) << std::endl;

	v.push_back(int(2));
	std::cout << v.size() << std::endl;
	std::cout << v[1] << std::endl;
	std::cout << &(v[1]) << std::endl;

	for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
		*it = 100;
	}
	std::cout << v[0] << " " << &(v[0]) << std::endl;
	std::cout << v[1] << " " << &(v[1]) << std::endl;
}
