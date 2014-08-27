#include <hazpred.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_ASM_BYTEORDER_H 
/*
  If we're running on intel, numbers are little endian... 
  load the translation routines
*/
#include <asm/byteorder.h>
#endif /*  */

#include "xportHandler.h"

#include <hzpm.h>
#include "ibmieee.h"

/*#define DEBUG 1*/

/* Private Member functions */
void   openTransport(char *filename);
void   printNamestr(struct namestr *namestr);
void   checkNamestr(struct namestr *namestr, int numVar);
int    findObsNum(FILE *file, char* buf, int obsSize);
void   getFlags(FILE *file, char* buf, int obsSize, long pos);
double getFlag(char *flag, FILE *file, char* buf, int obsSize, long pos);
void   setCountsInCommon(int obsNum, int varNum);
void   getParms(FILE *file, char* buf, int obsSize, int obsNum, int varNum);


/* This is the public interface function */
/* Data is passed back through the static Common C structure */
void hzpxprt (char *filename, struct common *C){
#ifdef DEBUG
  printf("hzpxprt\n");
#endif /*DEBUG*/

  Common = *C;
  openTransport(filename);
}


/* I think the rest of these are private workers */
void openTransport(char *filename)
{
#ifdef DEBUG
  printf("openTransport\n");
#endif /*DEBUG*/

  char buf[80], 	/* buffer for parsing the file header */
    *obsBuf;		/* buffer for readin observation record */
  char blank[8]={"        "};
  int  desLength,
    varNum,		/* number of variables (columns) */
    obsSize,	/* bytes of each observation (a single row) */
    obsNum,		/* number of observations (rows) */
    i;
  char to[8];
  struct namestr *namestr;
  FILE* fPtr;

  int size = (9*sizeof(short int) + 3*sizeof(char[8])+sizeof(char[40])+ 
	      sizeof(long)+54*sizeof(char)); 
   
#ifdef DEBUG
  printf("filename: %s\n", filename);
  printf("Size of NAMESTR: %d\n", sizeof(struct namestr));
  printf("Calculated Size of NAMESTR: %d\n", size); 
  printf("Size of NAMESTR->ntype: %d\n", sizeof(namestr->ntype));
#endif /* DEBUG */

  
  if ((fPtr = fopen(filename, "rb")) == NULL){
    perror("openTransport");
    exit (1);
  }
  /* Each record is integral 80 bytes long, padded with blanks if needed */
  /* first header record #1 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #1)");
    exit(1);
  }
#ifdef DEBUG 
  printf("Header #1 : %s\n", buf); 
#endif /* DEBUG */
 
  /* first real header record #2 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #2)");
    exit(1);
  }
#ifdef DEBUG 
  printf("Header #2 : %s\n", buf); 
#endif /* DEBUG */
  
  /* second real header record #3 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #3)");
    exit(1);
  }
#ifdef DEBUG 
  printf("Header #3 : %s\n", buf); 
#endif /* DEBUG */

  /* member header records #4 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #4)");
    exit(1);
  }
#ifdef DEBUG 
  printf("Header #4 : %s\n", buf); 
#endif /* DEBUG */

  /* get the size of variable descripter (namestr) from the 4th header line*/
  sscanf(buf+75, "%3d", &desLength);

  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #5)");
    exit(1);
  }
#ifdef DEBUG 
  printf("Header #5 : %s\n", buf); 
#endif /* DEBUG */

  /* member header data #6 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #6)");
    exit(1);
  }
#ifdef DEBUG 
  printf("Header #6 : %s\n", buf); 
#endif /* DEBUG */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #7)");
    exit(1);
  }
#ifdef DEBUG 
  printf("Header #7 : %s\n", buf); 
#endif /* DEBUG */

  /* namestr header record #8 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #8)");
    exit(1);
  }
  /* get number of variables */
  sscanf(buf+54, "%4d", &varNum);
#ifdef DEBUG 
  printf("Header #8 : %s\n", buf); 
#endif /* DEBUG */

