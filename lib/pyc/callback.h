
#ifndef __CALLBACK_INCLUDED
#define __CALLBACK_INCLUDED

typedef struct {
  PyObject_HEAD
  static PyObject *simmodule_list;
  static PyObject *wave_list;
  static uint32 list_ref_cnt;
  PyObject *ocean;
  PyObject *time;
  PyObject *event;
  PyObject *pval;
} Callback;

static PyObject *Callback_options(Callback *self, PyObject *args);
static PyObject *Callback_early_initial(Callback *self, PyObject *args);
static PyObject *Callback_initial(Callback *self, PyObject *args);
static PyObject *Callback_surf(Callback *self, PyObject *args);
static PyObject *Callback_add_callback(Callback *self, PyObject *args);
static PyObject *Callback_wave(Callback *self, PyObject *args);
static PyObject *Callback_pipe(Callback *self, PyObject *args);
static PyObject *Callback_finish(Callback *self);
static void Callback_dealloc(Callback* self);
static int Callback_init(Callback *self, PyObject *args, PyObject *kwds);
static int Callback_clear(Callback *self);
static int Callback_traverse(Callback *self, visitproc visit, void *arg);
static PyObject *Callback_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

#endif

