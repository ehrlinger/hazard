  %macro vars(in=n1burden, out=n1burden, impute=0, missing=0);
*______________________________________________________________________________;
*                                                                              ;
* Macro to bring in variables for analysis, including informative and non-     ;
* informative missing value substitution if desired, and common variables      ;
* transformations.                                                             ;
*______________________________________________________________________________;
*                                                                              ;
* Definitions of Calling Arguments:                    Default Value
*
*   in       = Input data set name                        n1burden
*   out      = Output data set name                       n1burden
*   missing  = Apply standardization for missing values   0
*   impute   = Informative imputing for missing values    0
*   offset   = Scaling and/or offset for stability        0
*______________________________________________________________________________;
*                                                                              ;
* Local macros for error-checking logs and inverses                            ;
*______________________________________________________________________________;
*                                                                              ;
* Logarithmic:                                                                 ;
  %macro ln(var,ln_var);
    &ln_var=.; if (&var)>0 then &ln_var=log(&var);
  %mend ln;
*******************************************************************************;
* Inverse:                                                                     ;
  %macro in(var,in_var);
    &in_var=.; if (&var)>0 then &in_var=1/(&var);
  %mend in;
*______________________________________________________________________________;
*                                                                              ;
  data &out; set &in;
*______________________________________________________________________________;
*                                                                              ;
* Manage missing values                                                        ;
*______________________________________________________________________________;
*                                                                              ;
* Missing value flags                                                          ;
*______________________________________________________________________________;
*                                                                              ;
* Data rescaling                                                               ;
*______________________________________________________________________________;
*                                                                              ;
  mets=m; drop m; /* seems to mess up the saved hazard, because it has an m */
  nod=n;  drop n; /* seems to mess up the bootstrap that uses n */
*______________________________________________________________________________;
*                                                                              ;
* New variables                                                                ;
*______________________________________________________________________________;
*                                                                              ;
* Demography                                                                   ;
  age=(opdate - dob)/365.2425;
  male=(gender='M'); if gender=' ' then male=.;
  female=(gender='F'); if gender=' ' then female=.;
  black=(race='B'); if race=' ' then black=.;
  white=(race='W'); if race=' ' then white=.;

  label
    age      = 'Age at operation'
    male     = 'Man'
    female   = 'Woman'
    black    = 'Ethnicity: Black'
    white    = 'Ethnicity: White'
  ;
*******************************************************************************;
* Preoperative factors                                                         ;
  if preadj=0 then trt='surg';
  if preadj>0 then trt='IT';
  it=(trt='IT');         if trt=' ' then it=.;
  prior=(psurg='Y');     if psurg=' '  then prior=.;
  prioreg=(egsurg='Y');  if egsurg=' ' then prioreg=.;

  label
    it       = 'Preoperative induction therapy'
    prior    = 'Prior surgery'
    prioreg  = 'Prior esophageal-gastric surgery'
  ;
