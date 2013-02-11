#include <stdio.h>
#include <string.h>

char *reverso(char *str, int start, int finish);

int main(int argc, char *argv[]) {
	int len;
	if (argc != 2) {
		fprintf(stderr, "usage: %s <string>\n", argv[0]);
		return 0;
	}

	len = strlen(argv[1]);
	printf("%s -> %s", argv[1], reverso(argv[1], 0, len));
	return 0;
}

char *string_concat(char *a, char *b) {
	return NULL;
}

char *reverso(char *str, int start, int finish) {
	int mid;
	if (start==finish)
		return str;
	mid = (start+finish)/2;
	return NULL;
}