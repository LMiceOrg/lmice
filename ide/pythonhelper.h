#ifndef PYTHONHELPER_H
#define PYTHONHELPER_H

#include <stdarg.h>
#include <QObject>
#include <QVariant>
#include <QWidget>
#include <QString>
#include <Python.h>

#include <vector>
#include <map>

class WindowCallable
{
public:
    typedef int (WindowCallable::*callFunction)(qlonglong wid, const char* method, QVariant* params, size_t size);

};

extern "C"
PyObject* Window_call(qlonglong winid, const char* method, PyObject* args);

typedef int (*func_call) (qlonglong wid, const char* method, QVariant* params, size_t size);

class PythonHelper : public QObject
{
    Q_OBJECT
public:
    enum cmd_type{SINGLE_LINE=256, MULTI_LINE=257};
    PythonHelper(QObject* parent = 0, QWidget* window=NULL);
    virtual ~PythonHelper();

    static bool init();
    static void finit();
    void reload();

    int exec(const char* cmd, int type = SINGLE_LINE);
    int eval(const char* cmd);
    int interpreter(const char* cmd, int type=SINGLE_LINE);
    int callModel(const char *method, const char *format = NULL, ...);

    static PyObject *windowCall(long windowid, const char* method, PyObject* args);
    static int registerCall(long windowid, func_call func);
    static int registerClassCall(WindowCallable* wnd, WindowCallable::callFunction);

    const char* returnType() const;
    PyObject* returnObject() const;
    int checkError();

signals:
    void errorTrigger(const QString&);
    void outputTrigger(const QString&);
private:
    static QByteArray  m_progname;
    static QByteArray m_pythonhome;
    static std::vector<QVariant> m_params;
    static std::map<long, std::vector<func_call> > m_funcs;
    static std::map<long, std::vector<WindowCallable::callFunction> > m_classfuncs;
    static bool m_pyinitialized;

    PyObject *m_lmice;
    PyObject *m_retobj;
    PyObject *m_globals;
    QString m_emsg;



};

#endif // PYTHONHELPER_H
