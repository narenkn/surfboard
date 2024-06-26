#include <Python.h>
#include <sys/stat.h>
#include <fstream>
#include <boost/algorithm/string/join.hpp>
#include "structmember.h"
#include "fsdbShr.h"
#include "ffrAPI.h"

#define STRINGIFY(x) #x
#define AT __FILE__ ":" STRINGIFY(__LINE__)

#include "py_creg.h"
#include "sb.h"
#include "module.h"
#include "callback.h"
#include "py_creg.cc"

static bool VC_prev = false, VC_first = false;

using namespace std;

PyObject *Ocean::ocean_dict = NULL;
uint32 Ocean::ocean_dict_count = 0;

static PyMemberDef Wave_members[] = {
  {"idcode", T_INT, offsetof(Wave, idcode), READONLY, "Wave's idcode"},
  {"event", T_INT, offsetof(Wave, event), READONLY, "Did Wave have an event in current simtime"},
  {NULL}  /* Sentinel */
};
static PyMethodDef Wave_methods[] = {
  {"pipe", (PyCFunction)Wave_pipe, METH_NOARGS, "Pipe the wave variable values" },
  {"path", (PyCFunction)Wave_path, METH_NOARGS, "Return the name, combining the ocean and last name" },
  {"numIdx", (PyCFunction)Wave_numIdx, METH_NOARGS, "Return number of index for this signal" },
  {"Idx", (PyCFunction)Wave_Idx, METH_VARARGS, "Return index range for this #index" },
  {"reg", (PyCFunction)Wave_creg, METH_NOARGS, "Return creg corresponding to current value of the wave" },
  {"i", (PyCFunction)Wave_int_c, METH_VARARGS, "Return int/long (control bits) corresponding to current value of the wave. Accepts one argument ::sb.DATA/::sb.CONTROL." },
  {"str", (PyCFunction)Wave_str_c, METH_VARARGS, "Return string representation of current value of the wave. Accepts one argument ::sb.HEX/sb.OCT/sb.BIN/sb.DEC." },
  {NULL}  /* Sentinel */
};
static PyGetSetDef Wave_getseters[] = {
  {"ocean", (getter)Wave_getocean, (setter)Wave_setocean, "ocean name", NULL},
  {NULL}  /* Sentinel */
};

static PyNumberMethods WaveNumMethods = {
  (binaryfunc)Wave_add,       /*	binaryfunc nb_add; */
  (binaryfunc)Wave_sub,       /*	binaryfunc nb_subtract; */
  (binaryfunc)Wave_mul,       /*	binaryfunc nb_multiply; */
  (binaryfunc)Wave_div,       /*	binaryfunc nb_divide; */
  (binaryfunc)Wave_rem,       /*	binaryfunc nb_remainder; */
  0,                          /*	binaryfunc nb_divmod; */
  0,                          /*	ternaryfunc nb_power; */
  (unaryfunc)Wave_neg,        /*	unaryfunc nb_negative; */
  (unaryfunc)Wave_int,        /*	unaryfunc nb_positive; */
  (unaryfunc)Wave_int,        /*	unaryfunc nb_absolute; */
  (inquiry)Wave_nonzero,      /*	inquiry nb_nonzero; */
  (unaryfunc)Wave_invert,     /*	unaryfunc nb_invert; */
  (binaryfunc)Wave_lshift,    /*	binaryfunc nb_lshift; */
  (binaryfunc)Wave_rshift,    /*	binaryfunc nb_rshift; */
  (binaryfunc)Wave_and,       /*	binaryfunc nb_and; */
  (binaryfunc)Wave_xor,       /*	binaryfunc nb_xor; */
  (binaryfunc)Wave_or,        /*	binaryfunc nb_or; */
  0,                          /*	coercion nb_coerce; */
  (unaryfunc)Wave_int,        /*	unaryfunc nb_int; */
  (unaryfunc)Wave_int,        /*	unaryfunc nb_long; */
  0,                          /*	unaryfunc nb_float; */
  (unaryfunc)Wave_oct,				/*	unaryfunc nb_oct; */
  (unaryfunc)Wave_hex, 				/*	unaryfunc nb_hex; */
  0,                          /*	binaryfunc nb_inplace_add; */
  0,                          /*	binaryfunc nb_inplace_subtract; */
  0,                          /*	binaryfunc nb_inplace_multiply; */
  0,                          /*	binaryfunc nb_inplace_divide; */
  0,                          /*	binaryfunc nb_inplace_remainder; */
  0,                          /*	ternaryfunc nb_inplace_power; */
  0,                          /*	binaryfunc nb_inplace_lshift; */
  0,                          /*	binaryfunc nb_inplace_rshift; */
  0,                          /*	binaryfunc nb_inplace_and; */
  0,                          /*	binaryfunc nb_inplace_xor; */
  0,                          /*	binaryfunc nb_inplace_or; */
  0,                          /*	binaryfunc nb_floor_divide; */
  0,                          /*	binaryfunc nb_true_divide; */
  0,                          /*	binaryfunc nb_inplace_floor_divide; */
  0,                          /*	binaryfunc nb_inplace_true_divide; */
};
static PySequenceMethods WaveSequMethods = {
  (inquiry)0,					/* inquiry sq_length; */
  (binaryfunc)0,					/* binaryfunc sq_concat; */
  (intargfunc)0,					/* intargfunc sq_repeat; */
  (intargfunc)0,					/* intargfunc sq_item; */
  (intintargfunc)0,					/* intintargfunc sq_slice; */
  (intobjargproc)0,					/* intobjargproc sq_ass_item; */
  (intintobjargproc)0,					/* intintobjargproc sq_ass_slice; */
  (objobjproc)0,					/* objobjproc sq_contains; */
  (binaryfunc)0,					/* binaryfunc sq_inplace_concat; */
  (intargfunc)0,					/* intargfunc sq_inplace_repeat; */
};
static PyTypeObject WaveType = {
  PyObject_HEAD_INIT(NULL)
  0,                         /*ob_size*/
  "sb.Wave",          /*tp_name*/
  sizeof(Wave),              /*tp_basicsize*/
  0,                         /*tp_itemsize*/
  (destructor)Wave_dealloc,  /*tp_dealloc*/
  (printfunc)Wave_print,     /*tp_print*/
  0,                         /*tp_getattr*/
  0,                         /*tp_setattr*/
  (cmpfunc)Wave_cmp,         /*tp_compare*/
  0,                         /*tp_repr*/
  &WaveNumMethods,           /*tp_as_number*/
  &WaveSequMethods,          /*tp_as_sequence*/
  0,                         /*tp_as_mapping*/
  0,                         /*tp_hash */
  (ternaryfunc)Wave_call,    /*tp_call*/
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC
  | Py_TPFLAGS_CHECKTYPES | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
  "Wave objects",           /* tp_doc */
  (traverseproc)Wave_traverse,		               /* tp_traverse */
  (inquiry)Wave_clear,		               /* tp_clear */
  (richcmpfunc)Wave_richcompare,/* tp_richcompare */
  0,		               /* tp_weaklistoffset */
  0,		               /* tp_iter */
  0,		               /* tp_iternext */
  Wave_methods,             /* tp_methods */
  Wave_members,             /* tp_members */
  Wave_getseters,           /* tp_getset */
  0,                        /* tp_base */
  0,                        /* tp_dict */
  0,                        /* tp_descr_get */
  0,                        /* tp_descr_set */
  0,                        /* tp_dictoffset */
  (initproc)Wave_init,      /* tp_init */
  PyType_GenericAlloc,      /* tp_alloc */
  Wave_new,                 /* tp_new */
  PyObject_GC_Del,          /* tp_free */
};

