#include "pythonembed.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>

#include "lmice_trace.h"

#define read_in(pos, sz) fread(buf+ pos, sz, 1 ,fp)
#define read_filehd() read_in(0, 24)
#define read_pkghdr() read_in(0, 16)
#define read_pkgdat() read_in(16, hdr->size)

static inline int decstring_to_int(const char* str, int pos, int len)
{
    const char* p = str+pos;
    int v = 0;
    for(int i=0; i<len; ++i)
    {
        if(p)
        {
            v = v*10 + (*p - 0x30);
            ++p;
        }
    }
    return v;
}

static inline long f_mktime(struct tm* pt)
{
    const long tmin=60;
    const long thour=60*tmin;
    const long tday=24*thour;
    const long tyear=365*tday;
    return (pt->tm_year-70)*tyear
            + (pt->tm_yday)*tday
            +(pt->tm_hour)*thour
            +pt->tm_min*tmin
            +pt->tm_sec;
}

static inline void readall_guava2(LS_ChinaL1Source* self) {
    char buf[256];
    struct pcap_pkghdr* hdr=(struct pcap_pkghdr*)(buf);
    struct pcap_pkgguava2* data =(struct pcap_pkgguava2*)(buf+16);
    const uint64_t dmax = 0x7fefffffffffffff;
    int ret;
    struct stat st;
    FILE* fp;

    /** Init */
    self->m_fsize = 0;
    if(self->m_list)
        free(self->m_list);

    ret = stat(self->m_path, &st);
    if(ret == 0 && (st.st_mode & S_IRUSR) )
    {
        fp = fopen(self->m_path, "rb");
        if(!fp)
        {
            return;
        }
        int cnt = st.st_size / (16+sizeof(struct pcap_pkgguava2) );
        //printf("cnt = %d, %lld, %lld, %lld\n", cnt, st.st_size, (16+sizeof(struct pcap_pkgguava2)), sizeof(Dummy_ChinaL1Msg) );
        self->m_list = (Dummy_ChinaL1Msg*)malloc(sizeof(Dummy_ChinaL1Msg)*cnt);
    }

    //printf("call read\n");
    time_t current;
    current = time(NULL);
    struct tm t;
    localtime_r(&current, &t);
    long tm_diff = mktime(&t) - f_mktime(&t);


    /* file header */
    read_filehd();

    do {
        /* package header */
        read_pkghdr();

        if(hdr->size != sizeof(struct pcap_pkgguava2))
        {
            fseek(fp, hdr->size, SEEK_CUR);
            if(feof(fp)) break;
            continue;
        }
        //printf("read %d\n", self->m_fsize);

        /* package data */
        read_pkgdat();

        /**! filter */
        if(LS_InstrumentFilter_check(self->m_ifilter, data->quot.InstrumentID) != 0)
            continue;



        if( ( (*(uint64_t*)&data->quot.AskPrice1) != dmax) &&
                ( *((uint64_t*)&data->quot.BidPrice1) != dmax) &&
                ( *((uint64_t*)&data->quot.LastPrice) != dmax) )
        {
            /** Calculate */
            Dummy_ChinaL1Msg* msg_dt = self->m_list+self->m_fsize;
            int64_t micro_time = 0;
            time_t data_time1;
            char* data_time = data->quot.UpdateTime;

            ++self->m_fsize;

            *((uint64_t*)msg_dt->m_inst) = *((uint64_t*)(data->quot.InstrumentID));

            //memcpy( tmp, data_time, 2 );
            t.tm_hour = decstring_to_int(data_time, 0, 2);
            //memcpy( tmp, data_time+3, 2 );
            t.tm_min = decstring_to_int(data_time, 3, 2);
            //memcpy( tmp, data_time+6, 2 );
            t.tm_sec = decstring_to_int(data_time, 6, 2);
            data_time1 = f_mktime(&t) + tm_diff;

            micro_time = (int64_t)data_time1*1000000LL+data->quot.UpdateMillisec*1000LL;//hdr->tv_usec;

            //Update ChinaL1Msg
            msg_dt->m_time = micro_time;

            msg_dt->m_limit_up = 0;
            msg_dt->m_limit_down = 0;

            msg_dt->m_bid = data->quot.BidPrice1;
            msg_dt->m_offer = data->quot.AskPrice1;
            msg_dt->m_bid_quantity = data->quot.BidVolume1;
            msg_dt->m_offer_quantity = data->quot.AskVolume1;
            msg_dt->m_volume = data->quot.Volume;//Volume ;
            msg_dt->m_notional = data->quot.Turnover;//Turnover;


        }

    } while(feof(fp) == 0);

    fclose(fp);

}


