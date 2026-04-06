/*
 * tests/unit/test_harness_hooks.c
 *
 * Default pre-test hook for the unit test harness.
 * This weak no-op is linked into all test binaries so RUN_TEST()
 * can safely call hazard_test_pre_run() even when a suite does not
 * use global HAZARD state.
 */

#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void hazard_test_pre_run(void)
{
    /* no-op by default */
}
