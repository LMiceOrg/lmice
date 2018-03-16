#ifndef CANDLESTICKSETTING_H
#define CANDLESTICKSETTING_H
#include "pythonembed.h"

#include "highlighter.h"
#include <QWidget>
#include <QTextEdit>
#include <QSharedPointer>

class PythonHelper;


namespace Ui {
class CandleStickSetting;
}

class EditPressFilter : public QObject
 {
     Q_OBJECT
signals:
    void loadFileEvent(const QString&, int begin, int end);
    void updateSettingEvent();
public:
    EditPressFilter(QTextEdit* parent);

protected:
     bool eventFilter(QObject *obj, QEvent *event);
private:
     QTextEdit* m_edit;
     QPoint m_moveStart;
 };

class CandleStickSetting : public QWidget
{
    Q_OBJECT

signals:
    void updateChinaLevel1Msg(const Dummy_ChinaL1Msg* msg, int size);
    void changeColor(const QColor& color);
public:
    explicit CandleStickSetting(QWidget *parent = 0);
    ~CandleStickSetting();

protected slots:
    void getOpenFile(const QString&, int begin, int end);
    void updateSetting();
private:
    Ui::CandleStickSetting *ui;
    Highlighter * m_highlighter;
    QSharedPointer<PyObject> m_module;
    PythonHelper* m_helper;
};

#endif // CANDLESTICKSETTING_H
