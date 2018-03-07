#include "candlesticksetting.h"
#include "ui_candlesticksetting.h"

#include <QTextDocumentFragment>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include <QDebug>
#include <QToolTip>

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

    connect(m_filter, SIGNAL(loadFileEvent(QString,int,int)), this, SLOT(getOpenFile(QString,int,int)));


}

CandleStickSetting::~CandleStickSetting()
{
    delete ui;
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
//        else if(evt->modifiers() == Qt::ControlModifier &&
//                evt->key() == Qt::Key_R)
//        {
//            QMouseEvent new_event(QEvent::MouseButtonDblClick
//                                  ,QPointF(0,0)
//                                ,Qt::LeftButton
//                                  ,Qt::LeftButton
//                                  ,Qt::NoModifier);

//            qApp->sendEvent(m_edit->parent(), &new_event);
//            return true;
//        }
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
        qDebug()<<"double clicked!";
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


//                cursor.setPosition(block.position()+loadIndex+matchStart.capturedLength());
//                cursor.setPosition(block.position()+endIndex, QTextCursor::KeepAnchor);
//                cursor.removeSelectedText();
//                cursor.insertText(name);

                int begin = block.position()+loadIndex+matchStart.capturedLength();
                int end = block.position()+endIndex;
                emit loadFileEvent("guava2",begin, end );
                //qDebug()<<loadIndex<<endIndex<<matchStart.capturedLength();
                goto continueProcess;


        }

        qApp->sendEvent(m_edit->parent(), event);
        return true;
    }
//    else if(event->type() == QEvent::MouseMove)
//    {
//        QMouseEvent *evt = static_cast<QMouseEvent*>(event);
//        if(evt->buttons() == Qt::LeftButton)
//        {
//            QWidget* widget = static_cast<QWidget*>(m_edit->parent());
//            widget->move( widget->mapToParent(evt->pos() - m_moveStart) );
//        }

//    }
//    else if(event->type() == QEvent::MouseButtonPress)
//    {
//        QMouseEvent *evt = static_cast<QMouseEvent*>(event);
//        if(evt->buttons() == Qt::LeftButton)
//        {
//            m_moveStart = evt->pos();
//        }
//    }
//    {
//        QMouseEvent *evt = static_cast<QMouseEvent*>(event);
//        QTextCursor cursor = m_edit->cursorForPosition(evt->pos());
//        cursor.select(QTextCursor::WordUnderCursor);
//        if(cursor.selectedText() == "load")
//        {
//            QRegularExpression loadEndExp = QRegularExpression("\\b\\w+(?=\\.load)");
//            QRegularExpressionMatch match = loadEndExp.match(cursor.block().text());
//            QTextCursor cur(cursor.block());
//            cur.setPosition(match.capturedStart());
//            cur.setPosition(match.capturedStart()+match.capturedLength(), QTextCursor::KeepAnchor);
//            qDebug()<<match.capturedStart()<<match.capturedLength()<<cur.selectedText()<<cursor.block().text();
//            QString name = cur.selectedText();
//            //QString name = cursor.block( match.capturedLength()
//            //m_edit->setStatusTip(QString("double click to load \"%1\" file").arg(name) );
//            QToolTip::showText(evt->pos(), QString("double click to load \"%1\" file").arg(name),
//                    m_edit);

//        }
//        else
//        {
//            m_edit->setStatusTip("");
//        }
//    }
//    else if(event->type() == QEvent::Resize)
//    {
//        QResizeEvent* evt = static_cast<QResizeEvent*>(event);
//        m_edit->resize(evt->size());
//        qDebug()<<"resize";
//    }
continueProcess:

    return QObject::eventFilter(obj, event);
}
