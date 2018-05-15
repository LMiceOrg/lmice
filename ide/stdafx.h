#ifndef STDAFX_H
#define STDAFX_H

#include <Python.h>
#include <frameobject.h>

//Qt Library
#include <QApplication>
#include <QtWidgets>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QByteArray>
#include <QActionGroup>
#include <QFileIconProvider>

//Std library
#include <vector>
#include <string>

//Platform specs
#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <Shellapi.h>
#pragma comment(lib, "Shell32.lib")
#endif

#endif // STDAFX_H
