#include "debug.h"

//FILE* log_file = NULL;
char *log_prefix = "";

char *sup_err_names[] = { "ERR_ZERO",
						 "ERR_ONE",
};

int equal(void *bufa, void *bufb, size_t len) {
	char *ca = (char *)bufa;
	char *cb = (char *)bufb;
	for (int i = 0; i < len; ++i) {
		if (ca[i] != cb[i]) {
			return 0;
		}
	}
	return 1;
}

void buf2hex(char *dst, unsigned char *src, int src_len) {
	for (int i = 0; i < src_len; ++i) {
		//debug("%i, %i", i, src[i]);
		sprintf(dst + (i * 2), "%.2x", src[i]);
		//debug("%s", dst);
	}
}
