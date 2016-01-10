#include <iostream>
#include <map>
#include <string>

extern "C" {
	float nowy_cosh(float x);
}

int main() {
	float c1 = nowy_cosh(1);
	float c2 = nowy_cosh(2);
	float c3 = nowy_cosh(5);
	return 0;
}