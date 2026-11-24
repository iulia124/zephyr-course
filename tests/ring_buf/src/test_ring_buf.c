/*
 * Ring Buffer Module - Homework Test Skeleton
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"


/*
 * Shared before hook:
 * reinitialise the ring buffer with capacity 4 before every test.
 */
static void before(void *f)
{
    ARG_UNUSED(f);
    rb_init(4);
}


/*
 * ============================================================================
 * Test Suite: ring_buf_init
 * ============================================================================
 */

ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);


/* Provided example */
ZTEST(ring_buf_init, test_fresh_state)
{
    zassert_true(rb_is_empty(),
                 "Fresh buffer must be empty");

    zassert_equal(rb_count(), 0,
                  "Fresh buffer count must be 0");
}


/* Reinitialisation must clear the previous state */
ZTEST(ring_buf_init, test_reinit_clears_state)
{
    zassert_ok(rb_push(99),
               "Push should succeed");

    zassert_equal(rb_count(), 1,
                  "Buffer should contain one item");

    zassert_ok(rb_init(4),
               "Reinitialization should succeed");

    zassert_true(rb_is_empty(),
                 "Buffer must be empty after reinit");

    zassert_equal(rb_count(), 0,
                  "Buffer count must be 0 after reinit");
}


/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 * ============================================================================
 */

ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);


/* Single push/pop */
ZTEST(ring_buf_push_pop, test_single_push_pop)
{
    int value;

    zassert_ok(rb_push(42),
               "Push should succeed");

    zassert_ok(rb_pop(&value),
               "Pop should succeed");

    zassert_equal(value, 42,
                  "Popped value must be 42");

    zassert_true(rb_is_empty(),
                 "Buffer must be empty after pop");
}


/* FIFO order: 1, 2, 3 must come out in the same order */
ZTEST(ring_buf_push_pop, test_fifo_order)
{
    int value;

    zassert_ok(rb_push(1), "Push 1 should succeed");
    zassert_ok(rb_push(2), "Push 2 should succeed");
    zassert_ok(rb_push(3), "Push 3 should succeed");

    zassert_ok(rb_pop(&value),
               "First pop should succeed");
    zassert_equal(value, 1,
                  "First value must be 1");

    zassert_ok(rb_pop(&value),
               "Second pop should succeed");
    zassert_equal(value, 2,
                  "Second value must be 2");

    zassert_ok(rb_pop(&value),
               "Third pop should succeed");
    zassert_equal(value, 3,
                  "Third value must be 3");

    zassert_true(rb_is_empty(),
                 "Buffer must be empty after all pops");
}


/* Pushing past capacity must return -ENOSPC */
ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
    zassert_ok(rb_push(1), "Push 1 should succeed");
    zassert_ok(rb_push(2), "Push 2 should succeed");
    zassert_ok(rb_push(3), "Push 3 should succeed");
    zassert_ok(rb_push(4), "Push 4 should succeed");

    zassert_true(rb_is_full(),
                 "Buffer must be full");

    zassert_equal(rb_push(99), -ENOSPC,
                  "Push on full buffer must return -ENOSPC");

    zassert_equal(rb_count(), 4,
                  "Rejected push must not change count");
}


/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 * ============================================================================
 */

ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);


/* Peek must not remove the value */
ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
    int value;

    zassert_ok(rb_push(7),
               "Push should succeed");

    zassert_ok(rb_peek(&value),
               "First peek should succeed");

    zassert_equal(value, 7,
                  "Peeked value must be 7");

    zassert_ok(rb_peek(&value),
               "Second peek should succeed");

    zassert_equal(value, 7,
                  "Second peek must still return 7");

    zassert_equal(rb_count(), 1,
                  "Peek must not consume the item");
}


/* NULL output pointer must return -EINVAL */
ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
    zassert_equal(rb_pop(NULL), -EINVAL,
                  "rb_pop(NULL) must return -EINVAL");
}


/* Buffer must report full after four pushes */
ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
    zassert_ok(rb_push(1), "Push 1 should succeed");
    zassert_ok(rb_push(2), "Push 2 should succeed");
    zassert_ok(rb_push(3), "Push 3 should succeed");
    zassert_ok(rb_push(4), "Push 4 should succeed");

    zassert_true(rb_is_full(),
                 "Buffer must report full");

    zassert_equal(rb_count(), 4,
                  "Full buffer count must be 4");
}