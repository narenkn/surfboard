
#ifndef __CREG_INCLUDED
#define __CREG_INCLUDED

#include <Python.h>
#include <longintrepr.h>

#define DEBUG_FUNC_IN false
#define DEBUG_FUNC_STR do { \
		if (DEBUG_FUNC_IN) \
			fprintf(stderr, "in func:%s file:%s\n", __FUNCTION__, __FILE__); \
	} while (0)
#define DEBUG_FUNC_ENDSTR do { \
		if (DEBUG_FUNC_IN) \
			fprintf(stderr, "exiting func:%s file:%s\n", __FUNCTION__, __FILE__); \
	} while (0)

extern char py_creg_err_str[256];

#define FATAL_ERROR(...) \
	do { \
		sprintf(py_creg_err_str, __VA_ARGS__); \
		PyErr_SetString(PyExc_RuntimeError, py_creg_err_str); \
		PyErr_Print(); \
	} while (0) \

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define protected public
#include "creg.h"
#undef protected

typedef enum {BIN=0, HEX=1, OCT=2, DECIMAL=3} RadixFormat;
typedef enum {LSB=0, MSB=1} IndexFormat;

typedef struct {
	PyObject_HEAD
	cReg *r;
} PycReg;

static PyObject *PycReg_alloc(PyTypeObject *type, int nitems);
static void PycReg_dealloc(PycReg* self);
static int PycReg_print(PycReg *reg, FILE *f, int flags);
static PyObject *PycReg_str(PycReg *reg);
static PyObject *PycReg_repr(PycReg *reg);
static PyObject *PycReg_call(PycReg *reg, PyObject *args, PyObject *kwds);
static int PycReg_cmp(PycReg *r1, PycReg *r2);
static PyObject *PycReg_richcompare(PyObject *o1, PyObject *o2, int flags);
static PyObject *PycReg_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int PycReg_init(PycReg *self, PyObject *args, PyObject *kwds);
static PyObject *PycReg_add(PycReg *reg, PyObject *op);
static PyObject *PycReg_sub(PycReg *reg, PyObject *op);
static PyObject *PycReg_mul(PycReg *reg, PyObject *op);
static PyObject *PycReg_div(PycReg *reg, PyObject *op);
static PyObject *PycReg_rem(PycReg *reg, PyObject *op);
static PyObject *PycReg_neg(PycReg *reg);
static PyObject *PycReg_pos(PycReg *reg);
static PyObject *PycReg_lint(PycReg *reg);
static PyObject *PycReg_oct(PycReg *reg);
static PyObject *PycReg_hex(PycReg *reg);
static PyObject *PycReg_lint_c(PycReg *reg, PyObject *args);
static PyObject *PycReg_str_c(PycReg *reg, PyObject *args);
static PyObject *PycReg_push_c(PycReg *reg, PyObject *args);
static PyObject *PycReg_resize_c(PycReg *reg, PyObject *args);
static PyObject *PycReg_pop_c(PycReg *reg, PyObject *args);
static PyObject *PycReg_invert(PycReg *reg);
static PyObject *PycReg_lshift(PycReg *reg, PyObject *op);
static PyObject *PycReg_rshift(PycReg *reg, PyObject *op);
static PyObject *PycReg_and(PycReg *reg, PyObject *op);
static PyObject *PycReg_or(PycReg *reg, PyObject *op);
static PyObject *PycReg_xor(PycReg *reg, PyObject *op);
static int PycReg_nonzero(PycReg *reg);

#endif
