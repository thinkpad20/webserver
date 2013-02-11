#include <stdio.h>

struct silly {
	int a, b, c;
};

int main() {
	struct silly s;
	printf("%s, %s, %s\n", s.a, s.b, s.c);

	bzero((char *)&s, sizeof(s));
	printf("%s, %s, %s\n", s.a, s.b, s.c);

	return 0;
}