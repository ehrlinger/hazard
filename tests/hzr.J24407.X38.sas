(   PROC HAZARD DATA=N1BURDEN CONSERVE OUTHAZ=OUTEST CONDITION=14 MI=200 STEEPEST        QUASI;        EVENT DEAD;        TIME IV_DEAD;        PARMS 
 MUE=1.002958 THALF=3.298245 NU=1 FIXNU M=0 FIXM              MUL=0.7151009 TAU=1 FIXTAU ALPHA=1 FIXALPHA GAMMA=1 FIXGAMMA              ETA=0.7236128 
 WEIBULL ;        EARLY          LN_NPOS=0.4616811, POSMARG=0.6858193, ANYPOST=0.6849007;        LATE          PM1A=1.086815, PM1B=1.564425,  ANYPOST=-2.06235, LN_OPYRS=-0.387502;   )
; OBSCOUNT 243 ;
JOBID J24407;
JOBIX X38;
