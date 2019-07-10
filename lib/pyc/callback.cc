#include <Python.h>
#include "module.h"

static PyMethodDef Callback_methods[] = {
	{"wave", (PyCFunction)Callback_wave, METH_VARARGS, PyDoc_STR("Gets the object to a waveform signal. Accepts path to signal, optional lengthof-pipe")},
	{"surf", (PyCFunction)Callback_surf, METH_VARARGS, PyDoc_STR("starts the waveform playback, for this object")},
	{"add_callback", (PyCFunction)Callback_add_callback, METH_VARARGS, PyDoc_STR("registers the Wave which triggers callback()")},
	{"options", (PyCFunction)Callback_options, METH_VARARGS, PyDoc_STR("called during sim time 0, but before early_initial")},
	{"initial", (PyCFunction)Callback_initial, METH_VARARGS, PyDoc_STR("called during sim time 0")},
	{"callback", (PyCFunction)Callback_initial, METH_VARARGS, PyDoc_STR("called during every event of callback trigger")},
	{"pipe", (PyCFunction)Callback_pipe, METH_VARARGS, PyDoc_STR("Pipe all wave objects belonging to this class (except the event)")},
	{"final", (PyCFunction)Callback_initial, METH_VARARGS, PyDoc_STR("called during end of all event triggers")},
	{"finish", (PyCFunction)Callback_finish, METH_NOARGS, PyDoc_STR("called anytime during playback to stop it")},
	{"early_initial", (PyCFunction)Callback_early_initial, METH_VARARGS, PyDoc_STR("called during sim time 0, but before initial")},
	{NULL,	NULL},
};

static PyMemberDef Callback_members[] = {
    {"fsdb", T_OBJECT_EX, offsetof(Callback, ocean), READONLY, "Callbacks fsdb object"},
    {"event", T_OBJECT_EX, offsetof(Callback, event), READONLY, "Wave object corresponding to the event"},
  	{"time", T_OBJECT_EX, offsetof(Callback, time), READONLY, "Return current simtime"},
    {NULL}  /* Sentinel */
};

PyObject *Callback::simmodule_list = NULL;
PyObject *Callback::wave_list = NULL;
uint32 Callback::list_ref_cnt = 0;

static PyTypeObject CallbackType = {
    PyObject_HEAD_INIT(NULL)
    0,                       /* ob_size */
    "sb.Callback",         /* tp_name */
    sizeof(Callback),          /* tp_basicsize */
    0,                       /* tp_itemsize */
    (destructor)Callback_dealloc, /* tp_dealloc */
    0,                       /* tp_print */
    0,                       /* tp_getattr */
    0,                       /* tp_setattr */
    0,                       /* tp_compare */
    0,                       /* tp_repr */
    0,                       /* tp_as_number */
    0,                       /* tp_as_sequence */
    0,                       /* tp_as_mapping */
    0,                       /* tp_hash */
    0,                       /* tp_call */
    0,                       /* tp_str */
    0,                       /* tp_getattro */
    0,                       /* tp_setattro */
    0,                       /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,   /* tp_flags */
    0,                       /* tp_doc */
    (traverseproc)Callback_traverse, /* tp_traverse */
    (inquiry)Callback_clear,                       /* tp_clear */
    0,                       /* tp_richcompare */
    0,                       /* tp_weaklistoffset */
    0,                       /* tp_iter */
    0,                       /* tp_iternext */
    Callback_methods,         /* tp_methods */
    Callback_members,         /* tp_members */
    0,                       /* tp_getset */
    0,                       /* tp_base */
    0,                       /* tp_dict */
    0,                       /* tp_descr_get */
    0,                       /* tp_descr_set */
    0,                       /* tp_dictoffset */
    (initproc)Callback_init,  /* tp_init */
    PyType_GenericAlloc,     /* tp_alloc */
    Callback_new,             /* tp_new */
    PyObject_GC_Del,         /* tp_free */
};

static PyObject *
Callback_options(Callback *self, PyObject *args)
{
	DEBUG_FUNC_STR;
	PyObject *ret = Py_True;
	Py_INCREF(ret);
	return ret;
}

static PyObject *
Callback_early_initial(Callback *self, PyObject *args)
{
	DEBUG_FUNC_STR;
	PyObject *ret = Py_True;
	Py_INCREF(ret);
	return ret;
}

static PyObject *
Callback_initial(Callback *self, PyObject *args)
{
	DEBUG_FUNC_STR;
	PyObject *ret = Py_True;
	Py_INCREF(ret);
	return ret;
}

static PyObject *
Callback_finish(Callback *self)
{
	terminate_surf = true;

	PyObject *ret = Py_None;
	Py_INCREF(ret);
	return ret;
}