static PyMemberDef Ocean_members[] = {
  {"time", T_OBJECT_EX, offsetof(Ocean, time), READONLY, "Return current simtime"},
  {NULL}  /* Sentinel */
};
static PyGetSetDef Ocean_getseters[] = {
  {NULL}  /* Sentinel */
};
static PyMethodDef Ocean_methods[] = {
  {"newWave", (PyCFunction)Ocean_newWave, METH_VARARGS, "Get a new wave from the ocean"},
  {"nextVC", (PyCFunction)Ocean_nextVC, METH_VARARGS, "Go to the next value change of the provided wave"},
  {"prevVC", (PyCFunction)Ocean_prevVC, METH_VARARGS, "Go to the prev value change of the provided wave"},
  {"firstVC", (PyCFunction)Ocean_firstVC, METH_VARARGS, "Go to the prev value change of the provided wave"},
  {"startSurf", (PyCFunction)Ocean_startSurf, METH_NOARGS, "Start surf the ocean"},
  {"surf", (PyCFunction)Ocean_surf, METH_NOARGS, PyDoc_STR("starts the waveform playback, for all objects associated with the Ocean")},
  //    {"getWaves", (PyCFunction)Ocean_startSurf, METH_NOARGS, "Start surf the ocean"},
  {NULL}  /* Sentinel */
};
static PyTypeObject OceanType = {
  PyObject_HEAD_INIT(NULL)
  0,                         /*ob_size*/
  "sb.Ocean",         /*tp_name*/
  sizeof(Ocean),             /*tp_basicsize*/
  0,                         /*tp_itemsize*/
  (destructor)Ocean_dealloc, /*tp_dealloc*/
  (printfunc)Ocean_print,    /*tp_print*/
  0,                         /*tp_getattr*/
  0,                         /*tp_setattr*/
  0,                         /*tp_compare*/
  0,                         /*tp_repr*/
  0,                         /*tp_as_number*/
  0,                         /*tp_as_sequence*/
  0,                         /*tp_as_mapping*/
  0,                         /*tp_hash */
  (ternaryfunc)Ocean_call,   /*tp_call*/
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC, /*tp_flags*/
  "Ocean objects",           /* tp_doc */
  (traverseproc)Ocean_traverse,		               /* tp_traverse */
  (inquiry)Ocean_clear,		               /* tp_clear */
  0,		               /* tp_richcompare */
  0,		               /* tp_weaklistoffset */
  0,		               /* tp_iter */
  0,		               /* tp_iternext */
  Ocean_methods,             /* tp_methods */
  Ocean_members,             /* tp_members */
  Ocean_getseters,           /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)Ocean_init,      /* tp_init */
  PyType_GenericAlloc,       /* tp_alloc */
  Ocean_new,                 /* tp_new */
  PyObject_GC_Del,           /* tp_free */
};

#include "module.cc"
#include "callback.cc"

/********************************************************************/
/*        Wave Object                                               */
/********************************************************************/
static void
Wave_dealloc(Wave *self)
{
  DEBUG_FUNC_STR;

  PyObject_GC_UnTrack(self);
  Py_TRASHCAN_SAFE_BEGIN(self)

    Py_XDECREF(self->sig_path);
  Py_XDECREF(self->ocean);

  if (NULL != self->var_hdl) {
    self->var_hdl->ffrFree();
    self->var_hdl = NULL;
  }

  /* */
  if (NULL != self->idx_store) {
    PyMem_Free(self->idx_store);
  }

  /* as sequence */
  if (NULL != self->sq_data) {
    for (uint32 ui1=0; ui1<(self->sq_len+1); ui1++) {
      Py_DECREF((PyObject *) self->sq_data[ui1]);
    }
    PyMem_FREE(self->sq_data);
    self->sq_data = NULL;
  }

  self->ob_type->tp_free((PyObject*)self);
  Py_TRASHCAN_SAFE_END(self)
    }

static int
Wave_clear(Wave* self)
{
  DEBUG_FUNC_STR;
  Py_XDECREF((PyObject *)self->ocean);
  self->ocean = NULL;
  return 0;
}

static PyObject *
Wave_call(Wave *self, PyObject *args, PyObject *other)
{
  DEBUG_FUNC_STR;
  uint32   pos;

  /* type check */
  if (!PyArg_ParseTuple(args, "i:call", &pos)) {
    return NULL;
  }

  /* pos : npos (ex sq_len:5)
     1  :  1
     5  :  5
     0  :  0
     >5  :  0
     <0  :  0
  */
  uint32 npos = pos < 0 ? -pos : pos;
  if ((npos < 0) || (npos>self->sq_len)) {
    npos = 0;
  }

  PycReg *reg = (PycReg *) self->sq_data[npos];
  PycReg *r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
  r1->r->resizeAndCopy(*(reg->r));

  return (PyObject *) r1;
}

static int
Wave_traverse(Wave *self, visitproc visit, void *arg)
{
  DEBUG_FUNC_STR;
  int vret;

  if (self->ocean) {
    vret = visit(self->ocean, arg);
    if (vret != 0)
      return vret;
  }

  if (self->sig_path) {
    vret = visit(self->sig_path, arg);
    if (vret != 0)
      return vret;
  }

  for (uint32 ui1=0; (ui1<=self->sq_len) && (NULL != self->sq_data); ui1++) {
    vret = visit((PyObject *)self->sq_data[ui1], arg);
    if (vret != 0)
      return vret;
  }

  return 0;
}

static PyObject *
Wave_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  DEBUG_FUNC_STR;
  Wave *self;

  self = (Wave *) type->tp_alloc(type, 0);
  if (self != NULL) {
    if (0 != type->tp_init((PyObject *)self, args, kwds)) {
      Py_DECREF((PyObject *)self);
      return NULL;
    }
  }

  return (PyObject *)self;
}

static int
Wave_init(Wave *self, PyObject *args, PyObject *kwds)
{
  DEBUG_FUNC_STR;
  PyObject *sig=NULL, *tmp;

  self->sig_path = NULL;
  self->ocean = NULL;
  self->idcode_valid = 0;
  self->wave_loaded = 0;
  self->num_bits = 0;
  self->num_idx = 0;
  self->idx_store = NULL;
  self->sq_len = 0;
  self->sq_data = NULL;
  self->fsdb_var = NULL;
  self->idcode = 0;
  self->var_hdl = NULL;

  /* parse args */
  if ((!args) || (! PyArg_ParseTuple(args, "S|O", &sig, &tmp)))
    return -1; 

  if (sig) {
    tmp = self->sig_path;
    Py_INCREF(sig);
    self->sig_path = sig;
    Py_XDECREF(tmp);
  }

  return 0;
}

static int
Wave_print(Wave *wave, FILE *f, int flags)
{
  DEBUG_FUNC_STR;
  int ret;

  ret =	PyObject_Print(wave->sig_path, f, flags);
  if (0 == ret)
    fprintf(f, ":%s", (char *)(wave->sq_data[0]->r->toHex().c_str()));

  return ret;
}

static PyObject *
Wave_getocean(Wave *self, void *closure)
{
  DEBUG_FUNC_STR;
  Py_INCREF(self->ocean);
  return self->ocean;
}

static PyObject *
Wave_pipe(Wave* self)
{
  DEBUG_FUNC_STR;
  unsigned int ui1;
  PycReg *r1;
  PyObject *ret;

  /* Pipe pushes out a data which is the one that you get */
  ret = (PyObject *) self->sq_data[self->sq_len];

  /* shift by 1, copy [1] over to [0] */
  for (ui1=self->sq_len; ui1; ui1--) {
    self->sq_data[ui1] = self->sq_data[ui1-1];
  }
  r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
  self->sq_data[0] = r1;
  (0 == self->sq_len) ? r1->r->resizeAndCopy(*(((PycReg *)ret)->r)) :
    r1->r->resizeAndCopy(*(((PycReg *)self->sq_data[1])->r));

  return ret;
}

static PyObject *
Wave_path(Wave* self)
{
  DEBUG_FUNC_STR;
  return PyString_FromString(PyString_AsString(self->sig_path));
}

static PyObject *
Wave_numIdx(Wave* self)
{
  DEBUG_FUNC_STR;
  return PyInt_FromLong(self->num_idx);
}

