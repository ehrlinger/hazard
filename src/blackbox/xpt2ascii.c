/**
   Driver for the xpt2ascii

   Command line driven...
   ./xpt2ascii file.xport
*/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
/* #define DEBUG*/

#define HAZPRED
#define STRUCT
#include "hazpred.h"
#include "hzpi.h"

#include <string.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>

static void print_results(void);
static void my_error_handler(void);
void openTransport(char *filename);
void printNamestr(struct namestr *namestr);
void printNameHdr();

void checkNamestr(struct namestr *namestr, int numVar);
int findObsNum(FILE *file, char* buf, int obsSize, long pos, int varNum);

int main(int argc, char** argv){
#ifdef DEBUG
  printf("main %s %d\n", argv[0], argc);
#endif /*DEBUG*/

  /* Check that we have the correct argument list */
  if (argc < 2){
    fprintf(stderr, 
	    "Usage: %s xport_filename \n",
	    argv[0]);
    
    exit(1);
  }
  
  char *filename = argv[1];

  /* Initialize the dataset 
   *
   * struct common *C - A collection of about 
   * everything, almost everything that is in
   * C is also global.
   */
  C = hzpi();

  /*
    effectively sets an exception
    (setjmp is a C goto)
  */
  if(setjmp(C->errtrap))
    my_error_handler();

  /* Read the input dataset */
  openTransport(filename);

  return (0);
}


static void my_error_handler(void){
#ifdef DEBUG
  printf("my_error_handler\n");
#endif /*DEBUG*/

  printf("Error trapped: %s\n\n\n",C->errflg);
  exit(EXIT_FAILURE);
}

