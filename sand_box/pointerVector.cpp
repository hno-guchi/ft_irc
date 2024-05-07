#include <vector>
#include <iostream>

int main() {
	std::vector<int *> v;

	int a = 1;
	int b = 2;
	int c = 3;

	v.push_back(&a);
	v.push_back(&b);
	v.push_back(&c);

	std::vector<int *>::iterator	it = v.begin();

	std::cout << "&a[" << &a << "] &v[0][" << &v[0] << "] *it[" << *it << "] **it[" << **it << "]" << std::endl;
	it++;
	std::cout << "&b[" << &b << "] &v[1][" << &v[1] << "] *it[" << *it << "] **it[" << **it << "]" << std::endl;
	it++;
	std::cout << "&c[" << &c << "] &v[2][" << &v[2] << "] *it[" << *it << "] **it[" << **it << "]" << std::endl;
	it++;

	std::vector<int *>::const_iterator	end = v.end();
	if (it == end) {
		std::cout << "it == v.end()" << std::endl;
	}

	it = v.begin();
	// *it = NULL;
	v.erase(it);
	std::cout << "&a[" << &a << "] &v[0][" << &v[0] << "] v[0][" << v[0] << "] *it[" << *it << "] **it[" << **it << "]" << std::endl;
	// *it = NULL;
	it = v.begin(); // *it = NULL をするとsegmenation fault
	std::cout << "&a[" << &a << "] &v[0][" << &v[0] << "] v[0][" << v[0] << "] *it[" << *it << "] **it[" << **it << "]" << std::endl;
	// std::vector<int*>::iterator	it2 = v.begin();
	// std::cout << "&a[" << &a << "] &v[0][" << &v[0] << "] v[0][" << v[0] << "] *it2[" << *it2 << "] **it2[" << **it2 << "]" << std::endl;
}
