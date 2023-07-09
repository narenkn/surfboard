
char py_creg_err_str[256];

#include "py_creg.h"
#define protected public
#include "creg.cc"
#undef protected

static PyMethodDef PycReg_methods[] = {
  {"i", (PyCFunction)PycReg_lint_c, METH_VARARGS, "Return int/long object (control value). Accepts one arguments ::sb.DATA/::sb.CONTROL." },
  {"str", (PyCFunction)PycReg_str_c, METH_VARARGS, "Return string object with value. Accepts one arguments ::sb.OCT/sb.BIN/sb.DEC/sb.HEX." },
  {"push", (PyCFunction)PycReg_push_c, METH_VARARGS, "Pushes one bit either at lsb/msb. Takes optional argument ::sb.LSB/sb.MSB." },
  {"resize", (PyCFunction)PycReg_resize_c, METH_VARARGS, "Resize the reg. One/Two args [Msb:Lsb]." },
  {"pop", (PyCFunction)PycReg_pop_c, METH_VARARGS, "Returns the lsb/msb bit (sb.ZERO/sb.ONE/sb.UNKNOWN/sb.TRISTATE) and decreases the size by one. Optional argument ::sb.LSB/sb.MSB." },
  {NULL}  /* Sentinel */
};

static PyNumberMethods PycRegNumMethods = {
  (binaryfunc)PycReg_add,     /*	binaryfunc nb_add; */
  (binaryfunc)PycReg_sub,     /*	binaryfunc nb_subtract; */
  (binaryfunc)PycReg_mul,     /*	binaryfunc nb_multiply; */
  (binaryfunc)PycReg_div,     /*	binaryfunc nb_divide; */
  (binaryfunc)PycReg_rem,     /*	binaryfunc nb_remainder; */
  0,                          /*	binaryfunc nb_divmod; */
  0,                          /*	ternaryfunc nb_power; */
  (unaryfunc)PycReg_neg,      /*	unaryfunc nb_negative; */
  (unaryfunc)PycReg_pos,      /*	unaryfunc nb_positive; */
  (unaryfunc)PycReg_pos,      /*	unaryfunc nb_absolute; */
  (inquiry)PycReg_nonzero,    /*	inquiry nb_nonzero; */
  (unaryfunc)PycReg_invert,   /*	unaryfunc nb_invert; */
  (binaryfunc)PycReg_lshift,  /*	binaryfunc nb_lshift; */
  (binaryfunc)PycReg_rshift,  /*	binaryfunc nb_rshift; */
  (binaryfunc)PycReg_and,     /*	binaryfunc nb_and; */
  (binaryfunc)PycReg_xor,     /*	binaryfunc nb_xor; */
  (binaryfunc)PycReg_or,      /*	binaryfunc nb_or; */
  0,    											/*	coercion nb_coerce; */
  (unaryfunc)PycReg_lint,			/*	unaryfunc nb_int; */
  (unaryfunc)PycReg_lint,			/*	unaryfunc nb_long; */
  0,                          /*	unaryfunc nb_float; */
  (unaryfunc)PycReg_oct,      /*	unaryfunc nb_oct; */
  (unaryfunc)PycReg_hex,      /*	unaryfunc nb_hex; */
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

static PyTypeObject PycRegType = {
  PyObject_HEAD_INIT(NULL)
  0,                         /*ob_size*/
  "sb.reg",          /*tp_name*/
  sizeof(PycReg),              /*tp_basicsize*/
  0,                         /*tp_itemsize*/
  (destructor)PycReg_dealloc,/*tp_dealloc*/
  (printfunc)PycReg_print,   /*tp_print*/
  0,                         /*tp_getattr*/
  0,                         /*tp_setattr*/
  (cmpfunc)PycReg_cmp,       /*tp_compare*/
  (reprfunc)PycReg_repr,     /*tp_repr*/
  &PycRegNumMethods,         /*tp_as_number*/
  0,                         /*tp_as_sequence*/
  0,                         /*tp_as_mapping*/
  0,                         /*tp_hash */
  (ternaryfunc)PycReg_call,  /*tp_call*/
  (reprfunc)PycReg_str,      /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES | Py_TPFLAGS_HAVE_RICHCOMPARE, /*tp_flags*/
  "PycReg objects",           /* tp_doc */
  0,		               /* tp_traverse */
  0,		               /* tp_clear */
  (richcmpfunc)PycReg_richcompare,/* tp_richcompare */
  0,		               /* tp_weaklistoffset */
  0,		               /* tp_iter */
  0,		               /* tp_iternext */
  PycReg_methods,             /* tp_methods */
  0,             /* tp_members */
  0,           /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)PycReg_init,      /* tp_init */
  (allocfunc)PycReg_alloc,                         /* tp_alloc */
  PycReg_new,                 /* tp_new */
};

static PyObject *
PycReg_alloc(PyTypeObject *type, int nitems)
{
  DEBUG_FUNC_STR;
  PyObject *obj = PyObject_GC_New(PyObject, type);
  ((PycReg *)obj)->r = new cReg();
  //	printf("PycReg_new for %p\n", obj);
  DEBUG_FUNC_ENDSTR;
  return obj;
}

static void
PycReg_dealloc(PycReg* self)
{
  DEBUG_FUNC_STR;
  delete self->r;
  PyObject_GC_Del(self);
  DEBUG_FUNC_ENDSTR;
}

static int
PycReg_print(PycReg *reg, FILE *f, int flags)
{
  DEBUG_FUNC_STR;
  fprintf(f, "%s", reg->r->toHex().c_str());

  return 0;
}

static PyObject *
PycReg_str(PycReg *reg)
{
  return PycReg_str_c(reg, NULL);
}

static PyObject *
PycReg_repr(PycReg *reg)
{
  return PyString_FromFormat("reg{size:%p}:%s", reg, reg->r->toHex().c_str());
}

