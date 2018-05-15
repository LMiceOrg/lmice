#include "pythonembed.h"    /*< Embeded python */
#include "lmice_trace.h"    /*< trace/log helper */

#include <stdarg.h>

/**! member definition */
static PyMemberDef LS_InstrumentFilter_members[] = {
    {"size", T_INT, (Py_ssize_t)offsetof(LS_InstrumentFilter, m_ssize), 0, "size of instruments"},
    {NULL, 0, 0, 0, NULL}  /* Sentinel */
};


/**! Getter/setter functions definition */
static PyObject *
LS_InstrumentFilter_get_inst(LS_InstrumentFilter *self, void *closure)
{
    PyObject *obj;
    (void)closure;

    if(self->m_ssize == 0)
        return PyString_FromString("");

    obj = PyList_New(self->m_ssize);
    for(int index=0; index<self->m_ssize; ++index)
    {
        PyObject* item;
        if(index < Primary_Instruments_Size)
            item = PyString_FromString(self->m_inst[index]);
        else
            item = PyString_FromString(self->m_list[index-Primary_Instruments_Size]);
        PyList_SET_ITEM(obj, index, item);
    }

    return obj;
}

static int
LS_InstrumentFilter_set_inst(LS_InstrumentFilter *self, PyObject *value, void *closure)
{
    char* v;
    (void)closure;
    if (value == NULL) {
        if(self->m_ssize > 0)
        {
            self->m_ssize = 0;
            self->m_lsize = 0;
            free(self->m_list);
            self->m_list = 0;
        }
        return 0;
    }

    if ( PyString_Check(value)) {
//        PyErr_SetString(PyExc_TypeError,
//                        "The data path attribute value must be a string");
        self->m_ssize = 1;

         v = PyString_AsString(value);
        memcpy(self->m_inst[0], v, strlen(v)>sizeof(inst8_t)-1?
                    strlen(v):sizeof(inst8_t)-1);
        return 0;
    }

    if( PyList_Check(value)) {
        int size;
        size = PyList_GET_SIZE(value);

        if(self->m_lsize+Primary_Instruments_Size < size)
        {
            free(self->m_list);
            self->m_lsize = size - Primary_Instruments_Size;
            self->m_list = (inst8_t*)malloc( sizeof(inst8_t)*self->m_lsize );
            memset(self->m_list, 0, sizeof(inst8_t)*self->m_lsize );
        }

        self->m_ssize = size;

        for(int index=0; index<self->m_ssize; ++index)
        {
            PyObject *item = PyList_GET_ITEM(value, index);

            if(! PyString_Check(item))
            {
                PyErr_SetString(PyExc_TypeError, "Instrument value must be a string,");
                self->m_ssize = 0;
                return -1;
            }

            v = PyString_AsString( item );

            if(index < Primary_Instruments_Size)
            {
                memset( self->m_inst[index], 0, sizeof(inst8_t) );
                memcpy( self->m_inst[index], v,
                        strlen(v)>sizeof(inst8_t)-1?
                                            strlen(v):sizeof(inst8_t)-1);
            }
            else
            {
                memset( self->m_list[index-Primary_Instruments_Size], 0, sizeof(inst8_t) );
                memcpy( self->m_list[index-Primary_Instruments_Size], v,
                        strlen(v)>sizeof(inst8_t)-1?
                                            strlen(v):sizeof(inst8_t)-1);
            }

        }

        return 0;
    }
lmice_debug_print("call tuple set inst\n");
    if( PyTuple_Check(value) )
    {
        int size;
        size = PyTuple_GET_SIZE(value);

        if(self->m_lsize+Primary_Instruments_Size < size)
        {
            free(self->m_list);
            self->m_lsize = size - Primary_Instruments_Size;
            self->m_list = (inst8_t*)malloc( sizeof(inst8_t)*self->m_lsize );
            memset(self->m_list, 0, sizeof(inst8_t)*self->m_lsize );
        }

        self->m_ssize = size;

        for(int index=0; index<self->m_ssize; ++index)
        {
            PyObject *item = PyTuple_GET_ITEM(value, index);

            if(! PyString_Check(item))
            {
                PyErr_SetString(PyExc_TypeError, "Instrument value must be a string,");
                self->m_ssize = 0;
                return -1;
            }

            v = PyString_AsString( item );

            if(index < Primary_Instruments_Size)
            {
                memset( self->m_inst[index], 0, sizeof(inst8_t) );
                memcpy( self->m_inst[index], v,
                        strlen(v)>sizeof(inst8_t)-1?
                                            strlen(v):sizeof(inst8_t)-1);
            }
            else
            {
                memset( self->m_list[index-Primary_Instruments_Size], 0, sizeof(inst8_t) );
                memcpy( self->m_list[index-Primary_Instruments_Size], v,
                        strlen(v)>sizeof(inst8_t)-1?
                                            strlen(v):sizeof(inst8_t)-1);
            }

        }

        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "Instrument value must a string, a list or a tuple.");


    return -1;
}

