#ifndef BADIFF_OPQ_H_
#define BADIFF_OPQ_H_

#include <badiff/op.h>

#ifdef __cplusplus
extern "C" {
#endif

int badiff_apply_opq(char *tgt, const op_t *q, const char *src);
int badiff_compute_opq(op_t *q, int q_len,  const char *src, int src_len, const char *tgt, int tgt_len);

int badiff_get_opq_tgt_len(const op_t *q);
int badiff_get_opq_src_len(const op_t *q);
int badiff_get_opq_q_len(const op_t *q);

#ifdef __cplusplus
}
#endif

#endif