#ifdef DEBUG
  printf("deslength=%ld, varNum=%ld\n", desLength,varNum);
  printf("sizeof... short %d charVarName %d, charLabel %d, char %d, long %d, unsigned char %d\n", 
	 sizeof(short), sizeof(charVarName),sizeof(charLabel),sizeof(char),
	 sizeof(long), sizeof(unsigned char));
#endif

  /* namestr records */
  namestr = (struct namestr*) malloc(sizeof(struct namestr)*varNum);

#ifdef  HAVE_ASM_BYTEORDER_H
  /* Create a point to manipulate the read in string */
  struct namestr* nstrPtr;
#endif /*  HAVE_ASM_BYTEORDER_H*/

  for (i=0; i<varNum; i++){
    if (fread(namestr+i, desLength, 1, fPtr) == -1){
      perror("openTransport (#7)");
      exit(1);
    }
  
#ifdef  HAVE_ASM_BYTEORDER_H
#ifdef DEBUG
    // printf(" little endian translation\n");
#endif /*DEBUG*/
    nstrPtr = namestr+i;
    
    /* all short ints in namestr - defined in common/structures.h */
    nstrPtr->ntype = ntohs(nstrPtr->ntype);
    nstrPtr->nhfun = ntohs(nstrPtr->nhfun);
    nstrPtr->nlng = ntohs(nstrPtr->nlng);
    nstrPtr->nvar0 = ntohs(nstrPtr->nvar0);
    nstrPtr->nfl = ntohs(nstrPtr->nfl);
    nstrPtr->nfd = ntohs(nstrPtr->nfd);
    nstrPtr->nfj = ntohs(nstrPtr->nfj);
    nstrPtr->nifl = ntohs(nstrPtr->nifl);
    nstrPtr->nifd = ntohs(nstrPtr->nifd);
    
    /* and one long int */
    nstrPtr->npos = ntohl(nstrPtr->npos);
    
#endif /* HAVE_ASM_BYTEORDER_H*/
    
#ifdef DEBUG
    printNamestr(namestr+i);
#endif
  }
  
  /* Validate the input by checking the current namestr
   */
  checkNamestr(namestr, varNum);
  
  /* Calculate the size of each observation record */
  
  obsSize = (namestr+varNum-1)->npos + (namestr+varNum-1)->nlng;
  
#ifdef DEBUG
  printf("obsSize %d\n", obsSize );
#endif  
  obsBuf = (char *) malloc(obsSize * sizeof (char));
#ifdef DEBUG
  printf("varNum: |%d|\n", varNum);
	 printf("obsSize: |%d| npos: |%d|  nlng: |%d|\n", obsSize,
	 namestr[varNum-1].npos, namestr[varNum-1].nlng);
#endif
  /* make up 80 chars even */
  if (fread(buf, 80-desLength*varNum%80, 1, fPtr) == -1){
    perror("openTransport (#8)");
    exit(1);
  }
#ifdef DEBUG
  printf("Header #9 : %s\n", buf); 
#endif /* DEBUG */
  
  int fpos;	/* current file position */
  /* save current file position */
  fpos = ftell(fPtr);
  
  /* observation header #10 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (#9)");
    exit(1);
  }
#ifdef DEBUG
  printf("Header #10 : %s\n", buf); 
#endif /* DEBUG */
  if(strspn(buf,"G1FLAG")> 0){
  /* recover the previous position */   
  fseek(fPtr, fpos, SEEK_SET);
  }
  
  /* Find the number if obsSize records remaining in the file */
  obsNum = findObsNum(fPtr, obsBuf, obsSize);
#ifdef DEBUG
  printf("ObsNum [p*3 + 6 flags + 8 shapes +3 intercepts]: %d\t varNum: %d\n", obsNum, varNum );
#endif

  /* Allocate memory for the observations from the
     xport file */

  hzpm(obsNum);

  /* Set a bunch of common parameter counts. */
  setCountsInCommon(obsNum, varNum);

  /* Get the flag values that specify the model */
  getFlags(fPtr, obsBuf, obsSize, (namestr+1)->npos);

  /* Get the covariance matrix */
  getParms(fPtr, obsBuf, obsSize, obsNum, varNum);
  
  fclose(fPtr);
}



