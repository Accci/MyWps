#include "childwnd.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QTextDocumentWriter>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTextListFormat>
#include <QTextBlockFormat>
#include <QtWidgets>
childWnd::childWnd()
{
    //子窗口关闭时销毁该类的实例对象
    setAttribute(Qt::WA_DeleteOnClose);

    m_bSaved = false;
}

void childWnd::newDoc()
{
    static int wndSeqNum = 1;
    m_CurDocPath = QString("WPS 文档 %1").arg(wndSeqNum++);

    //设置窗体标题，文档改动后名称后+'*'号标识
    setWindowTitle(m_CurDocPath + "[*]" + "- MyWPS");
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(docBeModified()));

}

QString childWnd::getcurDocName()
{
    return QFileInfo(m_CurDocPath).fileName();

}

bool childWnd::loadDoc(const QString &docName)
{
    if(!docName.isEmpty()){
        QFile file(docName);
        if(!file.exists())
            return false;
        if(!file.open(QFile::ReadOnly))
            return false;
        QByteArray text= file.readAll();

        if(Qt::mightBeRichText(text))
            setHtml(text);
        else
            setPlainText(text);
        setCurDoc(docName);
        connect(document(), SIGNAL(contentsChanged()),this, SLOT(docBeModified()));

    }
}

void childWnd::setCurDoc(const QString &docName)
{
    m_CurDocPath = QFileInfo(docName).canonicalFilePath();
    m_bSaved = true;                //文档已保存
    document()->setModified(false); //文档未改动
    setWindowModified(false);       //窗口不显示改动标识
    setWindowTitle(getcurDocName() + "[*]");
}

bool childWnd::saveDoc()
{
    if(m_bSaved)
        return saveDocOpt(m_CurDocPath);
    else
        saveAsDoc();
}

bool childWnd::saveAsDoc()
{
    QString docName = QFileDialog::getSaveFileName(this,
                                 "另存为",
                                 m_CurDocPath,
                                 "HTML文档(*.htm *.html);;"
                                 "所有文件(*.*)");
    if(docName.isEmpty())
        return false;
    else
        return saveDocOpt(docName);
}

bool childWnd::saveDocOpt(QString docName)
{
    if(!(docName.endsWith(".htm", Qt::CaseInsensitive) || docName.endsWith("html", Qt::CaseInsensitive)))
        docName += ".html";
    QTextDocumentWriter writer(docName);
    bool isSuccess = writer.write(this->document());
    if(isSuccess)
        setCurDoc(docName);
    return isSuccess;
}

void childWnd::setFormatOnSelectWord(const QTextCharFormat &fmt)
{
    QTextCursor tcurcor = textCursor();
    if(!tcurcor.hasSelection()) tcurcor.select(QTextCursor::WordUnderCursor);
    tcurcor.mergeCharFormat(fmt);
    mergeCurrentCharFormat(fmt);
}

void childWnd::setAlignofDocumentText(int alignType)
{
    if(alignType == 1)
        setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if(alignType == 2)
        setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if(alignType == 3)
        setAlignment(Qt::AlignCenter);
    else if(alignType == 4)
        setAlignment(Qt::AlignJustify);

}

void childWnd::setParaStyle(int pStyle)
{
    QTextCursor tcursor = textCursor();

    QTextListFormat::Style sname;

    if(pStyle != 0)
    {
        switch (pStyle) {
        case 1:
            sname = QTextListFormat::ListDisc; // 黑圆实心
            break;
        case 2:
            sname = QTextListFormat::ListCircle; //空心圆
            break;
        case 3:
            sname = QTextListFormat::ListSquare; //方形
            break;
        case 4:
            sname = QTextListFormat::ListDecimal;  //十进制
            break;
        case 5:
            sname = QTextListFormat::ListLowerAlpha; //小写字母
            break;
        case 6:
            sname = QTextListFormat::ListUpperAlpha; //大写字母
            break;
        case 7:
            sname = QTextListFormat::ListUpperRoman; //大写罗马字母
            break;
        case 8:
            sname = QTextListFormat::ListLowerRoman; //小写罗马
            break;
        default:
            sname = QTextListFormat::ListDisc;
            break;
        }

        tcursor.beginEditBlock();
        QTextBlockFormat tBlockFmt = tcursor.blockFormat();
        QTextListFormat tListFmt;
        if(tcursor.currentList())
            tListFmt = tcursor.currentList()->format();
        else{
            tListFmt.setIndent(tBlockFmt.indent()+1);
            tBlockFmt.setIndent(0);
            tcursor.setBlockFormat(tBlockFmt);
        }

        tListFmt.setStyle(sname);
        tcursor.createList(tListFmt);
        tcursor.endEditBlock();
    }else{
        QTextBlockFormat tbfmt;
        tbfmt.setObjectIndex(-1);
        tcursor.mergeBlockFormat(tbfmt);
    }
}

void childWnd::closeEvent(QCloseEvent *event)
{
    if(promptSave())
        event->accept();
    else
        event->ignore();
}

bool childWnd::promptSave()
{
    if(!document()->isModified())
        return true;

    QMessageBox::StandardButton res;
    res = QMessageBox::warning(this,
                           QString("系统提示"),
                           QString("文档%1已修改，是否保存?")
                           .arg(getcurDocName()),
                           QMessageBox::Yes|
                           QMessageBox::Discard|
                           QMessageBox::No);
    if(res == QMessageBox::Yes)
        return saveDoc();
    else if(res == QMessageBox::No)
        return false;
    return true;
}

void childWnd::docBeModified()
{
    setWindowModified(document()->isModified());
}
