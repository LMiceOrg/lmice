#ifndef EMBEDPYTHON_H
#define EMBEDPYTHON_H

#define PyObject_DEFINED

#include "guavaproto.h"

#include <Python.h>
#include <frameobject.h>
#include <structmember.h>
#include <stdint.h>

/**! instrument type */
typedef char(inst8_t)[8];
typedef char(path256_t)[256];

#define Object_Name(X) X##_Object
#define Lmice_Module_Name(X) Object_Name(X##_Lmice)

#define DataSource_Module_Name(X) Lmice_Module_Name(X##_DataSource)
#define DataProcess_Module_Name(X) Lmice_Module_Name(X##_DataProcess)

#define LS_ChinaL1Msg DataSource_Module_Name(ChinaL1Msg)
#define LS_ChinaL1Source DataSource_Module_Name(ChinaL1Source)

#define LS_InstrumentFilter DataProcess_Module_Name(InstrumentFilter)

#define LS_RedirectIO DataProcess_Module_Name(IO)

#define LS_WindowProxy DataProcess_Module_Name(WindowProxy)

//"m_inst", T_STRING, offsetof(LS_ChinaL1Msg, m_data)+offsetof(Dummy_ChinaL1Msg, m_inst), 0, "first name"
#define Prop_Def(name, type, desc, pmsg, cmsg) { #name, type, (Py_ssize_t)(offsetof(pmsg, m_data)+offsetof(cmsg, name)), 0, desc},
#define ChinaL1Msg_Prop_Def(name, type, desc) Prop_Def(name, type, desc, LS_ChinaL1Msg, Dummy_ChinaL1Msg)

#define ChinaL1Source_Prop_Def(name, type, desc) Prop_Def(name, type, desc, LS_ChinaL1Source, Dummy_ChinaL1Msg)

//{ #name, type, offsetof(LS_ChinaL1Msg, m_data)+offsetof(Dummy_ChinaL1Msg, name), 0, desc},
#pragma pack(push, 4)
typedef struct _Dummy_ChinaL1Msg
{
    inst8_t m_inst;
    int64_t m_time;     /*< time in epoch micro seconds */
    double m_bid;
    double m_offer;
    int32_t m_bid_quantity;
    int32_t m_offer_quantity;
    int32_t m_volume;
    double m_notional;
    double m_limit_up;
    double m_limit_down;
}Dummy_ChinaL1Msg;
#pragma pack(pop)

enum { Primary_Instruments_Size=8};
typedef struct _LS_InstrumentFilter {
    PyObject_HEAD

    int32_t m_lsize; /*< list alloc size */
    int32_t m_ssize; /*< instruments size */
    inst8_t m_inst[Primary_Instruments_Size]; /*< primary instruments */
    inst8_t *m_list; /*< secondary instruments */

}LS_InstrumentFilter;

extern PyTypeObject ChinaL1Source_Type;
extern PyTypeObject LS_InstrumentFilter_Type;


typedef struct _ChinaL1Msg{
    PyObject_HEAD

    Dummy_ChinaL1Msg m_data;

} LS_ChinaL1Msg;

typedef struct _ChinaL1Source {
    PyObject_HEAD


    Dummy_ChinaL1Msg m_data;
    LS_InstrumentFilter* m_ifilter;

    int m_ftype; /*< source type */
    int m_fsize; /*< tick size */
    int m_fpos;
    path256_t m_path;

    Dummy_ChinaL1Msg *m_list;

} LS_ChinaL1Source;

extern PyTypeObject LS_RedirectIO_Type;
/**! Rerect IO */
typedef struct _RedirectIO {
    PyObject_HEAD

    int m_type; /*< io type*/
    int m_stat;

} LS_RedirectIO;

/**! Window Proxy */
extern PyTypeObject LS_WindowProxy_Type;
typedef struct _LS_WindowProxy
{
    PyObject_HEAD

    int64_t m_window;
} LS_WindowProxy;

struct pcap_pkghdr {
    unsigned int tv_sec;
    unsigned int tv_usec;
    unsigned int size;
    unsigned int length;
};
#pragma pack(push, 1)
struct pcap_pkgdat {
    char udp_head[42];
    struct guava_udp_head head;
    union {
        struct guava_udp_normal normal;
        struct guava_udp_summary summary;
    };
};

typedef struct pcap_pkgdat pcap_guava1;

struct pcap_pkgguava2 {
    char udp_head[42];
    SHFEQuotDataT quot;
};
#pragma pack(pop)

PyMODINIT_FUNC
init_lmice_module(void);

PyMODINIT_FUNC
init_chinal1source_object(PyObject*m);

PyMODINIT_FUNC
LS_InstrumentFilter_register(PyObject*m);

PyMODINIT_FUNC
LS_RedirectIO_register(PyObject*m);

PyMODINIT_FUNC
update_sys_path(const char* path);

PyMODINIT_FUNC
init_windowproxy_module(PyObject* m);

int LS_InstrumentFilter_check(LS_InstrumentFilter* self, const char*v);


#endif // EMBEDPYTHON_H
