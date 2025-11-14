
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <tuple>
#include <vector>

#define DIKE_DEBUG_NAME "quadtree"
#include "DikeDebug.hpp"
#include "DikePath.hpp"
#include "DikeCalculateMethod.hpp"
#include "DikeCalculateMethodQuadTree.hpp"

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

#define MAXLON          (+180.00)
#define MINLON          (-180.00)

#define MAXLAT          (+90.00)
#define MINLAT          (-90.00)

#define MAXLEVEL        (18)

#define DIKE_STACK_VALUE_TYPE           int32_t
#define DIKE_STACK_GROW_FUNCTION(c, r)  ({ \
        if (!c) { \
                c = 1; \
        } \
        while ((DIKE_STACK_VALUE_TYPE) c < r) { \
                if (r >= 4096) { \
                        c += 4096; \
                } else { \
                        c *= 2; \
                } \
        } \
        c; \
})

struct dike_stack {
        DIKE_STACK_VALUE_TYPE count;
        DIKE_STACK_VALUE_TYPE size;
        DIKE_STACK_VALUE_TYPE available;
        unsigned char *buffer;
};

static inline void dike_stack_init (struct dike_stack *stack, unsigned long long size)
{
        stack->count = 0;
        stack->size = size;
        stack->available = 0;
        stack->buffer = NULL;
}

static inline void dike_stack_uninit (struct dike_stack *stack)
{
        stack->count = 0;
        stack->available = 0;
        if (stack->buffer != NULL) {
                free(stack->buffer);
        }
        stack->buffer = NULL;
}

#define __dike_stack_calculate_grow(current, request) ({ \
        unsigned long long calc; \
        calc = current; \
        calc = DIKE_STACK_GROW_FUNCTION(calc, request); \
        calc; \
})

static inline int dike_stack_reserve (struct dike_stack *stack, unsigned long long reserve)
{
        unsigned char *buffer;
        if (stack->available >= (DIKE_STACK_VALUE_TYPE) reserve) {
                return 0;
        }
#if 0
        reserve = __dike_stack_calculate_grow(stack->available, reserve);
#endif
        buffer = (unsigned char *) realloc(stack->buffer, reserve * stack->size);
        if (unlikely(buffer == NULL)) {
                buffer = (unsigned char *) malloc(reserve * stack->size);
                if (unlikely(buffer == NULL)) {
                        return -1;
                }
                memcpy(buffer, stack->buffer, stack->count * stack->size);
                free(stack->buffer);
        }
        stack->buffer = buffer;
        stack->available = reserve;
        return 0;
}

static inline unsigned long long dike_stack_count (const struct dike_stack *stack)
{
        return stack->count;
}

static inline int dike_stack_push (struct dike_stack * __restrict__ stack, const void * __restrict__ elem)
{
        int rc;
        rc = dike_stack_reserve(stack, __dike_stack_calculate_grow(stack->available, stack->count + 1));
        if (unlikely(rc != 0)) {
                return -1;
        }
        memcpy(stack->buffer + stack->count++ * stack->size, elem, stack->size);
        return 0;
}

static inline void dike_stack_del (struct dike_stack *stack, unsigned long long at)
{
        void *dst;
        void *src;
        unsigned long long size;
        if ((DIKE_STACK_VALUE_TYPE) at >= stack->count) {
                return;
        }
        dst = stack->buffer + at * stack->size;
        src = stack->buffer + (at + 1) * stack->size;
        size = (stack->count - at - 1) * stack->size;
        if (size != 0) {
                memmove(dst, src, size);
        }
        stack->count -= 1;
}

static inline void * dike_stack_at_unsafe (const struct dike_stack *stack, unsigned long long at)
{
        return stack->buffer + at * stack->size;
}

#define DIKE_BOUND_FORMAT         "%.7f, %.7f, %.7f, %.7f"
#define DIKE_BOUND_FORMAT_ARG(b)  (b).xmin, (b).ymin, (b).xmax, (b).ymax

struct dike_bound {
        double xmin;
        double ymin;
        double xmax;
        double ymax;
};

