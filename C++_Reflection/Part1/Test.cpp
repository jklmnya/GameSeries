#include "Reflection.h"

class A {

public:
	void Fun() {
		std::cout << "A::Fun" << std::endl;
	}
};

DEFINE_DYN_CREATE(A);
IMPLEMENT_DYN_CREATE(A);
REGIST_CLASS(A);

int main() {
	A* a = (A*)(ClassFactory::GetInstance().GetClassObjectByName("A"));
	if (a)
		a->Fun();
	return 0;
}