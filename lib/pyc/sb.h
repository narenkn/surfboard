
#ifndef __SB_H
#define __SB_H

typedef unsigned long int SB_ULong;
typedef signed long int SB_Long;

/********************************************************************/
/*        Types defined here                                        */
/********************************************************************/
typedef struct {
  PyObject_HEAD
  PyObject *sig_path;
  PyObject *ocean;
  /* */
  unsigned int idcode_valid : 1;
  unsigned int wave_loaded : 1;
  unsigned int num_bits : 16; /* Number of bits in this signal */
  unsigned int num_idx;
  unsigned int event;
  unsigned int *idx_store;
  /* Store sequence of values... */
  unsigned int sq_len;
  PycReg     **sq_data;
  /* The connection with FSDB */
  fsdbTreeCBDataVar *fsdb_var;
  fsdbVarIdcode idcode;
  ffrVCTrvsHdl  var_hdl;
} Wave;
static int Wave_clear(Wave *self);
static void Wave_dealloc(Wave* self);
static PyObject *Wave_call(Wave *self, PyObject *args, PyObject *other);
static int Wave_traverse(Wave *self, visitproc visit, void *arg);
static PyObject *Wave_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int Wave_init(Wave *self, PyObject *args, PyObject *kwds);
static int Wave_print(Wave *wave, FILE *f, int);
static PyObject *Wave_getocean(Wave *self, void *closure);
static PyObject *Wave_pipe(Wave* self);
static PyObject *Wave_path(Wave* self);
static PyObject *Wave_numIdx(Wave* self);
static PyObject *Wave_Idx(Wave* self, PyObject *args);
static PyObject *Wave_creg(Wave *wave);
static PyObject *Wave_int(Wave *wave);
static PyObject *Wave_int_c(Wave *wave, PyObject *args);
static PyObject *Wave_str_c(Wave *wave, PyObject *args);
static PyObject *Wave_oct(Wave *wave);
static PyObject *Wave_hex(Wave *wave);
static PyObject *Wave_neg(Wave *wave);
static int Wave_nonzero(Wave *wave);
static PyObject *Wave_invert(Wave *wave);
static int Wave_cmp(Wave *w1, Wave *w2);
static PyObject *Wave_richcompare(PyObject *, PyObject *, int flags);
static PyObject *Wave_add(Wave *wave, PyObject *op);
static PyObject *Wave_sub(Wave *wave, PyObject *op);
static PyObject *Wave_mul(Wave *wave, PyObject *op);
static PyObject *Wave_div(Wave *wave, PyObject *op);
static PyObject *Wave_rem(Wave *wave, PyObject *op);
static PyObject *Wave_and(Wave *wave, PyObject *op);
static PyObject *Wave_or(Wave *wave, PyObject *op);
static PyObject *Wave_xor(Wave *wave, PyObject *op);
static PyObject *Wave_lshift(Wave *wave, PyObject *op);
static PyObject *Wave_rshift(Wave *wave, PyObject *op);
static int Wave_setocean(Wave *self, PyObject *value, void *closure);
#if 0
static int Wave_Sq_length(Wave *wave);
static PyObject *Wave_Sq_item(Wave *wave, int pos);
#endif


typedef struct {
  PyObject_HEAD
  static PyObject *ocean_dict;
  PyObject *waves_dict;
  PyObject *file;
  PyObject *all_waves;
  PyObject *time;
  ffrObject *fsdb_obj;
  static uint32 ocean_dict_count;
  unsigned int initDone : 1;
  unsigned int fsdbLoaded : 1;
  unsigned int surfStarted : 1;
} Ocean;
static int Ocean_clear(Ocean *self);
static int Ocean_traverse(Ocean *self, visitproc visit, void *arg);
static void Ocean_dealloc(Ocean* self);
static PyObject *Ocean_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int Ocean_init(Ocean *self, PyObject *args, PyObject *kwds);
static int Ocean_print(Ocean *wave, FILE *f, int);
static PyObject *Ocean_call(Ocean *ocean, PyObject *args, PyObject *other);
static PyObject *Ocean_newWave(Ocean *self, PyObject *args, PyObject *kwds);
static PyObject *Ocean_nextVC(Ocean *self, PyObject *args);
static PyObject *Ocean_prevVC(Ocean *self, PyObject *args);
static PyObject *Ocean_firstVC(Ocean *self, PyObject *args);
static PyObject *_Ocean_nextVC(Ocean *self, Wave *wave);
static PyObject *Ocean_startSurf(Ocean* self);
static void Ocean_endSurf(Ocean* self);
static PyObject *Ocean_surf(Ocean *self, PyObject *args);
#endif
