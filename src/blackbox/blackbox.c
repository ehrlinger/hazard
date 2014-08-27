/**
 Driver for the blackbox

 Command line driven...
 ./blackbox hmdeadp.xport 1.0 144. 7 59. 3. 3. 0. 0. 1. 0.
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>

#define HAZPRED
#define STRUCT
#include <hazpred.h>
#include <hzpi.h>

#include "xportHandler.h"
#include <hzpo.h>
#include <hzpe.h>
#include <hzpp.h>

static void print_results();
static void my_error_handler(void);

int main(int argc, char** argv) {
#ifdef DEBUG
	printf("main\n");
#endif /*DEBUG*/

	/* Check that we have the correct argument list */
	if (argc < 5) {
		fprintf(stderr,
				"Usage: argv[0] xport_filename num_steps t_n num_vars vars... \n");

		exit(1);
	}

	char *filename = argv[1];

	//fprintf(stderr,"%s\n",filename);

	int i;

	/* Initialize the dataset
	 *
	 * struct common *C - A collection of about
	 * everything. Almost everything that is in
	 * C is also global.
	 */
	C = hzpi();

	/*
	 effectively sets an exception
	 (setjmp is a C goto)
	 */
	if (setjmp(C->errtrap))
		my_error_handler();

	/* Continue initializing */
	H = C->hzpstr;
	H->noCL = 0;

	/*
	 We may want to pass the h->CLimit value in for
	 variable ci's. The only difficult thing here is
	 we'll need to incorporate the student-t
	 distribution
	 */
	H->CLimit = 0.683; /* PROC HAZPRED default */

	/* What are these flags for? */
	H->Hflag = 1; /* Hazard calculation?  */
	H->Sflag = 1; /* Survival calculation? */

	/* Read the input dataset */
	hzpxprt(filename, C);

	/*
	 More memory setup..... manipulate Common, and
	 the phase parameters.
	 */
	hzpe();
	hzpp();

	/*
	 We are given a time step, an ending time
	 and the varable values. So we loop through the
	 requested time steps.
	 */
	int num_steps = atoi(argv[2]);
	float t_n = 1.0 * atof(argv[3]);
	int num_vars = atoi(argv[4]);

	/* Load the patient data from the arguments. */
	for (i = 1; i <= num_vars; i++) {
		C->obs[i] = atof(argv[4 + i]);
	}

	/*
	 Since everyone is alive at t_0, we start at t_1.
	 */
	int run_type = Chgd_Both;

	if (t_n > 1.0) {

		/* Now start stepping through the values. We want num_step lines, we calculate the log increment. */
		float linc = log(t_n) / num_steps;

		/* Get a line for every timestep. zero is assumed at 100% so we start at the first step.*/
		C->obs[0] = linc;

		// Not happy with log time steps either.
		int log_steps = 0; //num_steps/2;
		num_steps -= log_steps;
		for (i = 0; i < log_steps; i++) {
			//     printf("%f %f\n", inc, C->obs[0]);

			hzpo(run_type, 1);
			print_results();
			C->obs[0] = exp(log(C->obs[0]) + linc);

		}
	} else {
		C->obs[0] = 0;
	}
	// Now setup the linear steps.
	float inc = (t_n - C->obs[0]) / num_steps;
	if(C->obs[0] == 0) C->obs[0] = inc;
	for (i = 0; i < num_steps; i++) {
		//printf("%f %f\n", inc, C->obs[0]);

		hzpo(run_type, 1);
		print_results();
		C->obs[0] += inc;

	}
	C->obs[0] = t_n;
	hzpo(run_type, 1);
	print_results();
	run_type = Chgd_Time;

	// printf("MONTHS PCT_SURV PCT_CLLS PCT_CLUS _HAZARD _CLLHAZ _CLUHAZ\n");

	/*  printf("\n\n"); */

	return (0);
}

static void print_results() {
#ifdef DEBUG
	printf("print_results\n");
#endif /*DEBUG*/

	printf("%.4lf %.4lf %.4lf %.4lf %lf %lf %lf\n", C->obs[0], H->Surviv
			* 100.0, H->CL_L_S * 100.0, H->CL_U_S * 100.0, H->Hazard,
			H->CL_L_H, H->CL_U_H);
}

static void my_error_handler(void) {
#ifdef DEBUG
	printf("my_error_handler\n");
#endif /*DEBUG*/

	printf("Error trapped: %s\n\n\n", C->errflg);
	exit(EXIT_FAILURE);
}