static inline __attribute__ ((warn_unused_result)) struct dike_bound dike_bound_null (void)
{
        struct dike_bound bound;
        bound.xmin = (double) INT64_MAX;
        bound.ymin = (double) INT64_MAX;
        bound.xmax = (double) INT64_MIN;
        bound.ymax = (double) INT64_MIN;
        return bound;
}

static inline __attribute__ ((warn_unused_result)) int dike_bound_invalid (const struct dike_bound *bound)
{
        return (bound->xmax < bound->xmin || bound->ymax < bound->ymin);
}

static inline __attribute__ ((warn_unused_result)) struct dike_bound dike_bound_init (const double xmin, const double ymin, const double xmax, const double ymax)
{
        struct dike_bound bound;
        bound.xmin = xmin;
        bound.ymin = ymin;
        bound.xmax = xmax;
        bound.ymax = ymax;
        return bound;
}

static inline __attribute__ ((warn_unused_result)) int dike_bound_intersects (const struct dike_bound * __restrict__ a, const struct dike_bound * __restrict__  b)
{
        if (MAX(a->xmin, b->xmin) > MIN(a->xmax, b->xmax)) {
                return 0;
        }
        if (MAX(a->ymin, b->ymin) > MIN(a->ymax, b->ymax)) {
                return 0;
        }
        return 1;
}

static inline __attribute__ ((warn_unused_result)) int dike_bound_contains (const struct dike_bound * __restrict__ a, const struct dike_bound * __restrict__  b)
{
        if (dike_bound_intersects(a, b) == 0) {
                return 0;
        }
        if ((b->xmin >= a->xmin) && (b->xmax <= a->xmax) &&
            (b->ymin >= a->ymin) && (b->ymax <= a->ymax)) {
                /* full */
                return 1;
        }
        /* partial */
        return 2;
}

static inline __attribute__ ((warn_unused_result)) struct dike_bound dike_bound_union_xy (const struct dike_bound * __restrict__ a, double x, double y)
{
        struct dike_bound u;
        u.xmin = MIN(a->xmin, x);
        u.ymin = MIN(a->ymin, y);
        u.xmax = MAX(a->xmax, x);
        u.ymax = MAX(a->ymax, y);
	return u;
}

struct dike_qtree_item {
        int depth;
        struct dike_bound bound;
        void *data;
};

struct dike_qtree_bucket {
        int depth;
        struct dike_bound bound;
        struct dike_stack items;
        struct dike_qtree_bucket *childs[4];
        struct dike_qtree_bucket *parent;
};

struct dike_qtree {
        int depth;
        unsigned long long count;
        struct dike_bound bound;
        struct dike_qtree_bucket *root;
};

#define dike_qtree_capture_destroy(a)                   do { } while (0)
#define dike_qtree_capture_create(a, b, c, d)           do { } while (0)
#define dike_qtree_capture_add_bound(a, b, c)           do { } while (0)
#define dike_qtree_capture_check_bound(a, b)            do { } while (0)

static __attribute__ ((__unused__)) void dike_qtree_bucket_destroy (struct dike_qtree_bucket *bucket)
{
        unsigned long long c;
        if (bucket == NULL) {
                return;
        }
        for (c = 0; c < 4; c++) {
                dike_qtree_bucket_destroy(bucket->childs[c]);
        }
        dike_stack_uninit(&bucket->items);
        free(bucket);
}

static __attribute__ ((__unused__)) struct dike_qtree_bucket * dike_qtree_bucket_create (struct dike_bound *bound, struct dike_qtree_bucket *parent)
{
        struct dike_qtree_bucket *bucket;
        bucket = (struct dike_qtree_bucket *) calloc(1, sizeof(struct dike_qtree_bucket));
        if (bucket == NULL) {
                dikeErrorf("can not allocate memory");
                goto bail;
        }
        bucket->bound = *bound;
        bucket->parent = parent;
        if (bucket->parent != NULL) {
                bucket->depth = bucket->parent->depth + 1;
        }
        dike_stack_init(&bucket->items, sizeof(struct dike_qtree_item));
        return bucket;
bail:   if (bucket != NULL) {
                dike_qtree_bucket_destroy(bucket);
        }
        return NULL;
}