void getParms(FILE *file, char* buf, int obsSize, int obsNum, 
	      int varNum){
#ifdef DEBUG
  printf("getParms obsSize:%d obsNum:%d varNum:%d\n", obsSize, obsNum, 
	 varNum);
#endif /*DEBUG*/

  int i, j;
  char to[8]; /* for converted double value */

  /* first 6 obs are flags which have been read */
  for (i=0; i<obsNum-6; i++) {
    /* Start reading the observations. */
    if (fread(buf, obsSize, 1, file) == -1){
      perror("getParms");
      exit (1);
    }else{
      /* estimate */
      cnxptiee(buf+8, 1, to, 0);
      *(Common.theta+i) = *((double *)to);

      /* status */
      cnxptiee(buf+16, 1, to, 0);
      *(Common.status+i) = (short int) *((double *)to);
         
      /* get the variance and covariance variables */
      for (j=3; j<varNum; j++){
	cnxptiee(buf+j*8, 1, to, 0);
	*(Common.cov+i*(varNum-3)+j-3) = *((double *)to);
      }
      
#ifdef DEBUG
      printf("getParms");
      printf(" %3d  %6.3f %5d", i+7, *(Common.theta+i), *(Common.status+i));
      for (j=3; j<varNum; j++){
	printf("%6.3f", *(Common.cov+i*(varNum-3)+j-3));
      }
      printf("\n");

#endif

    }   
  }
}


void getFlags(FILE *file, char* buf, int obsSize, long pos){
  /* Read the early and late flag values. From the data
     section of the transport file. 

     The section has 6 name value pairs before the 
     covariance matrix.
  */
#ifdef DEBUG
  printf("getFlags(void) %d %d\n", obsSize, pos);
#endif /*DEBUG*/

  /* We know the order of the flags, so read them
     in... getFlag checks the name to ensure the
     file matches what we think we should get.
  */ 
  Common.early->g1flag = getFlag("G1FLAG  ", file, buf, obsSize, pos);
  Common.early->delta0 = getFlag("FIXDEL0 ", file, buf, obsSize, pos);
  Common.early->mNuOne = getFlag("FIXMNU1 ", file, buf, obsSize, pos);
  Common.late->g3flag = getFlag("G3FLAG  ", file, buf, obsSize, pos);
  Common.late->g_two = getFlag("FIXGE2  ", file, buf, obsSize, pos);
  Common.late->ga_two = getFlag("FIXGAE2 ", file, buf, obsSize, pos);
}


double getFlag(char *flag, FILE *file, char* buf, int obsSize, 
	       long pos){
#ifdef DEBUG
  printf("getFlags %s (double) %d %d", flag, obsSize, pos);
#endif /*DEBUG*/
  
  char to[8];	/* store IEEE flag value */
  if (fread(buf, obsSize, 1, file) == -1){
    perror("getFlag");
    exit (1);
  }else{
    if (memcmp(buf, flag, 8) != 0){
      perror("getFlag: unmatch flag name");
      exit (1);
    }else{
      cnxptiee(buf+pos, 1, to, 0);
#ifdef DEBUG
      printf("%s   %3d\n",flag, (short int) *((double *)to));
#endif /* DEBUG */
    }
  }
   
  return (short int) *((double *)to);	/* flag value */
}


void setCountsInCommon(int obsNum, int varNum)
{
#ifdef DEBUG
  printf("setCountsInCommon obsNum:%d, varNum:%d\n", obsNum, varNum);
#endif /*DEBUG*/
  
  Common.Nobs = obsNum;
  Common.Ntheta = obsNum-6;
  Common.p = (obsNum-17)/3;	/* num of concomitant variables */

#ifdef DEBUG
  printf("p=%d\n", Common.p);
#endif
   
  /* old code may wrong: Common->Ntheta<6 */
  if ((obsNum-17)%3 || Common.Ntheta<9){
    fprintf(stderr, "setCountsInCommon: improper number of obs\n");
    exit(1);
  }
  
  if ((Common.Ntheta+3) != varNum){
    if (varNum == 3)
      Common.hzpstr->noCL = 1;
    else{
      fprintf(stderr, "setCountsInCommon: incorrect file format\n");
      exit (1);

    }
  }
}


