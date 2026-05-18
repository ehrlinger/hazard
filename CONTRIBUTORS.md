# Contributors

HAZARD is the product of decades of work across biostatistics, cardiac surgery,
and software engineering. This file lists everyone who contributed to the
mathematical theory, FORTRAN implementation, or C port.

---

## Principal Investigator

**Eugene H. Blackstone, M.D.**
Department of Thoracic and Cardiovascular Surgery, Cleveland Clinic Foundation.
Head of the group that owns HAZARD and guiding force throughout the entire
history of the codebase, from inception in 1985 to the present. Primary FORTRAN
implementer (1985–1986): authored the math helper subroutines (`DLEXM1`,
`DLEXP1`, `DL1MEX`, `DLGAOB`, `DTRSFM`, and others), the optimizer interface,
and successive revisions through 1996.

---

## Mathematical Model

**Malcolm E. Turner Jr., Ph.D.**
Department of Biomathematics and Biostatistics, University of Alabama at Birmingham.
Originator of the three-phase parametric hazard model and the Conservation of Events
theorem (notes of February 14, 1985). Revisions and corrections through 1994.

**David C. Naftel, M.S.**
Department of Surgery, University of Alabama at Birmingham.
Co-developer of the decomposition of time-varying hazard into early, constant,
and late phases. Co-author: Blackstone EH, Naftel DC, Turner ME Jr.,
"The decomposition of time-varying hazard into phases, each incorporating a
separate stream of concomitant information." *J Am Stat Assoc* 1986;81:615–624.
Original C port of the FORTRAN implementation.

---

## FORTRAN Implementation (1985–1996)

**Charles R. Katholi, Ph.D.**
Department of Biostatistics, University of Alabama at Birmingham.
FORTRAN 77 translation of the original shaping function and math routines;
numerical analyst for the optimizer library (UNCMIN adaptation, Cholesky
decomposition, BFGS quasi-Newton). Implementer of machine-precision routines
(`MACHIN`, `CHLSKM`, `CHOSLV`).

**R. L. Kornegay**
Stepwise and backward variable selection routines (`STEPW`, `BACKW`, `DQSTAT`),
written October 1985, revised May 1986 and July 1987.

**L. Manderson**
Additional stepwise variable selection support routines (`SWVARI`, `SWVARQ`, `SWVARX`).

---

## Modernization (2000–present)

**John Ehrlinger, Ph.D.**
Heart, Vascular and Thoracic Institute, Cleveland Clinic Foundation.
Autotools build system (v4.1.0, 2000). Subsequent development: CI infrastructure
(GitHub Actions), corpus test suite, Doxygen documentation, telemetry, XPORT V8
detection, static build support, and all releases from v4.1.0 through v4.4.x.
