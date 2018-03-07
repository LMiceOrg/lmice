#include "guava2loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <QDir>
Guava2Loader::Guava2Loader(const QString &name, const QString& path)
{
    if(!name.isEmpty())
    {
        if(!path.isEmpty())
        {
            QDir dir(path);
            m_name = dir.absoluteFilePath(name);
        }
        else
        {
            QDir dir(name);
            m_name = dir.absolutePath();
        }
    }
}

void Guava2Loader::codeList(const QStringList &codes)
{
    m_codes = codes;
}

QStringList &Guava2Loader::codes()
{
    return m_codes;
}

void Guava2Loader::load(const QString &name)
{
    if(!name.isEmpty())
        m_name = name;

    FILE* fp;
    fp = fopen(m_name.toLocal8Bit().data(), "rb");

}
