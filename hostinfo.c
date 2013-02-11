#include "csapp.h"

int main(int argc, char *argv[]) {
	char **pp; /* cursor */
	struct in_addr addr; /* holds the IP in integer form */
	struct hostent *hostp; /* holds various info about the server */

	if (argc != 2) {
		fprintf(stderr, "usage: %s <domain name or dotted-decimal>\n", argv[0]);
		return 0;
	}

	/*inet_aton will convert a dotted decimal into an in_addr. If the user has
	  given a named form, this will be 0, and we'll use Gethostbyname. */
	if (inet_aton(argv[1], &addr) != 0)
		hostp = Gethostbyaddr((const char*) &addr, sizeof(addr), AF_INET);
	else
		hostp = Gethostbyname(argv[1]);

	printf("Official hostname: %s\n", hostp->h_name);

	for (pp=hostp->h_aliases; *pp != NULL; ++pp)
		printf("alias: %s\n", *pp);

	for (pp = hostp->h_addr_list; *pp != NULL; ++pp) {
		addr.s_addr = ((struct in_addr*)*pp)->s_addr;
		printf("address: %s\n", inet_ntoa(addr));
	}

	return 0;
}