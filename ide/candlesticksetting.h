#ifndef CANDLESTICKSETTING_H
#define CANDLESTICKSETTING_H

#include "highlighter.h"
#include <QWidget>
#include <QTextEdit>

namespace Ui {
class CandleStickSetting;
}

class EditPressFilter : public QObject
 {
     Q_OBJECT
signals:
    void loadFileEvent(const QString&, int begin, int end);
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

public:
    explicit CandleStickSetting(QWidget *parent = 0);
    ~CandleStickSetting();
protected slots:
    void getOpenFile(const QString&, int begin, int end);
private:
    Ui::CandleStickSetting *ui;
    Highlighter * m_highlighter;
};

#endif // CANDLESTICKSETTING_H
