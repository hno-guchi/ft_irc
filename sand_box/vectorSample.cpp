#include <iostream>
#include <vector>

int main() {
	int*	i = new int(1);

	std::vector<int *> v;

	v.push_back(i);
	std::vector<int *>::iterator it = v.begin();

	std::cout << "*i, i, &i" << std::endl;
	std::cout << *i << std::endl;
	std::cout << i << std::endl;
	std::cout << &i << std::endl;
	std::cout << std::endl;

	std::cout << "*(v[0]), v[0], &(v[0])" << std::endl;
	std::cout << *(v[0]) << std::endl;
	std::cout << v[0] << std::endl;
	std::cout << &(v[0]) << std::endl;
	std::cout << std::endl;

	std::cout << "**it, *it" << std::endl;
	std::cout << **it << std::endl;
	std::cout << *it << std::endl;
	std::cout << std::endl;

	std::cout << "v.size()" << std::endl;
	std::cout << v.size() << std::endl;

	// v[0] = NULL;
	*it = NULL;
	v.erase(it);
	// v[0] = NULL;											// segmentation fault
	// std::cout << v[0] << std::endl;
	// std::cout << &(v[0]) << std::endl;
	// std::cout << std::endl;

	std::cout << "v.size()" << std::endl;
	std::cout << v.size() << std::endl;
	std::cout << std::endl;

	std::cout << "*i, i, &i" << std::endl;
	std::cout << *i << std::endl;
	std::cout << i << std::endl;
	std::cout << &i << std::endl;
	std::cout << std::endl;

	std::cout << "&(v[0])" << std::endl;
	// std::cout << "*(v[0]), v[0], &(v[0])" << std::endl;
	// std::cout << *(v[0]) << std::endl;					// segmentation fault
	std::cout << v[0] << std::endl;						// segmentation fault
	std::cout << &(v[0]) << std::endl;
	std::cout << std::endl;

	// std::cout << "*it, &(*it)" << std::endl;
	std::cout << "&(*it)" << std::endl;
	// std::cout << **it << std::endl;						// segmentation fault
	std::cout << *it << std::endl;						// segmentation fault
	std::cout << &(*it) << std::endl;
	std::cout << std::endl;
}
