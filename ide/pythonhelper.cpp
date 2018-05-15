

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>

#include "pythonembed.h"

#include "lmice_trace.h"
#include "pythonhelper.h"

#include "candlesticksetting.h"

Q_DECLARE_METATYPE(Dummy_ChinaL1Msg*);

bool PythonHelper::m_pyinitialized = false;
QByteArray PythonHelper::m_progname;
QByteArray PythonHelper::m_pythonhome;
std::vector<QVariant> PythonHelper::m_params;
std::map<long, std::vector<func_call> > PythonHelper::m_funcs;
std::map<long, std::vector<WindowCallable::callFunction> >
    PythonHelper::m_classfuncs;

PythonHelper::PythonHelper(QObject* parent, QWidget* window) : QObject(parent) {
  m_lmice = NULL;
  m_retobj = NULL;

  init();

  if (Py_IsInitialized() != 0) {
    m_pyinitialized = true;

    /** Import main module */
    m_globals = PyDict_New();
    PyObject* mod = PyImport_ImportModule("__main__");
    PyDict_Merge(m_globals, PyModule_GetDict(mod), 1);
    Py_XDECREF(mod);

    mod = PyImport_ImportModule("sys");
    PyDict_SetItemString(m_globals, "sys", mod);
    Py_XDECREF(mod);

    /** Import modeltool module */
    init_lmice_module();

    /** Add current app path to sys path */
    update_sys_path(m_pythonhome.data());

    char model_name[] = "lmice";
    m_lmice = PyImport_ImportModuleEx(model_name, m_globals, m_globals, NULL);
    PyDict_SetItemString(m_globals, "lmice", m_lmice);

    PyObject* obj = PyInt_FromLong((long)window);
    PyDict_SetItemString(m_globals, "windowid", obj);

    exec("window = lmice.Window(windowid)");
  }
}

PythonHelper::~PythonHelper() {
  if (m_pyinitialized) {
    Py_XDECREF(m_retobj);
    // Py_XDECREF(globals);
    // Py_XDECREF(locals);
    Py_XDECREF(m_lmice);
    // Py_XDECREF(mainmodel);
    m_lmice = NULL;
    // mainmodel = NULL;
    m_retobj = NULL;
    // Py_Finalize();
  }
}

bool PythonHelper::init() {
  if (m_pyinitialized) return true;

  m_progname = QByteArray("python27");

  /** Setting Python folder */
  QDir dir(qApp->applicationDirPath());

#if __APPLE__
  dir.cdUp();
  dir.cd("Resources");
  dir.cd("site-packages");
#endif

  m_pythonhome = dir.absolutePath().toLocal8Bit();
  // QMessageBox::warning(NULL, "", p);

#if !__APPLE__
  Py_SetProgramName(py_progname.data());
  //    qDebug()<<"program path:"<<Py_GetProgramFullPath();
  //    qDebug()<<"program name:"<<Py_GetProgramName();
  //    qDebug()<<"python path:"<<Py_GetPath() ;
  Py_SetPythonHome(py_pythonhome.data());
#endif

  lmice_info_print("program name:%s\n", Py_GetPythonHome());
  lmice_info_print("python path:%s\n", Py_GetPath());

  /** Initialize Python */
  Py_InitializeEx(0);

  if (Py_IsInitialized() != 0)
    m_pyinitialized = true;
  else
    m_pyinitialized = false;

  return m_pyinitialized;
}

void PythonHelper::finit() { Py_Finalize(); }

int PythonHelper::exec(const char* cmd, int type) {
  Py_XDECREF(m_retobj);
  m_retobj = NULL;

  m_retobj = PyRun_String(cmd, type, m_globals, m_globals);

  checkError();

  m_emsg = QString::fromUtf8(cmd) + "\n";
  emit outputTrigger(m_emsg);

  return 0;
}

int PythonHelper::eval(const char* cmd) {
  PyObject* code = Py_CompileString(cmd, "PythonHelper-exec.c", Py_eval_input);
  if (code) {
    m_retobj = PyEval_EvalCode((PyCodeObject*)code, m_globals, m_globals);
  }

  checkError();

  Py_XDECREF(code);

  m_emsg = QString::fromUtf8(cmd) + "\n";
  emit outputTrigger(m_emsg);

  return 0;
}

