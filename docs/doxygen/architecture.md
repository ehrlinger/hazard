@page architecture Developer Architecture Guide

## Global State and the STRUCT/HAZARD Macro Pattern

All model state lives in a set of global C structs declared in
`src/common/structures.h`. The `#ifdef STRUCT` / `#define EXTERN`
pattern ensures exactly one translation unit allocates storage:

~~~c
// In exactly ONE .c file (hazard.c or hazpred.c):
#define STRUCT
#include "structures.h"   // allocates storage for all global structs

// In all other .c files:
#include "structures.h"   // gets extern declarations only
~~~

Key global structs:

| Struct | Global variable | Purpose |
|--------|----------------|---------|
| `struct early` | `Early` | Early phase parameters (delta, tHalf, nu, m, rho) |
| `struct late` | `Late` | Late phase parameters (tau, gamma, alpha, eta, muL) |
| `struct common` | `Common` | Run-wide state (errtrap, status[], errorno, LnLim, Machn) |
| `struct hazG1` | `HazG1` | Per-call G1 inputs/outputs (T, lnG1, lnSG1, g1flag) |
| `struct hazG3` | `HazG3` | Per-call G3 inputs/outputs |
| `struct opt` | (in HZRstr) | Optimizer state (itrcnt, nfncts, condition) |

The `#ifdef HAZARD` / `common.h` pair provides a parallel mechanism for
hazard-specific global variables.

## Error Handling — setjmp/longjmp Pattern

Errors propagate via `setjmp`/`longjmp` rather than return codes.
This allows numerical subroutines deep in the call stack to abort
without threading error returns through every caller.

~~~c
// At program start (in hazard.c or hazpred.c main()):
if (setjmp(Common.errtrap) != 0) {
    // Landed here from hzd_Error() deep in the call stack.
    // Common.errorno holds the error code.
    // Clean up and exit — never returns normally from this branch.
    hzd_exit(Common.errorno);
}

// Deep inside a numerical subroutine:
if (x <= 0.0) {
    hzd_Error("DLG1900", 2);  // longjmps to errtrap; never returns
}
~~~

`ErrorPassBack(code)` is used when a function returns a value and
cannot safely call `hzd_Error()` directly; it sets `Common.errorno`
and returns a sentinel, expecting the caller to check and propagate.

See hzd_Error.h and hzd_exit_codes.h for the error code catalogue.

## OBS Array Layout

Patient observation data is stored in a flat 1-D `double` array with
`pp7 = 7` values per row (one row per patient):

~~~c
// Access row I (1-based), column J (1-based):
OBS[I * pp7 - pp7 + J - 1]

// Shorthand via Im1p7 precomputed in the outer loop:
Im1p7 = I * pp7 - 1;
OBS[Im1p7 + J]    // J = 1..7
~~~

Column layout:

| Column J | Macro | Meaning |
|---|---|---|
| 1 | TIME | Event or censoring time |
| 2 | EVENT | 1 = observed event, 0 = censored |
| 3 | LCENSOR | Interval censoring left endpoint |
| 4 | RCENSOR | Right censoring flag |
| 5 | WEIGHT | Observation weight |
| 6 | ID | Patient identifier |
| 7 | STRATA | Stratum membership |

## Mathematical Prerequisites

Before calling any math helper or shaping function, the global
precision constants must be initialized:

~~~c
hzd_set_Machn();   // sets Common.Machn (machine epsilon)
hzd_set_LnLim();   // sets Common.LnLim (log underflow threshold)
~~~

These are called once at program startup in `hzd_init_Common()`.