static int dike_qtree_bucket_traverse (
                struct dike_qtree_bucket *bucket,
                int (*traverse_bucket_start) (void *context, struct dike_qtree_bucket *bucket),
                int (*traverse_item) (void *context, struct dike_qtree_bucket *bucket, struct dike_qtree_item *item),
                int (*traverse_bucket_end) (void *context),
                void *context)
{
        unsigned long long c;
        unsigned long long b;
        unsigned long long bl;
        struct dike_qtree_item *item;
        if (bucket == NULL) {
                return 0;
        }
        if (traverse_bucket_start != NULL) {
                if (dike_stack_count(&bucket->items) > 0) {
                        traverse_bucket_start(context, bucket);
                }
        }
        if (traverse_item != NULL) {
                for (b = 0, bl = dike_stack_count(&bucket->items); b < bl; b++) {
                        item = (struct dike_qtree_item *) dike_stack_at_unsafe(&bucket->items, b);
                        traverse_item(context, bucket, item);
                }
        }
        if (traverse_bucket_end != NULL) {
                if (dike_stack_count(&bucket->items) > 0) {
                        traverse_bucket_end(context);
                }
        }
        for (c = 0; c < 4; c++) {
                if (bucket->childs[c] == NULL) {
                        continue;
                }
                dike_qtree_bucket_traverse(bucket->childs[c], traverse_bucket_start, traverse_item, traverse_bucket_end, context);
        }
        return 0;
}

static __attribute__ ((__unused__)) int dike_qtree_bucket_check_bound (struct dike_qtree_bucket *bucket, struct dike_bound *bound, int (*compare) (void *context, dike_bound *bound, void *data, int radius), void *context, int radius)
{
        unsigned long long c;
        unsigned long long b;
        unsigned long long bl;
        struct dike_qtree_item *item;
        if (bucket == NULL) {
                return 0;
        }
        if (dike_bound_intersects(&bucket->bound, bound) == 0) {
                return 0;
        }
        for (b = 0, bl = dike_stack_count(&bucket->items); b < bl; b++) {
                item = (struct dike_qtree_item *) dike_stack_at_unsafe(&bucket->items, b);
                if (dike_bound_intersects(&item->bound, bound)) {
                        if (compare == NULL) {
                                return 1;
                        } else if (compare(context, &item->bound, item->data, radius)) {
                                return 1;
                        }
                }
        }
        for (c = 0; c < 4; c++) {
                if (dike_qtree_bucket_check_bound(bucket->childs[c], bound, compare, context, radius)) {
                        return 1;
                }
        }
        return 0;
}

