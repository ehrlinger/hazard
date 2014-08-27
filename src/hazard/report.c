#include <hzfpag.h>
#include <hzfxpc.h>
#include <hzfskp.h>

void results(void){
  hzfpag(10);
  hzfxpc("Results:",8,5);
  hzfskp(1);
  hzfxpc("========",8,5);
  hzfskp(2);
  if(H->sw==1 || H->bw==1 || H->f==1) {
    if(H->f==1){
      hzfxpc("Fast Backward Selection",23,53);
    }else if(H->sw==1){
      if(O->nosw){
	hzfxpc("Forward Selection",17,58);
      }else{
	hzfxpc("Forward Stepwise Selection",26,53);
      }
    }else if(H->bw==1){
      if(O->nosw){
	hzfxpc("Backward Selection",18,58);
      }else{
	hzfxpc("Backward Stepwise Selection",27,53);
      }
    }
    hzfskp(2);
    hzfxpc("* During variable selection steps,",34,5);
    hzfxpc(" variances are approximate because",34,0);
    hzfxpc(" shaping parameter covariances are",34,0);
    hzfxpc(" ignored.",9,0);
    hzfskp(2);
  }
}
