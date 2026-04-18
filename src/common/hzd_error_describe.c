#include <string.h>
#include "hzd_error_describe.h"

/* Mapping from legacy hzd_Error() codes to human descriptions.
   The code strings themselves remain in the source as stable identifiers —
   they double as unique site tags when grepping the codebase. */

struct entry { const char *code; const char *desc; };

static const struct entry table[] = {
  /* --- hzd_ln_G1_and_SG1 / hzd_ln_G1_and_SG1_m (G1 phase shaping) --- */
  {"DLG1900", "G1 shaping: observed time T must be strictly positive"},
  {"DLG1910", "G1 shaping: exp(X) overflow or non-finite intermediate"},
  {"DLG1935", "G1 shaping: g1flag out of range (expected 1..6)"},
  {"DLG1950", "G1 shaping: numerical failure in limiting-case branch"},

  /* --- hzd_ln_G3_and_SG3 / hzd_ln_G3_and_SG3_m (G3 phase shaping) --- */
  {"DLG3900", "G3 shaping: observed time T must be strictly positive"},
  {"DLG3910", "G3 shaping: exp(X) overflow or non-finite intermediate"},
  {"DLG3930", "G3 shaping: numerical failure in case branch"},

  /* --- hzd_ln_eX_min1 helper --- */
  {"DLEM1D50", "ln(e^X - 1): input produces non-finite result"},

  /* --- hzd_set_rho --- */
  {"DG1RHO960", "rho setter: invalid shaping parameter for G1 phase"},

  /* --- early/late parameter <-> time transforms --- */
  {"DTRSFM910",  "parameter transform: invalid shaping arguments"},
  {"DTRSFM930",  "parameter transform: numerical failure"},
  {"DTRSFM970",  "late phase p->t: t_half <= 0"},
  {"DTRSFM990",  "late phase p->t: delta out of range"},
  {"DTRSM1010",  "late phase p->t: mu out of range"},
  {"DTRSFM1030", "late phase p->t: nu out of range"},
  {"DTRSFM1050", "late phase p->t: resulting T non-finite"},
  {"DTRSFM1070", "late phase p->t: branch selection failed"},
  {"DTRSFM1090", "late phase p->t: convergence failure"},
  {"DTRSFM1110", "late phase p->t: guard case"},
  {"DTRSFM1120", "late phase t->p: initial guess invalid"},
  {"DTRSFM1130", "late phase t->p: guard case"},
  {"DTRSFM1140", "late phase t->p: intermediate non-finite"},
  {"DTRSFM1150", "late phase t->p: observed time T must be positive"},
  {"DTRSFM1160", "late phase t->p: shaping parameter out of domain"},
  {"DTRSFM1170", "late phase t->p: shaping parameter out of domain"},
  {"DTRSFM1180", "late phase t->p: shaping parameter out of domain"},
  {"DTRSFM1190", "late phase t->p: shaping parameter out of domain"},
  {"DTRSFM1200", "late phase t->p: shaping parameter out of domain"},
  {"DTRSFM1210", "late phase t->p: non-finite intermediate"},
  {"DTRSFM1220", "late phase t->p: branch selection failed"},

  /* --- hazpred mortality (hzpm) and output (hzpo) --- */
  {"HZPM",     "hazpred mortality: internal failure"},
  {"HZPO900",  "hazpred output: invalid state"},
  {"HZPO910",  "hazpred output: allocation failure"},
  {"HZPO920",  "hazpred output: invalid state"},
  {"HZPO930",  "hazpred output: invalid state"},
  {"HZPO940",  "hazpred output: invalid state"},
  {"HZPO970",  "hazpred output: invalid state"},
  {"HZPO990",  "hazpred output: invalid state"},
  {"HZPO1000", "hazpred output: invalid state"}
};

static const int TABLE_LEN = (int)(sizeof(table) / sizeof(table[0]));

const char *hzd_error_describe(const char *code){
  int i;
  if(!code) return NULL;
  /* Trim trailing whitespace in caller strings like "HZPM " by matching
     with length-bounded comparison. */
  for(i = 0; i < TABLE_LEN; i++){
    size_t n = strlen(table[i].code);
    if(strncmp(code, table[i].code, n) == 0){
      /* Require the rest of the caller string to be NUL or whitespace so
         "HZPM" doesn't accidentally match "HZPMX". */
      char trailing = code[n];
      if(trailing == '\0' || trailing == ' ' || trailing == '\t')
        return table[i].desc;
    }
  }
  return NULL;
}