static __attribute__ ((__unused__)) int dike_qtree_bucket_add_bound (struct dike_qtree_bucket *bucket, int depth, unsigned long long count, struct dike_bound *bound, void *data)
{
        int i;
        int j;
        int c;
        int rc;
        unsigned long long b;
        struct dike_qtree_item eitem;
        struct dike_bound cbound;
        dikeDebugf("  depth: %d", depth);
        dikeDebugf("  bucket: " DIKE_BOUND_FORMAT "", DIKE_BOUND_FORMAT_ARG(bucket->bound));
        dikeDebugf("  bound: " DIKE_BOUND_FORMAT "", DIKE_BOUND_FORMAT_ARG(*bound));
        if (depth <= 0) {
                dikeDebugf("      bucket: %d, " DIKE_BOUND_FORMAT "", bucket->depth, DIKE_BOUND_FORMAT_ARG(bucket->bound));
                eitem.depth = depth;
                eitem.bound = *bound;
                eitem.data = data;
                rc = dike_stack_push(&bucket->items, &eitem);
                if (rc != 0) {
                        dikeErrorf("can not push bound");
                        return -1;
                }
                return 0;
        }
        if (bucket->childs[0] == NULL &&
            bucket->childs[1] == NULL &&
            bucket->childs[2] == NULL &&
            bucket->childs[3] == NULL) {
                if (dike_stack_count(&bucket->items) < count) {
                        dikeDebugf("      bucket: %d, " DIKE_BOUND_FORMAT "", bucket->depth, DIKE_BOUND_FORMAT_ARG(bucket->bound));
                        eitem.depth = depth;
                        eitem.bound = *bound;
                        eitem.data = data;
                        rc = dike_stack_push(&bucket->items, &eitem);
                        if (rc != 0) {
                                dikeErrorf("can not push bound");
                                return -1;
                        }
                        return 0;
                }
        }
        for (i = 0, c = 0, j = 0; j < 4; j++) {
                if (j == 0) {
                        cbound = dike_bound_init(bucket->bound.xmin, bucket->bound.ymin, (bucket->bound.xmin + bucket->bound.xmax) / 2.00, (bucket->bound.ymin + bucket->bound.ymax) / 2.00);
                } else if (j == 1) {
                        cbound = dike_bound_init((bucket->bound.xmin + bucket->bound.xmax) / 2.00, bucket->bound.ymin, bucket->bound.xmax, (bucket->bound.ymin + bucket->bound.ymax) / 2.00);
                } else if (j == 2) {
                        cbound = dike_bound_init(bucket->bound.xmin, (bucket->bound.ymin + bucket->bound.ymax) / 2.00, (bucket->bound.xmin + bucket->bound.xmax) / 2.00, bucket->bound.ymax);
                } else if (j == 3) {
                        cbound = dike_bound_init((bucket->bound.xmin + bucket->bound.xmax) / 2.00, (bucket->bound.ymin + bucket->bound.ymax) / 2.00, bucket->bound.xmax, bucket->bound.ymax);
                } else {
                        dikeErrorf("invalid child");
                        return -1;
                }
                if (dike_bound_contains(&cbound, bound) == 1) {
                        dikeDebugf("    cbound: " DIKE_BOUND_FORMAT "", DIKE_BOUND_FORMAT_ARG(cbound));
                        c = j;
                        i++;
                }
        }
        dikeDebugf("    i: %d, c: %d", i, c);
        if (i == 0) {
                dikeDebugf("      bucket: %d, " DIKE_BOUND_FORMAT "", bucket->depth, DIKE_BOUND_FORMAT_ARG(bucket->bound));
                eitem.depth = depth;
                eitem.bound = *bound;
                eitem.data = data;
                rc = dike_stack_push(&bucket->items, &eitem);
                if (rc != 0) {
                        dikeErrorf("can not push bound");
                        return -1;
                }
        } else if (i == 1) {
                if (bucket->childs[c] == NULL) {
                        if (c == 0) {
                                cbound = dike_bound_init(bucket->bound.xmin, bucket->bound.ymin, (bucket->bound.xmin + bucket->bound.xmax) / 2.00, (bucket->bound.ymin + bucket->bound.ymax) / 2.00);
                        } else if (c == 1) {
                                cbound = dike_bound_init((bucket->bound.xmin + bucket->bound.xmax) / 2.00, bucket->bound.ymin, bucket->bound.xmax, (bucket->bound.ymin + bucket->bound.ymax) / 2.00);
                        } else if (c == 2) {
                                cbound = dike_bound_init(bucket->bound.xmin, (bucket->bound.ymin + bucket->bound.ymax) / 2.00, (bucket->bound.xmin + bucket->bound.xmax) / 2.00, bucket->bound.ymax);
                        } else if (c == 3) {
                                cbound = dike_bound_init((bucket->bound.xmin + bucket->bound.xmax) / 2.00, (bucket->bound.ymin + bucket->bound.ymax) / 2.00, bucket->bound.xmax, bucket->bound.ymax);
                        } else {
                                dikeErrorf("invalid child");
                                return -1;
                        }
                        if (dike_bound_invalid(&cbound)) {
                                dikeErrorf("bound is invalid");
                                return -1;
                        }
                        for (b = 0; b < dike_stack_count(&bucket->items); b++) {
                                eitem = *(struct dike_qtree_item *) dike_stack_at_unsafe(&bucket->items, b);
                                if (dike_bound_contains(&cbound, &eitem.bound) != 1) {
                                        break;
                                }
                        }
                        if (b >= dike_stack_count(&bucket->items)) {
                                dikeDebugf("      all in child");
#if 0
                                eitem.depth = depth;
                                eitem.bound = *bound;
                                eitem.data = data;
                                rc = dike_stack_push(&bucket->items, &eitem);
                                if (rc != 0) {
                                        dikeErrorf("can not push bound");
                                        return -1;
                                }
                                return 0;
#endif
                        }
                        dikeDebugf("      create child: " DIKE_BOUND_FORMAT "", DIKE_BOUND_FORMAT_ARG(cbound));
                        bucket->childs[c] = dike_qtree_bucket_create(&cbound, bucket);
                        if (bucket->childs[c] == NULL) {
                                dikeErrorf("can not push child");
                                return -1;
                        }
                        dikeDebugf("        moving items to child[%d]", c);
                        b = 0;
                        while (dike_stack_count(&bucket->items) - b > 0) {
                                eitem = *(struct dike_qtree_item *) dike_stack_at_unsafe(&bucket->items, dike_stack_count(&bucket->items) - b - 1);
                                dikeDebugf("          check item: %lld, depth: %d", dike_stack_count(&bucket->items) - b - 1, eitem.depth);
                                if (dike_bound_contains(&bucket->childs[c]->bound, &eitem.bound) != 1) {
                                        b++;
                                        continue;
                                }
                                if (eitem.depth <= 0) {
                                        b++;
                                        continue;
                                }
                                dike_stack_del(&bucket->items, dike_stack_count(&bucket->items) - b - 1);
                                eitem.depth -= 1;
                                rc = dike_stack_push(&bucket->childs[c]->items, &eitem);
                                if (rc != 0) {
                                        dikeErrorf("can not push bound");
                                        return -1;
                                }
                        }
                        dikeDebugf("        remaining: %lld, moved to child[%d]: %lld", dike_stack_count(&bucket->items), c, dike_stack_count(&bucket->childs[c]->items));
                }
                return dike_qtree_bucket_add_bound(bucket->childs[c], (depth > 0) ? depth - 1 : depth, count, bound, data);
        } else {
                dikeDebugf("      bucket: %d, " DIKE_BOUND_FORMAT "", bucket->depth, DIKE_BOUND_FORMAT_ARG(bucket->bound));
                eitem.depth = depth;
                eitem.bound = *bound;
                eitem.data = data;
                rc = dike_stack_push(&bucket->items, &eitem);
                if (rc != 0) {
                        dikeErrorf("can not push bound");
                        return -1;
                }
        }
        return 0;
}