static int
_Callback_time(PyObject *self, void *arg)
{
	DEBUG_FUNC_STR;

	Py_INCREF((PyObject *)arg);
        Py_XDECREF(((Callback *)self)->time);
	((Callback *)self)->time = arg;

	return 0;
}

static int
_Callback_surf(PyObject *self, void *arg)
{
	DEBUG_FUNC_STR;
	PyObject *ret;

	ret = PyObject_CallMethod(self, (char *)arg, NULL);
	if (NULL == ret)
		return -1;

	Py_DECREF(ret);
	return 0;
}

static PyObject *
Callback_surf(Callback *self, PyObject *args)
{
	DEBUG_FUNC_STR;
	PyObject *ret = Py_False, *ob1;

	/* Don't have to do anything if no object was created */
	if (0 == self->list_ref_cnt) {
		ret = Py_True;
		Py_INCREF(ret);
		return ret;
	}

	/* Pre init calls */
	assert(self->simmodule_list);
//	if (0 != PyList_Type.tp_traverse(self->simmodule_list, _Callback_surf, (void *)"options"))
//		goto Callback_surf_return;
	if (0 != _Callback_surf((PyObject *)self, (void *)"options"))
		goto Callback_surf_return;

	/* checks */
	if ((NULL == self->ocean) || (NULL == self->time)) {
		PyErr_SetString(PyExc_SystemError, "FSDB object not initialised for Callback Object : internal error?");
		return NULL;
	}

	/* init */
	terminate_surf = false;

	/* Time 0 calls */
	if (0 != _Callback_surf((PyObject *)self, (void *)"early_initial"))
		goto Callback_surf_return;

	/* */
	Py_DECREF(self->time);
	self->time = PyInt_FromLong(0);
	if (0 != _Callback_surf((PyObject *)self, (void *)"initial"))
		goto Callback_surf_return;

	/* At this point all init should have been done */
	if ((NULL == self->event) || (terminate_surf)) {
		ret = Py_True;
		goto Callback_surf_return;
	}

	/* Initialise surfboard and connect waves */
	ob1 = Ocean_startSurf((Ocean *)(self->ocean));
	if (NULL == ob1)
		return ob1;
	Py_DECREF(ob1);

	/* Goto the first VC */
	VC_first = true;
	ob1 = _Ocean_nextVC((Ocean *)(self->ocean), (Wave *)(self->event));
	VC_first = false;
	if (Py_False == ob1)
		return ob1;
	Py_DECREF(ob1);

	/* Sequential, Monitor */
	for (;! terminate_surf;) {
		Py_DECREF(self->time);
		self->time = PyInt_FromLong( ((PyIntObject *) (((Ocean *)(self->ocean))->time))->ob_ival );
		if (0 != _Callback_surf((PyObject *)self, (void *)"callback"))
			goto Callback_surf_return;
		ob1 = _Ocean_nextVC((Ocean *)(self->ocean), (Wave *)(self->event));
		/* we are not going to access anything inside ob1, hence dec-ref; compare */
		Py_DECREF(ob1);
		if (Py_True != ob1)
			break;
	}

	Ocean_endSurf((Ocean *)(self->ocean));
	if (0 == _Callback_surf((PyObject *)self, (void *)"final")) {
		ret = Py_True;
	}

	/* */
	send_sb_data();

Callback_surf_return:
	if (Py_True != ret)
		return NULL;

	Py_INCREF(ret);
	return ret;
}

static PyObject *
Callback_add_callback(Callback *self, PyObject *args)
{
	PyObject *ob2, *ret;

	DEBUG_FUNC_STR;
	/* Check object status and arguments */
	if (NULL == self->ocean) {
		PyErr_SetString(PyExc_KeyError, "FSDB file is not initialised : internal error?");
		return NULL;
	}
	if (NULL != self->event) {
		PyErr_SetString(PyExc_KeyError, "Attempt to add second event on this object");
		return NULL;
	}

	/* get the event */
	ob2 = Ocean_newWave((Ocean *)self->ocean, args, NULL);
	if (NULL == ob2)
		return ob2;
	if (NULL != self->event) {
		if (ob2 != self->event) {
			PyErr_SetString(PyExc_KeyError, "Attempt to add second event on this object");
			return NULL;
		}
		Py_DECREF(ob2);
	} else {
		self->event = ob2;
	}

	ret = Py_True;
	Py_INCREF(ret);
	return ret;
}

