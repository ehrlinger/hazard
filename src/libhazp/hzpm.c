#include "hazpred.h"
#include "hzd_Error.h"

/* #define DEBUG 1 */
/*
  hzpm - Read the actual Covariance matrix from the xport data set.

  Used by: file reader.

  Uses:
  hzd_Error

  Globals:
  Common.Nobs
  obsCnt
  Common.Ntheta
  Common.p
  Common.status
  Intfc.malloc((size_t)Common.Ntheta*sizeof(short int));
  Common.theta
  Common.obs
  HZPstr.Sflag
  dCf
  HZPstr.Hflag
  dHf
  Common.cov

  Passed: obsCnt

  Returns: 
*/
void hzpm(long int obsCnt){
#ifdef DEBUG
  printf("hzpm %d\n", obsCnt);
#endif /*DEBUG*/


  Common.Nobs = obsCnt;
  /*
   * The first 6 observations are always  
   * G1FLAG, FIXDEL0, FIXMNU1, G3FLAG, FIXGE2, FIXGAE2
   */

  Common.Ntheta = obsCnt-6;
  /* Followed by 8 shaping parameters
   * DELTA, THALF, NU, M, TAU, GAMMA, ALPHA, ETA
   */
  Common.p = (Common.Ntheta-11)/3;

  /* Check the size of P calculation.. of course it 
     better be right */
  if((Common.p*3+11)!=Common.Ntheta){
    printf("NTheta: %d\tP: %d\t3 p + 11 = %d\n", Common.Ntheta, Common.p, Common.p*3+11);
    hzd_Error("HZPM ",1);

  }
  /* Allocate some memory */

  Common.status = Intfc.malloc((size_t)Common.Ntheta*
			       sizeof(short int));
  Common.theta = Intfc.malloc((size_t)Common.Ntheta*sizeof(double));
  Common.obs = Intfc.malloc((size_t)(Common.Ntheta)*sizeof(double));

  if(HZPstr.Sflag)
    dCf = Intfc.malloc((size_t)Common.Ntheta*sizeof(double));

  if(HZPstr.Hflag)
    dHf = Intfc.malloc((size_t)Common.Ntheta*sizeof(double));

  Common.cov = Intfc.malloc((size_t)(Common.Ntheta*Common.Ntheta)*
			    sizeof(double));
}
