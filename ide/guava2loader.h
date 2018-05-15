#ifndef GUAVA2LOADER_H
#define GUAVA2LOADER_H

#include <QString>
#include <QStringList>

class Guava2Loader
{
public:
    Guava2Loader(const QString& name ="", const QString& path="");
    void codeList(const QStringList& codes);
    QStringList& codes();
    void load(const QString& name = "");
private:
    QString m_name;
    QStringList m_codes;
};

#endif // GUAVA2LOADER_H