static PyMemberDef ChinaL1Source_members[] = {
    //ChinaL1Source_Prop_Def(m_inst,             T_STRING,   "instrument")
    ChinaL1Source_Prop_Def(m_time,             T_LONG,     "micro time")
    ChinaL1Source_Prop_Def(m_bid,              T_DOUBLE,   "bid price")
    ChinaL1Source_Prop_Def(m_offer,            T_DOUBLE,   "bid price")
    ChinaL1Source_Prop_Def(m_bid_quantity,     T_INT,      "bid quantity")
    ChinaL1Source_Prop_Def(m_offer_quantity,   T_INT,      "offer quantity")
    ChinaL1Source_Prop_Def(m_volume,           T_INT,      "volume")
    ChinaL1Source_Prop_Def(m_notional,         T_DOUBLE,   "notional")
    ChinaL1Source_Prop_Def(m_limit_up,         T_DOUBLE,   "limit up")
    ChinaL1Source_Prop_Def(m_limit_down,       T_DOUBLE,   "limit down")

    {"msg_size", T_INT, (Py_ssize_t)offsetof(LS_ChinaL1Source, m_fsize), 0, "size of messages"},
    {"msg_pos", T_INT, (Py_ssize_t)offsetof(LS_ChinaL1Source, m_fpos), 0, "message position"},
    {"inst_filter", T_OBJECT_EX, (Py_ssize_t)offsetof(LS_ChinaL1Source, m_ifilter), 0, "instrument filter"},
    //{"data_path", T_STRING, (Py_ssize_t)offsetof(LS_ChinaL1Source, m_path), 0, "source path"},
    {NULL, 0, 0, 0, NULL}  /* Sentinel */
};

static PyObject *
ChinaL1Source_get_data_path(LS_ChinaL1Source *self, void *closure)
{
    PyObject *obj;
    (void)closure;

    obj = PyString_FromString(self->m_path);

    return obj;
}

static int
ChinaL1Source_set_data_path(LS_ChinaL1Source *self, PyObject *value, void *closure)
{
    char* v;
    (void)closure;
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot set empty data path");
        return -1;
    }

    if (! PyString_Check(value)) {
        PyErr_SetString(PyExc_TypeError,
                        "The data path attribute value must be a string");
        return -1;
    }

    v = PyString_AsString(value);
    memset(self->m_path, 0, sizeof(self->m_path));
    memcpy(self->m_path, v, strlen(v)>sizeof(self->m_path)-1?
               strlen(v):sizeof(self->m_path)-1);


    return 0;
}

static PyObject *
ChinaL1Source_get_m_inst(LS_ChinaL1Source *self, void *closure)
{
    PyObject *obj;
    (void)closure;

    obj = PyString_FromString(self->m_data.m_inst);

    return obj;
}

static int
ChinaL1Source_set_m_inst(LS_ChinaL1Source *self, PyObject *value, void *closure)
{
    if (value == NULL) {


        PyErr_SetString(PyExc_TypeError, "Cannot set empty instrument");

        return 0;
    }

    if (! PyString_Check(value)) {
        PyErr_SetString(PyExc_TypeError,
                        "The instrument attribute value must be a string");
        lmice_warning_print("set m_inst failed\n");
        return -1;
    }

    char* v = PyString_AsString(value);
    memset(self->m_data.m_inst, 0, sizeof(self->m_data.m_inst));
    memcpy(self->m_data.m_inst, v, strlen(v)>sizeof(self->m_data.m_inst)-1?
               strlen(v):sizeof(self->m_data.m_inst)-1);


    return 0;
}

static PyGetSetDef ChinaL1Source_getseters[] = {
    {"m_inst",
     (getter)ChinaL1Source_get_m_inst, (setter)ChinaL1Source_set_m_inst,
     "instrument",
     NULL},
    {"data_path",
     (getter)ChinaL1Source_get_data_path, (setter)ChinaL1Source_set_data_path,
     "data path",
     NULL},

    {NULL, NULL, NULL, NULL, NULL}  /* Sentinel */
};


static int
ChinaL1Source_clear(LS_ChinaL1Source *self)
{
    memset(&(self->m_data), 0, sizeof(self->m_data));
    self->m_fsize =0;
    self->m_ftype=0;
    memset(self->m_path, 0, sizeof(self->m_path));
    return 0;
}

static void
ChinaL1Source_dealloc(LS_ChinaL1Source* self)
{
    PyObject_GC_UnTrack(self);
    ChinaL1Source_clear(self);
    Py_XDECREF(self->m_ifilter);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
ChinaL1Source_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LS_ChinaL1Source *self;

    self = (LS_ChinaL1Source *)type->tp_alloc(type, 0);
    if (self != NULL) {
        memset(&(self->m_data), 0, sizeof(self->m_data));
        self->m_ftype = 0;
        self->m_fsize = 0;
        self->m_list = NULL;
        memset(self->m_path, 0, sizeof(self->m_path) );

        self->m_ifilter = (LS_InstrumentFilter*) LS_InstrumentFilter_Type.tp_new(&LS_InstrumentFilter_Type, 0, 0);

        printf("call ChinaL1Source_new\n");

    }

    return (PyObject *)self;
}