static __attribute__ ((__unused__)) void dike_qtree_destroy (struct dike_qtree *qtree)
{
        if (qtree == NULL) {
                return;
        }
        dike_qtree_capture_destroy(qtree);
        dike_qtree_bucket_destroy(qtree->root);
        free(qtree);
}

static __attribute__ ((__unused__)) struct dike_qtree * dike_qtree_create (struct dike_bound *bound, int depth, int count)
{
        struct dike_qtree *qtree;
        qtree = NULL;
        if (bound == NULL) {
                dikeErrorf("bound is invalid");
                goto bail;
        }
        qtree = (struct dike_qtree *) calloc(1, sizeof(struct dike_qtree));
        if (qtree == NULL) {
                dikeErrorf("can not callocate memory");
                goto bail;
        }
        dike_qtree_capture_create(qtree, bound, depth, count);
        qtree->depth = depth;
        qtree->count = count;
        qtree->bound = *bound;
        qtree->root = dike_qtree_bucket_create(bound, NULL);
        if (qtree->root == NULL) {
                dikeErrorf("can not create root");
                goto bail;
        }
        return qtree;
bail:   if (qtree != NULL) {
                dike_qtree_destroy(qtree);
        }
        return NULL;
}

static __attribute__ ((__unused__)) int dike_qtree_add_bound (struct dike_qtree *qtree, struct dike_bound *bound, int depth, void *data)
{
        if (qtree == NULL) {
                dikeErrorf("qtree is invalid");
                goto bail;
        }
        if (bound == NULL) {
                dikeErrorf("bound is invalid");
                goto bail;
        }
        dike_qtree_capture_add_bound(qtree, bound, depth);
        if (depth < 0) {
                depth = qtree->depth;
        }
        if (depth > qtree->depth) {
                depth = qtree->depth;
        }
        dikeDebugf("qtree: " DIKE_BOUND_FORMAT "", DIKE_BOUND_FORMAT_ARG(qtree->bound));
        dikeDebugf("bound: " DIKE_BOUND_FORMAT "", DIKE_BOUND_FORMAT_ARG(*bound));
        {
                int rc;
                struct dike_qtree_item eitem;
                if (dike_bound_contains(&qtree->root->bound, bound) != 1) {
                        dikeDebugf("  add to root");
                        eitem.depth = depth;
                        eitem.bound = *bound;
                        eitem.data = data;
                        rc = dike_stack_push(&qtree->root->items, &eitem);
                        if (rc != 0) {
                                dikeErrorf("can not push bound");
                                return -1;
                        }
                        return 0;
                }
        }
        return dike_qtree_bucket_add_bound(qtree->root, depth, qtree->count, bound, data);
bail:   return -1;
}

