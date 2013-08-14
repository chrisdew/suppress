/*
 * debug.h
 *
 *  Created on: 5 Feb 2013
 *      Author: chris
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef NDEBUG
#define debug(M, ...)
#define debug_hex(BUF, LEN)
#else
#define debug(M, ...) fprintf(stderr, "%i: %sDEBUG %10.10s:%3.0d: " M "\n", getpid(), log_prefix, __FILE__, __LINE__, ##__VA_ARGS__); fflush(stderr);
#define debug_hex(BUF, LEN) do { \
	debug("hex: %lli bytes", (long long) LEN); \
	unsigned char *macro_buf = (unsigned char *) (BUF); \
	for (int macro_i = 0; macro_i < LEN; ++macro_i) { \
		fprintf(stderr, "%.2x", macro_buf[macro_i]); \
	} \
	fprintf(stderr, "\n"); \
} while(0);
#endif

#define CEIL(a, b) (((a) / (b)) + ((a % b) > 0 ? 1 : 0))

//FILE *log_file = stderr;

#define SUP_ERR_START (-100)
enum SUP_ERRS {
	ERR_ZERO,
	ERR_ONE,
	SUP_NUM_ERRS
};

#define MIN(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

extern char *log_prefix;
extern char *sup_err_names[];
int equal(void *bufa, void *bufb, size_t len);
void buf2hex(char *dst, unsigned char *src, int src_len);

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, "%i: %s[ERROR] (%s:%d: errno: %i %s) " M "\n", getpid(), log_prefix, __FILE__, __LINE__, errno, clean_errno(), ##__VA_ARGS__); fflush(stderr)

#define log_warn(M, ...) fprintf(stderr, "%i: %s[WARN] (%s:%d: errno: %s) " M "\n", getpid(), log_prefix, __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__); fflush(stderr)

#define log_info(M, ...) fprintf(stderr, "%i: %s[INFO] (%s:%d) " M "\n", getpid(), log_prefix, __FILE__, __LINE__, ##__VA_ARGS__); fflush(stderr)

#define non_zero(A, N, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto N; }

#define assert_positive(A, N, O, P, M, ...) if((A < 1)) { log_err(M, ##__VA_ARGS__); errno=0; O = P; goto N; }

#define assert_zero(A, N, O, P, M, ...) if((A != 0)) { log_err(M, ##__VA_ARGS__); errno=0; O = P; goto N; }

#define check_non_neg(result, err_label, var, var_value, format, ...) if((result < 0)) { log_err(format, ##__VA_ARGS__); errno=0; var = var_value; goto err_label; }

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }





// master side
#define quit_if(COND, SUP_ERR) do { \
	if(COND) { \
		log_err("%s", sup_err_names[SUP_ERR]); \
		exit(SUP_ERR_START - SUP_ERR); \
	} \
} while(0);

// slave side




#endif /* DEBUG_H_ */