static PyObject *
PycReg_call(PycReg *reg, PyObject *args, PyObject *kwds)
{
  PyObject *o1=NULL, *o2=NULL, *o3=NULL;
  PyObject *t1, *t2;
  cReg::DataControl dc = cReg::DATA;
  uint32 o1_ui, o2_ui;

  DEBUG_FUNC_STR;
  if (!args || (!PyArg_ParseTuple(args, "O|OO", &o1, &o2, &o3))) {
    PyErr_SetString(PyExc_ValueError, "Accepts atleast one argument!");
    return NULL;
  }

  if (o1 && (!PyInt_Check(o1)) && (!PyLong_Check(o1))) {
    goto PycReg_call_index1;
  }
  if (o2 && (!PyInt_Check(o2)) && (!PyLong_Check(o2))) {
    goto PycReg_call_index1;
  }
  if (o3 && (!PyInt_Check(o3)) && (!PyLong_Check(o3))) {
    goto PycReg_call_index1;
  }

  if (o3) {
    if (! PyInt_Check(o3)) {
      PyErr_SetString(PyExc_TypeError, "Invalid argument!");
      return NULL;
    }
    dc = (cReg::DataControl) PyInt_AS_LONG(o3);
  }

  if (o1 && o2) {
    o1_ui = (uint32) (PyInt_Check(o1) ? PyInt_AsLong(o1) : PyLong_AsLong(o1));
    o2_ui = (uint32) (PyInt_Check(o2) ? PyInt_AsLong(o2) : PyLong_AsLong(o2));
    if (o2_ui > o1_ui) {
      uint32 ui1;
      ui1 = o1_ui; o1_ui = o2_ui; o2_ui = ui1;
    }

    /* */
    t1 = PycReg_lint_c(reg, o3);
    t2 = PyLong_Type.tp_as_number->nb_rshift(t1, o2);
    Py_DECREF(t1);
    if (PyInt_Check(t2)) {
      ((PyIntObject *)t2)->ob_ival &= ((uint64) -1) >> ((sizeof(uint64)<<3)-1-(o1_ui-o2_ui));
      return t2;
    }
    /* for long object */
    uint32 ui1, ui2, ui3;
    bool masked = false;
    ui2 = ABS(((PyLongObject *)t2)->ob_size);
    ui3 = o1_ui - o2_ui;
    for (ui1=0; ui1<ui2; ui1++) {
      if (masked) {
        ((PyLongObject *)t2)->ob_digit[ui1] = 0;
      } else if ((sizeof(digit)<<3) < ui3) {
        ((PyLongObject *)t2)->ob_digit[ui1] &= ((digit) -1) >> ((sizeof(digit)<<3)-1-ui3);
        masked = true;
      }
    }
    return t2;
  } else if (o1) {
    o1_ui = (uint32) (PyInt_Check(o1) ? PyInt_AsLong(o1) : PyLong_AsLong(o1));
    return PyInt_FromLong(reg->r->getInt32(o1_ui, o1_ui, dc));
  }

 PycReg_call_index1:
  PyErr_SetString(PyExc_ValueError, "Options are not of supported type!");
  return NULL;
}

static int
PycReg_cmp(PycReg *r1, PycReg *r2)
{
  DEBUG_FUNC_STR;
  /* Find in which operand is the PycReg type .. */
  if ((! PyObject_TypeCheck((PyObject *)r1, &PycRegType)) || (! PyObject_TypeCheck((PyObject *)r2, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "Attempt to compare on non-cReg objects!");
    return -1;
  }

  if (r1->r->isTripleEqual(*(r2->r)))
    return 0;
  else if (cReg::ONE == (*(r1->r) < *(r2->r)))
    return -1;
  else if (cReg::ONE == (*(r1->r) > *(r2->r)))
    return 1;
  return (*(r1->r) == *(r2->r));
}

static PyObject *
PycReg_richcompare(PyObject *o1, PyObject *o2, int flags)
{
  DEBUG_FUNC_STR;
  PycReg *r1;
  PyIntObject *i1;
  PyLongObject *l1;
  PyObject *ret;
  uint64 ui_res;

  //	cerr << "PycReg_richcompare called" << endl;
  if ((PyObject_TypeCheck(o1, &PycRegType)) &&
      (PyObject_TypeCheck(o2, &PycRegType))) {
    /* cReg, cReg */
    ui_res = PycReg_cmp((PycReg *)o1, (PycReg *)o2);
  } else if ((PyObject_TypeCheck(o1, &PycRegType)) &&
             (PyInt_Check(o2))) {
    /* cReg, int */
    r1 = (PycReg *) o1;
    i1 = (PyIntObject *) o2;
    ui_res = (*(r1->r) == i1->ob_ival) ? 0 :
      ((*(r1->r) > i1->ob_ival) ? 1 : -1);
  } else if ((PyObject_TypeCheck(o2, &PycRegType)) &&
             (PyInt_Check(o1))) {
    /* int, cReg */
    r1 = (PycReg *) o2;
    i1 = (PyIntObject *) o1;
    ui_res = (*(r1->r) == i1->ob_ival) ? 0 :
      ((*(r1->r) > i1->ob_ival) ? 1 : -1);
  } else if ((PyObject_TypeCheck(o1, &PycRegType)) &&
             (PyLong_Check(o2))) {
    l1 = (PyLongObject *) PycReg_lint((PycReg *)o1);
    ui_res = PyLong_Type.tp_compare((PyObject*) l1, o2);
  } else if ((PyObject_TypeCheck(o2, &PycRegType)) &&
             (PyLong_Check(o1))) {
    l1 = (PyLongObject *) PycReg_lint((PycReg *)o2);
    ui_res = PyLong_Type.tp_compare(o1, (PyObject*) l1);
  } else {
    PyErr_SetString(PyExc_TypeError, "Comparision can't be made!");
    return NULL;
  }

  /* comparison is done and 0/1/-1 is held in ui_res */
  ret = Py_False;
  if (ui_res == 0) {
    /* both are equal */
    if ((flags == Py_LE) || (flags == Py_EQ) || (flags == Py_GE))
      ret = Py_True;
  } else if (ui_res == 1) {
    /* LHS > RHS */
    if ((flags == Py_NE) || (flags == Py_GT) || (flags == Py_GE))
      ret = Py_True;
  } else {
    /* LHS < RHS */
    if ((flags == Py_NE) || (flags == Py_LT) || (flags == Py_LE))
      ret = Py_True;
  }

  Py_INCREF(ret);
  return ret;
}

static PyObject *
PycReg_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  DEBUG_FUNC_STR;

  PyObject *obj = PycReg_alloc(type, 0);

  DEBUG_FUNC_ENDSTR;
  return obj;
}