static __attribute__ ((__unused__)) int dike_qtree_check_bound (struct dike_qtree *qtree, struct dike_bound *bound, int (*compare) (void *context, dike_bound *bound, void *data, int radius), void *context, int radius)
{
        if (qtree == NULL) {
                dikeErrorf("qtree is invalid");
                goto bail;
        }
        if (bound == NULL) {
                dikeErrorf("bound is invalid");
                goto bail;
        }
        dike_qtree_capture_check_bound(qtree, bound);
        return dike_qtree_bucket_check_bound(qtree->root, bound, compare, context, radius);
bail:   return -1;
}

static __attribute__ ((__unused__)) int dike_qtree_traverse (
                struct dike_qtree *qtree,
                int (*traverse_bucket_start) (void *context, struct dike_qtree_bucket *bucket),
                int (*traverse_item) (void *context, struct dike_qtree_bucket *bucket, struct dike_qtree_item *item),
                int (*traverse_bucket_end) (void *context),
                void *context)
{
        if (qtree == NULL) {
                dikeErrorf("qtree is invalid");
                goto bail;
        }
        if (traverse_bucket_start == NULL &&
            traverse_item == NULL &&
            traverse_bucket_end == NULL) {
                dikeErrorf("traverse is invalid");
                goto bail;
        }
        return dike_qtree_bucket_traverse(qtree->root, traverse_bucket_start, traverse_item, traverse_bucket_end, context);
bail:   return -1;
}

static int qtree_traverse_bucket_start (void *context, struct dike_qtree_bucket *bucket)
{
        (void) context;
        dikeDebugf("  bucket: " DIKE_BOUND_FORMAT "", DIKE_BOUND_FORMAT_ARG(bucket->bound));
        return 0;
}

static int qtree_traverse_item (void *context, struct dike_qtree_bucket *bucket, struct dike_qtree_item *item)
{
        (void) context;
        dikeDebugf("  bucket: " DIKE_BOUND_FORMAT "", DIKE_BOUND_FORMAT_ARG(bucket->bound));
        dikeDebugf("  item  : " DIKE_BOUND_FORMAT "", DIKE_BOUND_FORMAT_ARG(item->bound));
        return 0;
}

static int qtree_traverse_bucket_end (void *context)
{
        (void) context;
        return 0;
}

class DikeCalculateMethodQuadTreePrivate {
public:
        DikeCalculateMethodQuadTreePrivate (void);
        ~DikeCalculateMethodQuadTreePrivate (void);

        int addTrack (DikePath *path);
        int addRecord (DikePath *path);

        int setCoverageRadius (int radius);

        std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>> calculate (void);

private:
        std::vector<DikePath *> _tracks;
        std::vector<DikePath *> _records;

        int _coverageRadius;

        static int DikeCalculateMethodQuadTreePrivateCalculateQuadTreeCompare (void *context, dike_bound *bound, void *data, int radius);
};

DikeCalculateMethodQuadTreePrivate::DikeCalculateMethodQuadTreePrivate (void)
{
}

DikeCalculateMethodQuadTreePrivate::~DikeCalculateMethodQuadTreePrivate (void)
{
        int i;
        int il;
        for (i = 0, il = _tracks.size(); i < il; i++) {
                _tracks[i]->decref();
        }
        for (i = 0, il = _records.size(); i < il; i++) {
                _records[i]->decref();
        }
}

