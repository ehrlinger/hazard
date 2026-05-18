/**
 * @file hzd_early_p2t.c
 * @brief Early-phase model parameters → unconstrained optimizer theta[].
 *
 * Maps (DELTA, THALF, NU, M) → theta[0..3] for the BFGS optimizer.
 * The delta transform is \f$\theta_0 = \ln(-\ln|\delta|)\f$, chosen in
 * October 1986 for better likelihood contour behavior over the
 * earlier \f$\ln(\delta)\f$ form.
 *
 * **FORTRAN origin:** `model91/dtrsfm` IOP=1 (E.H. Blackstone / C.R. Katholi,
 * November 1985; revised October 1986).
 *
 * @see hzd_early_p2t.h
 * @see hzd_early_t2p.c for the inverse.
 */
#include <string.h>
#include "structures.h"
#include "hzd_Error.h"

void hzd_early_p2t(double *sTheta){
  double absDelta;

  if(Common.status[0]==1) {
    absDelta = fabs(Early.delta);
    if(absDelta>=ONE)
      hzd_Error("DTRSFM910",2);
    else
      sTheta[0] = LOGe(-LOGe(absDelta));
  }
  if(Common.status[1]==1){
    if(Early.tHalf<=ZERO){
      hzd_Error("DTRSFM930",2);
    }else{
      sTheta[1] = LOGe(Early.tHalf);
    }
  }
  if(Common.status[2]==1)
    sTheta[2] = LOGe(fabs(Early.nu));
  if(Common.status[3]==1)
    sTheta[3] = LOGe(fabs(Early.m));
}
