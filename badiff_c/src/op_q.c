#include <badiff/opq.h>

#include <string.h>

int badiff_apply_opq(char *tgt, const op_t *q, const char *src) {
  char *tgt_was = tgt;

  while (q->op != OP_STOP) {
    if (q->op == OP_DELETE) {
      src += q->len;
    } else if (q->op == OP_INSERT) {
      memcpy(tgt, q->buf, q->len);
      tgt += q->len;
    } else if (q->op == OP_NEXT) {
      memcpy(tgt, src, q->len);
      src += q->len;
      tgt += q->len;
    }
    ++q;
  }
  return tgt - tgt_was;
}

int badiff_compute_opq(op_t *q, int q_len, const char *src, int src_len,
                       const char *tgt, int tgt_len) {
  // hardcode to a replace for now
  if (q_len < 3)
    return -1;
  q[0].op = OP_DELETE;
  q[0].len = src_len;
  q[1].op = OP_INSERT;
  q[1].len = tgt_len;
  q[1].buf = tgt;
  q[2].op = OP_STOP;
  return 3;
}

int badiff_get_opq_tgt_len(const op_t *q) {
  int len = 0;
  while (q->op != OP_STOP) {
    if (q->op == OP_INSERT || q->op == OP_NEXT)
      len += q->len;
    ++q;
  }
  return len;
}
int badiff_get_opq_src_len(const op_t *q) {
  int len = 0;
  while (q->op != OP_STOP) {
    if (q->op == OP_DELETE || q->op == OP_NEXT)
      len += q->len;
    ++q;
  }
  return len;
}

int badiff_get_opq_q_len(const op_t *q) {
  int len = 0;
  while (q->op != OP_STOP) {
    ++len;
    ++q;
  }
  return len + 1; // Include the OP_STOP
}
