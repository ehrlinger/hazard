#include <string.h>
#include "hazpred.h"
void hzp_calc_survival(void){
  cumHaz = Cf[0]+Cf[1]+Cf[2];
  if((-cumHaz)<LnLim.absMin)
    HZPstr.Surviv = ZERO;
  else
    HZPstr.Surviv = ePOW(-cumHaz);
  if((-Cf[0])<LnLim.absMin)
    HZPstr.earlyS = ZERO;
  else
    HZPstr.earlyS = ePOW(-Cf[0]);
  if((-Cf[1])<LnLim.absMin)
    HZPstr.constS = ZERO;
  else
    HZPstr.constS = ePOW(-Cf[1]);
  if((-Cf[2])<LnLim.absMin)
    HZPstr.lateS = ZERO;
  else
    HZPstr.lateS = ePOW(-Cf[2]);
}