static PyObject *
Wave_Idx(Wave* self, PyObject *args)
{
  DEBUG_FUNC_STR;
  PyObject *tup;
  PyIntObject *inp;
  Ocean    *ocean;
  unsigned int ui1;

  /* init */
  inp = NULL;
  ocean = (Ocean *) self->ocean;
  if (1 != ocean->surfStarted) {
    PyErr_SetString(PyExc_LookupError, "Ocean's surf hasn't yet started...");
    return NULL;
  }

  if (!args || (! PyArg_ParseTuple(args, "O", (PyObject **)&inp)))
    return NULL;
  if (NULL == inp)
    return NULL;

  ui1 = inp->ob_ival;
  if (ui1 >= self->num_idx) {
    PyErr_SetString(PyExc_LookupError, "Index out of bound...");
    return NULL;
  }

  tup = PyTuple_New(2);
  if (NULL != tup) {
    PyTuple_SetItem(tup, 0, PyInt_FromLong(self->idx_store[(ui1<<1)]));
    PyTuple_SetItem(tup, 1, PyInt_FromLong(self->idx_store[(ui1<<1)+1]));
  }

  return tup;
}

static PyObject *
Wave_creg(Wave *wave)
{
  DEBUG_FUNC_STR;
  /* Find in which operand is the Wave type .. */
  if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  }

  PycReg *r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
  r1->r->resizeAndCopy(*(wave->sq_data[0]->r));
  return (PyObject *) r1;
}

static PyObject *
Wave_int(Wave *wave)
{
  DEBUG_FUNC_STR;
  /* Find in which operand is the Wave type .. */
  if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  }

  return PycReg_lint(wave->sq_data[0]);
}

static PyObject *
Wave_int_c(Wave *wave, PyObject *args)
{
  DEBUG_FUNC_STR;

  /* Find in which operand is the Wave type .. */
  if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  }

  return PycReg_lint_c(wave->sq_data[0], args);
}

static PyObject *
Wave_str_c(Wave *wave, PyObject *args)
{
  DEBUG_FUNC_STR;

  /* Find in which operand is the Wave type .. */
  if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  }

  return PycReg_str_c(wave->sq_data[0], args);
}

static PyObject *
Wave_oct(Wave *wave)
{
  DEBUG_FUNC_STR;

  /* Find in which operand is the Wave type .. */
  if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  }

  return PycReg_oct(wave->sq_data[0]);
}

static PyObject *
Wave_hex(Wave *wave)
{
  DEBUG_FUNC_STR;

  /* Find in which operand is the Wave type .. */
  if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  }

  return PycReg_hex(wave->sq_data[0]);
}

static PyObject *
Wave_neg(Wave *wave)
{
  DEBUG_FUNC_STR;
  /* Find in which operand is the Wave type .. */
  if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  }

  return PycReg_neg(wave->sq_data[0]);
}

static int
Wave_nonzero(Wave *wave)
{
  DEBUG_FUNC_STR;
  /* Find in which operand is the Wave type .. */
  if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return 0;
  }

  return PycReg_nonzero(wave->sq_data[0]);
}

static PyObject *
Wave_invert(Wave *wave)
{
  DEBUG_FUNC_STR;
  /* Find in which operand is the Wave type .. */
  if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  }

  return PycReg_invert(wave->sq_data[0]);
}

