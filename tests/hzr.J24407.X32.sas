(   PROC HAZARD DATA=N1BURDEN CONSERVE OUTHAZ=OUTEST CONDITION=14 MI=200;        EVENT DEAD;        TIME IV_DEAD;        PARMS MUE=4.56392 
 THALF=3.446444 NU=1 FIXNU M=0 FIXM              MUL=0.6741454 TAU=1 FIXTAU ALPHA=1 FIXALPHA GAMMA=1 FIXGAMMA              ETA=0.7069431 WEIBULL ;    
     EARLY          IN_POS=-0.127405, POSMARG=0.5395116, ANYPOST=0.5994018;        LATE          PM1A=1.074573, PM1B=1.485148, ANYPOST=-2.01541,  LN_OPYRS=-0.365839;   )
; OBSCOUNT 243 ;
JOBID J24407;
JOBIX X32;