static PyObject *
Callback_wave(Callback *self, PyObject *args)
{
	DEBUG_FUNC_STR;
	PyObject *ob1;

	assert (NULL != self->ocean);
	ob1 = Ocean_newWave((Ocean *)self->ocean, args, NULL);
	if (NULL == ob1)
		return ob1;
	assert(NULL != self->wave_list);

	uint32 ui1 = PyList_Size(self->wave_list);
	uint32 ui2;
	for (ui2=0; ui2<ui1; ui2++) {
		if (PyList_GET_ITEM(self->wave_list, ui2) == ob1)
			break;
	}
	if (ui1 == ui2) { /* not matched */
		Py_INCREF(ob1);
		PyList_Append(self->wave_list, ob1);
	}

	return ob1;
}

static int
_Callback_pipe(PyObject *self, void *arg)
{
	DEBUG_FUNC_STR;
	PyObject *ob1 = Wave_pipe((Wave *)self);
	if (NULL == ob1)
		return -1;
	Py_DECREF(ob1);
	return 0;
}

static PyObject *
Callback_pipe(Callback *self, PyObject *args)
{
	DEBUG_FUNC_STR;
	assert(NULL != self->wave_list);
	if (0 != PyList_Type.tp_traverse(self->wave_list, _Callback_pipe, NULL))
		return NULL;

	PyObject *ret = Py_True;
	Py_INCREF(ret);
	return ret;
}

static void
Callback_dealloc(Callback* self)
{
	DEBUG_FUNC_STR;

	PyObject_GC_UnTrack(self);
	Py_TRASHCAN_SAFE_BEGIN(self)

	/* look at static items */
	assert (0 != self->list_ref_cnt);
	self->list_ref_cnt--;
	if (0 == self->list_ref_cnt) {
		assert(self->simmodule_list);
		PyList_Type.tp_traverse(self->simmodule_list, delete_visitproc, NULL);
		Py_DECREF(self->simmodule_list);
		self->simmodule_list = NULL;

		assert(self->wave_list);
		PyList_Type.tp_traverse(self->wave_list, delete_visitproc, NULL);
		Py_DECREF(self->wave_list);
		self->wave_list = NULL;
	}

	Py_XDECREF(self->ocean);
	Py_XDECREF(self->time);
	Py_XDECREF(self->event);
	Py_XDECREF(self->pval);

	/* thrash it */
	self->ob_type->tp_free((PyObject*)self);
	Py_TRASHCAN_SAFE_END(self)
}

static int
Callback_init(Callback *self, PyObject *args, PyObject *kwds)
{
	DEBUG_FUNC_STR;
	PyObject *arg = NULL, *ob1;

	/* check args */
	if ((NULL == args) || (!PyArg_ParseTuple(args, "O", &arg))) {
		PyErr_SetString(PyExc_KeyError, "Exactly one argument (\"verilog.fsdb\" | ocean) is required");
		return -1;
	}
        if (PyObject_TypeCheck((PyObject *)arg, &OceanType)) {
          self->ocean = arg;
          Py_INCREF(arg);
        } else if (PyString_Check((PyObject *)arg)) {
          ob1 = OceanType.tp_new(&OceanType, args, NULL);
          if (NULL == ob1) {
            Py_XDECREF(arg);
            return -1;
          }
          /* get the ocean */
          if (0 != OceanType.tp_init(ob1, args, NULL)) {
            Py_XDECREF(arg);
            return -1;
          }
          self->ocean = ob1;
        } else {
          PyErr_SetString(PyExc_KeyError, "Exactly one argument (\"verilog.fsdb\" | ocean) is required");
          Py_XDECREF(arg);
          return -1;
        }
	assert (arg);

	self->time = PyInt_FromLong(0);

	self->event = NULL;
	self->pval = NULL;
	return 0;
}

static int
Callback_traverse(Callback *self, visitproc visit, void *arg)
{
	DEBUG_FUNC_STR;
	return 0;
}

static int
Callback_clear(Callback *self)
{
	DEBUG_FUNC_STR;

	Py_XDECREF(self->ocean);
	self->ocean = NULL;

	Py_XDECREF(self->time);
	self->time = NULL;

	Py_XDECREF(self->event);
	self->event = NULL;

	Py_XDECREF(self->pval);
	self->pval = NULL;

	assert (0 != self->list_ref_cnt);
	assert (self->wave_list);
	assert (self->simmodule_list);
	return 0;
}

static PyObject *
Callback_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	DEBUG_FUNC_STR;
	Callback *self;

	/* get storage and init */
	self = (Callback *)type->tp_alloc(type, 0);
	CallbackType.tp_init((PyObject *)self, args, kwds);

	/* alloc static members */
	if (0 == self->list_ref_cnt) {
		assert(NULL == self->simmodule_list);
		self->simmodule_list = PyList_New(0);

		assert(NULL == self->wave_list);
		self->wave_list = PyList_New(0);
	}
        self->list_ref_cnt++;

	Py_INCREF((PyObject *)self);
	PyList_Append(self->simmodule_list, (PyObject *)self);

	return (PyObject *)self;
}