int findObsNum(FILE *file, char* buf, int obsSize, long pos, int varNum){
#ifdef DEBUG
  printf("findObsNum obsSize:%d pos:%d\n", obsSize, pos);
#endif /*DEBUG*/
 
  int obs=0;
  int fpos;	/* current file position */
  char blank[8]={"        "};
  char to[8];	/* store IEEE flag value */
  int j;

  /* save current file position */
  fpos = ftell(file);

  while (1){
    if (fread(buf, obsSize, 1, file) != 0){
      
#ifdef DEBUG
      printf("findObsNum[%d]: %s\n", obs, buf);
#endif /* DEBUG*/

      if(obs < 6){
	printf("flag: %s ", memcpy(to, buf, 8));
	cnxptiee(buf+pos, 1, to, 0);
	printf("%3d\n", (short int) *((double *)to));
      }else{
	/* estimate */
	cnxptiee(buf+8, 1, to, 0);
	printf("%3d  %6.3f ", obs,*((double *)to));
	
	/* status */
	cnxptiee(buf+16, 1, to, 0);
	printf(" %5d ",(short int) *((double *)to));
	
	/* get the variance and covariance variables */
	for (j=3; j<varNum; j++){
	  cnxptiee(buf+j*8, 1, to, 0);
	  printf(" %6.3f ", *((double *)to));
	}
	
	printf("\n");
      }

      if (memcmp(buf, blank, 8) == 0){
#ifdef DEBUG
	printf("findObsNum fread 8 blanks: obsNum=%d\n", obs);
#endif /* DEBUG*/  
	break;
      }else{
	obs++;
      }
    }else{
      perror("findObsNum");
#ifdef DEBUG
      printf("findObsNum fread = 0\n");
#endif /* DEBUG*/  
      break;
    } 
  }
   
  /* recover the previous position */   
  fseek(file, fpos, SEEK_SET);
   
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
    perror("checkNamestr: 1st column should be char\n\t");
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
void printNameHdr(){
#ifdef DEBUG
  printf("printNameHdr\n");
#endif /*DEBUG*/

  printf("ntype\tnhfun\tnlng\tnvar0\tnname   nlabel                                  ");
  printf("\tnform\tnfl\tnfd\tnfj\tnfill\tniform  \tnifl\tnifd\tnpos\trest\n");
  return;
}

void printNamestr(struct namestr *namestr){
#ifdef DEBUG
  printf("printNamestr\n");
#endif /*DEBUG*/

  printf("%d\t%d\t%d\t%d", namestr->ntype, namestr->nhfun, namestr->nlng, namestr->nvar0);
  printf("\t|%8s| ", namestr->nname);
  printf("\t|%8s| ", namestr->nform);
  printf("\t%d\t%d\t%d", namestr->nfl, namestr->nfd, namestr->nfj);
  printf("\t|%2s| ", namestr->nfill);
  printf("\t|%8s| ", namestr->niform);
  printf("\t%d\t%d\t%d", namestr->nifl, namestr->nifd, namestr->npos);
  printf("\t|%52s|\n", namestr->rest);
  return;
}

void openTransport(char *filename){
#ifdef DEBUG
  printf("openTransport\n");
#endif /*DEBUG*/


  char buf[80], 	/* buffer for parsing the file header */
    *obsBuf;		/* buffer for readin observation record */
  char blank[8]={"        "};
  int  desLength,
    varNum,		/* number of variables (columns) */
    obsSize,    	/* bytes of each observation (a single row) */
    obsNum,		/* number of observations (rows) */
    i;
  char to[8];
  struct namestr *namestr;
  FILE* fPtr;

  int size = (9*sizeof(short int) + 3*sizeof(char[8])+sizeof(char[40])+ 
	      sizeof(long)+54*sizeof(char)); 

  printf("filename: %s\n", filename);
  printf("Size of NAMESTR: %d\n", sizeof(struct namestr));
  printf("Calculated Size of NAMESTR: %d\n", size); 
  printf("Size of NAMESTR->ntype: %d\n", sizeof(namestr->ntype));

  
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
  printf("(header #1): %s\n", buf);

  /* first real header record #2 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #2)");
    exit(1);
  }
  printf("(header #2): %s\n", buf);
   
  /* second real header record #3 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #3)");
    exit(1);
  }
  printf("(header #3): %s\n", buf);

  /* member header records #4 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #4)");
    exit(1);
  }
  printf("(header #4): %s\n", buf);

  /* get the size of variable descripter (namestr) from the 4th header line*/
  sscanf(buf+75, "%3ld", &desLength);

  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #5)");
    exit(1);
  }
  printf("(header #5): %s\n", buf);

  /* member header data #5 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #6)");
    exit(1);
  }
  printf("(header #6): %s\n", buf);

  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #7)");
    exit(1);
  }
  printf("(header #7): %s\n", buf);

  /* namestr header record #6 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (header #8)");
    exit(1);
  }
  printf("(header #8): %s\n", buf);

  /* get number of variables */
  sscanf(buf+54, "%4ld", &varNum);

  printf("deslength=%ld, varNum=%ld\n", desLength,varNum);
  printf("sizeof... short %d\tcharVarName %d,\tcharLabel %d,\tchar %d,\tlong %d, unsigned char %d\n", 
	 sizeof(short), sizeof(charVarName),sizeof(charLabel),sizeof(char),
	 sizeof(long), sizeof(unsigned char));

  /* namestr records #7 */
  namestr = (struct namestr*) malloc(sizeof(struct namestr)*varNum);

#ifdef  HAVE_ASM_BYTEORDER_H
  /* Create a point to manipulate the read in string */
  struct namestr* nstrPtr;
#ifdef DEBUG
  printf(" little endian translation\n");
#endif /*DEBUG*/
#endif /*  HAVE_ASM_BYTEORDER_H*/

  printNameHdr();

  for (i=0; i<varNum; i++)
    {
      if (fread(namestr+i, desLength, 1, fPtr) == -1){
	perror("openTransport (varNum)");
	fprintf(stderr, "varNum: %d\n", i);
	exit(1);
      }

#ifdef  HAVE_ASM_BYTEORDER_H

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
      printNamestr(namestr+i);
    }
  checkNamestr(namestr, varNum);
  obsSize = (namestr+varNum-1)->npos + (namestr+varNum-1)->nlng;

  printf("obsSize %d - calculated from name string (npos(%d) + nlng(%d))\n", 
	 obsSize, (namestr+varNum-1)->npos,(namestr+varNum-1)->nlng);
  printf("obsSize %d - calculated from ((varNum(%d)-1)*8) + nlng(%d))\n", 
	 (((varNum-1)*8) + (namestr+varNum-1)->nlng), varNum,(namestr+varNum-1)->nlng);
  
  obsBuf = (char *) malloc(obsSize * sizeof (char));
  /* make up 80 chars even */
  if (fread(buf, 80-desLength*varNum%80, 1, fPtr) == -1){
    perror("openTransport (#7)");
    exit(1);
  }
  
  /* observation header #8 */
  if (fread(buf, 80, 1, fPtr) == -1){
    perror("openTransport (#8)");
    exit(1);
  }

  /* data records #9 */
  obsNum = findObsNum(fPtr, obsBuf, obsSize, (namestr+1)->npos, varNum);

  printf("ObsNum [p*3 + 6 flags + 8 shapes +3 intercepts]: %d\t varNum: %d\n", obsNum, varNum );

  fclose(fPtr);
}