static int
PycReg_init(PycReg *self, PyObject *args, PyObject *kwds)
{
  DEBUG_FUNC_STR;
  PyObject *val=NULL, *val2=NULL;
  uint32 ui1, ui2;
  uint64 ul2;

  //	printf("PycReg_init for %p [%u:%u]\n", self, self->r->msb, self->r->lsb);
  /* parse args */
  if (!args || PyArg_ParseTuple(args, "")) {
    self->r->resize(0, 0);
    return 0;
  } else if (PyArg_ParseTuple(args, "iiO", &ui1, &ui2, &val)) {
    if (ui1 > ui2)
      self->r->resize(ui1, ui2);
    else
      self->r->resize(ui2, ui1);
    if (PyString_Check(val))
      *(self->r) = (char *) PyString_AsString(val);
    else if (PyLong_Check(val))
      *(self->r) = (uint64) PyLong_AsLong(val);
    else if (PyInt_Check(val))
      *(self->r) = (uint64) PyInt_AsLong(val);
    else
      self->r->initialize(cReg::ZERO);
  } else if (PyArg_ParseTuple(args, "ii", &ui1, &ui2)) {
    if (ui1 > ui2)
      self->r->resize(ui1, ui2);
    else
      self->r->resize(ui2, ui1);
    self->r->initialize(cReg::ZERO);
  } else if (PyArg_ParseTuple(args, "l", &ul2)) {
    self->r->resize(63, 0);
    *(self->r) = (uint64) ul2;
  } else if (PyArg_ParseTuple(args, "S", &val2)) {
    ui2 = (self->r->msb > self->r->lsb) ? self->r->msb - self->r->lsb : self->r->lsb - self->r->msb;
    if (ui2 < 63)
      self->r->resize(63, 0);
    //		printf("PycReg_init 1 %p [%u:%u] '%s'\n", self, self->r->msb, self->r->lsb, (char *) PyString_AsString(val2));
    *(self->r) = (char *) PyString_AsString(val2);
  } else {
    self->r->resize(0, 0);
    *(self->r) = 0;
  }

  PyErr_Clear();
  DEBUG_FUNC_ENDSTR;
  return 0;
}

