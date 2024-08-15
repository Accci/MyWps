#ifndef CHILDWND_H
#define CHILDWND_H
#include <QTextEdit>

class childWnd : public QTextEdit
{
    Q_OBJECT
public:
    childWnd();
    QString m_CurDocPath;   //当前的文档路径
    void newDoc();          //新建文件
    QString getcurDocName();//文件路径提取文档名
    bool loadDoc(const QString& docName);
    void setCurDoc(const QString& docName);
    bool saveDoc();
    bool saveAsDoc();
    bool saveDocOpt(QString docName);
    void setFormatOnSelectWord(const QTextCharFormat &fmt);
    void setAlignofDocumentText(int alignType);
    void setParaStyle(int pStyle);
protected:
    void closeEvent(QCloseEvent *event);
private:
    bool promptSave();
private slots:
    void docBeModified();   //文档修改时，窗口的标题栏加*
private:
    bool m_bSaved;          //文档是否保存
};

#endif // CHILDWND_H
