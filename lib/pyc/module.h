
#ifndef __MODULE_INCLUDED
#define __MODULE_INCLUDED

typedef struct {
	PyObject_HEAD
	static PyObject *simmodule_list;
	static PyObject *wave_list;
	static uint32 list_ref_cnt;
	PyObject *ocean;
	PyObject *time;
} SimModule;

static PyObject *SimModule_options(SimModule *self, PyObject *args);
static PyObject *SimModule_early_initial(SimModule *self, PyObject *args);
static PyObject *SimModule_initial(SimModule *self, PyObject *args);
static PyObject *SimModule_surf(SimModule *self, PyObject *args);
static PyObject *SimModule_wave(SimModule *self, PyObject *args);
static PyObject *SimModule_pipe(SimModule *self, PyObject *args);
static PyObject *SimModule_finish(SimModule *self);
static int delete_visitproc(PyObject *self, void *arg);
static void SimModule_dealloc(SimModule* self);
static int SimModule_init(SimModule *self, PyObject *args, PyObject *kwds);
static int SimModule_clear(SimModule *self);
static int SimModule_traverse(SimModule *self, visitproc visit, void *arg);
static PyObject *SimModule_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static void _setHDLValue(Wave *w1, byte_T *vc_ptr);

#endif