/** Count the number of records of size obsSize in the
    file, from the current location to the end.
*/
int findObsNum(FILE *file, char* buf, int obsSize){

#ifdef DEBUG
  printf("findObsNum %d\n", obsSize);
#endif /*DEBUG*/

  int obs=0;
  int fpos;	/* current file position */
  char blank[8]={"        "};
   
  /* save current file position */
  fpos = ftell(file);

  while (!feof(file)){
    if (fread(buf, obsSize, 1, file) != 0){
    	if(ferror(file) != 0){
  			perror("findObsNum ferror:");
    	}
    	
#ifdef DEBUG
      printf("findObsNum[%d]:\n", obs);
#endif /*DEBUG*/

      if (memcmp(buf, blank, 8) == 0){
#ifdef DEBUG
	printf("findObsNum fread 8 blanks\n");
#endif /* DEBUG*/  

	break;

      }else{
	obs++;
	#ifdef DEBUG
      printf("findObsNum fread = %d \n", obs);
#endif /* DEBUG*/ 
	
      }
    }else{
     
#ifdef DEBUG
      perror("findObsNum");
      printf("findObsNum fread = 0 %d \n", obs);
#endif /* DEBUG*/ 

#ifdef DEBUG
	int leftovers=0;

	while (!feof(file)){
    if (fread(buf, 1, 1, file) != 0){
    	if(ferror(file) != 0){
  			perror("findObsNum ferror:");
    	}
      leftovers++;
    }
	}
    printf("Leftover chars: |%d|", leftovers);
    
#endif /* DEBUG*/ 
      break;

    }
  }
   
#ifdef DEBUG
  printf("obsNum[%d]:\n", obs);
#endif /*DEBUG*/

  /* recover the previous position */   
  fseek(file, fpos, SEEK_SET);
  
  /* Return the number of records read */
  return obs;
}


void checkNamestr(struct namestr *namestr, int numVar){

  int i;
   
#ifdef DEBUG
  printf("checkNamestr: numVar %d\n", numVar);
#endif /* DEBUG */


  if (numVar < 3){
    perror("checkNamestr: improper transport file (<3 columns)\n\t");
    exit(1);
  }


  /* check variable type */
  if (namestr->ntype != 2){
    fprintf(stderr,"namestr->ntype: %d\n", namestr->ntype);
    perror("checkNamestr: 1st column should be char\n ");
    exit(1);
  }
  for (i=1; i<numVar; i++){
    if ((namestr+i)->ntype != 1){
      fprintf(stderr, "checkNamestr: column %d is not numerical\n", i+1);
      exit (1);
    }
  }

  /* check other fields */
  for (i=0; i<numVar; i++){
    if ((namestr+i)->nlng != 8){
      fprintf(stderr, "checkNamestr: var length should be 8\n");
      exit (1);
    }

    if ((namestr+i)->npos != i*8){
      fprintf(stderr, "checkNamestr: star position wrong\n");
      exit (1);
    }
  }
}


void printNamestr(struct namestr *namestr){
#ifdef DEBUG
  printf("printNamestr\n");
#endif /*DEBUG*/

  printf("ntype=|%d|\n", namestr->ntype);
  printf("nhfun=|%d|\n", namestr->nhfun);
  printf("nlng=|%d|\n", namestr->nlng);
  printf("nvar0=|%d|\n", namestr->nvar0);
  printf("nname=|%8s|\n", namestr->nname);
  printf("nlabel=|%40s|\n", namestr->nlabel);
  printf("nform=|%8s|\n", namestr->nform);
  printf("nfl=|%d|\n", namestr->nfl);
  printf("nfd=|%d|\n", namestr->nfd);
  printf("nfj=|%d|\n", namestr->nfj);
  printf("nfill=|%2s|\n", namestr->nfill);
  printf("niform=|%8s|\n", namestr->niform);
  printf("nifl=|%d|\n", namestr->nifl);
  printf("nifd=|%d|\n", namestr->nifd);
  printf("npos=|%ld|\n", namestr->npos);
  printf("rest=|%52s|\n", namestr->rest);
  printf("\n");
  return;
}
