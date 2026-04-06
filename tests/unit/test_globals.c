/*
 * tests/unit/test_globals.c
 *
 * Allocates ALL global storage required by the tested C modules and
 * provides a minimal initialization routine for unit tests.
 *
 * EXACTLY ONE translation unit in a test binary must include
 * structures.h with STRUCT defined and common.h with HAZARD defined.
 * This is that file.  All other test files treat the globals as extern.
 *
 * Background:
 *   structures.h uses a STRUCT/EXTERN macro pattern: when STRUCT is
 *   defined, EXTERN expands to nothing (defining storage); otherwise it
 *   expands to "extern" (declaring it).  common.h uses HAZARD in the
 *   same way for HZRstr, G, and Params.  The include guards prevent
 *   structures.h from being processed twice even when common.h
 *   re-includes it.
 */

#define STRUCT      /* Allocate: LnLim, Early, TempE, Common, Late, ... */
#include "structures.h"

#define HAZARD      /* Allocate: HZRstr, G, Params, gte, gteva          */
#include "common.h"

#include "hzd_set_Machn.h"
#include "hzd_set_LnLim.h"

/*
 * hazard_test_init()
 *
 * Minimal initialization required before calling any numeric helper
 * from src/common/.  Populates:
 *   Machn  — IEEE 754 machine constants (DBL_MIN, DBL_MAX, epsilon)
 *   LnLim  — Derived log-domain thresholds used by hzd_ln_eX_min1 etc.
 *   Common.errorno — cleared to zero
 *
 * Does NOT initialize HZRstr, Early, G, or the parser state — those
 * are only needed by higher-level functions tested in other test files.
 */
void hazard_test_init(void)
{
    Common.errorno = 0;
    hzd_set_Machn();   /* fills Machn from float.h constants            */
    hzd_set_LnLim();   /* fills LnLim from Machn                        */
}

void hazard_test_pre_run(void)
{
    Common.errorno = 0;
}