*******************************************************************************;
* Preoperative esophageal ultrasound (EUS)                                     ;
  ceus=(eus='Y');
  ct_0=0; ct_hgd=0; ct_1=0; ct_2=0; ct_3=0; ct_4=0;
  if t_eus='0' then do; ct=1; ct_0=1; end;              /* cT0           */
    else if t_eus='IS' then do; ct=2; ct_hgd=1; end;    /* cTis or cHGD  */
    else if t_eus='1' then do; ct=3; ct_1=1; end;       /* cT1           */
    else if t_eus='2' then do; ct=4; ct_2=1; end;       /* cT2           */
    else if t_eus='3' then do; ct=5; ct_3=1; end;       /* cT3           */
    else if t_eus='4' then do; ct=6; ct_4=1; end;       /* cT4           */
  ct_gr=ct; if ct=6 then ct_gr=ct_gr-1; if ct>1 then ct_gr=ct_gr-1;
  ct_0hgd=max(ct_0, ct_hgd);
  ct_low=ct<5;
  ct_hi=(ct ge 5);
  if t_eus=' ' or t_eus='X' then do;
    ct=.; ct_gr=.; ct_hgd=.; ct_0=.; ct_1=.; ct_2=.; ct_3=.; ct_4=.;
    ct_0hgd=.; ct_low=.; ct_hi=.;
  end;

  cn1=(n_eus='1'); if n_eus=' ' or n_eus='X' then cn1=.;

  csx=0; cs0=0; cs1=0; cs2a=0; cs2b=0; cs3=0;
    if t_eus='0' and n_eus='0' then do; cstage=1; csx=1; end;
    else if t_eus='IS' and n_eus='0' then do; cstage=2; cs0=1; end;
    else if t_eus='1' and n_eus='0' then do; cstage=3; cs1=1; end;
    else if t_eus in ('2','3') and n_eus='0' then do; cstage=4; cs2a=1; end;
    else if t_eus in ('0','1','2') and n_eus='1' then do; cstage=5; cs2b=1; end;
    else if (t_eus='3' and n_eus='1') or t_eus='4' then do; cstage=6; cs3=1; end;
  cstgx2a=cstage; if cstage<3 then cstgx2a=3; cstgx2a=cstgx2a-2;
  if t_eus=' ' or n_eus=' ' or t_eus='X' or n_eus='X' then do;
    cstage=.; cstgx2a=.; csx=.; cs0=.; cs1=.; cs2a=.; cs2b=.; cs3=.;
  end;

  label
    ceus     = 'Clinical: ultrasound study'
    ct       = 'Clinical: depth of invasion (1-6)'
    t_eus    = 'Clinical: depth of invasion categories'
    ct_0     = 'Clinical: cT0 (none)'
    ct_hgd   = 'Clinical: cTis (HGD)'
    ct_1     = 'Clinical: cT1'
    ct_2     = 'Clinical: cT2'
    ct_3     = 'Clinical: cT3'
    ct_4     = 'Clinical: cT4'
    cn1      = 'Clinical: N1 disease'
    csx      = 'Clinical: stage X'
    cs0      = 'Clinical: stage 0'
    cs1      = 'Clinical: stage I'
    cs2a     = 'Clinical: stage IIA'
    cs2b     = 'Clinical: stage IIB'
    cs3      = 'Clinical: stage III'
    cstage   = 'Clinical: stage'
    cstgx2a  = 'Clinical: stage, combine X, I, IIA'
  ;
*******************************************************************************;
* Surgical variabels                                                           ;
  surgonly=0;  if trt='surg' then surgonly=1;
  anas_n=(app1=5 or app2=5 or app3=5);
  appr_ch=((app1 in (1,2)) or (app2 in (1,2)) or (app3 in (1,2)));
  appr_th=(app1=3 or app2=3 or app3=3);
  if app1=. and app2=. and app3=. then do; appr_ch=.; appr_th=.; end;

  label
    surgonly = 'Surgery: no prior induction therapy'
    anas_n   = 'Surgery: neck anastomosis'
    appr_th  = 'Surgery: transhiatal'
    appr_ch  = 'Surgery: transthoracic'
  ;
