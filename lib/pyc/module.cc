#include <Python.h>
#include <string>
#include "module.h"

static PyMethodDef SimModule_methods[] = {
	{"wave", (PyCFunction)SimModule_wave, METH_VARARGS, PyDoc_STR("Gets the object to a waveform signal. Accepts path to signal, optional lengthof-pipe")},
	{"surf", (PyCFunction)SimModule_surf, METH_VARARGS, PyDoc_STR("starts the waveform playback, for all objects associated with the fsdb")},
	{"options", (PyCFunction)SimModule_options, METH_VARARGS, PyDoc_STR("called during sim time 0, but before early_initial")},
	{"initial", (PyCFunction)SimModule_initial, METH_VARARGS, PyDoc_STR("called during sim time 0")},
	{"sequential", (PyCFunction)SimModule_initial, METH_VARARGS, PyDoc_STR("called during every event")},
	{"monitor", (PyCFunction)SimModule_initial, METH_VARARGS, PyDoc_STR("called during every event")},
	{"pipe", (PyCFunction)SimModule_pipe, METH_VARARGS, PyDoc_STR("Pipe all wave objects belonging to this class")},
	{"final", (PyCFunction)SimModule_initial, METH_VARARGS, PyDoc_STR("called during end of all events")},
	{"finish", (PyCFunction)SimModule_finish, METH_NOARGS, PyDoc_STR("called anytime during playback to stop it")},
	{"early_initial", (PyCFunction)SimModule_early_initial, METH_VARARGS, PyDoc_STR("called during sim time 0, but before initial")},
	{NULL,	NULL},
};

static PyMemberDef SimModule_members[] = {
    {"fsdb", T_OBJECT_EX, offsetof(SimModule, ocean), READONLY, "SimModules fsdb object"},
    {"time", T_OBJECT_EX, offsetof(SimModule, time), READONLY, "Current simulation time"},
    {NULL}  /* Sentinel */
};

PyObject *SimModule::simmodule_list = NULL;
PyObject *SimModule::wave_list = NULL;
uint32 SimModule::list_ref_cnt = 0;

static PyTypeObject SimModuleType = {
    PyObject_HEAD_INIT(NULL)
    0,                       /* ob_size */
    "sb.SimModule",         /* tp_name */
    sizeof(SimModule),          /* tp_basicsize */
    0,                       /* tp_itemsize */
    (destructor)SimModule_dealloc, /* tp_dealloc */
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
    (traverseproc)SimModule_traverse, /* tp_traverse */
    (inquiry)SimModule_clear,                       /* tp_clear */
    0,                       /* tp_richcompare */
    0,                       /* tp_weaklistoffset */
    0,                       /* tp_iter */
    0,                       /* tp_iternext */
    SimModule_methods,         /* tp_methods */
    SimModule_members,         /* tp_members */
    0,                       /* tp_getset */
    0,                       /* tp_base */
    0,                       /* tp_dict */
    0,                       /* tp_descr_get */
    0,                       /* tp_descr_set */
    0,                       /* tp_dictoffset */
    (initproc)SimModule_init,  /* tp_init */
    PyType_GenericAlloc,     /* tp_alloc */
    SimModule_new,             /* tp_new */
    PyObject_GC_Del,         /* tp_free */
};

static PyObject *
SimModule_options(SimModule *self, PyObject *args)
{
	DEBUG_FUNC_STR;
	PyObject *ret = Py_True;
	Py_INCREF(ret);
	return ret;
}

static PyObject *
SimModule_early_initial(SimModule *self, PyObject *args)
{
	DEBUG_FUNC_STR;
	PyObject *ret = Py_True;
	Py_INCREF(ret);
	return ret;
}

static PyObject *
SimModule_initial(SimModule *self, PyObject *args)
{
	DEBUG_FUNC_STR;
	PyObject *ret = Py_True;
	Py_INCREF(ret);
	return ret;
}

static bool terminate_surf = false;
static PyObject *
SimModule_finish(SimModule *self)
{
	terminate_surf = true;

	PyObject *ret = Py_None;
	Py_INCREF(ret);
	return ret;
}