int PythonHelper::interpreter(const char* cmd, int type) {
  PyObject* code =
      Py_CompileString(cmd, "PythonHelper-exec.c", Py_single_input);
  if (code) {
    PyRun_String(cmd, type, m_globals, NULL);
  }
  Py_XDECREF(m_retobj);
  m_retobj = NULL;
  m_retobj = PyRun_String(cmd, type, m_globals, NULL);

  return 0;
}

int PythonHelper::callModel(const char* method, const char* format, ...) {
  PyObject* args = NULL;
  PyObject* md = NULL;
  va_list vl;
  int ret;

  Py_XDECREF(m_retobj);
  m_retobj = NULL;

  if (!m_lmice) return 1;

  md = PyString_FromString(method);
  lmice_warning_print("method type is :%s\n", md->ob_type->tp_name);

  if (format) {
    va_start(vl, format);
    args = Py_VaBuildValue(format, vl);
    va_end(vl);
    if (checkError() == 0)
      m_retobj = PyObject_CallMethodObjArgs(m_lmice, md, args, NULL);
  } else {
    checkError();
    lmice_critical_print("1\n");
    m_retobj = PyObject_CallMethodObjArgs(m_lmice, md, NULL);
    lmice_critical_print("2 %llu\n", (uint64_t)m_retobj);
  }
  ret = checkError();
  if (ret == 0) {
    PyObject* sargs = PyObject_Str(args);
    if (sargs) {
      lmice_warning_print("call lmice.%s%s\n", method,
                          PyString_AS_STRING(sargs));
    } else {
      lmice_warning_print("call lmice.%s()\n", method);
    }
  } else {
    lmice_warning_print("call lmice.%s error %d\n", method, ret);
  }

  Py_XDECREF(args);
  Py_XDECREF(md);

  return 0;
}