/**! getter/setter definition */
static PyGetSetDef LS_InstrumentFilter_getseters[] = {
    {"m_inst",
     (getter)LS_InstrumentFilter_get_inst, (setter)LS_InstrumentFilter_set_inst,
     "instrument",
     NULL},

    {NULL, NULL, NULL, NULL, NULL}  /* Sentinel */
};

/**! default function definition */
static int
LS_InstrumentFilter_clear(LS_InstrumentFilter *self)
{
    free(self->m_list);
    self->m_list = NULL;
    self->m_ssize =0;
    self->m_lsize=0;
    return 0;
}

static void
LS_InstrumentFilter_dealloc(LS_InstrumentFilter* self)
{
    PyObject_GC_UnTrack(self);
    LS_InstrumentFilter_clear(self);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
LS_InstrumentFilter_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LS_InstrumentFilter *self;

    lmice_debug_print("call LS_InstrumentFilter_new\n");

    self = (LS_InstrumentFilter *)type->tp_alloc(type, 0);
    if (self != NULL) {

        self->m_ssize = 0;
        self->m_lsize = 0;
        self->m_list = NULL;

    }

    return (PyObject *)self;
}


static int
LS_InstrumentFilter_init(LS_InstrumentFilter *self, PyObject *args, PyObject *kwds)
{
    const char *name=NULL;
    static char *kwlist[] = {"inst", NULL};
    PyObject *names = NULL;
    int len;
    int ret;

    lmice_debug_print("call LS_InstrumentFilter_init\n");


    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|s#()$", kwlist,
                                      &name, &len, &names))
    {
        return -1;
    }
    if(name)
    {
        memcpy(self->m_inst[0], name, len>sizeof(inst8_t)-1?sizeof(inst8_t)-1:len);
        self->m_ssize = 1;
    }
    if(names)
    {
        ret = LS_InstrumentFilter_set_inst(self, names, NULL);
    }



    return ret;
}
static int
LS_InstrumentFilter_traverse(LS_InstrumentFilter *self, visitproc visit, void *arg)
{
    (void)self;
    (void)visit;
    (void)arg;
    //Py_VISIT(self->foo);
    return 0;
}

/**! functions definition*/
static PyObject *
LS_InstrumentFilter_at(LS_InstrumentFilter* self, PyObject *args)
{
    int pos=0;
    int ret;

    ret = PyArg_ParseTuple(args, "i", &pos);
    if(!ret)
    {
        PyErr_SetString(PyExc_TypeError, "the argument must be an integer");
        return NULL;
    }
    if(pos <0)
    {
        PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("the argument must be a nonnegative integer,  {%d} given.", pos) );
        //PyErr_SetString(PyExc_TypeError, "the argument must be a nonnegative integer");
        return NULL;
    }

    if(pos >= self->m_ssize)
    {
        PyErr_SetString(PyExc_ValueError, "the argument is greater than the size");
        return NULL;
    }


    if(pos < Primary_Instruments_Size)
        return PyString_FromString( self->m_inst[pos] );
    else
        return PyString_FromString( self->m_list[pos - Primary_Instruments_Size] );


}

int LS_InstrumentFilter_check(LS_InstrumentFilter* self, const char*v)
{
    if(self->m_ssize == 0)
        return 0;

    for(int index=0; index<self->m_ssize; ++index)
    {
        int cp;
        if(index < Primary_Instruments_Size)
        {
            cp = strncmp(self->m_inst[index], v, sizeof(inst8_t));
        }
        else
        {
            cp = strncmp(self->m_list[index - Primary_Instruments_Size], v, sizeof(inst8_t));
        }

        if(cp == 0)
        {
            return 0;
        }
    }
    return -1;
}

static PyObject *
LS_InstrumentFilter_check_filter(LS_InstrumentFilter* self, PyObject *args)
{
    const char* v;
    int len;
    int ret;
    ret = PyArg_ParseTuple(args, "s#", &v, &len);
    if(!ret)
    {
        PyErr_SetString(PyExc_TypeError, "the argument must be an string");
        return Py_False;
    }

    ret = LS_InstrumentFilter_check(self, v);
    if(ret ==0)return Py_True;
    return Py_False;


}

