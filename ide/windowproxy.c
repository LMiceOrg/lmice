#include "pythonembed.h"
#include "lmice_trace.h"    /*< trace/log helper */

#include <stdio.h>

/**! member definition */
static PyMemberDef LS_WindowProxy_members[] = {
    {"windowid", T_LONG, (Py_ssize_t)offsetof(LS_WindowProxy, m_window), 0, "window id"},
    {NULL, 0, 0, 0, NULL}  /* Sentinel */
};

/**! Getter/setter functions definition */

/**! getter/setter definition */
static PyGetSetDef LS_WindowProxy_getseters[] = {
    {NULL, NULL, NULL, NULL, NULL}  /* Sentinel */
};

/**! default function definition */
static int
LS_WindowProxy_clear(LS_WindowProxy *self)
{
    return 0;
}

static void
LS_WindowProxy_dealloc(LS_WindowProxy* self)
{
    PyObject_GC_UnTrack(self);
    LS_WindowProxy_clear(self);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
LS_WindowProxy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LS_WindowProxy *self;

    //lmice_debug_print("call LS_RedirectIO_new\n");

    self = (LS_WindowProxy *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}


static int
LS_WindowProxy_init(LS_WindowProxy *self, PyObject *args, PyObject *kwds)
{
    int ret = 0;
    long tp = 0;

    (void)self;
    (void)args;
    (void)kwds;

    if(args && PyTuple_Check(args) )
        PyArg_ParseTuple(args, "|l", &tp);

//    self->m_type = tp;
//    self->m_stat = 0;
    self->m_window = tp;

    //lmice_debug_print("call LS_RedirectIO_init %d\n" , tp);

    return ret;
}
static int
LS_WindowProxy_traverse(LS_WindowProxy *self, visitproc visit, void *arg)
{
    (void)self;
    (void)visit;
    (void)arg;
    //Py_VISIT(self->foo);
    return 0;
}

void Window_call(long long winid, const char* method);

/**! functions definition*/
static PyObject* LS_WindowProxy_call(PyObject *self, PyObject *args)
{
    (void)self;
    int ret;
    long winid;
    const char* method;

    ret = PyArg_ParseTuple(args, "s", &method);
    if(ret)
    {
        Window_call(((LS_WindowProxy *)self)->m_window, method);
    }

    return PyInt_FromSsize_t(ret);

}

static PyObject* LS_WindowProxy_prop(PyObject *self, PyObject *args)
{
}

/**! function method */
static PyMethodDef LS_WindowProxy_methods[] =
{
    { "call", (PyCFunction)LS_WindowProxy_call, METH_VARARGS, "Call window function" },
    { "prop", (PyCFunction)LS_WindowProxy_prop, METH_VARARGS, "Get/Set props" },
    { NULL, NULL, 0, NULL },
};

/**! object definition */
PyTypeObject LS_WindowProxy_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
        "lmice.LS_WindowProxy",             /* tp_name */
        sizeof(LS_WindowProxy),             /* tp_basicsize */
        0,                         /* tp_itemsize */
        (destructor)LS_WindowProxy_dealloc, /* tp_dealloc */
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
        "LS_WindowProxy objects",           /* tp_doc */
        (traverseproc)LS_WindowProxy_traverse,   /* tp_traverse */
        (inquiry)LS_WindowProxy_clear,           /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        LS_WindowProxy_methods,      /* tp_methods */
        LS_WindowProxy_members,      /* tp_members */
        LS_WindowProxy_getseters,    /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        (initproc)LS_WindowProxy_init,      /* tp_init */
        0,                         /* tp_alloc */
        LS_WindowProxy_new,                 /* tp_new */
};


void init_windowproxy_module(PyObject* m)
{
    if (PyType_Ready(&LS_WindowProxy_Type) < 0)
            return;
    Py_INCREF(&LS_WindowProxy_Type);
    PyModule_AddObject(m, "Window", (PyObject *)&LS_WindowProxy_Type);

}