PyObject* PythonHelper::windowCall(long windowid, const char* method,
                                   PyObject* args) {
  QWidget* window = reinterpret_cast<QWidget*>(windowid);
  CandleStickSetting* setting = qobject_cast<CandleStickSetting*>(window);
  Py_ssize_t size = PyTuple_GET_SIZE(args);
  QString str = QString(method);

  m_params.clear();
  m_params.reserve(size);

  for (Py_ssize_t i = 0; i < size; ++i) {
    PyObject* o;
    o = PyTuple_GET_ITEM(args, i);
    if (PyObject_TypeCheck(Py_TYPE(o), &ChinaL1Source_Type)) {
      LS_ChinaL1Source* source;
      Dummy_ChinaL1Msg* data;
      int len;
      source = (LS_ChinaL1Source*)o;
      data = source->m_list;
      len = source->m_fsize;
      QVariant var = QVariant::fromValue(data);
      m_params.push_back(var);
    } else if (PyString_Check(o)) {
      const char* s = PyString_AS_STRING(o);
      QString data = QString(s);
      QVariant var = QVariant::fromValue(data);
      m_params.push_back(var);
    } else if (PyInt_Check(o)) {
      long data = PyInt_AS_LONG(o);
      QVariant var = QVariant::fromValue(data);
      m_params.push_back(var);
    } else if (PyFloat_Check(o)) {
      double data = PyFloat_AS_DOUBLE(o);
      QVariant var = QVariant::fromValue(data);
      m_params.push_back(var);
    }
  }

  try {
    int ret;
    auto ite = m_funcs.find(windowid);
    if (ite != m_funcs.end()) {
      std::vector<func_call>& funcs = ite->second;
      for (size_t i = 0; i < funcs.size(); ++i) {
        ret = funcs[i](windowid, method, &m_params[0], m_params.size());
        if (ret != 0) break;
      }
    }

    auto ite2 = m_classfuncs.find(windowid);
    if (ite2 != m_classfuncs.end()) {
      WindowCallable* wnd = reinterpret_cast<WindowCallable*>(windowid);
      std::vector<WindowCallable::callFunction>& funcs = ite2->second;
      for (size_t i = 0; i < funcs.size(); ++i) {
        ret = (wnd->*funcs[i])(windowid, method, &m_params[0], m_params.size());
        if (ret != 0) break;
      }
    }
    return PyInt_FromLong(0);

    if (str.compare("SetColor", Qt::CaseInsensitive) == 0) {
      PyObject* o;
      QColor c(Qt::white);
      if (size == 1) {
        o = PyTuple_GET_ITEM(args, 0);
        if (PyString_Check(o)) {
          const char* color = PyString_AS_STRING(o);
          c = QColor(color);

        } else if (PyInt_Check(o)) {
          int color = PyInt_AsLong(o);
          c = QColor(color);
        }
      } else if (size == 3) {
        int color[3];
        for (int i = 0; i < 3; ++i) {
          o = PyTuple_GET_ITEM(args, i);
          if (PyInt_Check(o)) color[i] = PyInt_AsLong(o);
        }
        c = QColor(color[0], color[1], color[2]);
      }
      if (setting) setting->changeColor(c);
    } else if (str.compare("GetColor", Qt::CaseInsensitive) == 0) {
      PyObject* o = PyTuple_New(3);
      PyTuple_SET_ITEM(o, 0, PyInt_FromLong(255));
      PyTuple_SET_ITEM(o, 1, PyInt_FromLong(255));
      PyTuple_SET_ITEM(o, 2, PyInt_FromLong(255));
      return o;
    } else if (str.compare("Clear", Qt::CaseInsensitive) == 0) {
      setting->removeAllSeries();
    } else if (str.compare("DrawSeries", Qt::CaseInsensitive) == 0) {
      int len = 0;
      Dummy_ChinaL1Msg* data = NULL;
      PyObject* o;
      if (size >= 1) {
        o = PyTuple_GET_ITEM(args, 0);
        LS_ChinaL1Source* source;
        if (o && PyType_IsSubtype(Py_TYPE(o), &ChinaL1Source_Type)) {
          source = (LS_ChinaL1Source*)o;
          data = source->m_list;
          len = source->m_fsize;
        }
      }
      if (size >= 2) {
        o = PyTuple_GET_ITEM(args, 1);
        if (PyInt_CheckExact(o)) {
          len = PyInt_AS_LONG(o);
        }
      }

      if (setting && data) setting->updateChinaLevel1Msg(data, len);
    }

    return PyInt_FromLong(0);
  } catch (const char* str) {
    PyErr_SetString(PyExc_RuntimeError, str);
  } catch (...) {
    PyErr_SetString(PyExc_RuntimeError, "Unknown error");
  }
  return PyInt_FromLong(-1);
}

int PythonHelper::registerCall(long windowid, func_call func) {
  auto ite = m_funcs.find(windowid);
  if (ite != m_funcs.end()) {
    std::vector<func_call>& funcs = ite->second;
    funcs.push_back(func);
  } else {
    std::vector<func_call> funcs;
    funcs.push_back(func);
    m_funcs.insert(std::pair<long, std::vector<func_call> >(windowid, funcs));
  }
  return 0;
}

int PythonHelper::registerClassCall(WindowCallable* wnd,
                                    WindowCallable::callFunction func) {
  qlonglong windowid = reinterpret_cast<qlonglong>(wnd);

  auto ite = m_classfuncs.find(windowid);
  if (ite != m_classfuncs.end()) {
    std::vector<WindowCallable::callFunction>& funcs = ite->second;
    funcs.push_back(func);
  } else {
    std::vector<WindowCallable::callFunction> funcs;
    funcs.push_back(func);
    m_classfuncs.insert(
        std::pair<long, std::vector<WindowCallable::callFunction> >(windowid,
                                                                    funcs));
  }

  return 0;
}

const char* PythonHelper::returnType() const {
  if (!m_retobj) return NULL;
  return m_retobj->ob_type->tp_name;
}

// template<class T>
// T PythonHelper::returnObject() const
//{
//    return T();
//}

// template<>
PyObject* PythonHelper::returnObject() const { return m_retobj; }

