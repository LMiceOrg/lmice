#include "highlighter.h"
#include <QTextDocument>
#include <QDebug>
//! [0]
Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    QFont font = parent->defaultFont();

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns //! C++
            << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
            << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
            << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
            << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
            << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
            << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
            << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
            << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
            << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
            << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b"
               //! python
            << "\\bdef\\b" << "\\bsys\\b" << "\\bmath\\b"
            << "\\bos\\b" << "\\bre\\b" << "\\bprint\\b"
            << "\\breturn\\b" << "\\bimport\\b" << "\\bfor\\b"
            << "\\bin\\b" << "\\b__name__\\b" << "\\bif\\b" << "\\bwhile\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    QStringList operands;
    operands<<"\\band\\b"<<"\\bor\\b"<<"\\bnot\\b";
    keywordFormat.setForeground(Qt::red);
    foreach (const QString &pattern, operands) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    //! const value format
    QTextCharFormat valueFormat;
    valueFormat.setForeground(Qt::darkRed);
    valueFormat.setFontPointSize(font.pointSize()+2);
    valueFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    rule.pattern = QRegularExpression("(?<=\\=)\\s*[0-9.]+(|L|UL|LL|ULL|f)\\s*(?=\\;|,|\\s|$)");
    rule.format = valueFormat;
    highlightingRules.append(rule);

    //! String format
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\'[^\']*\'");
    highlightingRules.append(rule);

    //! Call Function format
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkBlue);
    functionFormat.setFontPointSize(font.pointSize()+2);
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    //! Function format

    functionFormat.setFontPointSize(font.pointSize()+4);
    functionFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("(?<=def)\\s+\\b[A-Za-z0-9_]+(?=\\s*\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);



    //! Internal function format
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    classFormat.setFontPointSize(font.pointSize()+3);
    rule.format = classFormat;


    rule.pattern = QRegularExpression("(?<="
                                      "sys\\.|math\\.|re\\."
                                      "|struct\\.|os.|os\\.path\\."
                                      ")"
                                      "\\b[A-Za-z0-9_]+");

    highlightingRules.append(rule);

    //! file load
    classFormat.setForeground(Qt::darkGray);
    rule.format = classFormat;


    rule.pattern = QRegularExpression("\\b("
                                      "guava2\\.|guava\\.|excel\\."
                                      ")"
                                      "load"
                                      "(?=\\()");

    highlightingRules.append(rule);


    //! [3]
    singleLineCommentFormat.setForeground(Qt::green);
    singleLineCommentFormat.setFontItalic(true);
    rule.format = singleLineCommentFormat;

    rule.pattern = QRegularExpression("//[^\n]*");
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression("#[^\n]*");
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::green);
    //! [3]


    //! [6]
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    //    //! Package load
    //    QRegularExpression loadExp = QRegularExpression("^\\s*load_guava2_package\\(");
    //    QRegularExpression loadEndExp = QRegularExpression("\\)");
    //    int loadIndex = text.indexOf(loadExp);
    //    if(loadIndex >= 0)
    //    {
    //        QRegularExpressionMatch match = loadEndExp.match(text, loadIndex);
    //        int endIndex = match.capturedStart();
    //        int commentLength = 0;
    //        if(endIndex >= 0)
    //        {
    //            bool drawImage = true;
    //            QTextBlock b = currentBlock();
    //            QTextCharFormat format = b.begin().fragment().charFormat();
    //            bool isImage = format.isImageFormat();
    //            if(isImage)
    //            {
    //                drawImage = false;
    //            }


    //            if(drawImage) {
    //                QTextCursor cursor(b);
    //                //cursor.setPosition(commentLength);
    //                QImage img("/home/hehao/work/lmice/ide/qcharts.png");
    //                img = img.scaled(16,16);
    //                document()->addResource(QTextDocument::ImageResource, QUrl("myimage"), img);

    //                cursor.insertImage("myimage");
    //                qDebug()<<"insert image:"<<b.text();

    //            }

    //        }
    //    }

    //! init state
    setCurrentBlockState(0);
    //! [8]
    int prevstate = previousBlockState();
    if (prevstate == -1)
        prevstate = 0;

    //! MultipleLine Comment
    int startIndex = 0;
    if (!(prevstate & MultipleLineComment))
        startIndex = text.indexOf(commentStartExpression);

    //!
    while (startIndex >= 0) {
        //! [10] //! [11]
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {

            setCurrentBlockState(currentBlockState() | MultipleLineComment);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }

    QString triplequote = "\"\"\"";
    startIndex = 0;
    if (!(prevstate & TripleDoubleQuote) )
        startIndex = text.indexOf(triplequote);
    while (startIndex >= 0)
    {
        int endIndex = 0;
        if (prevstate & TripleDoubleQuote)
            endIndex = text.indexOf(triplequote, startIndex);
        else
            endIndex = text.indexOf(triplequote, startIndex+3);

        int commentLength = 0;
        if(endIndex == -1)
        {
            setCurrentBlockState(currentBlockState()|TripleDoubleQuote);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + 3;
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(triplequote, startIndex + commentLength);

    }

    triplequote = "'''";
    startIndex = 0;
    if (!(prevstate & TripleQuote) )
        startIndex = text.indexOf(triplequote);
    while (startIndex >= 0)
    {
        int endIndex = 0;
        if (prevstate & TripleQuote)
            endIndex = text.indexOf(triplequote, startIndex);
        else
            endIndex = text.indexOf(triplequote, startIndex+3);

        int commentLength = 0;
        if(endIndex == -1)
        {
            setCurrentBlockState(currentBlockState()|TripleQuote);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + 3;
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(triplequote, startIndex + commentLength);

    }
}
//! [11]