/**! function method */
static PyMethodDef LS_InstrumentFilter_methods[] = {
    {"at", (PyCFunction)LS_InstrumentFilter_at, METH_VARARGS, "return the argument"},
    {"check", (PyCFunction)LS_InstrumentFilter_check, METH_VARARGS, "check the instrument"},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

/**! PySequenceMethods */
Py_ssize_t LS_InstrumentFilter_len(LS_InstrumentFilter* self)
{
    return self->m_ssize;
}

PyObject* LS_InstrumentFilter_item(LS_InstrumentFilter * self, Py_ssize_t idx)
{
    PyObject* obj;
    obj=PyInt_FromSsize_t(idx);

    return LS_InstrumentFilter_at(self, obj);
}
int LS_InstrumentFilter_contains(LS_InstrumentFilter * self, PyObject * value)
{
    if(! PyString_Check(value))
    {
        PyErr_SetString(PyExc_ValueError, "value must be a string");
        return -1;
    }

    lmice_debug_print("call LS_InstrumentFilter_contains\n");

    char* v;
    v = PyString_AsString(value);
    for(int index=0; index<self->m_ssize; ++index)
    {
        int cp;
        if(index < Primary_Instruments_Size)
        {
            cp = strcmp(self->m_inst[index], v);
        }
        else
        {
            cp = strcmp(self->m_list[index - Primary_Instruments_Size], v);
        }

        if(cp == 0)
        {
            return 1;
        }
    }
    return 0;
}

PyObject * LS_InstrumentFilter_concat(LS_InstrumentFilter *self, LS_InstrumentFilter *o)
{
    LS_InstrumentFilter* obj = NULL;
    int len = o->m_ssize + self->m_ssize;
    if(o && self)
    {
        obj = LS_InstrumentFilter_new(&LS_InstrumentFilter_Type, NULL, NULL);
    }

    obj->m_ssize = 0;
    obj->m_lsize = len-Primary_Instruments_Size;
    if(obj->m_lsize > 0)
    {
        obj->m_list = (inst8_t*)malloc(sizeof(inst8_t)*obj->m_lsize );
        memset(obj->m_list, 0, sizeof(inst8_t)*obj->m_lsize);
    }

    lmice_debug_print("concat %d + %d = %d\n", self->m_ssize, o->m_ssize, len);
    inst8_t *src;
    inst8_t *dst;
    for(int index = 0; index < self->m_ssize; ++index)
    {
        if(obj->m_ssize < Primary_Instruments_Size)
            dst = &obj->m_inst[obj->m_ssize];
        else
            dst = &obj->m_list[obj->m_ssize - Primary_Instruments_Size ];

        if(index < Primary_Instruments_Size)
            src = &self->m_inst[index];
        else
            src = &self->m_list[index - Primary_Instruments_Size];

        memcpy(dst, src, sizeof(inst8_t));
        ++obj->m_ssize;
    }

    for(int index = 0; index < o->m_ssize; ++index)
    {
        if(obj->m_ssize < Primary_Instruments_Size)
            dst = &obj->m_inst[obj->m_ssize];
        else
            dst = &obj->m_list[obj->m_ssize - Primary_Instruments_Size ];

        if(index < Primary_Instruments_Size)
            src = &o->m_inst[index];
        else
            src = &o->m_list[index - Primary_Instruments_Size];

        memcpy(dst, src, sizeof(inst8_t));
        ++obj->m_ssize;
    }

    return (PyObject*)obj;
}

static PySequenceMethods LS_InstrumentFilter_Sequence =
{
        (lenfunc)LS_InstrumentFilter_len,    /*< lenfunc sq_length*/
    (binaryfunc)LS_InstrumentFilter_concat, /*< binaryfunc sq_concat; */
    0, /*< ssizeargfunc sq_repeat; */
    (ssizeargfunc)LS_InstrumentFilter_item, /*< ssizeargfunc sq_item; */
    0, /*< ssizessizeargfunc sq_slice; */
    0, /*< ssizeobjargproc sq_ass_item; */
    0, /* ssizessizeobjargproc sq_ass_slice; */
    (objobjproc)LS_InstrumentFilter_contains, /* objobjproc sq_contains; */
    /* Added in release 2.0 */
    0, /* binaryfunc sq_inplace_concat; */
    0, /* ssizeargfunc sq_inplace_repeat; */
};

/**! object definition */
PyTypeObject LS_InstrumentFilter_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
        "lmice.InstrumentFilter",             /* tp_name */
        sizeof(LS_InstrumentFilter),             /* tp_basicsize */
        0,                         /* tp_itemsize */
        (destructor)LS_InstrumentFilter_dealloc, /* tp_dealloc */
        0,                         /* tp_print */
        0,                         /* tp_getattr */
        0,                         /* tp_setattr */
        0,                         /* tp_compare */
        0,                         /* tp_repr */
        0,                         /* tp_as_number */
        &LS_InstrumentFilter_Sequence,                         /* tp_as_sequence */
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
        "LS_InstrumentFilter objects",           /* tp_doc */
        (traverseproc)LS_InstrumentFilter_traverse,   /* tp_traverse */
        (inquiry)LS_InstrumentFilter_clear,           /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        LS_InstrumentFilter_methods,      /* tp_methods */
        LS_InstrumentFilter_members,      /* tp_members */
        LS_InstrumentFilter_getseters,    /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        (initproc)LS_InstrumentFilter_init,      /* tp_init */
        0,                         /* tp_alloc */
        LS_InstrumentFilter_new,                 /* tp_new */
};

void LS_InstrumentFilter_register(PyObject*m)
{
    if (PyType_Ready(&LS_InstrumentFilter_Type) < 0)
            return;
    Py_INCREF(&LS_InstrumentFilter_Type);
    PyModule_AddObject(m, "InstrumentFilter", (PyObject *)&LS_InstrumentFilter_Type);
}
