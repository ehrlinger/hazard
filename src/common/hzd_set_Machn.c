#include "structures.h"


#ifdef HAVE_CONFIG_H
/* include Autoconf-generated header */
#include "config.h"
#endif
#include <float.h>

void hzd_set_Machn(void){
#ifdef DEBUG
     printf("hzd_set_Machn\n");
#endif /*DEBUG*/

  /* Not sure why we need these */
  Machn.zero = 0.0e0;
  Machn.half = 0.5e0;
  Machn.one = 1.0e0;
  Machn.two = 2.0e0;

  /* These are defined in float.h */
  Machn.absMin = DBL_MIN;
  Machn.absMax = DBL_MAX;
  Machn.negMax = -DBL_MAX;
  Machn.Eps = DBL_EPSILON;

  /* Where are these from? */
  Machn.norinv.C0 = 2.515517e0;
  Machn.norinv.C1 = 0.802853e0;
  Machn.norinv.C2 = 0.010328e0;
  Machn.norinv.D1 = 1.432788e0;
  Machn.norinv.D2 = 0.189269e0;
  Machn.norinv.D3 = 0.001308e0;
}
