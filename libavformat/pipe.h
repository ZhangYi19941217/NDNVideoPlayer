#ifndef PIPE_H_
#define PIPE_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define PURE                __attribute__((pure))
#define MALLOC_LIKE         __attribute__((malloc))
#define NO_NULL_POINTERS    __attribute__((nonnull))
#define WARN_UNUSED_RESULT  __attribute__((warn_unused_result))
#else
/* Feel free to fill in results for more compilers =) */
#define PURE
#define MALLOC_LIKE
#define NO_NULL_POINTERS
#define WARN_UNUSED_RESULT
#endif

typedef struct pipe_t          pipe_t;
typedef struct pipe_producer_t pipe_producer_t;
typedef struct pipe_consumer_t pipe_consumer_t;
typedef struct pipe_generic_t  pipe_generic_t;

#define PIPE_GENERIC(handle) ((pipe_generic_t*)(handle))

/*
 * Initializes a new pipe storing elements of size `elem_size'. A pusher handle
 * is returned, from which you may push elements into the pipe.
 *
 * If `limit' is 0, the pipe has no maximum size. If it is nonzero, the pipe
 * will never have more than `limit' elements in it at any time. In most cases,
 * you want this to be 0. However, limits help prevent an explosion of memory
 * usage in cases where production is significantly faster than consumption.
 */
pipe_t* MALLOC_LIKE WARN_UNUSED_RESULT pipe_new(size_t elem_size, size_t limit);

/*
 * Makes a production handle to the pipe, allowing push operations. This
 * function is extremely cheap; it doesn't allocate memory.
 */
pipe_producer_t* NO_NULL_POINTERS WARN_UNUSED_RESULT pipe_producer_new(pipe_t*);

/*
 * Makes a consumption handle to the pipe, allowing pop operations. This
 * function is extremely cheap; it doesn't allocate memory.
 */
pipe_consumer_t* NO_NULL_POINTERS WARN_UNUSED_RESULT pipe_consumer_new(pipe_t*);



size_t NO_NULL_POINTERS WARN_UNUSED_RESULT pipe_used_size(pipe_t*);

/*
 * If you call *_new, you must call the corresponding *_free. Failure to do so
 * may result in resource leaks, undefined behavior, and spontaneous combustion.
 */

void pipe_free(pipe_t*);
void pipe_producer_free(pipe_producer_t*);
void pipe_consumer_free(pipe_consumer_t*);

/* Copies `count' elements from `elems' into the pipe. */
void NO_NULL_POINTERS pipe_push(pipe_producer_t*, const void* elems, size_t count);

/*
 * Copies `count' elements from `elems' into the pipe.
 *
 * WARNING: You probably want pipe_push.
 *
 * If the pipe is full, this version of pipe_push will automatically pop enough
 * elements from the front of the queue to make room for the new elements in
 * the pipe.
 *
 * Disabled unless someone tells me they want it. Enabling this function voids
 * your warranty.
 */
/*
void NO_NULL_POINTERS pipe_push_clobber(pipe_producer_t*,
                                        const void* elems, size_t count);
*/

/*
 * Tries to pop `count' elements out of the pipe and into `target', returning
 * the number of elements successfully copied. This function will block until:
 *
 * a) `target' has been filled with `count' elements, or
 * b) all producer_t handles have been freed (including the parent pipe_t).
 *
 * If this function returns 0, there will be no more elements coming in. Every
 * subsequent call will return 0.
 *
 * The difference between this function and pipe_pop_eager is that this one will
 * do its best to fill `target' before returning whereas pipe_pop_eagar will
 * return as soon as any elements are available.
 */
size_t NO_NULL_POINTERS WARN_UNUSED_RESULT pipe_pop(pipe_consumer_t*,
                                                    void* target,
                                                    size_t count);

/*
 * Tries to pop `count' elements out of the pipe and into `target', returning
 * the number of elements successfully copied. This function will block until:
 *
 * a) there is at least one element in the pipe.
 * b) all producer_t handles have been freed (including the parent pipe_t).
 *
 * If this function returns 0, there will be no more elements coming in. Every
 * subsequent call will return 0.
 *
 * The difference between this function and pipe_pop is that this one will
 * return as soon as any elements are available, whereas pipe_pop will do its
 * best to fill `target' first.
 */
size_t NO_NULL_POINTERS WARN_UNUSED_RESULT pipe_pop_eager(pipe_consumer_t*,
                                                          void* target,
                                                          size_t count);

/*
 * Modifies the pipe to have room for at least `count' elements. If more room
 * is already allocated, the call does nothing. This can be useful if requests
 * tend to come in bursts.
 *
 * The default minimum is 32 elements. To reset the reservation size to the
 * default, set count to 0.
 */
void NO_NULL_POINTERS pipe_reserve(pipe_generic_t*, size_t count);

/*
 * Determines the size of a pipe's elements. This can be used for generic
 * pipe-processing algorithms to reserve appropriately-sized buffers.
 */
size_t PURE NO_NULL_POINTERS pipe_elem_size(pipe_generic_t*);


/*
 * Use this to run the pipe self-test. It will call abort() if anything is
 * wrong. This is usually unnecessary. If this is never called, pipe_test.c
 * does not need to be linked.
 */
void pipe_run_test_suite(void);

#undef WARN_UNUSED_RESULT
#undef NO_NULL_POINTERS
#undef MALLOC_LIKE
#undef PURE

#ifdef __cplusplus
}
#endif

#endif