*******************************************************************************;
* Cancer pathology                                                             ;
  barr=(barretts='Y');
  pt_0=0; pt_hgd=0; pt_1=0; pt_1im=0; pt_1sm=0; pt_2=0; pt_3=0; pt_4=0;
  pt=.;
  if t='0' then do; pt=1; pt_0=1; end;
    else if t='0.5' then do; pt=2; pt_hgd=1; end;
    else if t='1im' then do; pt=3; pt_1im=1; pt_1=1; end;
    else if t='1sm' then do; pt=4; pt_1sm=1; pt_1=1; end;
    else if t='2' then do; pt=5; pt_2=1; end;
    else if t='3' then do; pt=6; pt_3=1; end;
    else if t='4' then do; pt=7; pt_4=1; end;

  pctpos=.; if totnodes>0 then pctpos = 100*(posnodes/totnodes);
    if totnodes=0 then posnodes=.;
  pn1 = (n='1'); if n=' ' then pn1=.;
    if pn1=0 then do; pctpos=0; posnodes=0; end;

  pm1a=(m='1A');  pm1b=(m='1B'); pm=(pm1a or pm1b);
    if m=' ' then do;  pm1a=.;  pm1b=.;  pm=.; end;

  st0=(stage='0');  st1=(stage='1');  st2a=(stage='2A');  st2b=(stage='2B');
  st3=(stage='3');  st4a=(stage='4A');  st4b=(stage='4B');
  if stage=' ' or stage='X' then do;
    st0=.; st1=.; st2a=.; st2b=.; st3=.; st4a=.; st4b=.;
  end;
  castage=0*st0 + 1*st1 + 2*st2a + 3*st2b + 4*st3 + 5*st4a + 6*st4b;

  sq=(celltype='SQ'); if celltype=' ' then sq=.;
  ad=(celltype='AD'); if celltype=' ' then ad=.;
  sig=(signet='Y');   if signet=' ' then sig=.;

  if differen='POOR' then diff=4;
  if differen='MP'   then diff=3;
  if differen='MOD'  then diff=2;
  if differen='MW'   then diff=1;
  if differen='WELL' then diff=0;
  if differen=' ' then diff=.;

  posmarg=(margins='Y'); if margins=' ' then posmarg=.;

  label
    barr     = 'Pathologic: Barretts'
    pt       = 'Pathologic: depth of invasion (0-6)'
    pt_0     = 'Pathologic: T0 (none)'
    pt_hgd   = 'Pathologic: T-Tissue (HGD)'
    pt_1     = 'Pathologic: T-1'
    pt_1im   = 'Pathologic: T-1, intramucosal'
    pt_1sm   = 'Pathologic: T-1, submucosal'
    pt_2     = 'Pathologic: T-2'
    pt_3     = 'Pathologic: T-3'
    pt_4     = 'Pathologic: T-4'
    pn1      = 'Pathologic: regional lymph nodes'
    totnodes = 'Pathologic: total nodes'
    posnodes = 'Pathologic: positive nodes'
    pctpos   = 'Pathologic: % positive nodes'
    sq       = 'Pathologic: squamous cell carcinoma'
    ad       = 'Pathologic: adenocarcinoma'
    sig      = 'Pathologic: signet'
    pm1a     = 'Pathologic: m1a disease'
    pm1b     = 'Pathologic: m1b disease'
    pm       = 'Pathologic: metastases'
    castage  = 'Pathologic: depth of invasion (0-6)'
    st0      = 'Pathologic: stage 0'
    st1      = 'Pathologic: stage 1'
    st2a     = 'Pathologic: stage 2a'
    st2b     = 'Pathologic: stage 2b'
    st3      = 'Pathologic: stage 3'
    st4a     = 'Pathologic: stage 4a'
    st4b     = 'Pathologic: stage 4b'
    posmarg  = 'Pathologic: positive margins'
    diff     = 'Pathologic: differentiation'
  ;
*******************************************************************************;
* Downstaging                                                                  ;
  d_t=pt-ct;
  d_n=pn1-cn1;
  label
    d_t      = 'Difference: T'
    d_n      = 'Difference: N'
  ;
*******************************************************************************;
* Postsurgery treatment                                                        ;
  anypost=(postadj>0);
  label anypost = 'PO: Radio- or chemo-therapy';
*******************************************************************************;
* Follow-up variables                                                          ;
  dead=(status='D');
  iv_dead=(fupdate - opdate)/365.2425;
  dead_h=0; if status='D' and fupdate le disdate then dead_h=1;
  dead_30d=0; if dead and iv_dead le 30/365.2425 then dead_30d=1;

  carecur=(recur='Y');
  iv_recur=iv_dead; if carecur then iv_recur=(recdate - opdate)/365.2425;

  label
    dead     = 'Staus: dead'
    dead_h   = 'Staus: hospital death'
    dead_30d = 'Staus: operativ death'
    iv_dead  = 'Staus: interval (years) to death'
    carecur  = 'Staus: cancer recurence'
    iv_recur = 'Staus: interval to cancer recurence'
  ;
*******************************************************************************;
* Experience variables                                                         ;
  opyrs=(opdate - mdy(1,1,1983))/365.2425;
  label opyrs = 'date of operation (since 1983)';
*______________________________________________________________________________;
*                                                                              ;
* Stratification variables                                                     ;
*______________________________________________________________________________;
*                                                                              ;
* Age groupings by 3, 4, 5, 6 points                                           ;
  age3grp='3_ge 60  ';
  if age < 60 then age3grp='2_55-60';
  if age < 55 then age3grp='1_< 55';

  age4grp='4_ge 65  ';
  if age < 65 then age4grp='3_60-65';
  if age < 60 then age4grp='2_50-60';
  if age < 50 then age4grp='1_< 50';

  age5grp='5_ge 65  ';
  if age < 65 then age5grp='4_60-65';
  if age < 60 then age5grp='3_55-60';
  if age < 55 then age5grp='2_50-55';
  if age < 50 then age5grp='1_< 50';

  age6grp='6_ge 70  ';
  if age < 70 then age6grp='5_65-70';
  if age < 65 then age6grp='4_60-65';
  if age < 60 then age6grp='3_50-60';
  if age < 50 then age6grp='2_45-50';
  if age < 45 then age6grp='1_< 45';
