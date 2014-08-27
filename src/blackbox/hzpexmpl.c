/**
   Driver for the blackbox
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#define HAZPRED
#define STRUCT
#include <hazpred.h>
#include <hzpi.h>

#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>

/*
static struct COMMON *C;
static struct HZPSTR *H;
*/
static int patient,mvr,avr;

static void do_obs_loop(void);
static void print_results(void);
static void my_error_handler(void);

int main(int argc, char** argv)
{
#ifdef DEBUG
     printf("main\n");
#endif /*DEBUG*/

  /* Check that we have a data file */
   if (argc != 2){
     fprintf(stderr, "main: one argument needed (xport data filename)\n");
     exit(1);
   }
   
   /* Initialize the dataset */
   C = hzpi();

   /* effectively sets an exception (setjmp is a C goto) */
   if(setjmp(C->errtrap))
      my_error_handler();

   /* Continue initializing */
   H = C->hzpstr;
   H->noCL = 0;
   H->CLimit = 0.683;  /* PROC HAZPRED default */
   H->Hflag = 1;
   H->Sflag = 1;

   /* Read the input dataset */
   hzpxprt(argv[1], C);

   hzpe();
   hzpp();

   /* Set the patient data */
   patient = 1;                          mvr=0; avr=1;
   C->obs[1] /* AGE_COP */ = 57.0;       C->obs[2] /* NYHA    */ = 3.0;
   C->obs[5] /* AO_PINC */ = 0.0;
   C->obs[6] /* BLACK   */ = 0.0;        C->obs[7] /* I_PATH  */ = 0.0;
   /* then do the analysis */
   do_obs_loop();

   patient = 2;                          mvr=1; avr=0;
   C->obs[1] /* AGE_COP */ = 59.0;       C->obs[2] /* NYHA    */ = 3.0;
   C->obs[5] /* AO_PINC */ = 0.0;
   C->obs[6] /* BLACK   */ = 1.0;        C->obs[7] /* I_PATH  */ = 0.0;
   do_obs_loop();

   patient = 3;                          mvr=1; avr=0;
   C->obs[1] /* AGE_COP */ = 63.0;       C->obs[2] /* NYHA    */ = 4.0;
   C->obs[5] /* AO_PINC */ = 0.0;
   C->obs[6] /* BLACK   */ = 0.0;        C->obs[7] /* I_PATH  */ = 1.0;
   do_obs_loop();

   patient = 4;                          mvr=0; avr=1;
   C->obs[1] /* AGE_COP */ = 51.0;       C->obs[2] /* NYHA    */ = 3.0;
   C->obs[5] /* AO_PINC */ = 1.0;
   C->obs[6] /* BLACK   */ = 0.0;        C->obs[7] /* I_PATH  */ = 0.0;
   do_obs_loop();

   patient = 5;                          mvr=1; avr=1;
   C->obs[1] /* AGE_COP */ = 48.0;       C->obs[2] /* NYHA    */ = 4.0;
   C->obs[5] /* AO_PINC */ = 1.0;
   C->obs[6] /* BLACK   */ = 1.0;        C->obs[7] /* I_PATH  */ = 0.0;
   do_obs_loop();

   patient = 6;                          mvr=1; avr=1;
   C->obs[1] /* AGE_COP */ = 50.0;       C->obs[2] /* NYHA    */ = 3.0;
   C->obs[5] /* AO_PINC */ = 1.0;
   C->obs[6] /* BLACK   */ = 1.0;        C->obs[7] /* I_PATH  */ = 1.0;
   do_obs_loop();

   return (0);
}

static void do_obs_loop(void)
{
#ifdef DEBUG
     printf("do_obs_loop\n");
#endif /*DEBUG*/

   int aortic,mitral;

   printf("---------------------------------- Patient #%d "
          "----------------------------------\n\n",patient);
   printf("       avr = %d      mvr=%d\n\n", avr, mvr);

   aortic = (avr>0);
   mitral = (mvr>0);

   /* 3 and 4 are derived predictor variables for this model */
   if(C->obs[4] /* DOUBLE */ = (aortic && mitral))
      aortic = mitral = 0;

   if(patient==6)
      aortic = mitral = 1;      

   C->obs[3] /* MV_NYHA */ = C->obs[2]*mitral;

   /* Dump predictor variables */
   printf("[1]:= %f, [2]:=%f, [3]:=%f, [4]:=%f, [5]:=%f, [6]:=%f, [7]:=%f\n\n",
	  C->obs[1],C->obs[2],C->obs[3],C->obs[4],C->obs[5],C->obs[6],C->obs[7] );

   printf(" MONTHS    PCT_SURV    PCT_CLLS    PCT_CLUS     _HAZARD     "
          "_CLLHAZ     _CLUHAZ\n");

   /* Start the timesteps */
   /* Get the day line (time scale is months) */
   C->obs[0] = 30*12/365.2425;
   hzpo(Chgd_Both,1);
   print_results();

   /* Get the 6 month line */
   C->obs[0] = 6;
   hzpo(Chgd_Time,1);
   print_results();

   /* Get a line for every year there after for 6 years. */
   for(C->obs[0]=12.0; C->obs[0]<=72.0; C->obs[0]+=12.0) 
   {
      hzpo(Chgd_Time,1);
      print_results();
   }

   printf("\n\n");
}

static void print_results(void)
{
#ifdef DEBUG
     printf("print_results\n");
#endif /*DEBUG*/

   printf("%7.4lf    %8.4lf    %8.4lf    %8.4lf    %8.6lf    %8.6lf    "
          "%8.6lf\n",C->obs[0],H->Surviv*100.0,H->CL_L_S*100.0,
          H->CL_U_S*100.0,H->Hazard,H->CL_L_H,H->CL_U_H);
}

static void my_error_handler(void)
{
#ifdef DEBUG
     printf("my_error_handler\n");
#endif /*DEBUG*/

   printf("Error trapped: %s\n\n\n",C->errflg);
   exit(EXIT_FAILURE);
}