static int load_file(LS_ChinaL1Source *self, const char* name, int len)
{
    if(name)
    {
        printf("name = %s\n", name);
        memcpy(self->m_path, name,
               sizeof(self->m_path)-1>len?len:sizeof(self->m_path)-1);
        if(strstr(name, "_guava2"))
        {
                    self->m_ftype = 1;
                    readall_guava2(self);
                    if(self->m_list)
                    {
                        Dummy_ChinaL1Msg* msg_dt = self->m_list;
                        //printf("msg_dt =%lf\n", msg_dt->m_bid);
                        //self->m_data.m_bid = msg_dt->m_bid;
                        memcpy(&(self->m_data), msg_dt, sizeof(Dummy_ChinaL1Msg));
                        self->m_fpos = 0;
                    }
                    return 0;
        }
    }
    return -1;
}

static int
ChinaL1Source_init(LS_ChinaL1Source *self, PyObject *args, PyObject *kwds)
{
    const char *name=NULL;
    static char *kwlist[] = {"name", "format", NULL};
    int len;
    int type;
    int ret;

    printf("call ChinaL1Source_init\n");


    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|s#i", kwlist,
                                      &name, &len, &type))
        return -1;

    if(name)
        return load_file(self, name, len);



    return 0;
}
static int
ChinaL1Source_traverse(LS_ChinaL1Source *self, visitproc visit, void *arg)
{
    (void)self;
    (void)visit;
    (void)arg;
    //Py_VISIT(self->foo);
    return 0;
}
static PyObject *
ChinaL1Source_load(LS_ChinaL1Source* self, PyObject *args)
{
    const char* name=NULL;
    int ret;
    int len;

    ret = PyArg_ParseTuple(args, "s#", &name, &len);
    if(!ret)
        return Py_False;

    ret = load_file(self, name, len);

    if(ret == 0) return Py_True;
    return Py_False;

}

static PyObject *
ChinaL1Source_at(LS_ChinaL1Source* self, PyObject *args)
{
    int pos=0;
    int ret;


    if(self->m_fsize <= 0)
        return Py_BuildValue("i", pos);

    ret = PyArg_ParseTuple(args, "i", &pos);
    if(!ret)
        return Py_BuildValue("i", pos);

    if(pos >= self->m_fsize || pos < 0)
    {
        return Py_BuildValue("i", pos);
    }

    Dummy_ChinaL1Msg* msg_dt = self->m_list+ pos;
    //printf("msg_dt =%lf\n", msg_dt->m_bid);
    //self->m_data.m_bid = msg_dt->m_bid;
    memcpy(&(self->m_data), msg_dt, sizeof(Dummy_ChinaL1Msg));
    self->m_fpos = pos;
    //printf(" %s m_data =%lf\n", self->m_data.m_inst, self->m_data.m_bid);


    return Py_BuildValue("i", pos);
}

static PyObject *
ChinaL1Source_next(LS_ChinaL1Source* self)
{
    if(self->m_fsize == 0)
        return NULL;

    ++self->m_fpos;
    if(self->m_fpos >= self->m_fsize)
        self->m_fpos = self->m_fsize-1;
    if(self->m_fpos < 0)
        self->m_fpos = 0;
    Dummy_ChinaL1Msg* msg_dt = self->m_list+ self->m_fpos;
    memcpy(&self->m_data, msg_dt, sizeof(Dummy_ChinaL1Msg));

    return NULL;

}

static PyMethodDef ChinaL1Source_methods[] = {
    {"load",(PyCFunction)ChinaL1Source_load, METH_VARARGS, "Load ChinaL1 Message"},
    {"at", (PyCFunction)ChinaL1Source_at, METH_VARARGS, "Get index ChinaL1 Message"},
    {"next", (PyCFunction)ChinaL1Source_next, METH_NOARGS, "Get next ChinaL1 Message"},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

PyTypeObject ChinaL1Source_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
        "lmice.ChinaL1Source",             /* tp_name */
        sizeof(LS_ChinaL1Source),             /* tp_basicsize */
        0,                         /* tp_itemsize */
        (destructor)ChinaL1Source_dealloc, /* tp_dealloc */
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
        "ChinaL1Source objects",           /* tp_doc */
        (traverseproc)ChinaL1Source_traverse,   /* tp_traverse */
        (inquiry)ChinaL1Source_clear,           /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        ChinaL1Source_methods,      /* tp_methods */
        ChinaL1Source_members,      /* tp_members */
        ChinaL1Source_getseters,    /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        (initproc)ChinaL1Source_init,      /* tp_init */
        0,                         /* tp_alloc */
        ChinaL1Source_new,                 /* tp_new */
};

void init_chinal1source_object(PyObject*m)
{
    if (PyType_Ready(&ChinaL1Source_Type) < 0)
            return;
    Py_INCREF(&ChinaL1Source_Type);
    PyModule_AddObject(m, "ChinaL1Source", (PyObject *)&ChinaL1Source_Type);
}