*******************************************************************************;
* Nodes                                                                        ;
  pctn3grp='3_gt 50%  ';
  if pctpos LE 50 then pctn3grp='2_10-50%';
  if pctpos LE 10 then pctn3grp='1_LE 10%';

  nodp3grp='3_ge 5   ';
  if posnodes < 5 then nodp3grp='2_3-5';
  if posnodes < 3 then nodp3grp='1_< 3';

  nodp7grp='6_> 10    ';
  if posnodes < 10 then nodp7grp='5_7-10';
  if posnodes < 7 then nodp7grp='4_5-6';
  if posnodes < 5 then nodp7grp='3_3-4';
  if posnodes = 2 then nodp7grp='2_2';
  if posnodes = 1 then nodp7grp='1_1';
  if posnodes = 0 then nodp7grp='0_0';
*******************************************************************************;
* Date of operation                                                            ;
  opyrgrp='4_gt 15%  ';
  if opyrs LE 15 then opyrgrp='3_10-15';
  if opyrs LE 10 then opyrgrp='2_5-10';
  if opyrs LE 5 then opyrgrp='1_LE 5';

  label
        age3grp  = 'Age group (3 levels)'
        age4grp  = 'Age group (4 levels)'
        age5grp  = 'Age group (5 levels)'
        age6grp  = 'Age group (6 levels)'
        pctn3grp = 'Percent of nodes positive group'
        posnodes = 'Number of nodes positive group'
        opyrs    = 'Date of operation'
  ;
*______________________________________________________________________________;
*                                                                              ;
* Data transformations of scale                                                ;
*______________________________________________________________________________;
*                                                                              ;
* Logarithmic:                                                                 ;
  %ln(age, ln_age);
  %ln(ct, ln_ct);
  %ln(ct_gr, ln_ct_gr);
  %ln(pt, ln_pt);
  %ln(cstage, ln_cstg);
  %ln(cstgx2a, ln_cstgx);
  %ln(castage+1, ln_stage);
  %ln(diff+1, ln_diff);
  %ln((pctpos+1)/100, ln_pos);
  %ln(posnodes+1, ln_npos);
  %ln(opyrs, ln_opyrs);
*******************************************************************************;
* Inverse:                                                                     ;
  %in(age/50, in_age);
  %in(ct, in_ct);
  %in(ct_gr, in_ct_gr);
  %in(pt, in_pt);
  %in(cstage, in_cstg);
  %in(cstgx2a, in_cstgx);
  %in(castage+1, in_stage);
  %in(diff+1, in_diff);
  %in((pctpos+1)/100, in_pos);
  %in(posnodes+1, in_npos);
  %in(opyrs/10, in_opyrs);
*******************************************************************************;
* Power:                                                                       ;
  age2=(age/50)**2;
  agee=exp(age/50);
  in2ct=in_ct**2;
  in2ct_gr=in_ct_gr**2;
  in2pt=in_pt**2;
  in2cstg=in_cstg**2;
  in2cstgx=in_cstgx**2;
  in2_pos=in_pos**2;
  in2npos=in_npos**2;
  in2opyrs=in_opyrs*in_opyrs;
  opyrs2=opyrs**2;
*______________________________________________________________________________;
*                                                                              ;
* Missing value standardization according to presence or absence of cancer     ;
  %if &missing=1 %then %do;
    proc standard data=&out out=&out replace; var
         black white prior prioreg pctpos ln_pos in_pos in2_pos posnodes ln_npos
         in_npos in2npos castage st0 st1 st2a st2b st3 st4a st4b ln_stage
         in_stage

   ;
  %end;
*______________________________________________________________________________;
*                                                                              ;
* Transformations of scale                                                     ;
*______________________________________________________________________________;
*                                                                              ;
   data &out; set &out;
