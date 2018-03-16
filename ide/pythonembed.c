/**!
 # Embeded python

    a python helper module for stock/future exchange, backtesting and simulation

    The top most module names "lmice", includes several helper classes and to
instance DataSource objects, DataProcess objects, Builder objects, ...etc.

    The DataSource submodules represent formal stock/future/reporting data,
and also represent informal data, web data and user-defined documents. The
supported scenarios include .

    The DataProcess submodules represent how to deal with data source, mapping,
convertion, filtering, cleaning, calculation, transformation and reporting.

    The Builder submodules represent the control flow of financial engineering
system.


 */
#include "pythonembed.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>

#include "lmice_trace.h"


//static PyMemberDef ChinaL1Msg_members[] = {
//    ChinaL1Msg_Prop_Def(m_inst,             T_STRING,   "instrument")
//    ChinaL1Msg_Prop_Def(m_time,             T_LONG,     "micro time")
//    ChinaL1Msg_Prop_Def(m_bid,              T_DOUBLE,   "bid price")
//    ChinaL1Msg_Prop_Def(m_offer,            T_DOUBLE,   "bid price")
//    ChinaL1Msg_Prop_Def(m_bid_quantity,     T_INT,      "bid quantity")
//    ChinaL1Msg_Prop_Def(m_offer_quantity,   T_INT,      "offer quantity")
//    ChinaL1Msg_Prop_Def(m_volume,           T_INT,      "volume")
//    ChinaL1Msg_Prop_Def(m_notional,         T_DOUBLE,   "notional")
//    ChinaL1Msg_Prop_Def(m_limit_up,         T_DOUBLE,   "limit up")
//    ChinaL1Msg_Prop_Def(m_limit_down,       T_DOUBLE,   "limit down")
//    {NULL, 0, 0, 0, NULL}  /* Sentinel */
//};



//static PyObject*
//guava_load(PyObject *self, PyObject *args, PyObject* kw)
//{
//    static char* kw_list[]={"name", NULL};
//    char* str;
//    Py_ssize_t len;
//    int ret;

//    (void)self;
//    ret = PyArg_ParseTupleAndKeywords(args, kw, "s#", kw_list, &str, &len);
//    if(!ret)
//        return NULL;

//    FILE* fp;
//    struct stat st;
//    ret = stat(str, &st);
//    if(ret == 0 && (st.st_mode & S_IRUSR) )
//    {
//        fp = fopen(str, "rb");

//    if(fp)

//    return PyLong_FromSize_t(len);
//    //return Py_BuildValue("i", len);
//}

//static PyMethodDef EmbedLmiceModuleMethods[] = {
//    {"guava_load", guava_load, METH_VARARGS|METH_KEYWORDS,
//     "Load guava2 data from name(file)."},
//    {NULL, NULL, 0, NULL}
//};

PyObject *g_py_stdout = NULL;
PyObject *g_py_stderr = NULL;

PyObject* stdio_redirect(PyObject* self, PyObject* args)
{
    /**!
    PyObject * io;
    LS_RedirectIO* obj;
    io = LS_RedirectIO_Type.tp_new(&LS_RedirectIO_Type, self, args);
    obj = (LS_RedirectIO*)io;
    */


    if(g_py_stdout == NULL)
    {
        PyObject* mod = NULL;
        PyObject* o=NULL;
        PyObject* oe=NULL;
        PyObject* arg=NULL;
        PyObject* arg1=NULL;

        mod = PyImport_AddModule("sys");

        g_py_stdout = PyObject_GetAttrString(mod, "stdout");
        g_py_stderr = PyObject_GetAttrString(mod, "stderr");


        o = LS_RedirectIO_Type.tp_new(&LS_RedirectIO_Type, Py_None, Py_None);
        LS_RedirectIO_Type.tp_init(o, Py_None, Py_None);

        arg = PyTuple_New(1);
        arg1 = PyInt_FromLong(lmice_trace_error);
        PyTuple_SetItem(arg, 0, arg1);

        oe = LS_RedirectIO_Type.tp_new(&LS_RedirectIO_Type, Py_None, Py_None);
        LS_RedirectIO_Type.tp_init(oe, arg, Py_None);


        PyObject_SetAttrString(mod, "stdout", o);
        PyObject_SetAttrString(mod, "stderr", oe);


        Py_XDECREF(mod);
        Py_XDECREF(o);
        Py_XDECREF(oe);
        Py_XDECREF(arg);
        Py_XDECREF(arg1);
    }

    return Py_True;

}

PyObject* stdio_reset(PyObject* self, PyObject* args)
{
    if(g_py_stdout)
    {
        PyObject* mod = NULL;
        mod = PyImport_AddModule("sys");

        PyObject_SetAttrString(mod, "stdout", g_py_stdout);
        PyObject_SetAttrString(mod, "stderr", g_py_stderr);

        Py_XDECREF(mod);
        Py_XDECREF(g_py_stdout);
        Py_XDECREF(g_py_stderr);

        g_py_stdout=NULL;
        g_py_stderr=NULL;

    }
    return Py_True;
}


static PyMethodDef lmice_methods[] = {
    {"ioredirect", stdio_redirect, METH_NOARGS, "Redirect stdout and stderr"},
    {"ioreset", stdio_reset, METH_NOARGS, "Reset stdout and stderr"},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

extern PyTypeObject ChinaL1Source_Type;
PyMODINIT_FUNC
init_lmice_module(void)
{
    PyObject* m;
    m = Py_InitModule3("lmice", lmice_methods,
                       "LMICE module is a helper for stock/future exchange, backtesting and simulation.");

    if (m == NULL)
        return;

    PyModule_AddStringConstant(m, "__file__", "lmice.c");
    PyModule_AddStringConstant(m, "__package__", "lmice");

    init_chinal1source_object(m);
    LS_InstrumentFilter_register(m);
    LS_RedirectIO_register(m);
    init_windowproxy_module(m);

    PyObject* obj = PyModule_GetDict(m);
    lmice_critical_print("lmice dict:%s\n", PyString_AS_STRING( PyObject_Str(obj) ) );

}

void update_sys_path(const char* path)
{
    PyObject* mod =NULL;
    PyObject* o=NULL;
    PyObject* arg=NULL;

    mod = PyImport_AddModule("sys");

    o = PyObject_GetAttrString(mod, "path");
    if(PyList_Check(o))
    {
        arg = PyString_FromString(path);
        PyList_Insert(o, 0, arg);
    }

    Py_XDECREF(o);
    Py_XDECREF(arg);

}