static int
_SimModule_surf(PyObject *self, void *arg)
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
SimModule_surf(SimModule *self, PyObject *args)
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
	int pos = 0;


	/* Don't have to do anything if no object was created */
	if (0 == self->list_ref_cnt) {
		ret = Py_True;
		Py_INCREF(ret);
		return ret;
	}

	/* Pre init calls */
	assert(self->simmodule_list);
//	if (0 != PyList_Type.tp_traverse(self->simmodule_list, _SimModule_surf, (void *)"options"))
//		return NULL;
	if (0 != _SimModule_surf((PyObject *)self, (void *)"options"))
		return NULL;

	/* checks */
	if ((NULL == self->ocean) || (NULL == self->time)) {
		PyErr_SetString(PyExc_SystemError, "FSDB object not initialised for SimModule Object : internal error?");
		return NULL;
	}

	/* init */
	Py_DECREF(self->time);
	self->time = PyInt_FromLong(0);
	terminate_surf = false;

	/* Time 0 calls */
	if (0 != _SimModule_surf((PyObject *)self, (void *)"early_initial"))
		return NULL;

	/* */
	if (0 != _SimModule_surf((PyObject *)self, (void *)"initial"))
		return NULL;

	/* At this point all init should have been done */
	if (terminate_surf) {
		ret = Py_True;
		goto SimModule_surf_error_release_mem;
	}

	/* Initialise surfboard and connect waves */
	ob1 = Ocean_startSurf((Ocean *)(self->ocean));
	if (NULL == ob1)
		return ob1;
	Py_DECREF(ob1);

	/* initialise datastruct */
	sig_num = PyDict_Size(((Ocean *)(self->ocean))->waves_dict);
	sig_arr = new fsdbVarIdcode[sig_num];
	sig_arr_ptr = new Wave *[sig_num];

	/* We know that this dictionary's value's refcount will not be decremented
	hence we can use waves, but remember to kill the dictionay at the end of this
	function */
	idcode_map = PyDict_New();
	uint32 ui2;
	for (ui2 = 0; PyDict_Next(((Ocean *)(self->ocean))->waves_dict, &pos, &key, &value); ui2++) {
		w1 = (Wave *) value;
		sig_arr_ptr[ui2] = w1;
		sig_arr[ui2] = w1->idcode;
		w1->event = 0;
		if (0 != PyDict_SetItem(idcode_map, PyInt_FromLong(w1->idcode), (PyObject *)w1))
			goto SimModule_surf_error_release_mem;
		assert (ui2<sig_num);
	}
	assert (ui2 == sig_num);

	//
	// Create a time-based traverse handle to encapsulate the signals to be
	// traversed.
	//
	tb_vc_trvs_hdl = ((Ocean *)(self->ocean))->fsdb_obj->ffrCreateTimeBasedVCTrvsHdl(sig_num, sig_arr);
	if (NULL == tb_vc_trvs_hdl) {
		PyErr_SetString(PyExc_KeyError, "Fail to create time-based vc trvs hdl!");
		goto SimModule_surf_error_release_mem;
	}
	if (FSDB_RC_SUCCESS != tb_vc_trvs_hdl->ffrGetVC(&vc_ptr)) {
		goto SimModule_surf_error_release_mem;
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
			Py_DECREF(((Ocean *)(self->ocean))->time);
			(((Ocean *)(self->ocean))->time) = PyInt_FromLong(time_ul);
			Py_DECREF(self->time);
			self->time = PyInt_FromLong(time_ul);

			if (0 != _SimModule_surf((PyObject *)self, (void *)"monitor"))
				goto SimModule_surf_error_release_mem;
			if (0 != _SimModule_surf((PyObject *)self, (void *)"sequential"))
				goto SimModule_surf_error_release_mem;
			for (uint ui1=0; ui1<sig_num; ui1++)
				sig_arr_ptr[ui1]->event = 0;
		}
		tb_vc_trvs_hdl->ffrGetVarIdcode(&var_idcode);
		tb_vc_trvs_hdl->ffrGetVC(&vc_ptr);
		w1 = (Wave *)PyDict_GetItem(idcode_map, PyInt_FromLong(var_idcode));
		_setHDLValue(w1, vc_ptr);
		w1->event = 1;
	}

	if (0 == _SimModule_surf((PyObject *)self, (void *)"final")) {
		ret = Py_True;
	}

	/* */
	send_sb_data();

