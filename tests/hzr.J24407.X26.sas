(    PROC HAZARD DATA=N1BURDEN NOCOV NOCOR CONDITION=14 MI=200;         EVENT DEAD;         TIME IV_DEAD;         PARMS MUE=2.853738 THALF=3.028267 
 NU=1 FIXNU M=0 FIXM               FIXTHALF FIXETA               MUL=0.2754651 TAU=1 FIXTAU ALPHA=1 FIXALPHA GAMMA=1 FIXGAMMA               
 ETA=0.716051 WEIBULL;          RESTRICT POSNODES LN_NPOS IN_NPOS IN2NPOS PCTPOS LN_POS IN_POS;          SELECTION SLE=0.5 SLS=0.10 MAXSTEPS=5 
 ROBUST;         EARLY           MALE/E, FEMALE/E, AGE, LN_AGE, IN_AGE, AGE2, AGEE, BLACK/E, WHITE/E,           PRIOR, PRIOREG,           BARR, PT, 
 PT_1, PT_1IM, PT_1SM, PT_2, PT_3, PT_4, LN_PT, IN_PT, IN2PT,        POSNODES, LN_NPOS, IN_NPOS, IN2NPOS, PCTPOS, LN_POS, IN_POS/S,        IN2_POS, 
 SQ,        AD, SIG/E, PM1A, PM1B, PM, CASTAGE, LN_STAGE, IN_STAGE, ST2B, ST3, ST4A,        ST4B, POSMARG/S, DIFF, LN_DIFF, IN_DIFF,           
 ANAS_N, APPR_CH, APPR_TH,           ANYPOST/S,           OPYRS, LN_OPYRS, IN_OPYRS, IN2OPYRS, OPYRS2         ;         LATE           MALE/E, 
 FEMALE/E, AGE, LN_AGE, IN_AGE, AGE2, AGEE, BLACK/E, WHITE/E,           PRIOR, PRIOREG,           BARR, PT, PT_1, PT_1IM, PT_1SM, PT_2, PT_3, PT_4, 
 LN_PT, IN_PT, IN2PT,        POSNODES, LN_NPOS, IN_NPOS, IN2NPOS, PCTPOS, LN_POS, IN_POS, IN2_POS, SQ,        AD, SIG/E, PM1A/S, PM1B/S, PM, CASTAGE, 
 LN_STAGE, IN_STAGE, ST2B, ST3,        ST4A, ST4B, POSMARG/S, DIFF, LN_DIFF, IN_DIFF,           ANAS_N, APPR_CH, APPR_TH,           ANYPOST/S,            OPYRS, LN_OPYRS/S, IN_OPYRS, IN2OPYRS, OPYRS2         ;   )
; OBSCOUNT 243 ;
JOBID J24407;
JOBIX X26;
