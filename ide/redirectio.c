#include "pythonembed.h"
#include "lmice_trace.h"    /*< trace/log helper */

#include <stdio.h>

/**! member definition */
static PyMemberDef LS_RedirectIO_members[] = {
    /*{"size", T_INT, (Py_ssize_t)offsetof(LS_RedirectIO, m_ssize), 0, "size of instruments"},*/
    {NULL, 0, 0, 0, NULL}  /* Sentinel */
};

/**! Getter/setter functions definition */

/**! getter/setter definition */
static PyGetSetDef LS_RedirectIO_getseters[] = {
    {NULL, NULL, NULL, NULL, NULL}  /* Sentinel */
};


/**! default function definition */
static int
LS_RedirectIO_clear(LS_RedirectIO *self)
{
    return 0;
}

static void
LS_RedirectIO_dealloc(LS_RedirectIO* self)
{
    PyObject_GC_UnTrack(self);
    LS_RedirectIO_clear(self);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
LS_RedirectIO_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LS_RedirectIO *self;

    //lmice_debug_print("call LS_RedirectIO_new\n");

    self = (LS_RedirectIO *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->m_type = LMICE_TRACE_INFO;
    }

    return (PyObject *)self;
}


static int
LS_RedirectIO_init(LS_RedirectIO *self, PyObject *args, PyObject *kwds)
{
    int ret = 0;
    int tp = 0;

    (void)self;
    (void)args;
    (void)kwds;

    if(args && PyTuple_Check(args) )
        PyArg_ParseTuple(args, "|i", &tp);

    self->m_type = tp;
    self->m_stat = 0;

    //lmice_debug_print("call LS_RedirectIO_init %d\n" , tp);

    return ret;
}
static int
LS_RedirectIO_traverse(LS_RedirectIO *self, visitproc visit, void *arg)
{
    (void)self;
    (void)visit;
    (void)arg;
    //Py_VISIT(self->foo);
    return 0;
}

/**! functions definition*/
static PyObject *
LS_RedirectIO_write(LS_RedirectIO* self, PyObject *args)
{
    const char* str;
    Py_ssize_t len=0;
    int ret;

    ret = PyArg_ParseTuple(args, "s#", &str, &len);
    if(!ret)
    {
        PyErr_SetString(PyExc_TypeError, "the argument must be an string");
    }

    if(len > 1 && self->m_stat == 0)
    {
        eal_trace_color_print_per_thread(self->m_type);
        self->m_stat =1;
    }
    else if(len == 1 && str[0] == '\n')
    {
        self->m_stat = 0;
    }
    fwrite(str, 1, len, stdout);

    fflush(stdout);


    return PyLong_FromSsize_t(len);


}

/**! function method */
static PyMethodDef LS_RedirectIO_methods[] = {
    {"write", LS_RedirectIO_write, METH_VARARGS, "write string"},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

/**! object definition */
PyTypeObject LS_RedirectIO_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
        "lmice.RedirectIO",             /* tp_name */
        sizeof(LS_RedirectIO),             /* tp_basicsize */
        0,                         /* tp_itemsize */
        (destructor)LS_RedirectIO_dealloc, /* tp_dealloc */
        0,                         /* tp_print */
        0,                         /* tp_getattr */
        0,                         /* tp_setattr */
        0,                         /* tp_compare */
        0,                         /* tp_repr */
        0,                         /* tp_as_number */
        0,                         /* tp_as_sequence */
        0,                         /* tp_as_mapping */
        0,                         /* tp_hash */
        0,                         /* tp_call */
        0,                         /* tp_str */
        0,                         /* tp_getattro */
        0,                         /* tp_setattro */
        0,                         /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
            Py_TPFLAGS_BASETYPE |
            Py_TPFLAGS_HAVE_GC,    /* tp_flags */
        "RedirectIO objects",           /* tp_doc */
        (traverseproc)LS_RedirectIO_traverse,   /* tp_traverse */
        (inquiry)LS_RedirectIO_clear,           /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        LS_RedirectIO_methods,      /* tp_methods */
        LS_RedirectIO_members,      /* tp_members */
        LS_RedirectIO_getseters,    /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        (initproc)LS_RedirectIO_init,      /* tp_init */
        0,                         /* tp_alloc */
        LS_RedirectIO_new,                 /* tp_new */
};

void LS_RedirectIO_register(PyObject*m)
{
    if (PyType_Ready(&LS_RedirectIO_Type) < 0)
            return;
    Py_INCREF(&LS_RedirectIO_Type);
    PyModule_AddObject(m, "RedirectIO", (PyObject *)&LS_RedirectIO_Type);
}