static int
Wave_cmp(Wave *w1, Wave *w2)
{
  DEBUG_FUNC_STR;

  /* Find in which operand is the Wave type .. */
  if ((! PyObject_TypeCheck((PyObject *)w1, &WaveType)) || (! PyObject_TypeCheck(w2, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return -1;
  }

  return PycReg_cmp(w1->sq_data[0], w2->sq_data[0]);
}

static PyObject *
Wave_richcompare(PyObject *o1, PyObject *o2, int flags)
{

  DEBUG_FUNC_STR;
  //	cerr << "Wave_richcompare called" << endl;
  if ((PyObject_TypeCheck(o1, &WaveType)) &&
      (PyObject_TypeCheck(o2, &WaveType))) {
    /* wave, wave */
    return PycReg_richcompare((PyObject *) ((Wave *)o1)->sq_data[0], (PyObject *) ((Wave *)o2)->sq_data[0], flags);
  } else if ((PyObject_TypeCheck(o1, &WaveType)) &&
             (PyInt_Check(o2) || PyLong_Check(o2))) {
    /* wave, int */
    return PycReg_richcompare((PyObject *) ((Wave *)o1)->sq_data[0], o2, flags);
  } else if ((PyObject_TypeCheck(o2, &WaveType)) &&
             (PyInt_Check(o1) || PyLong_Check(o1))) {
    /* int, wave */
    return PycReg_richcompare((PyObject *) ((Wave *)o2)->sq_data[0], o1, flags);
  } else {
    PyErr_SetString(PyExc_TypeError, "Comparision can't be made!");
    return NULL;
  }
}

static PyObject *
Wave_add(Wave *wave, PyObject *op)
{
  DEBUG_FUNC_STR;
  Wave *w1;

  /* Find in which operand is the Wave type .. */
  w1 = wave;
  if ((! PyObject_TypeCheck((PyObject *)wave, &WaveType)) && (! PyObject_TypeCheck(op, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &WaveType)) {
    w1 = (Wave *)op;
    op = (PyObject *)wave;
  }

  return PycReg_add(w1->sq_data[0], op);
}

static PyObject *
Wave_sub(Wave *wave, PyObject *op)
{
  DEBUG_FUNC_STR;
  Wave *w1;

  /* Find in which operand is the Wave type .. */
  w1 = wave;
  if ((! PyObject_TypeCheck((PyObject *)wave, &WaveType)) && (! PyObject_TypeCheck(op, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &WaveType)) {
    w1 = (Wave *)op;
    op = (PyObject *)wave;
  }

  return PycReg_sub(w1->sq_data[0], op);
}

static PyObject *
Wave_mul(Wave *wave, PyObject *op)
{
  DEBUG_FUNC_STR;
  Wave *w1;

  /* Find in which operand is the Wave type .. */
  w1 = wave;
  if ((! PyObject_TypeCheck((PyObject *)wave, &WaveType)) && (! PyObject_TypeCheck(op, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &WaveType)) {
    w1 = (Wave *)op;
    op = (PyObject *)wave;
  }

  return PycReg_mul(w1->sq_data[0], op);
}

static PyObject *
Wave_div(Wave *wave, PyObject *op)
{
  DEBUG_FUNC_STR;
  Wave *w1;

  /* Find in which operand is the Wave type .. */
  w1 = wave;
  if ((! PyObject_TypeCheck((PyObject *)wave, &WaveType)) && (! PyObject_TypeCheck(op, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &WaveType)) {
    w1 = (Wave *)op;
    op = (PyObject *)wave;
  }

  return PycReg_div(w1->sq_data[0], op);
}

static PyObject *
Wave_rem(Wave *wave, PyObject *op)
{
  DEBUG_FUNC_STR;
  Wave *w1;

  /* Find in which operand is the Wave type .. */
  w1 = wave;
  if ((! PyObject_TypeCheck((PyObject *)wave, &WaveType)) && (! PyObject_TypeCheck(op, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &WaveType)) {
    w1 = (Wave *)op;
    op = (PyObject *)wave;
  }

  return PycReg_rem(w1->sq_data[0], op);
}

static PyObject *
Wave_and(Wave *wave, PyObject *op)
{
  DEBUG_FUNC_STR;
  Wave *w1;

  /* Find in which operand is the Wave type .. */
  w1 = wave;
  if ((! PyObject_TypeCheck((PyObject *)wave, &WaveType)) && (! PyObject_TypeCheck(op, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &WaveType)) {
    w1 = (Wave *)op;
    op = (PyObject *)wave;
  }

  return PycReg_and(w1->sq_data[0], op);
}

static PyObject *
Wave_or(Wave *wave, PyObject *op)
{
  DEBUG_FUNC_STR;
  Wave *w1;

  /* Find in which operand is the Wave type .. */
  w1 = wave;
  if ((! PyObject_TypeCheck((PyObject *)wave, &WaveType)) && (! PyObject_TypeCheck(op, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &WaveType)) {
    w1 = (Wave *)op;
    op = (PyObject *)wave;
  }

  return PycReg_or(w1->sq_data[0], op);
}

static PyObject *
Wave_xor(Wave *wave, PyObject *op)
{
  DEBUG_FUNC_STR;
  Wave *w1;

  /* Find in which operand is the Wave type .. */
  w1 = wave;
  if ((! PyObject_TypeCheck((PyObject *)wave, &WaveType)) && (! PyObject_TypeCheck(op, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &WaveType)) {
    w1 = (Wave *)op;
    op = (PyObject *)wave;
  }

  return PycReg_xor(w1->sq_data[0], op);
}

static PyObject *
Wave_lshift(Wave *wave, PyObject *op)
{
  DEBUG_FUNC_STR;
  Wave *w1;

  /* Find in which operand is the Wave type .. */
  w1 = wave;
  if ((! PyObject_TypeCheck((PyObject *)wave, &WaveType)) && (! PyObject_TypeCheck(op, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &WaveType)) {
    w1 = (Wave *)op;
    op = (PyObject *)wave;
  }

  return PycReg_lshift(w1->sq_data[0], op);
}

static PyObject *
Wave_rshift(Wave *wave, PyObject *op)
{
  DEBUG_FUNC_STR;
  Wave *w1;

  /* Find in which operand is the Wave type .. */
  w1 = wave;
  if ((! PyObject_TypeCheck((PyObject *)wave, &WaveType)) && (! PyObject_TypeCheck(op, &WaveType))) {
    PyErr_SetString(PyExc_TypeError, "Wave operation called on non-Wave objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &WaveType)) {
    w1 = (Wave *)op;
    op = (PyObject *)wave;
  }

  return PycReg_rshift(w1->sq_data[0], op);
}

#if 0
static int
Wave_Sq_length(Wave *wave)
{
  DEBUG_FUNC_STR;
  return -1; /*FIXME */
}

static PyObject * /*FIXME */
Wave_Sq_item(Wave *wave, int pos)
{
  DEBUG_FUNC_STR;

  uint32 npos = pos < 0 ? -pos : pos;
  if ((npos < 0) || (npos>wave->sq_len)) {
    npos = 0;
  }

  PycReg *reg = (PycReg *) wave->sq_data[npos];
  PycReg *r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
  r1->r->resizeAndCopy(*(reg->r));

  return (PyObject *) r1;
}
#endif

/********************************************************************/
/*        Ocean Object                                               */
/********************************************************************/
static int
Ocean_clear(Ocean *self)
{
  DEBUG_FUNC_STR;

  if (NULL != self->waves_dict) {
    PyObject *key, *value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(self->waves_dict, &pos, &key, &value)) {
      Py_DECREF(key);
      Py_DECREF(value);
    }
    Py_DECREF(self->waves_dict);
  }
  self->waves_dict = NULL;

  if (NULL != self->all_waves) {
    PyList_Type.tp_traverse(self->all_waves, delete_visitproc, NULL);
    Py_DECREF(self->all_waves);
  }
  self->all_waves = NULL;

  Py_XDECREF(self->time);
  self->time = NULL;

  assert (0 != self->ocean_dict_count);
  assert (self->ocean_dict);

  return 0;
}

static int
Ocean_traverse(Ocean *self, visitproc visit, void *arg)
{
  DEBUG_FUNC_STR;
  int vret;

  if (self->waves_dict) {
    vret = visit(self->waves_dict, arg);
    if (vret != 0)
      return vret;
  }

  if (self->file) {
    vret = visit(self->file, arg);
    if (vret != 0)
      return vret;
  }

  return 0;
}

static void
Ocean_dealloc(Ocean* self)
{
  DEBUG_FUNC_STR;
  PyObject *key, *value;
  Py_ssize_t pos;

  PyObject_GC_UnTrack(self);
  Py_TRASHCAN_SAFE_BEGIN(self)

    Py_XDECREF(self->file);

  if (NULL != self->fsdb_obj) {
    self->fsdb_obj->ffrUnloadSignals();
    self->fsdb_obj->ffrClose();
    self->fsdb_obj = NULL;
  }

  if (NULL != self->waves_dict) {
    pos = 0;
    while (PyDict_Next(self->waves_dict, &pos, &key, &value)) {
      Py_DECREF(key);
      Py_DECREF(value);
    }
    Py_DECREF(self->waves_dict);
  }

  if (NULL != self->all_waves) {
    PyList_Type.tp_traverse(self->all_waves, delete_visitproc, NULL);
    Py_DECREF(self->all_waves);
  }

  Py_XDECREF(self->time);

  /* */
  assert(0 != self->ocean_dict_count);
  self->ocean_dict_count--;
  if (0 == self->ocean_dict_count) {
    pos = 0;
    while (PyDict_Next(self->ocean_dict, &pos, &key, &value)) {
      Py_DECREF(key);
      Py_DECREF(value);
    }
    Py_DECREF(self->ocean_dict);
    self->ocean_dict = NULL;
  }

  /* */
  self->ob_type->tp_free((PyObject*)self);
  Py_TRASHCAN_SAFE_END(self)
    }

static PyObject *
Ocean_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  DEBUG_FUNC_STR;
  Ocean *self;
  PyObject *file;

  self = (Ocean *)type->tp_alloc(type, 0);
  if (0 != type->tp_init((PyObject *)self, args, kwds))
    return NULL;

  if (args && PyArg_ParseTuple(args, "S", &file)) {
    /* alloc static members */
    if (0 == self->ocean_dict_count) {
      assert(NULL == self->ocean_dict);
      self->ocean_dict = PyDict_New();
      self->ocean_dict_count++;
    }
    PyObject *ob = PyDict_GetItem(self->ocean_dict, file);
    if (NULL != ob) {
      Py_DECREF(self);
      Py_INCREF(ob);
      return ob;
    }
    Py_INCREF(self);
    Py_INCREF(file);
    PyDict_SetItem(self->ocean_dict, file, (PyObject *)self);
  }

  return (PyObject *)self;
}

static int
Ocean_init(Ocean *self, PyObject *args, PyObject *kwds)
{
  DEBUG_FUNC_STR;
  PyObject *file;

  static char *kwlist[] = {"file", NULL};

  /* init */
  file = NULL;

  if (self->initDone)
    return 0;

  if (self != NULL) {
    self->waves_dict = NULL;
    self->file = NULL;
    self->fsdb_obj = NULL;
    self->initDone = 0;
    self->fsdbLoaded = 0;
    self->surfStarted = 0;
    self->all_waves = NULL;
    self->time = PyInt_FromLong(0);
  }

  assert (NULL == self->waves_dict);
  self->waves_dict = PyDict_New();
  if (NULL == self->waves_dict) {
    return -1;
  }

  /* */
  if ((! args) || (! PyArg_ParseTupleAndKeywords(args, kwds, "S", kwlist, &file)))
    return -1;

  /* Check and load smaller fsdb */
  bool small_fsdb_exists = false;
  char buf[512];
  do {
    struct stat fstat, sfstat;

    /* decide if need to create new fsdb */
    sprintf(buf, "%s", PyString_AS_STRING(file));
    if (0 != stat(buf, &fstat)) {
      break;
    }
    sprintf(buf, "%s.%x", PyString_AS_STRING(file), geteuid());
    if (0 == stat(buf, &sfstat)) {
      small_fsdb_exists = (fstat.st_mtime < sfstat.st_mtime) ? true : false;
    }
  } while (0);

  if (small_fsdb_exists) {
    file = PyString_FromString(buf);
  }
  if (NULL != file) {
    Py_INCREF(file);
    self->file = file;

    /* Signifies success of init */
    self->initDone = 1;
  }
  if (small_fsdb_exists) {
    Py_XDECREF(file);
  }

  return 0;
}

static int
Ocean_print(Ocean *wave, FILE *f, int flags)
{
  DEBUG_FUNC_STR;
  fprintf(f, "Ocean:");
  PyObject_Print(wave->file, f, flags);
  return 0;
}

static PyObject *
Ocean_call(Ocean *ocean, PyObject *args, PyObject *other)
{
  DEBUG_FUNC_STR;
  string   opt;
  char     *s1;

  /* type check */
  if (!PyArg_ParseTuple(args, "s:call", &s1)) {
    return NULL;
  }
  opt = s1;

  /* */
  if (1 != ocean->surfStarted) {
    PyErr_SetString(PyExc_LookupError, "Ocean's surf hasn't yet started...");
    return NULL;
  }

  if (opt == "time") {
    Py_INCREF(ocean->time);
    return ocean->time;
  } else if (opt == "unit") {
    return PyString_FromString(ocean->fsdb_obj->ffrGetScaleUnit());
  } else
    return NULL;
	
  /* Never reached probably */
  return (PyObject *)ocean;
}

static PyObject *
Ocean_prevVC(Ocean *self, PyObject *args)
{
  DEBUG_FUNC_STR;
  PyObject *ret;

  VC_prev = true;
  ret = Ocean_nextVC(self, args);
  VC_prev = false;

  return ret;
}

static PyObject *
Ocean_firstVC(Ocean *self, PyObject *args)
{
  DEBUG_FUNC_STR;
  PyObject *ret;

  VC_first = true;
  ret = Ocean_nextVC(self, args);
  VC_first = false;

  return ret;
}

static PyObject *
_Ocean_nextVC(Ocean *self, Wave *wave)
{
  DEBUG_FUNC_STR;
  PyObject *ret;
  Wave *w1;

  /* Error checking */
  if (! self->surfStarted) {
    PyErr_SetString(PyExc_LookupError, "Please start surf before nextVC...");
    return NULL;
  } else if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    PyErr_SetString(PyExc_KeyError, "Not a wave type object...");
    return NULL;
  } else if ((PyObject *)self != wave->ocean) {
    PyErr_SetString(PyExc_KeyError, "Wave had been obtained from another ocean, can't use it here...");
    return NULL;
  }

  /* Go to the next value change of object */
  ret = Py_False;
  Py_INCREF(ret);
  if (VC_prev) {
    if (FSDB_RC_SUCCESS != wave->var_hdl->ffrGotoPrevVC()) {
      return ret;
    }
  } else if (VC_first) {
    if (FSDB_RC_SUCCESS != wave->var_hdl->ffrGotoTheFirstVC()) {
      return ret;
    }
  } else if (FSDB_RC_SUCCESS != wave->var_hdl->ffrGotoNextVC()) {
    return ret;
  }
  Py_DECREF(ret);

  /* Update all objects to the value at this time... */
  uint32 ui1, ui2, ui3, ui4, ui5, ui6, ui7;
  uint32 val_1, val_0;
  byte_T *vc_ptr;
  fsdbTag64 t1, time;
  wave->var_hdl->ffrGetXTag(&time);
  for (ui2=PyList_Size(self->all_waves), ui1=0; ui1<ui2; ui1++) {
    w1 = (Wave *) PyList_GetItem((PyObject *)self->all_waves, ui1);
    w1->event = 0;
    t1 = time;
    w1->var_hdl->ffrGotoXTag(&t1);
    /* Get value and apply to signal */
    w1->var_hdl->ffrGetVC(&vc_ptr);
    ui6 = w1->num_bits;
    for (ui4=0, ui5=0; (ui4<ui6) && (NULL != vc_ptr); ui4+=32, ui5++) {
      val_1 = 0; val_0 = 0;
      ui7 = ((ui4+32) > ui6) ? ui6 : ui4+32;
      for (ui3=ui4; ui3<ui7; ui3++) {
        // std::cerr << "vc_ptr:" << ui6-ui3-1<<endl;
        if ( (w1->dtidcode < FSDB_DT_VHDL_BOOLEAN) ||
             (w1->dtidcode > FSDB_DT_VHDL_INTEGER_X_VALUE) ) { /* Verilog */
          switch(vc_ptr[ui6-ui3-1]) {
          case FSDB_BT_VCD_X:
            val_0 |= 1<<(ui3-ui4);
          case FSDB_BT_VCD_Z:
            val_1 |= 1<<(ui3-ui4);
            break;
          case FSDB_BT_VCD_1:
            val_0 |= 1<<(ui3-ui4);
            break;
          default:
            break;
          }
        } else { /* VHDL */
          switch(vc_ptr[ui6-ui3-1]) {
          case FSDB_BT_VHDL_STD_ULOGIC_0:
            break;
          case FSDB_BT_VHDL_STD_ULOGIC_Z:
            val_1 |= 1<<(ui3-ui4);
            break;
          case FSDB_BT_VHDL_STD_ULOGIC_1:
            val_0 |= 1<<(ui3-ui4);
            break;
          default:
            val_0 |= 1<<(ui3-ui4);
            val_1 |= 1<<(ui3-ui4);
            break;
          }
        }
      }
      if ((0 == w1->event) && (val_0 != w1->sq_data[0]->r->getDataWord(ui5)))
        w1->event = 1;
      if ((0 == w1->event) && (val_1 != w1->sq_data[0]->r->getControlWord(ui5)))
        w1->event = 1;
      w1->sq_data[0]->r->setDataWord(ui5, val_0);
      w1->sq_data[0]->r->setControlWord(ui5, val_1);
    }
  }

  /* update ocean's time */
  uint64 time_ul;
  time_ul = time.H; time_ul <<= 32; time_ul |= time.L;
  Py_DECREF(self->time);
  self->time = PyInt_FromLong(time_ul);

  /* return success */
  ret = Py_True;
  Py_INCREF(ret);
  return ret;
}

static PyObject *
Ocean_nextVC(Ocean *self, PyObject *args)
{
  PyObject *var;
  Wave *wave;

  /* Check for args */
  var = NULL;
  if (!args || (! PyArg_ParseTuple(args, "O", &var)))
    return NULL;
  if (NULL == var) {
    PyErr_SetString(PyExc_AttributeError, "No value provided for wave");
    return NULL;
  }
  wave = (Wave *)var;

  return _Ocean_nextVC(self, wave);
}

static PyObject *
Ocean_newWave(Ocean *self, PyObject *args, PyObject *kwds)
{
  DEBUG_FUNC_STR;
  static PyObject *var;
  Wave   *wave;
  uint32 sq_len;

  /* init */
  sq_len = 0;
  var = NULL;

  //	static char *kwlist[] = {"sig", "pipe", NULL};
  //	if (!args || (! PyArg_ParseTupleAndKeywords(args, kwds, "S|i", kwlist, &var, &sq_len)))
  if (!args || (! PyArg_ParseTuple(args, "S|i", &var, &sq_len)))
    return NULL;

  if (!(self->initDone) || (self->surfStarted)) {
    PyErr_SetString(PyExc_LookupError, "New Waves should be obtained before Starting to Surf...");
    return NULL;
  }

  if (NULL == var) {
    PyErr_SetString(PyExc_AttributeError, "No value provided for wave");
    return NULL;
  }

  /* Check if it already exists */
  wave = (Wave *)PyDict_GetItemString((PyObject *)self->waves_dict, PyString_AsString(var));
  if (NULL != wave) {
    Py_INCREF(wave);
    return (PyObject *)wave;
  }

  /* Create the wave */
  wave = (Wave *) Wave_new(&WaveType, args, kwds);
  if (NULL == wave)
    return NULL;
  assert (wave);
  Py_INCREF(self);
  wave->ocean = (PyObject *) self;
  PyDict_SetItemString((PyObject *)self->waves_dict, PyString_AsString(var), (PyObject *)wave);

  /* maintain pipe of signal values */
  PycReg **sq_data = (PycReg **) PyMem_MALLOC(sizeof(PycReg *) * (sq_len+1));
  if (NULL == sq_data) {
    PyErr_SetString(PyExc_MemoryError, "Unable to allocate requested memory...");
    return 0;
  }
  for (uint32 ui1=0; ui1<(sq_len+1); ui1++) {
    wave->sq_len = ui1;
    sq_data[ui1] = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    if (NULL == sq_data[ui1]) {
      PyErr_SetString(PyExc_MemoryError, "Unable to allocate requested memory...");
      return 0;
    }
    Py_INCREF((PyObject *) (sq_data[ui1]));
  }
  wave->sq_len = sq_len;
  wave->sq_data = sq_data;

  return (PyObject *)wave;
}

/* Methodology to keep track of which hierarchy we are in... */
static vector<std::string> path;
static string current_hier;

bool_T
MyTreeCBFunc (fsdbTreeCBType cb_type, void *client_data, void *cb_data)
{
  DEBUG_FUNC_STR;
  fsdbTreeCBDataVar *var;
  Ocean *ocean = (Ocean *) client_data;
  string::size_type ui1;
  unsigned int ui4, ui5, *ui_store;
  string sig, s1;
  PyObject *osig;
  Wave     *wsig;
  const char *c1;

  /* init */
  ui_store = NULL;
  // cerr << "entry MyTreeCBFunc" << endl;

  switch (cb_type) {
  case FSDB_TREE_CBT_BEGIN_TREE:
    break;
  // case FSDB_TREE_CBT_ARRAY_BEGIN:
  case FSDB_TREE_CBT_RECORD_BEGIN:
  case FSDB_TREE_CBT_BUS_BEGIN:
  case FSDB_TREE_CBT_STRUCT_BEGIN:
  case FSDB_TREE_CBT_SCOPE:
    if (FSDB_TREE_CBT_ARRAY_BEGIN == cb_type) {
      fsdbDTArrayRec *scope = (fsdbDTArrayRec*) cb_data;
      c1 = (char *) scope->name;
    } else if (FSDB_TREE_CBT_RECORD_BEGIN == cb_type) {
      fsdbDTRecordRec *scope = (fsdbDTRecordRec*) cb_data;
      c1 = (char *) scope->name;
    } else if (FSDB_TREE_CBT_BUS_BEGIN == cb_type) {
      fsdbTreeCBDataScope *scope = (fsdbTreeCBDataScope*) cb_data;
      c1 = (char *) scope->name;
    } else if (FSDB_TREE_CBT_STRUCT_BEGIN == cb_type) {
      fsdbDTStructRec *scope = (fsdbDTStructRec*) cb_data;
      c1 = (char *) scope->name;
    } else if (FSDB_TREE_CBT_SCOPE == cb_type) {
      fsdbTreeCBDataScope *scope = (fsdbTreeCBDataScope*) cb_data;
      c1 = (char *) scope->name;
    }
    path.push_back(std::string{c1});
    current_hier = boost::algorithm::join(path, ".");
    break;
  // case FSDB_TREE_CBT_ARRAY_END:
  case FSDB_TREE_CBT_RECORD_END:
  case FSDB_TREE_CBT_BUS_END:
  case FSDB_TREE_CBT_STRUCT_END:
  case FSDB_TREE_CBT_UPSCOPE:
    path.pop_back();
    current_hier = boost::algorithm::join(path, ".");
    break;
  case FSDB_TREE_CBT_VAR:
    var = (fsdbTreeCBDataVar*)cb_data;
    sig = current_hier + ".";
    /* Remove trailing [] */
    s1 = (char *) var->name;
    // if (current_hier.find("rgx_usc3_smp_tb_top.i_rgx_usc3_smp.i_rgx_usc3_smp_tpxcache.g_rgx_usc3_smp_tpxcache_backend[3].i_rgx_usc3_smp_tpxcache_backend") != std::string::npos) {
    //   treedebug = true;
    // if ("sys.pkg00.mpu00.df_tcdx_t4.TCDX1.TREQQ." == sig) {
    //   cout << "Var " << sig << s1 << endl;
    // } else       treedebug = false;
    ui5 = 0;
    do {
      ui1 = s1.rfind("[");
      if (ui1 == string::npos)
        break;
      /* Store the index in an array */
      ui5 += 2;
      if (ui_store == NULL)
        ui_store = (unsigned int *) PyMem_MALLOC(ui5 * sizeof(unsigned int));
      else
        ui_store = (unsigned int *) PyMem_REALLOC(ui_store, ui5 * sizeof(unsigned int));
      ui5 -= 2;
      /* Remove [] */
      c1 = s1.c_str() + ui1 + 1;
      ui4 = sscanf(c1, "%d:%d", (ui_store+ui5), (ui_store+ui5+1));
      if (ui4 == 1) {
        (*(ui_store+ui5)) --;
        (*(ui_store+ui5+1)) --;
        // cerr << "index:(" << ui4 << "):" << ui_store[ui5] << ":" << ui_store[ui5+1] << endl;
      }
      s1.resize(ui1);
      ui5 += 2;
    } while (0);
    sig += s1;
    osig = PyDict_GetItemString(ocean->waves_dict, (char *) sig.c_str());
    //cerr << "Got variable " << sig << endl;
    if (NULL != osig) {
      //cerr << "Found variable:" << sig << " num_idx:" << (ui5>>1) << endl;
      wsig = (Wave *) osig;
      wsig->idcode = var->u.idcode;
      wsig->dtidcode = var->dtidcode;
      wsig->idcode_valid = 1;
      wsig->num_idx = ui5>>1;
      wsig->idx_store = ui_store;
      wsig->fsdb_var = var;
    }
    break;
  default:
    break;
  }

  // cerr << "exit MyTreeCBFunc" << endl;
  return true;
}

static PyObject *
Ocean_startSurf(Ocean* self)
{
  DEBUG_FUNC_STR;
  char *fname;
  uint32  all_waves_size, ui2;
  PyObject *all_waves;
  Wave *wave;

  //	cerr << "enter startSurf" << endl;
  if ((1 != self->initDone) || (self->surfStarted) || (NULL == self->file)) {
    PyErr_SetString(PyExc_AttributeError, "Internal error 1, in surf");
    return NULL;
  }

  /* Time initialised to 0 */
  ((PyIntObject *)self->time)->ob_ival = 0;

  /* Check for new variable additions */
  all_waves = PyDict_Values(self->waves_dict);
  all_waves_size = PyList_Size(all_waves);
  if (1 == self->fsdbLoaded) {
    for (ui2=0; ui2<all_waves_size; ui2++) {
      wave = (Wave *) PyList_GetItem(all_waves, ui2);
      if (0 == wave->wave_loaded) {
        self->fsdbLoaded = 0;
        break;
      }
    }

    if (1 == self->fsdbLoaded) {
      Py_DECREF(all_waves);
      PyObject *ret = Py_True;
      Py_INCREF(ret);
      return ret;
    }

    /* */
    for (ui2=0; ui2<all_waves_size; ui2++) {
      wave = (Wave *) PyList_GetItem(all_waves, ui2);
      if (NULL != wave->var_hdl) wave->var_hdl->ffrFree();
    }
    assert(self->fsdb_obj);
    self->fsdb_obj->ffrUnloadSignals();
    self->fsdb_obj->ffrClose();
    self->fsdb_obj = NULL;
  }
  assert (0 == self->fsdbLoaded);

  /* Check and open fsdb file */
  fname = PyString_AS_STRING(self->file);
  ffrObject::ffrInfoSuppress(true);
  if (0 == ffrObject::ffrIsFSDB(fname)) {
    PyErr_SetString(PyExc_KeyError, "Not an fsdb file.");
    return NULL;
  }
  self->fsdb_obj = ffrObject::ffrOpen2(fname, MyTreeCBFunc, (void *)self);
  if (NULL == self->fsdb_obj) {
    PyErr_SetString(PyExc_KeyError, "Internal : ffrObject::ffrOpen2() failed.");
    return NULL;
  }

  /* Read all vars */
  self->fsdb_obj->ffrReadScopeVarTree();

  self->fsdbLoaded = 1;
  self->surfStarted = 1;

  /* check if all waves inside ocean got assigned.. */
  for (ui2=0; ui2<all_waves_size; ui2++) {
    wave = (Wave *) PyList_GetItem(all_waves, ui2);
    //		if (! PyObject_TypeCheck((PyObject *)wave, &WaveType)) {
    //			cerr << "Not wave type..." << wave << endl;
    //		}
    //		cerr << "Iterating " << (string) PyString_AsString(wave->sig_path) << endl;
    if (1 != wave->idcode_valid) {
      PyErr_SetString(PyExc_KeyError, PyString_AsString(wave->sig_path));
      return NULL;
    }
    /* */
    self->fsdb_obj->ffrAddToSignalList(wave->idcode);
    wave->wave_loaded = 1;
  }

  /* Load vc */
  self->fsdb_obj->ffrLoadSignals();

  /* */
  for (ui2=0; ui2<all_waves_size; ui2++) {
    wave = (Wave *) PyList_GetItem(all_waves, ui2);
    if (NULL != wave->var_hdl)
      continue;
    wave->var_hdl = self->fsdb_obj->ffrCreateVCTraverseHandle(wave->idcode);
    if (NULL == wave->var_hdl) {
      PyErr_SetString(PyExc_SystemError, "Internal Error " AT);
      return NULL;
    }
    wave->num_bits = wave->var_hdl->ffrGetBitSize();
    for (uint32 ui3=0; ui3<=wave->sq_len; ui3++) {
      wave->sq_data[ui3]->r->resize(wave->num_bits-1, 0);
    }
    if (FSDB_BYTES_PER_BIT_1B != wave->var_hdl->ffrGetBytesPerBit()) {
      PyErr_SetString(PyExc_LookupError, "Variable big : contact Narendran Kumaragurunathan for a fix");
      return NULL;
    }
  }

  /* */
  Py_XDECREF(self->all_waves);
  self->all_waves = all_waves;

  //	cerr << "exit startSurf" << endl;
  PyObject *ret = Py_True;
  Py_INCREF(ret);
  return ret;
}

void
Ocean_endSurf(Ocean* self)
{
  self->surfStarted = 0;
}

static PyObject *
Ocean_surf(Ocean *self, PyObject *args)
{
  DEBUG_FUNC_STR;
  PyObject *ret = Py_False, *ob1;
  PyObject *idcode_map = NULL;
  fsdbVarIdcode *sig_arr = NULL, var_idcode;
  Wave **sig_arr_ptr = NULL;
  /* */
  ffrTimeBasedVCTrvsHdl tb_vc_trvs_hdl = NULL;
  fsdbTag64 time, t1;
  uint32 sig_num;
  byte_T *vc_ptr;
  PyObject *key, *value;
  Wave *w1;
  Py_ssize_t pos = 0;

  /* Pre init calls */
  if (SimModule::simmodule_list) {
    if (0 != PyList_Type.tp_traverse(SimModule::simmodule_list, _SimModule_surf, (void *)"options")) {
      PyErr_SetString(PyExc_SystemError, "Internal Error " AT);
      return NULL;
    }
  }
  if (Callback::simmodule_list) {
    if (0 != PyList_Type.tp_traverse(Callback::simmodule_list, _Callback_surf, (void *)"options")) {
      PyErr_SetString(PyExc_SystemError, "Internal Error " AT);
      return NULL;
    }
  }

  /* init */
  Py_XDECREF(self->time);
  self->time = PyInt_FromLong(0);
  terminate_surf = false;

  /* Time 0 calls */
  if (SimModule::simmodule_list) {
    if (0 != PyList_Type.tp_traverse(SimModule::simmodule_list, _SimModule_surf, (void *)"early_initial")) {
      PyErr_SetString(PyExc_SystemError, "Internal Error " AT);
      return NULL;
    }
  }
  if (Callback::simmodule_list) {
    if (0 != PyList_Type.tp_traverse(Callback::simmodule_list, _Callback_surf, (void *)"early_initial")) {
      PyErr_SetString(PyExc_SystemError, "Internal Error " AT);
      return NULL;
    }
  }

  /* */
  if (SimModule::simmodule_list) {
    if (0 != PyList_Type.tp_traverse(SimModule::simmodule_list, _SimModule_surf, (void *)"initial")) {
      PyErr_SetString(PyExc_SystemError, "Internal Error " AT);
      return NULL;
    }
  }
  if (Callback::simmodule_list) {
    if (0 != PyList_Type.tp_traverse(Callback::simmodule_list, _Callback_surf, (void *)"initial")) {
      PyErr_SetString(PyExc_SystemError, "Internal Error " AT);
      return NULL;
    }
  }

  /* At this point all init should have been done */
  if (terminate_surf) {
    ret = Py_True;
    goto Ocean_surf_error_release_mem;
  }

  /* Initialise surfboard and connect waves */
  ob1 = Ocean_startSurf(self);
  if (NULL == ob1) {
    PyErr_SetString(PyExc_SystemError, "Internal Error " AT);
    return ob1;
  }
  Py_DECREF(ob1);

  /* checks */
  if ((NULL == self->fsdb_obj) || (NULL == self->time)) {
    PyErr_SetString(PyExc_SystemError, "FSDB object not initialised for Ocean Object : internal error?");
    return NULL;
  }

  /* initialise datastruct */
  sig_num = PyDict_Size(self->waves_dict);
  sig_arr = new fsdbVarIdcode[sig_num];
  sig_arr_ptr = new Wave *[sig_num];

  /* We know that this dictionary's value's refcount will not be decremented
     hence we can use waves, but remember to kill the dictionay at the end of this
     function */
  idcode_map = PyDict_New();
  uint32 ui2;
  for (ui2 = 0; PyDict_Next(self->waves_dict, &pos, &key, &value); ui2++) {
    w1 = (Wave *) value;
    sig_arr_ptr[ui2] = w1;
    sig_arr[ui2] = w1->idcode;
    w1->event = 0;
    if (0 != PyDict_SetItem(idcode_map, PyInt_FromLong(w1->idcode), (PyObject *)w1))
      goto Ocean_surf_error_release_mem;
    assert (ui2<sig_num);
  }
  assert (ui2 == sig_num);

  //
  // Create a time-based traverse handle to encapsulate the signals to be
  // traversed.
  //
  tb_vc_trvs_hdl = self->fsdb_obj->ffrCreateTimeBasedVCTrvsHdl(sig_num, sig_arr);
  if (NULL == tb_vc_trvs_hdl) {
    PyErr_SetString(PyExc_KeyError, "Fail to create time-based vc trvs hdl!");
    goto Ocean_surf_error_release_mem;
  }
  if (FSDB_RC_SUCCESS != tb_vc_trvs_hdl->ffrGetVC(&vc_ptr)) {
    goto Ocean_surf_error_release_mem;
  }
  tb_vc_trvs_hdl->ffrGetVarIdcode(&var_idcode);
  tb_vc_trvs_hdl->ffrGetXTag(&time);
  w1 = (Wave *)PyDict_GetItem(idcode_map, PyInt_FromLong(var_idcode));
  _setHDLValue(w1, vc_ptr);
  w1->event = 1;

  //
  // Iterate until no more vc
  //
  // The order of the vc at the same time step are returned according to
  // their sequence number. If sequence number is not turned on, the order
  // will be undetermined.
  //
  while ((!terminate_surf) && (FSDB_RC_SUCCESS == tb_vc_trvs_hdl->ffrGotoNextVC())) {
    t1 = time;
    tb_vc_trvs_hdl->ffrGetXTag(&time);
    if ((t1.H != time.H) || (t1.L != time.L)) {
      /* Update time */
      uint64 time_ul = 0;
      time_ul = t1.H; time_ul <<= 32; time_ul |= t1.L;
      Py_DECREF(self->time);
      self->time = PyInt_FromLong(time_ul);

      if (SimModule::simmodule_list) {
        if (0 != PyList_Type.tp_traverse(SimModule::simmodule_list, _SimModule_time, (void *)self->time))
          goto Ocean_surf_error_release_mem;
        if (0 != PyList_Type.tp_traverse(SimModule::simmodule_list, _SimModule_surf, (void *)"monitor"))
          goto Ocean_surf_error_release_mem;
        if (0 != PyList_Type.tp_traverse(SimModule::simmodule_list, _SimModule_surf, (void *)"sequential"))
          goto Ocean_surf_error_release_mem;
      }
      if (Callback::simmodule_list) {
        if (0 != PyList_Type.tp_traverse(Callback::simmodule_list, _Callback_time, (void *)self->time))
          goto Ocean_surf_error_release_mem;
        uint length = PyList_Size(Callback::simmodule_list);
        for(uint ui1=0; ui1<length; ui1++) {
          PyObject* temp = PyList_GetItem(Callback::simmodule_list, ui1);
          ob1 = Wave_int_c((Wave *)(((Callback *)temp)->event), NULL);
          if ( (NULL == (((Callback *)temp)->pval)) ||
               (0 != PyInt_Type.tp_compare
                ((PyObject *)(((Callback *)temp)->pval), ob1)) ) {
            Py_XDECREF(((Callback *)temp)->pval);
            ((Callback *)temp)->pval = ob1;
            if (0 != _Callback_surf(temp, (void *)"callback"))
              goto Ocean_surf_error_release_mem;
          } else {
            Py_XDECREF(ob1);
          }
        }
      }
      for (uint ui1=0; ui1<sig_num; ui1++)
        sig_arr_ptr[ui1]->event = 0;
    }
    tb_vc_trvs_hdl->ffrGetVarIdcode(&var_idcode);
    tb_vc_trvs_hdl->ffrGetVC(&vc_ptr);
    w1 = (Wave *)PyDict_GetItem(idcode_map, PyInt_FromLong(var_idcode));
    _setHDLValue(w1, vc_ptr);
    w1->event = 1;
  }

  if (SimModule::simmodule_list) {
    ret = (0 == PyList_Type.tp_traverse(SimModule::simmodule_list, _SimModule_surf, (void *)"final")) ? Py_True : Py_False;
  }
  if (Callback::simmodule_list) {
    ret = (0 == PyList_Type.tp_traverse(Callback::simmodule_list, _Callback_surf, (void *)"final")) ? Py_True : Py_False;
  }

  /* Create smaller FSDB */
  do {
    struct stat fstat, sfstat;
    bool create_small_fsdb = false;
    char buf[512+256];
    char *cp;

    /* decide if need to create new fsdb */
    cp = PyString_AS_STRING(self->file);
    uint ui2 = PyString_Size(self->file);
    if ( ('.' != cp[ui2-5]) || ('f' != cp[ui2-4]) ||
         ('s' != cp[ui2-3]) || ('d' != cp[ui2-2]) ||
         ('b' != cp[ui2-1]) || (ui2 < 6) )
      break;
    sprintf(buf, "%s", cp);
    if (0 != stat(buf, &fstat)) {
      cerr << "Can't access the FSDB file" << endl;
      break;
    }
    sprintf(buf, "%s.%x", cp, geteuid());
    create_small_fsdb = true;
    if (0 == stat(buf, &sfstat)) {
      create_small_fsdb = (fstat.st_mtime > sfstat.st_mtime) ? true : false;
    }
    if (!create_small_fsdb) break;

    /* create it */
    cerr << "Creating smaller FSDB : " << buf << endl;
    ofstream myfile;
    myfile.open ("sig.l");
    uint length = PyList_Size(self->all_waves);
    for(uint ui1=0; ui1<length; ui1++) {
      Wave* temp = (Wave *)PyList_GetItem(self->all_waves, ui1);
      char *n = PyString_AS_STRING(temp->sig_path);
      myfile << "fuSetSignal ";
      for (uint ui2=0; 0 != n[ui2]; ui2++) {
        myfile << (('.'==n[ui2]) ? '/' : n[ui2]);
      }
      myfile << endl;
    }
    myfile.close();
    sprintf(buf, "fsdbextract %s -f sig.l -o %s.%x", cp, cp, geteuid());
    if (0 != system(buf)) {
      cerr << "command '" << buf << "' : Failed" << endl;
    }
  } while (0);

 Ocean_surf_error_release_mem:
  pos = 0;
  if (NULL != idcode_map) {
    while (PyDict_Next(idcode_map, &pos, &key, &value)) {
      Py_DECREF(key);
    }
    Py_DECREF(idcode_map);
  }
  //
  // Remember to call ffrFree() to free the memory occupied by this
  // time-based vc trvs hdl
  //
  if (NULL != tb_vc_trvs_hdl)
    tb_vc_trvs_hdl->ffrFree();
  //
  Ocean_endSurf(self);

  /* */
  if (NULL != sig_arr)
    delete[] sig_arr;
  if (NULL != sig_arr_ptr)
    delete[] sig_arr_ptr;

  if (Py_True != ret)
    return NULL;

  Py_INCREF(ret);
  return ret;
}

/********************************************************************/
/*        Routines dependent on both Modules                        */
/********************************************************************/
static int
Wave_setocean(Wave *self, PyObject *value, void *closure)
{
  DEBUG_FUNC_STR;
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete the ocean attribute");
    return -1;
  }
  
  if (! PyObject_TypeCheck(value, &OceanType)) {
    PyErr_SetString(PyExc_TypeError, 
                    "The ocean attribute value must be a string");
    return -1;
  }
      
  Py_XDECREF(self->ocean);
  Py_INCREF(value);
  self->ocean = value;    

  return 0;
}

/********************************************************************/
/*        SurfBoard Module                                          */
/********************************************************************/
static PyMethodDef module_methods[] = {
  {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC extern "C" void
#endif
PyMODINIT_FUNC
initsb(void)
{
  DEBUG_FUNC_STR;
  PyObject* m;

  if (PyType_Ready(&PycRegType) < 0)
    return;
  if (PyType_Ready(&OceanType) < 0)
    return;
  if (PyType_Ready(&WaveType) < 0)
    return;
  if (PyType_Ready(&CallbackType) < 0)
    return;
  if (PyType_Ready(&SimModuleType) < 0)
    return;

  m = Py_InitModule3("sb", module_methods,
                     "Module can be used to ride waves.");

  if (m == NULL)
    return;

  Py_INCREF(&PycRegType);
  PyModule_AddObject(m, "reg", (PyObject *)&PycRegType);
  Py_INCREF(&WaveType);
  PyModule_AddObject(m, "Wave", (PyObject *)&WaveType);
  Py_INCREF(&OceanType);
  PyModule_AddObject(m, "Ocean", (PyObject *)&OceanType);
  Py_INCREF(&CallbackType);
  PyModule_AddObject(m, "Callback", (PyObject *) &CallbackType);
  Py_INCREF(&SimModuleType);
  PyModule_AddObject(m, "SimModule", (PyObject *) &SimModuleType);
  /* */
  PyModule_AddIntConstant(m, "ZERO", cReg::ZERO);
  PyModule_AddIntConstant(m, "ONE", cReg::ONE);
  PyModule_AddIntConstant(m, "TRISTATE", cReg::TRISTATE);
  PyModule_AddIntConstant(m, "UNKNOWN", cReg::UNKNOWN);
  PyModule_AddIntConstant(m, "DATA", cReg::DATA);
  PyModule_AddIntConstant(m, "CONTROL", cReg::CONTROL);
  PyModule_AddIntConstant(m, "BIN", BIN);
  PyModule_AddIntConstant(m, "HEX", HEX);
  PyModule_AddIntConstant(m, "DEC", DECIMAL);
  PyModule_AddIntConstant(m, "OCT", OCT);
  PyModule_AddIntConstant(m, "LSB", LSB);
  PyModule_AddIntConstant(m, "MSB", MSB);

  DEBUG_FUNC_ENDSTR;
}

