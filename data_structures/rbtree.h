#pragma once

#include "../utils/typedefs.h"

/*
 * Red-Black Tree Implementation
 * Adapted for VoidFrameX Kernel
 */

struct rb_node {
  unsigned long __rb_parent_color;
  struct rb_node *rb_right;
  struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));
/* The alignment might seem pointless, but allegedly CRIS needs it */

struct rb_root {
  struct rb_node *rb_node;
};

#define RB_ROOT                                                                \
  (struct rb_root) { NULL }
#define rb_entry(ptr, type, member)                                            \
  ((type *)((char *)(ptr) - __builtin_offsetof(type, member)))

#define RB_BLACK 0
#define RB_RED 1

extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);

/* Find logical next and previous nodes in a tree */
extern struct rb_node *rb_next(struct rb_node *);
extern struct rb_node *rb_prev(struct rb_node *);
extern struct rb_node *rb_first(const struct rb_root *);
extern struct rb_node *rb_last(const struct rb_root *);

/* Fast replacement of a single node without remove/rebalance */
extern void rb_replace_node(struct rb_node *victim, struct rb_node *new,
                            struct rb_root *root);

static inline void rb_link_node(struct rb_node *node, struct rb_node *parent,
                                struct rb_node **rb_link) {
  node->__rb_parent_color = (unsigned long)parent;
  node->rb_left = node->rb_right = NULL;

  *rb_link = node;
}

static inline struct rb_node *rb_parent(struct rb_node *node) {
  return (struct rb_node *)(node->__rb_parent_color & ~3);
}

static inline int rb_is_red(struct rb_node *node) {
  return node->__rb_parent_color & 1;
}

static inline int rb_is_black(struct rb_node *node) { return !rb_is_red(node); }

static inline void rb_set_red(struct rb_node *node) {
  node->__rb_parent_color |= RB_RED;
}

static inline void rb_set_black(struct rb_node *node) {
  node->__rb_parent_color &= ~RB_RED;
}

static inline void rb_set_parent(struct rb_node *node, struct rb_node *parent) {
  node->__rb_parent_color =
      (node->__rb_parent_color & 3) | (unsigned long)parent;
}

static inline void rb_set_color(struct rb_node *node, int color) {
  node->__rb_parent_color = (node->__rb_parent_color & ~1) | color;
}
