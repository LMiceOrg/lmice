#include "candlesticksetting.h"
#include "ui_candlesticksetting.h"


#include <QTextDocumentFragment>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include <QDebug>
#include <QToolTip>

#include <Python.h>
#include "pythonembed.h"
#include "pythonhelper.h"
#include "lmice_trace.h"

CandleStickSetting::CandleStickSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CandleStickSetting)
{
    ui->setupUi(this);

    QFont font = ui->textEdit->font();
    font.setPointSize(12);

    EditPressFilter* m_filter = new EditPressFilter(ui->textEdit);
    ui->textEdit->installEventFilter(m_filter);
    ui->textEdit->viewport()->installEventFilter(m_filter);
    m_highlighter = new Highlighter(ui->textEdit->document());

    ui->textEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->textEdit->setFocus();

    QObject::connect(ui->textEdit,&QTextEdit::cursorPositionChanged, [=]()
    {
        QList<QTextEdit::ExtraSelection> extraSelections;

           if (!ui->textEdit->isReadOnly()) {
               QTextEdit::ExtraSelection selection;

               QColor lineColor = QColor(Qt::yellow).lighter(160);

               selection.format.setBackground(lineColor);
               selection.format.setProperty(QTextFormat::FullWidthSelection, true);
               selection.cursor = ui->textEdit->textCursor();
               selection.cursor.clearSelection();
               extraSelections.append(selection);
           }

           ui->textEdit->setExtraSelections(extraSelections);
    });

    //Margin
    setContentsMargins(1,1,1,1);

    connect(m_filter, SIGNAL(loadFileEvent(QString,int,int)), this, SLOT(getOpenFile(QString,int,int)));
    connect(m_filter, SIGNAL(updateSettingEvent()), this, SLOT(updateSetting()));

    m_helper = new PythonHelper(this, this);
    CandleStickSetting::callFunction func =static_cast<WindowCallable::callFunction>( &CandleStickSetting::call);
    m_helper->registerClassCall(this, func);
    qDebug()<<"init python helper:"<<(qlonglong)(ui->textEdit);



}

CandleStickSetting::~CandleStickSetting()
{
}

int CandleStickSetting::call(qlonglong wid, const char *method, QVariant *params, size_t size)
{
    QString str = QString(method);
    qDebug()<<"call method "<<method<<size;
    if(str.compare("SetColor", Qt::CaseInsensitive) == 0)
    {
        if(size == 1)
        {
            QColor c = (*params).value<QColor>();
            emit changeColor(c);
        }
    }
    else if(str.compare("Clear", Qt::CaseInsensitive) == 0)
    {
        emit removeAllSeries();
    }
    else if(str.compare("GetColor", Qt::CaseInsensitive) == 0)
    {
        throw "GetColor no implement!";
    }
    return 0;
}

void CandleStickSetting::exec(const QString &cmd)
{
    m_helper->exec(cmd.toUtf8().data());
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.insertText(cmd);
}

void CandleStickSetting::getOpenFile(const QString &type, int begin, int end)
{
    QString name = QFileDialog::getOpenFileName(this, tr("Load %1 data").arg(type));
    if(name.isEmpty())
        return;
    name = QString("\"%1\"").arg(name);

    QTextCursor cursor(ui->textEdit->document());
    cursor.setPosition(begin);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(name);

    //qDebug()<<begin<<end;

}

void CandleStickSetting::updateSetting()
{
    //lmice_info_print("updating setting\n");
    QString code;
    QTextCursor cursor = ui->textEdit->textCursor();
    code = cursor.selection().toPlainText();
    if(code.isEmpty())
        code = ui->textEdit->toPlainText();
    m_helper->exec(code.toUtf8().data(), m_helper->MULTI_LINE);

//    //PyRun_SimpleString(code.toUtf8().data());
//    PyObject* mod = PyImport_ImportModule("__main__");
//    PyObject* globals = PyModule_GetDict(mod);
//    PyObject *sSource = PyString_FromString("source");
//    PyObject* level1 = PyDict_GetItem(globals, sSource);

//    if(level1 && PyType_IsSubtype(Py_TYPE(level1), &ChinaL1Source_Type) )
//    {
//       LS_ChinaL1Source* msg = (LS_ChinaL1Source*)level1;
//        emit updateChinaLevel1Msg(msg->m_list, msg->m_fsize);
//    }
//    lmice_info_print("updated setting\n");
    fflush(stdout);
    fflush(stderr);
}

EditPressFilter::EditPressFilter(QTextEdit *parent)
{
    m_edit = parent;
}

bool EditPressFilter::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *evt = static_cast<QKeyEvent*>(event);
        if( evt->key() == Qt::Key_Tab)
        {
            QTextCursor cursor = m_edit->textCursor();
            cursor.insertText("    ");
            return true;
        }
        else if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
        {
            QTextCursor cursor = m_edit->textCursor();
            QTextBlock block = cursor.block();

            QString s = block.text();
            int cnt = 0;
            foreach(auto c, s)
            {
                if(c == " ")
                    cnt ++;
                else
                    break;
            }
            cursor.insertBlock();
            for(int i=0; i<cnt;++i)
            {
                cursor.insertText(" ");
            }

            return true;
        }
        else if(evt->modifiers() == Qt::ControlModifier &&
                evt->key() == Qt::Key_R)
        {

            emit updateSettingEvent();
            return true;
        }
        else if(evt->modifiers() == Qt::ControlModifier &&
                evt->key() == Qt::Key_O)
        {
            QString name = QFileDialog::getOpenFileName(NULL, tr("Load setting from file"));
            if(!name.isEmpty())
            {
                QFile file(name);
                if( !file.open(QIODevice::ReadOnly|QIODevice::Text) )
                {
                    return true;
                }
                m_edit->setPlainText( file.readAll() );
            }

            return true;
        }

    }
    else if(event->type() == QEvent::MouseButtonDblClick)
    {
        QMouseEvent *evt = static_cast<QMouseEvent*>(event);
        if(evt->buttons() == Qt::RightButton)
        {
            qApp->sendEvent(m_edit->parent(), event);
            return true;
        }

        //qDebug()<<"double clicked!";
        QTextCursor cursor = m_edit->cursorForPosition(evt->pos());

        cursor.select(QTextCursor::WordUnderCursor);
        //qDebug()<<cursor.;
        QString act = cursor.selection().toPlainText();
        QTextBlock block = cursor.block();
        QString text = block.text();
        //qDebug()<<act;
        if(act == "load") {
                QRegularExpression loadExp = QRegularExpression("load\\(");
                QRegularExpressionMatch matchStart = loadExp.match(text, 0);
                int loadIndex = matchStart.capturedStart();
                if(loadIndex <0)
                    goto continueProcess;

                QRegularExpression loadEndExp = QRegularExpression("\\)");
                QRegularExpressionMatch match = loadEndExp.match(text, loadIndex);
                int endIndex = match.capturedStart();
                if(endIndex <0)
                    goto continueProcess;


                int begin = block.position()+loadIndex+matchStart.capturedLength();
                int end = block.position()+endIndex;
                emit loadFileEvent("guava2",begin, end );
                //qDebug()<<loadIndex<<endIndex<<matchStart.capturedLength();
                goto continueProcess;


        }


    }
continueProcess:

    return QObject::eventFilter(obj, event);
}