SimModule_surf_error_release_mem:
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
	Ocean_endSurf((Ocean *)(self->ocean));

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

static void
_setHDLValue(Wave *w1, byte_T *vc_ptr)
{
	/* Update object to the value ... */
	uint32 ui3, ui4, ui5, ui6, ui7;
	uint32 val_1, val_0;
	ui6 = w1->num_bits;
	for (ui4=0, ui5=0; ui4<ui6; ui4+=32, ui5++) {
		val_1 = 0; val_0 = 0;
		ui7 = ((ui4+32) > ui6) ? ui6 : ui4+32;
		for (ui3=ui4; ui3<ui7; ui3++) {
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
		}
		w1->sq_data[0]->r->setDataWord(ui5, val_0);
		w1->sq_data[0]->r->setControlWord(ui5, val_1);
	}
}

static PyObject *
SimModule_wave(SimModule *self, PyObject *args)
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
_SimModule_pipe(PyObject *self, void *arg)
{
	DEBUG_FUNC_STR;
	PyObject *ob1 = Wave_pipe((Wave *)self);
	if (NULL == ob1)
		return -1;
	Py_DECREF(ob1);
	return 0;
}

static PyObject *
SimModule_pipe(SimModule *self, PyObject *args)
{
	DEBUG_FUNC_STR;
	assert(NULL != self->wave_list);
	if (0 != PyList_Type.tp_traverse(self->wave_list, _SimModule_pipe, NULL))
		return NULL;

	PyObject *ret = Py_True;
	Py_INCREF(ret);
	return ret;
}

static int
delete_visitproc(PyObject *self, void *arg)
{
	DEBUG_FUNC_STR;
	Py_DECREF(self);
	return 0;
}

static void
SimModule_dealloc(SimModule* self)
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

	/* thrash it */
	self->ob_type->tp_free((PyObject*)self);
	Py_TRASHCAN_SAFE_END(self)
}

static int
SimModule_init(SimModule *self, PyObject *args, PyObject *kwds)
{
	DEBUG_FUNC_STR;
	PyObject *fsdb_name = NULL, *ob1;

	/* check args */
	if ((NULL == args) || (!PyArg_ParseTuple(args, "S", &fsdb_name))) {
		PyErr_SetString(PyExc_KeyError, "Exactly one string argument is required");
		return -1;
	}
	assert (fsdb_name);

	/* get the ocean */
	ob1 = OceanType.tp_new(&OceanType, args, NULL);
	if (NULL == ob1)
		return -1;
	if (0 != OceanType.tp_init(ob1, args, NULL))
		return -1;
	self->ocean = ob1;

	self->time = PyInt_FromLong(0);

	return 0;
}

static int
SimModule_traverse(SimModule *self, visitproc visit, void *arg)
{
	DEBUG_FUNC_STR;
	return 0;
}

static int
SimModule_clear(SimModule *self)
{
	DEBUG_FUNC_STR;

	Py_XDECREF(self->ocean);
	self->ocean = NULL;

	Py_XDECREF(self->time);
	self->time = NULL;

	assert (0 != self->list_ref_cnt);
	assert (self->wave_list);
	assert (self->simmodule_list);
	return 0;
}

static PyObject *
SimModule_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	DEBUG_FUNC_STR;
	SimModule *self;

	/* get storage and init */
	self = (SimModule *)type->tp_alloc(type, 0);
	SimModuleType.tp_init((PyObject *)self, args, kwds);

	/* alloc static members */
	if (0 == self->list_ref_cnt) {
		assert(NULL == self->simmodule_list);
		self->simmodule_list = PyList_New(0);

		assert(NULL == self->wave_list);
		self->wave_list = PyList_New(0);

		self->list_ref_cnt++;
	}
	Py_INCREF((PyObject *)self);
	PyList_Append(self->simmodule_list, (PyObject *)self);

	return (PyObject *)self;
}