int DikeCalculateMethodQuadTreePrivate::addTrack (DikePath *path)
{
        _tracks.push_back(path->incref());
        return 0;
}

int DikeCalculateMethodQuadTreePrivate::addRecord (DikePath *path)
{
        _records.push_back(path->incref());
        return 0;
}

int DikeCalculateMethodQuadTreePrivate::setCoverageRadius (int coverageRadius)
{
        _coverageRadius = coverageRadius;
        return 0;
}

int DikeCalculateMethodQuadTreePrivate::DikeCalculateMethodQuadTreePrivateCalculateQuadTreeCompare (void *context, dike_bound *bound, void *data, int radius)
{
        double dist;
        DikePoint *tpoint;
        DikePoint *rpoint;

        (void) bound;

        tpoint = (DikePoint *) context;
        rpoint = (DikePoint *) data;

        dist = DikePoint::DikePointDistanceEuclidean(tpoint, rpoint);
        if (dist <= radius) {
                return 1;
        }
        return 0;
}

std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>> DikeCalculateMethodQuadTreePrivate::calculate (void)
{
        int i, il;
        int j, jl;
        int k, kl;
        int l, ll;

        struct dike_qtree *qtree;
        struct dike_bound qbound;

        int pts;
        int mpts;
        int pmpts;

        double dts;
        double mdts;

        DikePath *trackAccepted;
        DikePath *trackRejected;

        pts = 0;
        mpts = 0;
        pmpts = 0;

        dts = 0;
        mdts = 0;

        trackAccepted = NULL;
        trackRejected = NULL;

        qtree = NULL;

        trackAccepted = new DikePath();
        if (trackAccepted == NULL) {
                dikeErrorf("can not create trackAccepted");
                goto bail;
        }
        trackRejected = new DikePath();
        if (trackRejected == NULL) {
                dikeErrorf("can not create trackRejected");
                goto bail;
        }

        dikeDebugf("building qtree");

        qbound = dike_bound_null();
        qbound = dike_bound_union_xy(&qbound, MINLON, MINLAT);
        qbound = dike_bound_union_xy(&qbound, MAXLON, MAXLAT);

        qtree = dike_qtree_create(&qbound, MAXLEVEL, 32);
        if (qtree == NULL) {
                dikeErrorf("can not create qtree");
                goto bail;
        }

        dikeDebugf("building records");
        for (k = 0, kl = _records.size(); k < kl; k++) {
                dikeDebugf("  - %d points", _records[k]->getPointsCount());
                for (l = 0, ll = _records[k]->getPointsCount(); l < ll; l++) {
                        struct dike_bound rbound;
                        std::tuple<DikePath::Command, DikePoint> *rpoint;

                        rpoint = _records[k]->getPoint(l);

                        rbound = dike_bound_null();
                        rbound = dike_bound_union_xy(&rbound, std::get<1>(*rpoint).lon(), std::get<1>(*rpoint).lat());
                        dike_qtree_add_bound(qtree, &rbound, -1, &std::get<1>(*rpoint));
                }
        }

        dikeDebugf("checking tracks");
        for (i = 0, il = _tracks.size(); i < il; i++) {
                dikeDebugf("  - %d points", _tracks[i]->getPointsCount());
                for (j = 0, jl = _tracks[i]->getPointsCount(); j < jl; j++) {
                        int rc;
                        struct dike_bound tbound;
                        std::tuple<DikePath::Command, DikePoint> *tpoint;
                        std::tuple<DikePath::Command, DikePoint> *ppoint;

                        ppoint = (j == 0) ? NULL : _tracks[i]->getPoint(j - 1);
                        tpoint = _tracks[i]->getPoint(j);

                        pts += 1;
                        dts += (ppoint == NULL) ? 0 : DikePoint::DikePointDistanceEuclidean(&std::get<1>(*ppoint), &std::get<1>(*tpoint));

                        DikePoint spoint = DikePoint::DikePointCalculateDerivedPosition(&std::get<1>(*tpoint), _coverageRadius, -180.0);
                        DikePoint wpoint = DikePoint::DikePointCalculateDerivedPosition(&std::get<1>(*tpoint), _coverageRadius, -90.0);
                        DikePoint epoint = DikePoint::DikePointCalculateDerivedPosition(&std::get<1>(*tpoint), _coverageRadius, 90.0);
                        DikePoint npoint = DikePoint::DikePointCalculateDerivedPosition(&std::get<1>(*tpoint), _coverageRadius, 0.0);

                        dikeTracef("- tpoint: %.7f, %.7f", std::get<1>(*tpoint).lon(), std::get<1>(*tpoint).lat());
                        dikeTracef("  spoint: %.7f, %.7f", spoint.lon(), spoint.lat());
                        dikeTracef("  wpoint: %.7f, %.7f", wpoint.lon(), wpoint.lat());
                        dikeTracef("  epoint: %.7f, %.7f", epoint.lon(), epoint.lat());
                        dikeTracef("  npoint: %.7f, %.7f", npoint.lon(), npoint.lat());

                        tbound = dike_bound_null();
                        tbound = dike_bound_union_xy(&tbound, std::get<1>(*tpoint).lon(), std::get<1>(*tpoint).lat());
                        tbound = dike_bound_union_xy(&tbound, spoint.lon(), spoint.lat());
                        tbound = dike_bound_union_xy(&tbound, wpoint.lon(), wpoint.lat());
                        tbound = dike_bound_union_xy(&tbound, epoint.lon(), epoint.lat());
                        tbound = dike_bound_union_xy(&tbound, npoint.lon(), npoint.lat());

                        rc = dike_qtree_check_bound(qtree, &tbound, DikeCalculateMethodQuadTreePrivateCalculateQuadTreeCompare, &std::get<1>(*tpoint), _coverageRadius);
                        if (rc == 1) {
                                if (pmpts) {
                                        mpts += 1;
                                        mdts += (ppoint == NULL) ? 0 : DikePoint::DikePointDistanceEuclidean(&std::get<1>(*ppoint), &std::get<1>(*tpoint));
                                } else if (j == 0) {
                                        mpts += 1;
                                }

                                if (ppoint != NULL) {
                                        trackAccepted->addPoint(DikePath::CommandMoveTo, std::get<1>(*ppoint));
                                        trackAccepted->addPoint(DikePath::CommandLineTo, std::get<1>(*tpoint));
                                }
                        } else {
                                if (ppoint != NULL) {
                                        trackRejected->addPoint(DikePath::CommandMoveTo, std::get<1>(*ppoint));
                                        trackRejected->addPoint(DikePath::CommandLineTo, std::get<1>(*tpoint));
                                }
                        }
                        pmpts = rc;
                }
        }

        dike_qtree_traverse(qtree, qtree_traverse_bucket_start, qtree_traverse_item, qtree_traverse_bucket_end, NULL);

        dike_qtree_destroy(qtree);
        return std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>(0, mpts, pts, mdts, dts, trackAccepted, trackRejected);
bail:
        if (qtree != NULL) {
                dike_qtree_destroy(qtree);
        }
        if (trackAccepted != NULL) {
                trackAccepted->decref();
        }
        if (trackRejected != NULL) {
                trackRejected->decref();
        }
        return std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>(-1, 0, 0, 0, 0, nullptr, nullptr);
}

DikeCalculateMethodQuadTree::DikeCalculateMethodQuadTree (const DikeCalculateMethodOptions &options)
        : DikeCalculateMethod(options)
{
        _private = new DikeCalculateMethodQuadTreePrivate();
        _private->setCoverageRadius(getCoverageRadius());
}

DikeCalculateMethodQuadTree::~DikeCalculateMethodQuadTree (void)
{
        if (_private != NULL) {
                delete _private;
        }
}

int DikeCalculateMethodQuadTree::addTrack (DikePath *path)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->addTrack(path);
bail:   return -1;
}

int DikeCalculateMethodQuadTree::addRecord (DikePath *path)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->addRecord(path);
bail:   return -1;
}

std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>> DikeCalculateMethodQuadTree::calculate (void)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->calculate();
bail:   return std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>(-1, 0, 0, 0, 0, nullptr, nullptr);
}
