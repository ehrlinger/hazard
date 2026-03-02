/*
 * tests/unit/test_harness.h
 *
 * Minimal C test harness — zero external dependencies.
 * Drop-in single-header library for the HAZARD unit test suite.
 *
 * Usage:
 *   Include this header in each test file.
 *   Define test functions with TEST_FN(), run them with RUN_TEST().
 *   Call print_test_results() at the end of main() and return
 *   test_exit_code() so the shell can detect failures.
 */

#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* Shared counters — one per translation unit (static to avoid ODR    */
/* conflicts when multiple test files are compiled into the same bin). */
/* ------------------------------------------------------------------ */
static int _pass_count = 0;
static int _fail_count = 0;

/* ------------------------------------------------------------------ */
/* Assertion macros                                                    */
/* ------------------------------------------------------------------ */

/* Boolean condition */
#define ASSERT_TRUE(cond)                                              \
    do {                                                               \
        if (cond) {                                                    \
            _pass_count++;                                             \
        } else {                                                       \
            fprintf(stderr, "  FAIL [%s:%d] expected true: %s\n",    \
                    __FILE__, __LINE__, #cond);                        \
            _fail_count++;                                             \
        }                                                              \
    } while (0)

/* Floating-point equality within absolute tolerance */
#define ASSERT_EQ_DOUBLE(got, want, tol)                               \
    do {                                                               \
        double _g = (double)(got);                                     \
        double _w = (double)(want);                                    \
        double _t = (double)(tol);                                     \
        if (fabs(_g - _w) <= _t) {                                    \
            _pass_count++;                                             \
        } else {                                                       \
            fprintf(stderr,                                            \
                "  FAIL [%s:%d] %s\n"                                 \
                "         got=%.15g  want=%.15g  tol=%.3g\n",         \
                __FILE__, __LINE__, #got, _g, _w, _t);                \
            _fail_count++;                                             \
        }                                                              \
    } while (0)

/* Exact floating-point equality (tol=0) */
#define ASSERT_EQ_DOUBLE_EXACT(got, want) ASSERT_EQ_DOUBLE(got, want, 0.0)

/* Integer equality */
#define ASSERT_EQ_INT(got, want)                                       \
    do {                                                               \
        int _g = (int)(got);                                           \
        int _w = (int)(want);                                          \
        if (_g == _w) {                                                \
            _pass_count++;                                             \
        } else {                                                       \
            fprintf(stderr, "  FAIL [%s:%d] %s: got=%d, want=%d\n",  \
                    __FILE__, __LINE__, #got, _g, _w);                 \
            _fail_count++;                                             \
        }                                                              \
    } while (0)

/* a < b */
#define ASSERT_LT_DOUBLE(a, b)                                         \
    do {                                                               \
        double _a = (double)(a), _b = (double)(b);                    \
        if (_a < _b) {                                                 \
            _pass_count++;                                             \
        } else {                                                       \
            fprintf(stderr, "  FAIL [%s:%d] expected %s < %s"         \
                    " (got %.15g, %.15g)\n",                           \
                    __FILE__, __LINE__, #a, #b, _a, _b);              \
            _fail_count++;                                             \
        }                                                              \
    } while (0)

/* Value is finite (not NaN, not Inf) */
#define ASSERT_FINITE(x)                                               \
    do {                                                               \
        double _x = (double)(x);                                       \
        if (isfinite(_x)) {                                            \
            _pass_count++;                                             \
        } else {                                                       \
            fprintf(stderr, "  FAIL [%s:%d] %s is not finite"         \
                    " (got %.15g)\n",                                  \
                    __FILE__, __LINE__, #x, _x);                       \
            _fail_count++;                                             \
        }                                                              \
    } while (0)

/* ------------------------------------------------------------------ */
/* Test runner macros                                                  */
/* ------------------------------------------------------------------ */

/* Print a suite header */
#define TEST_SUITE(name)                                               \
    fprintf(stderr, "\n=== %s ===\n", name)

/* Run a named test function (declared as static void name(void)) */
#define RUN_TEST(fn)                                                   \
    do {                                                               \
        fprintf(stderr, "  · %s\n", #fn);                             \
        fn();                                                          \
    } while (0)

/* ------------------------------------------------------------------ */
/* Results reporting                                                   */
/* ------------------------------------------------------------------ */
static void print_test_results(void)
{
    int total = _pass_count + _fail_count;
    fprintf(stderr, "\n----------------------------------------\n");
    fprintf(stderr, "Results: %d/%d passed", _pass_count, total);
    if (_fail_count == 0)
        fprintf(stderr, "  (ALL PASS)\n");
    else
        fprintf(stderr, ", %d FAILED\n", _fail_count);
}

static int test_exit_code(void)
{
    return (_fail_count > 0) ? 1 : 0;
}

#endif /* TEST_HARNESS_H */
