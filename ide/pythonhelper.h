#ifndef PYTHONHELPER_H
#define PYTHONHELPER_H

#include <stdarg.h>
#include <QObject>
#include <QWidget>
#include <QString>
#include <Python.h>

extern "C"
void Window_call(qlonglong winid, const char* method);

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
    int windowCall(const char* method);
    const char* returnType() const;
    PyObject* returnObject() const;
    int checkError();

signals:
    void errorTrigger(const QString&);
    void outputTrigger(const QString&);
private:
    static QByteArray  m_progname;
    static QByteArray m_pythonhome;

    PyObject *m_lmice;
    PyObject *m_retobj;
    PyObject *m_globals;

    static bool m_pyinitialized;
    QString m_emsg;

};

#endif // PYTHONHELPER_H