static PyObject *
PycReg_add(PycReg *reg, PyObject *op)
{
  DEBUG_FUNC_STR;
  PycReg *r1, *r2, *r3;

  /* Find in which operand is the PycReg type .. */
  r1 = reg;
  if ((! PyObject_TypeCheck((PyObject *)reg, &PycRegType)) && (! PyObject_TypeCheck(op, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "PycReg operation called on non-PycReg objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &PycRegType)) {
    r1 = (PycReg *)op;
    op = (PyObject *)reg;
  }

  /* Check the next operand */
  if (PyInt_Check(op) || PyLong_Check(op)) {
    uint64 ui64_1 = PyInt_Check(op) ? PyInt_AS_LONG(op) : PyLong_AsLong(op);
    r3 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r3->r->resize(r1->r->getMsb(), r1->r->getLsb());
    *(r3->r) = ui64_1;
    *(r3->r) += *(r1->r);
    return (PyObject *) r3;
  } else if (PyObject_TypeCheck(op, &PycRegType)) {
    r3 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r2 = (PycReg *) op;
    r3->r->resizeAndCopy(*(r2->r));
    *(r3->r) += *(r1->r);
    return (PyObject *) r3;
  }

  PyErr_SetString(PyExc_TypeError, "Second operand should be int or PycReg type object");
  return NULL;
}

static PyObject *
PycReg_sub(PycReg *reg, PyObject *op)
{
  DEBUG_FUNC_STR;
  PycReg *r1, *r2, *r3;
  bool exchd = false;

  /* Find in which operand is the PycReg type .. */
  r1 = reg;
  if ((! PyObject_TypeCheck((PyObject *)reg, &PycRegType)) && (! PyObject_TypeCheck(op, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "PycReg operation called on non-PycReg objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &PycRegType)) {
    r1 = (PycReg *)op;
    op = (PyObject *)reg;
    exchd = true;
  }

  /* Check the next operand */
  if (PyInt_Check(op) || PyLong_Check(op)) {
    uint64 ui64_1 = PyInt_Check(op) ? PyInt_AS_LONG(op) : PyLong_AsLong(op);
    r3 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r3->r->resize(r1->r->getMsb(), r1->r->getLsb());
    *(r3->r) = ui64_1;
    if (exchd)
      *(r3->r) -= *(r1->r);
    else
      *(r1->r) -= *(r3->r);
    return (PyObject *) r3;
  } else if (PyObject_TypeCheck(op, &PycRegType)) {
    r3 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r2 = (PycReg *) op;
    r3->r->resizeAndCopy(*(r2->r));
    if (exchd)
      *(r3->r) -= *(r1->r);
    else
      *(r1->r) -= *(r3->r);
    return (PyObject *) r3;
  }

  PyErr_SetString(PyExc_TypeError, "Second operand should be int or PycReg type object");
  return NULL;
}

static PyObject *
PycReg_mul(PycReg *reg, PyObject *op)
{
  DEBUG_FUNC_STR;
  PycReg *r1, *r2, *r3;

  /* Find in which operand is the PycReg type .. */
  r1 = reg;
  if ((! PyObject_TypeCheck((PyObject *)reg, &PycRegType)) && (! PyObject_TypeCheck(op, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "PycReg operation called on non-PycReg objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &PycRegType)) {
    r1 = (PycReg *)op;
    op = (PyObject *)reg;
  }

  /* Check the next operand */
  if (PyInt_Check(op) || PyLong_Check(op)) {
    uint64 ui64_1 = PyInt_Check(op) ? PyInt_AS_LONG(op) : PyLong_AsLong(op);
    r2 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r2->r->resize(r1->r->getMsb(), r1->r->getLsb());
    *(r2->r) = r1->r->getInt64() * ui64_1;
    return (PyObject *) r2;
  } else if (PyObject_TypeCheck(op, &PycRegType)) {
    r2 = (PycReg *) op;
    r3 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r3->r->resize(r1->r->getMsb(), r1->r->getLsb());
    *(r3->r) = r1->r->getInt64() * r2->r->getInt64();
    return (PyObject *) r3;
  }

  PyErr_SetString(PyExc_TypeError, "Second operand should be int or PycReg type object");
  return NULL;
}

static PyObject *
PycReg_div(PycReg *reg, PyObject *op)
{
  DEBUG_FUNC_STR;
  PycReg *r1, *r2, *r3;
  bool exchd = false;

  /* Find in which operand is the PycReg type .. */
  r1 = reg;
  if ((! PyObject_TypeCheck((PyObject *)reg, &PycRegType)) && (! PyObject_TypeCheck(op, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "PycReg operation called on non-PycReg objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &PycRegType)) {
    r1 = (PycReg *)op;
    op = (PyObject *)reg;
    exchd = true;
  }

  /* Check the next operand */
  if (PyInt_Check(op) || PyLong_Check(op)) {
    uint64 ui64_1 = PyInt_Check(op) ? PyInt_AS_LONG(op) : PyLong_AsLong(op);
    r2 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r2->r->resize(r1->r->getMsb(), r1->r->getLsb());
    if (exchd)
      *(r2->r) = (uint64) (ui64_1 / r1->r->getInt64());
    else
      *(r2->r) = (uint64) (r1->r->getInt64() / ui64_1);
    return (PyObject *) r2;
  } else if (PyObject_TypeCheck(op, &PycRegType)) {
    r2 = (PycReg *) op;
    r3 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r3->r->resize(r1->r->getMsb(), r1->r->getLsb());
    *(r3->r) = r1->r->getInt64() / r2->r->getInt64();
    return (PyObject *) r3;
  }

  PyErr_SetString(PyExc_TypeError, "Second operand should be int or PycReg type object");
  return NULL;
}

static PyObject *
PycReg_rem(PycReg *reg, PyObject *op)
{
  DEBUG_FUNC_STR;
  PycReg *r1, *r2, *r3;
  bool exchd = false;

  /* Find in which operand is the PycReg type .. */
  r1 = reg;
  if ((! PyObject_TypeCheck((PyObject *)reg, &PycRegType)) && (! PyObject_TypeCheck(op, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "PycReg operation called on non-PycReg objects!");
    return NULL;
  } else if (PyObject_TypeCheck((PyObject *)op, &PycRegType)) {
    r1 = (PycReg *)op;
    op = (PyObject *)reg;
    exchd = true;
  }

  /* Check the next operand */
  if (PyInt_Check(op) || PyLong_Check(op)) {
    uint64 ui64_1 = PyInt_Check(op) ? PyInt_AS_LONG(op) : PyLong_AsLong(op);
    r2 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r2->r->resize(r1->r->getMsb(), r1->r->getLsb());
    if (exchd)
      *(r2->r) = (uint64) (ui64_1 % r1->r->getInt64());
    else
      *(r2->r) = (uint64) (r1->r->getInt64() % ui64_1);
    return (PyObject *) r2;
  } else if (PyObject_TypeCheck(op, &PycRegType)) {
    r2 = (PycReg *) op;
    r3 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r3->r->resize(r1->r->getMsb(), r1->r->getLsb());
    *(r3->r) = r1->r->getInt64() % r2->r->getInt64();
    return (PyObject *) r3;
  }

  PyErr_SetString(PyExc_TypeError, "Second operand should be int or PycReg type object");
  return NULL;
}

static PyObject *
PycReg_neg(PycReg *reg)
{
  DEBUG_FUNC_STR;
  PycReg *r1;

  r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
  r1->r->resize(reg->r->getMsb(), reg->r->getLsb());
  *(r1->r) -= *(reg->r);
  return (PyObject *) r1;
}

static PyObject *
PycReg_pos(PycReg *reg)
{
  DEBUG_FUNC_STR;
  PycReg *r1;

  r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
  r1->r->resizeAndCopy(*(reg->r));
  return (PyObject *) r1;
}

static PyObject *
PycReg_lint(PycReg *reg)
{
  DEBUG_FUNC_STR;
  return PycReg_lint_c(reg, NULL);
}

static PyObject *
PycReg_lint_c(PycReg *reg, PyObject *args)
{
  uint32 ui1, ui2, ui3, ui4, size, msb, _msb, lsb, _lsb;
  PyObject *z, *x1, *x2, *x3;
  PyIntObject *inp = NULL;
  cReg::DataControl dc = cReg::DATA;
  DEBUG_FUNC_STR;

  /* init */
  size = reg->r->getWidth();

  /* Check args */
  if (args && PyArg_ParseTuple(args, "O", (PyObject **)&inp)) {
    if (! PyInt_Check(inp)) {
      PyErr_SetString(PyExc_TypeError, "Invalid argument!");
      return NULL;
    }
    dc = (cReg::DataControl) PyInt_AS_LONG(inp);
  } else {
    PyErr_Clear();
  }

  /* Short cut, if small number */
  if (64 >= size)
    return (PyObject *) PyInt_FromLong(reg->r->getInt64(dc));

  /* Allocate the long int */
  ui2 = size >> 4; /* Naren: FIXME need to use log_10(1^^size) */
  z = PyLong_FromLong(0);
  if (z == NULL)
    return NULL;

  /* Just copy */
  msb = reg->r->msb;
  lsb = reg->r->lsb;
  x1 = PyInt_FromSize_t(32);
  for (ui4=0, ui1=0, _lsb=(msb&~0x1F); (ui1<ui2) && (_lsb <= msb); ui1++) {
    /* end condition */
    if (_lsb > msb) break;

    /* 32 bits at a time... obtain the next bits */
    _msb = (_lsb+31) > msb ? msb : _lsb+31;
    ui4 = reg->r->getInt32(_msb, _lsb, dc);
    _lsb -= 32; /* next _lsb */

    /* shift bits */
    x2 = PyNumber_Lshift(z, x1);
    if (NULL == x2) {
      return NULL;
    }
    Py_XDECREF(z);
    z = x2;

    /* OR with obtained values */
    x2 = PyInt_FromLong(ui4);
    x3 = PyNumber_Or(z, x2);
    if (NULL == x3) {
      return NULL;
    }
    Py_XDECREF(x2);
    Py_XDECREF(z);
    z = x3;
  }
  Py_XDECREF(x1);

  return (PyObject *) z;
}

PyObject *
PycReg_oct(PycReg *reg)
{
  char t_str[32];
  PyObject *z, *z1, *z2;

  DEBUG_FUNC_STR;
  z = PycReg_lint_c(reg, NULL);
  if (PyInt_Check(z)) {
    z1 = PyInt_Type.tp_as_number->nb_oct(z);
  } else if (PyLong_Check(z)) {
    z1 = PyLong_Type.tp_as_number->nb_oct(z);
  } else {
    PyErr_SetString(PyExc_TypeError, "PycReg or : Internal error (in str_c)!");
    return NULL;
  }
  Py_DECREF(z);

  sprintf(t_str, "%0d'o", reg->r->getWidth());
  z2 = (PyObject *) PyString_FromString(t_str);
  PyString_Concat(&z2, z1);
  Py_DECREF(z1);
  return z2;
}

PyObject *
PycReg_hex(PycReg *reg)
{
  DEBUG_FUNC_STR;
  return PyString_FromString(reg->r->toHex().c_str());
}

static PyObject *
PycReg_str_c(PycReg *reg, PyObject *args)
{
  PyObject *inp = NULL, *z, *z1;
  RadixFormat rf = HEX;

  DEBUG_FUNC_STR;
  /* Check args */
  if (args && PyArg_ParseTuple(args, "O", (PyObject **)&inp)) {
    if (! PyInt_Check(inp)) {
      PyErr_SetString(PyExc_TypeError, "Invalid argument!");
      return NULL;
    }
    rf = (RadixFormat) PyInt_AS_LONG(inp);
  } else {
    PyErr_Clear();
  }

  if (HEX == rf) {
    return PycReg_hex(reg);
  } else if (BIN == rf) {
    return PyString_FromString(reg->r->toBin().c_str());
  } else if (OCT == rf) {
    return PycReg_oct(reg);
  }

  z = PycReg_lint_c(reg, NULL);
  if (PyInt_Check(z)) {
    z1 = PyInt_Type.tp_repr(z);
  } else if (PyLong_Check(z)) {
    z1 = PyLong_Type.tp_repr(z);
  } else {
    PyErr_SetString(PyExc_TypeError, "PycReg or : Internal error (in str_c)!");
    return NULL;
  }

  Py_DECREF(z);
  return z1;
}

static PyObject *
PycReg_push_c(PycReg *reg, PyObject *args)
{
  PyObject *inp = NULL, *z = NULL;
  uint32 ui1;
  IndexFormat ixf;

  DEBUG_FUNC_STR;
  /* Check args */
  if (!args || (! PyArg_ParseTuple(args, "O|O", &inp, &z))) {
    PyErr_SetString(PyExc_TypeError, "Invalid arguments : first arg the value bit, optional second arg should be ::sb.LSB/sb.MSB!");
    return NULL;
  }

  if (z && (! PyInt_Check(z))) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument : second arg should be ::sb.LSB/sb.MSB!");
    return NULL;
  }
  ixf = (IndexFormat) (z ? PyInt_AS_LONG(z) : LSB);

  if ( (!PyLong_Check(inp)) && (!PyInt_Check(inp)) ) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument:push takes int/long!");
    return NULL;
  }
  ui1 = PyInt_Check(inp) ? PyInt_AS_LONG(inp) : PyLong_AsLong(inp);

  (LSB == ixf) ? reg->r->pushLsb((cReg::FourState)(ui1 & 0x3)) : reg->r->pushMsb((cReg::FourState)(ui1 & 0x3));

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
PycReg_resize_c(PycReg *reg, PyObject *args)
{
  PyObject *msb = NULL, *lsb = NULL;
  uint32 msb_i, lsb_i;
  DEBUG_FUNC_STR;

  /* Check args */
  if (!args || (! PyArg_ParseTuple(args, "O|O", &msb, &lsb))) {
    PyErr_SetString(PyExc_TypeError, "Invalid arguments : first arg the value bit, optional second arg should be ::sb.LSB/sb.MSB!");
    return NULL;
  }

  if ((NULL == msb) || ((!PyInt_Check(msb) && (!PyLong_Check(msb))))) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument : Expecting int arguments!");
    return NULL;
  }
  msb_i = PyInt_Check(msb) ? PyInt_AS_LONG(msb) : PyLong_AsLong(msb);

  if (NULL == lsb)
    lsb_i = 0;
  else {
    if ((!PyInt_Check(lsb) && (!PyLong_Check(lsb)))) {
      PyErr_SetString(PyExc_TypeError, "Invalid argument : Expecting int arguments!");
      return NULL;
    }
    lsb_i = PyInt_Check(lsb) ? PyInt_AS_LONG(lsb) : PyLong_AsLong(lsb);
  }

  reg->r->resize(msb_i, lsb_i);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
PycReg_pop_c(PycReg *reg, PyObject *args)
{
  PyObject *z = NULL;
  uint32 ui1;
  IndexFormat ixf;

  DEBUG_FUNC_STR;
  /* Check args */
  if (!args || (! PyArg_ParseTuple(args, "|O", &z))) {
    PyErr_SetString(PyExc_TypeError, "Invalid arguments : optional first arg should be ::sb.LSB/sb.MSB!");
    return NULL;
  }

  if (z && (! PyInt_Check(z))) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument : second arg should be ::sb.LSB/sb.MSB!");
    return NULL;
  }
  ixf = (IndexFormat) (z ? PyInt_AS_LONG(z) : LSB);

  ui1 = (LSB == ixf) ? reg->r->popLsb() : reg->r->popMsb();

  return PyInt_FromLong(ui1);
}

static int
PycReg_nonzero(PycReg *reg)
{
  DEBUG_FUNC_STR;
  if (reg->r->isAllZero())
    return 0;
  return 1;
}

static PyObject *
PycReg_invert(PycReg *reg)
{
  DEBUG_FUNC_STR;
  PycReg *r1;

  r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
  r1->r->resize(reg->r->getMsb(), reg->r->getLsb());
  ~*(r1->r);
  return (PyObject *) r1;
}

static PyObject *
PycReg_lshift(PycReg *reg, PyObject *op)
{
  DEBUG_FUNC_STR;
  PycReg *r1, *r2;
  PyIntObject *iobj;
  uint64 ul1;

  /* Find in which operand is the reg type .. */
  if ((! PyObject_TypeCheck((PyObject *)reg, &PycRegType)) && (! PyObject_TypeCheck(op, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "PycReg operation called on non-PycReg objects!");
    return NULL;
  }

  /* If both are PycReg types */
  if (PyObject_TypeCheck((PyObject *)reg, &PycRegType) && PyObject_TypeCheck(op, &PycRegType)) {
    r2 = (PycReg *) op;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r1->r->resizeAndCopy(*(reg->r));
    if (0 != r2->r->getInt64(cReg::CONTROL)) {
      PyErr_SetString(PyExc_TypeError, "PycReg operation called and rhs operand is X/Z!");
      return NULL;
    }
    ul1 = r2->r->getInt64();
    *(r1->r) <<= ul1;
    return (PyObject *) r1;
  }

  if (PyInt_Check(op)) {
    /* Implied : PyObject_TypeCheck((PyObject *)reg, &PycRegType) */
    iobj = (PyIntObject *)op;
    ul1 = iobj->ob_ival;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r1->r->resizeAndCopy(*(reg->r));
    *(r1->r) <<= ul1;
    return (PyObject *) r1;
  }

  if (PyInt_Check(reg)) {
    /* Implied : PyObject_TypeCheck((PyObject *)op, &PycRegType) */
    iobj = (PyIntObject *)reg;
    ul1 = iobj->ob_ival;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r2 = (PycReg *) op;
    r1->r->resizeAndCopy(*(r2->r));
    *(r1->r) <<= ul1;
    return (PyObject *) r1;
  }

  /* even long types are not supported */
  PyErr_SetString(PyExc_TypeError, "PycReg lshift : Internal error (only python-int type is suppored)!");
  return NULL;
}

static PyObject *
PycReg_rshift(PycReg *reg, PyObject *op)
{
  DEBUG_FUNC_STR;
  PycReg *r1, *r2;
  PyIntObject *iobj;
  uint64 ul1;

  /* Find in which operand is the reg type .. */
  if ((! PyObject_TypeCheck((PyObject *)reg, &PycRegType)) && (! PyObject_TypeCheck(op, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "PycReg operation called on non-PycReg objects!");
    return NULL;
  }

  /* If both are PycReg types */
  if (PyObject_TypeCheck((PyObject *)reg, &PycRegType) && PyObject_TypeCheck(op, &PycRegType)) {
    r2 = (PycReg *) op;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r1->r->resizeAndCopy(*(reg->r));
    if (0 != r2->r->getInt64(cReg::CONTROL)) {
      PyErr_SetString(PyExc_TypeError, "PycReg operation called and rhs operand is X/Z!");
      return NULL;
    }
    ul1 = r2->r->getInt64();
    *(r1->r) >>= ul1;
    return (PyObject *) r1;
  }

  if (PyInt_Check(op)) {
    /* Implied : PyObject_TypeCheck((PyObject *)reg, &PycRegType) */
    iobj = (PyIntObject *)op;
    ul1 = iobj->ob_ival;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r1->r->resizeAndCopy(*(reg->r));
    *(r1->r) >>= ul1;
    return (PyObject *) r1;
  }

  if (PyInt_Check(reg)) {
    /* Implied : PyObject_TypeCheck((PyObject *)op, &PycRegType) */
    iobj = (PyIntObject *)reg;
    ul1 = iobj->ob_ival;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r2 = (PycReg *) op;
    r1->r->resizeAndCopy(*(r2->r));
    *(r1->r) >>= ul1;
    return (PyObject *) r1;
  }

  PyErr_SetString(PyExc_TypeError, "PycReg rshift : Internal error (only python-int type is suppored)!");
  return NULL;
}

static PyObject *
PycReg_and(PycReg *reg, PyObject *op)
{
  DEBUG_FUNC_STR;
  PycReg *r1, *r2;
  PyIntObject *iobj;
  PyLongObject *lobj;

  /* Find in which operand is the reg type .. */
  if ((! PyObject_TypeCheck((PyObject *)reg, &PycRegType)) && (! PyObject_TypeCheck(op, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "PycReg operation called on non-PycReg objects!");
    return NULL;
  }

  /* If both are PycReg types */
  if (PyObject_TypeCheck((PyObject *)reg, &PycRegType) && PyObject_TypeCheck(op, &PycRegType)) {
    r2 = (PycReg *) op;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r1->r->resizeAndCopy(*(reg->r));
    *(r1->r) &= *(r2->r);
    return (PyObject *) r1;
  }

  if (PyInt_Check(op)) {
    /* Implied : PyObject_TypeCheck((PyObject *)reg, &PycRegType) */
    iobj = (PyIntObject *)op;
    cReg cr1(63,0);
    cr1 = (uint64)(iobj->ob_ival);
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r1->r->resizeAndCopy(*(reg->r));
    *(r1->r) &= cr1;
    return (PyObject *) r1;
  }

  if (PyInt_Check(reg)) {
    /* Implied : PyObject_TypeCheck((PyObject *)op, &PycRegType) */
    iobj = (PyIntObject *)reg;
    cReg cr1(63,0);
    cr1 = (uint64)(iobj->ob_ival);
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r2 = (PycReg *) op;
    r1->r->resizeAndCopy(*(r2->r));
    *(r1->r) &= cr1;
    return (PyObject *) r1;
  }

  if ( PyLong_Check(op) || PyLong_Check(reg) ) {
    if (PyLong_Check(reg)) {
      /* shuffle param */
      lobj = (PyLongObject *) reg;
      reg = (PycReg *) op;
      op = (PyObject *) lobj;
    }

    /* Is it a sane machine ? */
    if (0 != (32 % (sizeof(digit)<<3))) {
      PyErr_SetString(PyExc_SystemError, "Attempt to use op& LongInt in a machine with imcompatible digit size!");
      return NULL;
    }
    /* init */
    lobj = (PyLongObject *)op;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    /* check max size and resize, copy accordingly */
    uint32 len_lint = ABS(lobj->ob_size);
    //		bool   negint = (lobj->ob_size < 0) ? true : false;
    uint32 ui1 = len_lint * (sizeof(digit)<<3);
    ui1 = (ui1 < reg->r->getWidth()) ? reg->r->getWidth() : ui1;
    ui1 = (ui1+0x1F);
    ui1 &= ~0x1F;
    r1->r->resize(ui1-1, 0);
    reg->r->copy(*(r1->r));
    /* iterate through size of lint and operate */
    uint32 ui2, ui3=0, ui4, ui5, ui6;
    for (ui2=0, ui5=0; ui2<len_lint; ui2++) {
      ui4 = ui2 % sizeof(digit);
      if (0 == ui4)
        ui3 = lobj->ob_digit[ui2];
      else {
        ui6 = lobj->ob_digit[ui2];
        ui3 |= ui6 << (ui4 * (sizeof(digit)<<3));
      }
      /* store it, force store at end-of-loop */
      if ((0 == ((ui2+1) % sizeof(digit))) || ((ui2+1) == len_lint)) {
        r1->r->setDataWord(ui5, ui3 & r1->r->getDataWord(ui5));
        ui5++;
      }
    }

    return (PyObject *) r1;
  }

  PyErr_SetString(PyExc_TypeError, "PycReg and : Internal error (only python-int/long types are suppored)!");
  return NULL;
}

static PyObject *
PycReg_xor(PycReg *reg, PyObject *op)
{
  DEBUG_FUNC_STR;
  PycReg *r1, *r2;
  PyIntObject *iobj;
  PyLongObject *lobj;

  /* Find in which operand is the reg type .. */
  if ((! PyObject_TypeCheck((PyObject *)reg, &PycRegType)) && (! PyObject_TypeCheck(op, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "PycReg operation called on non-PycReg objects!");
    return NULL;
  }

  /* If both are PycReg types */
  if (PyObject_TypeCheck((PyObject *)reg, &PycRegType) && PyObject_TypeCheck(op, &PycRegType)) {
    r2 = (PycReg *) op;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r1->r->resizeAndCopy(*(reg->r));
    *(r1->r) ^= *(r2->r);
    return (PyObject *) r1;
  }

  if (PyInt_Check(op)) {
    /* Implied : PyObject_TypeCheck((PyObject *)reg, &PycRegType) */
    iobj = (PyIntObject *)op;
    cReg cr1(63,0);
    cr1 = (uint64)(iobj->ob_ival);
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r1->r->resizeAndCopy(*(reg->r));
    *(r1->r) ^= cr1;
    return (PyObject *) r1;
  }

  if (PyInt_Check(reg)) {
    /* Implied : PyObject_TypeCheck((PyObject *)op, &PycRegType) */
    iobj = (PyIntObject *)reg;
    cReg cr1(63,0);
    cr1 = (uint64)(iobj->ob_ival);
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r2 = (PycReg *) op;
    r1->r->resizeAndCopy(*(r2->r));
    *(r1->r) ^= cr1;
    return (PyObject *) r1;
  }

  if ( PyLong_Check(op) || PyLong_Check(reg) ) {
    if (PyLong_Check(reg)) {
      /* shuffle param */
      lobj = (PyLongObject *) reg;
      reg = (PycReg *) op;
      op = (PyObject *) lobj;
    }

    /* Is it a sane machine ? */
    if (0 != (32 % (sizeof(digit)<<3))) {
      PyErr_SetString(PyExc_SystemError, "Attempt to use op^ LongInt in a machine with imcompatible digit size!");
      return NULL;
    }
    /* init */
    lobj = (PyLongObject *)op;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    /* check max size and resize, copy accordingly */
    uint32 len_lint = ABS(lobj->ob_size);
    //		bool   negint = (lobj->ob_size < 0) ? true : false;
    uint32 ui1 = len_lint * (sizeof(digit)<<3);
    ui1 = (ui1 < reg->r->getWidth()) ? reg->r->getWidth() : ui1;
    ui1 = (ui1+0x1F);
    ui1 &= ~0x1F;
    r1->r->resize(ui1-1, 0);
    reg->r->copy(*(r1->r));
    /* iterate through size of lint and operate */
    uint32 ui2, ui3=0, ui4, ui5, ui6;
    for (ui2=0, ui5=0; ui2<len_lint; ui2++) {
      ui4 = ui2 % sizeof(digit);
      if (0 == ui4)
        ui3 = lobj->ob_digit[ui2];
      else {
        ui6 = lobj->ob_digit[ui2];
        ui3 |= ui6 << (ui4 * (sizeof(digit)<<3));
      }
      /* store it, force store at end-of-loop */
      if ((0 == ((ui2+1) % sizeof(digit))) || ((ui2+1) == len_lint)) {
        r1->r->setDataWord(ui5, ui3 ^ r1->r->getDataWord(ui5));
        ui5++;
      }
    }

    return (PyObject *) r1;
  }

  PyErr_SetString(PyExc_TypeError, "PycReg xor : Internal error (only python-int/long types are suppored)!");
  return NULL;
}

static PyObject *
PycReg_or(PycReg *reg, PyObject *op)
{
  DEBUG_FUNC_STR;
  PycReg *r1, *r2;
  PyIntObject *iobj;
  PyLongObject *lobj;

  /* Find in which operand is the reg type .. */
  if ((! PyObject_TypeCheck((PyObject *)reg, &PycRegType)) && (! PyObject_TypeCheck(op, &PycRegType))) {
    PyErr_SetString(PyExc_TypeError, "PycReg operation called on non-PycReg objects!");
    return NULL;
  }

  /* If both are PycReg types */
  if (PyObject_TypeCheck((PyObject *)reg, &PycRegType) && PyObject_TypeCheck(op, &PycRegType)) {
    r2 = (PycReg *) op;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r1->r->resizeAndCopy(*(reg->r));
    *(r1->r) |= *(r2->r);
    return (PyObject *) r1;
  }

  if (PyInt_Check(op)) {
    /* Implied : PyObject_TypeCheck((PyObject *)reg, &PycRegType) */
    iobj = (PyIntObject *)op;
    cReg cr1(63,0);
    cr1 = (uint64)(iobj->ob_ival);
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r1->r->resizeAndCopy(*(reg->r));
    *(r1->r) |= cr1;
    return (PyObject *) r1;
  }

  if (PyInt_Check(reg)) {
    /* Implied : PyObject_TypeCheck((PyObject *)op, &PycRegType) */
    iobj = (PyIntObject *)reg;
    cReg cr1(63,0);
    cr1 = (uint64)(iobj->ob_ival);
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    r2 = (PycReg *) op;
    r1->r->resizeAndCopy(*(r2->r));
    *(r1->r) |= cr1;
    return (PyObject *) r1;
  }

  if ( PyLong_Check(op) || PyLong_Check(reg) ) {
    if (PyLong_Check(reg)) {
      /* shuffle param */
      lobj = (PyLongObject *) reg;
      reg = (PycReg *) op;
      op = (PyObject *) lobj;
    }

    /* Is it a sane machine ? */
    if (0 != (32 % (sizeof(digit)<<3))) {
      PyErr_SetString(PyExc_SystemError, "Attempt to use op| LongInt in a machine with imcompatible digit size!");
      return NULL;
    }
    /* init */
    lobj = (PyLongObject *)op;
    r1 = (PycReg *) PycReg_new(&PycRegType, NULL, NULL);
    /* check max size and resize, copy accordingly */
    uint32 len_lint = ABS(lobj->ob_size);
    //		bool   negint = (lobj->ob_size < 0) ? true : false;
    uint32 ui1 = len_lint * (sizeof(digit)<<3);
    ui1 = (ui1 < reg->r->getWidth()) ? reg->r->getWidth() : ui1;
    ui1 = (ui1+0x1F);
    ui1 &= ~0x1F;
    r1->r->resize(ui1-1, 0);
    reg->r->copy(*(r1->r));
    /* iterate through size of lint and operate */
    uint32 ui2, ui3=0, ui4, ui5, ui6;
    for (ui2=0, ui5=0; ui2<len_lint; ui2++) {
      ui4 = ui2 % sizeof(digit);
      if (0 == ui4)
        ui3 = lobj->ob_digit[ui2];
      else {
        ui6 = lobj->ob_digit[ui2];
        ui3 |= ui6 << (ui4 * (sizeof(digit)<<3));
      }
      /* store it, force store at end-of-loop */
      if ((0 == ((ui2+1) % sizeof(digit))) || ((ui2+1) == len_lint)) {
        r1->r->setDataWord(ui5, ui3 ^ r1->r->getDataWord(ui5));
        ui5++;
      }
    }

    return (PyObject *) r1;
  }

  PyErr_SetString(PyExc_TypeError, "PycReg or : Internal error (only python-int/long types are suppored)!");
  return NULL;
}

