@mainpage HAZARD — Three-Phase Parametric Hazard Analysis

HAZARD is a parametric survival analysis system implementing
the three-phase hazard model of Blackstone, Naftel, and Turner
(1986). It fits time-varying hazard functions composed of:

- **Early phase** — rapidly decelerating initial hazard (G1 shaping function)
- **Constant phase** — time-independent background hazard
- **Late phase** — slowly accelerating chronic hazard (G3 shaping function)

The binary (`hazard`) reads a PROC HAZARD statement from stdin and
an XPORT-format dataset, fits the model by maximum likelihood, and
writes a formatted listing to stdout.

## Key References

- Blackstone EH, Naftel DC, Turner ME Jr. "The decomposition of
  time-varying hazard into phases, each incorporating a separate
  stream of concomitant information." *J Am Stat Assoc* 1986;81:615-624.
- Turner ME Jr., et al. *Survival Analysis: A Practical Approach*. 1994.

## Navigation

- @ref architecture "Developer Architecture" — global state, error handling,
  STRUCT/HAZARD macro pattern, OBS array layout
- **Mathematical Core** — G1/G3 shaping functions (hzd_ln_G1_and_SG1.h,
  hzd_ln_G3_and_SG3.h), transforms (hzd_early_p2t.h, hzd_late_p2t.h),
  log-likelihood (setlik.h, setcoe.h)
- **I/O Boundary** — XPORT reader (xportHandler.h), version detection
  (xport_version.h), exit codes (hzd_exit_codes.h)

## Relationship to TemporalHazard (R port)

The R package [TemporalHazard](https://github.com/ehrlinger/TemporalHazard)
is a faithful port of the C numerical core. Function correspondence:

| C function | R equivalent |
|---|---|
| `hzd_ln_G1_and_SG1()` | `hzd_G1()` |
| `hzd_ln_G3_and_SG3()` | `hzd_G3()` |
| `hzd_early_p2t()` / `hzd_early_t2p()` | `early_p2t()` / `early_t2p()` |
| `hzd_late_p2t()` / `hzd_late_t2p()` | `late_p2t()` / `late_t2p()` |
| `setlik()` | `loglik()` |

## FORTRAN Provenance

The C code was mechanically translated from a well-commented FORTRAN implementation
(Turner et al., 1991). Mathematical documentation for each function traces to the
original FORTRAN comments. See `docs/fortran-c-mapping.md` for the full
FORTRAN subroutine → C function cross-reference.
