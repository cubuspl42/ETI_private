#include <iostream>
#include <map>
#include <string>

extern "C" {
	float srednia_arytmetyczna(float tab[], unsigned n);
}

int main() {
	float tab[] = { 0.8, 0.2, 0.1, 1.5, 2.0 };
	float a = srednia_arytmetyczna(tab, sizeof(tab) / sizeof(*tab));
	printf("%f\n", a);
	return 0;
}