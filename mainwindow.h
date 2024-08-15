#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "childwnd.h"
#include <QSignalMapper>
#include <QMdiSubWindow>
#include <QtPrintSupport/QPrinter>

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initMainWindow();
    void docNew();
    void docOpen();
    void docSave();
    void docSaveAs();
    void docPrint();
    void docPrintView();
    void docUndo();
    void docRedo();
    void docCut();
    void docCopy();
    void docPaste();
    void textBold();
    void textUnderLine();
    void textItalic();
    void textFamily(const QString &fmly);
    void TextSize(const QString &ps);
    void paraStyle(int nStyle);
private slots:
    void on_newAction_triggered();
    void refreshMenus();
    childWnd* activateChildWnd();
    void addSubWndListMenu();

    void on_closeAction_triggered();

    void on_closeAllAction_triggered();

    void on_titleAction_triggered();

    void on_cascadeAction_triggered();

    void on_nextAction_triggered();

    void on_previousAction_triggered();
    void setActiveSubWindow(QWidget* wnd);

    void on_openAction_triggered();

    void on_saveAction_triggered();

    void on_saveAsAction_triggered();

    void on_undoAction_triggered();

    void on_redoAction_triggered();

    void on_cutAction_triggered();

    void on_copyAction_triggered();

    void on_pasteAction_triggered();

    void on_boldAction_triggered();

    void on_italicAction_triggered();

    void on_underlineAction_triggered();

    void on_sizeComboBox_activated(const QString &arg1);

    void on_fontComboBox_activated(const QString &arg1);

    void on_leftAlignAction_triggered();

    void on_centerAction_triggered();

    void on_rightAlignAction_triggered();

    void on_justifyAction_triggered();

    void on_comboBox_activated(int index);

    void on_printAction_triggered();
    void printPreview(QPrinter* printer);

    void on_printPreviewAction_triggered();

protected:
    void closeEvent(QCloseEvent* event);

private:
       void formatEnabled();
       QMdiSubWindow *findChildWnd(const QString& docName);



private:
    Ui::MainWindow *ui;
    QSignalMapper* m_WndMapper;
};

#endif // MAINWINDOW_H
