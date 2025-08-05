#ifndef BADIFF_OP_H_
#define BADIFF_OP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define OP_STOP 0
#define OP_DELETE 1
#define OP_INSERT 2
#define OP_NEXT 3

typedef struct op {
    int op;
    const char* buf;
    int len;
} op_t;

#ifdef __cplusplus
}
#endif

#endif