// template<>
// std::vector<Dummy_ChinaL1Msg> PythonHelper::returnObject() const
//{
//    std::vector<Dummy_ChinaL1Msg> vec;
//    if(m_retobj)
//    {
//        if( strcmp(ChinaL1Source_Type.tp_name, returnType()) == 0)
//        {
//            LS_ChinaL1Source* obj=(LS_ChinaL1Source*)m_retobj;
//            vec.resize(obj->m_fsize);
//            memcpy(&vec[0], obj->m_list,
//            sizeof(Dummy_ChinaL1Msg)*obj->m_fsize);
//        }
//    }
//    return vec;
//}

int PythonHelper::checkError() {
  int ret = 0;
  QString errType;
  // Print error stack
  if (PyErr_Occurred()) {
    ret = 1;
    m_emsg.clear();
    //        PyErr_Print();
    //        qDebug()<<"1";
    PyObject *ptp = NULL, *pv = NULL, *ptb = NULL;
    PyErr_Fetch(&ptp, &pv, &ptb);

    if (ptb) {
      //            qDebug()<<"3";

      PyTracebackObject* traceback = ((PyTracebackObject*)ptb);
      for (; traceback; traceback = traceback->tb_next) {
        PyCodeObject* codeobj = traceback->tb_frame->f_code;
        m_emsg += tr("Traceback (most recent call last):\n"
                     "    File \"%2\", line %3, in %1\n")
                      .arg(PyString_AsString(codeobj->co_name))
                      .arg(PyString_AsString(codeobj->co_filename))
                      .arg(traceback->tb_lineno);
      }
    }

    //        qDebug()<<"2";
    if (ptp && PyType_Check(ptp)) {
      PyObject* tp;
      tp = PyObject_GetAttrString(ptp, "__name__");
      errType = PyString_AS_STRING(tp);
      Py_XDECREF(tp);
      //            qDebug()<<"2.1";
      //            emsg += tr("%1\n")
      //                    .arg(((PyTypeObject*)(ptp))->tp_name);
    }

    if (pv && PyString_Check(pv)) {
      //            qDebug()<<"2.2";
      m_emsg += tr("%2: %1\n").arg(PyString_AS_STRING(pv)).arg(errType);

      // qDebug()<<"Error Value: emsg.c_str():"<<pv->ob_type->tp_name;
    } else if (pv && PyUnicode_Check(pv)) {
      PyObject* spv = PyUnicode_AsUTF8String(pv);

      m_emsg +=
          tr("%2: u\"%1\"\n").arg(tr(PyString_AS_STRING(spv))).arg(errType);
      Py_XDECREF(spv);

    } else if (pv && PyTuple_Check(pv)) {
      Py_ssize_t len = PyTuple_GET_SIZE(pv);
      if (len == 5) {
        m_emsg += tr("%6: Files: %2 (%3) \n %5 %1: %4\n")
                      .arg(PyString_AS_STRING(PyTuple_GET_ITEM(pv, 0)))
                      .arg(PyString_AS_STRING(PyTuple_GET_ITEM(pv, 1)))
                      .arg(PyString_AS_STRING(PyTuple_GET_ITEM(pv, 2)))
                      .arg(PyString_AS_STRING(PyTuple_GET_ITEM(pv, 3)))
                      .arg(PyString_AS_STRING(PyTuple_GET_ITEM(pv, 4)))
                      .arg(errType);
      } else {
        PyObject* spv = PyObject_Str(pv);
        m_emsg += tr("%2: %1\n").arg(PyString_AS_STRING(spv)).arg(errType);
        Py_XDECREF(spv);
      }

    } else if (pv) {
      PyObject* spv = PyObject_Str(pv);
      if (spv) {
        m_emsg += tr("%3: %1:%2\n")
                      .arg(pv->ob_type->tp_name)
                      .arg(PyString_AS_STRING(spv))
                      .arg(errType);
      } else {
        m_emsg += tr("%2: %1\n").arg(pv->ob_type->tp_name).arg(errType);
      }
      Py_XDECREF(spv);
    }

    PyErr_Clear();
    lmice_critical_print("%s", m_emsg.toUtf8().data());

    // emit errorTrigger(emsg);
  }
  return ret;
}

PyObject* Window_call(qlonglong winid, const char* method, PyObject* args) {
  return PythonHelper::windowCall(winid, method, args);
}
