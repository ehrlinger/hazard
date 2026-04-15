/*
 * tests/unit/test_harness_hooks.c
 *
 * Default pre-test hook for the unit test harness.
 * Keep this as a normal (non-weak) definition for portability across
 * toolchains such as MinGW/COFF, where weak symbol behavior can vary.
 */

void hazard_test_pre_run(void)
{
    /* no-op by default */
}

/* Shared test counters (declared extern in test_harness.h). */
int _pass_count = 0;
int _fail_count = 0;