*______________________________________________________________________________;
*                                                                              ;
* Interaction terms                                                            ;
*______________________________________________________________________________;
*                                                                              ;
* Clinical variables                                                           ;
  cn0_ct=(1-cn1)*ct;
  cn0lct=(1-cn1)*ln_ct;
  cn0ict=(1-cn1)*in_ct;
  cn0i2ct=(1-cn1)*in2ct;
  cn0_ctgr=(1-cn1)*ct_gr;
  cn0lctgr=(1-cn1)*ln_ct_gr;
  cn0ictgr=(1-cn1)*in_ct_gr;
  cn0_cstg=(1-cn1)*cstage;
  cn0lcstg=(1-cn1)*ln_cstg;
  cn0icstg=(1-cn1)*in_cstg;
  cn0i2cst=(1-cn1)*in2cstg;
*******************************************************************************;
* Pathology variables                                                          ;
  pn0_pt=(1-pn1)*pt;
  pn0lpt=(1-pn1)*ln_pt;
  pn0ipt=(1-pn1)*in_pt;
  pn0i2pt=(1-pn1)*in2pt;
  pn1inpos=pn1*in_npos;
*******************************************************************************;
* Procedure and experience variables                                           ;
  pn0_it=it*(1-pn1);
  pn1_it=it*pn1;
  pn1chest=pn1*appr_ch;
  pn0chest=(1-pn1)*appr_ch;
  it_opyrs=it*opyrs;
  itlopyrs=it*ln_opyrs;
  itiopyrs=it*in_opyrs;
  it_pt=it*pt;
  itlpt=it*ln_pt;
  itipt=it*in_pt;
  iti2_pt=it*in2pt;
  it_chest=it*appr_ch;
  it_pstg=it*castage;
  itlpstg=it*ln_stage;
  itipstg=it*in_stage;
  it_d_t=it*d_t;
  it_d_n=it*d_n;
  run;
  %mend;
*______________________________________________________________________________;
*                                                                              ;
*       V A R I A B L E S   A V A I L A B L E   F O R   A N A L Y S I S        ;
*______________________________________________________________________________;
*                                                                              ;
  %macro skip;
  /* Demographic */
       male, female, age, ln_age, in_age, age2, agee, black, white,
 
  /* Preoperative factors */
       it, prior, prioreg,

  /* Preoperative esophageal uptrasound (EUS) */
       ct, ct_0, ct_hgd, ct_1, ct_2, ct_3, ct_4, ln_ct, in_ct, in2ct, ct_gr,
       ln_ct_gr, in_ct_gr, in2ct_gr, ct_0hgd, ct_low, ct_hi, cn1, csx, cs0, cs1,
       cs2a, cs2b, cs3, cstage, ln_cstg, in_cstg, in2cstg, cstgx2a, ln_cstgx,
       in_cstgx, in2cstgx,

  /* Surgical procedure */
       surgonly, anas_n, appr_ch, appr_th,

  /* Cancer pathology */
       barr, pt, pt_0, pt_hgd, pt_1, pt_1im, pt_1sm, pt_2, pt_3, pt_4, ln_pt,
       in_pt, in2pt, pn1, posnodes, ln_npos, in_npos, in2npos, pctpos, ln_pos,
       in_pos, in2_pos, sq, ad, sig, pm1a, pm1b, pm, castage, ln_stage,
       in_stage, st0, st1, st2a, st2b, st3, st4a, st4b, posmarg, diff, ln_diff,
       in_diff,

  /* Downstaging */
       d_t, d_n,

  /* Postoperative therapy */
       anypost,

  /* Experience */
       opyrs, ln_opyrs, in_opyrs,in2opyrs, opyrs2,
 
  /* Interaction terms:  Clinical */
       cn0_ct, cn0lct, cn0ict, cn0i2ct, cn0_ctgr, cn0lctgr, cn0ictgr, cn0_cstg,
       cn0lcstg, cn0icstg, cn0i2cst,

  /* Interaction terms:  Pathologic */
       pn0_it, pn1_it, pn0_pt, pn0lpt, pn0ipt, pn0i2pt, pn1inpos,

  /* Interaction terms:  Procedure and experience */
       pn0chest, pn1chest, it_opyrs, itlopyrs, itiopyrs, it_pt, itlpt, itipt,
       iti2_pt, it_chest, it_pstg, it_pstg, it_pstg, it_d_t, it_d_n, itptdown,
       itpndown,

  /* Missing value flags */

  /* Outcomes */
      dead, iv_dead, dead_h, dead_30d,
       carecur, iv_recur,
  %mend;
*******************************************************************************;
