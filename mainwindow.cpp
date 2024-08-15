#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include "childwnd.h"
#include <QMdiSubWindow>
#include <QSignalMapper>
#include <QList>
#include <QFileDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initMainWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initMainWindow()
{
    //初始化字号的列表项
    QFontDatabase fontdb;
    foreach(int fontsize, fontdb.standardSizes())
        ui->sizeComboBox->addItem(QString::number(fontsize));
    QFont defFont;      //当前应用程序默认的字体
    QString sFontSize;
    int defFontSize;    //当前应用程序默认的字体的字号
    int defFontindex;   //当前字号再组合框中的索引号

    defFont = QApplication::font();
    defFontSize = defFont.pointSize();
    sFontSize = QString::number(defFontSize);
    defFontindex = ui->sizeComboBox->findText(sFontSize);

    ui->sizeComboBox->setCurrentIndex(defFontindex);

    //设置多文档区域的滚动条
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


    connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::refreshMenus);
    connect(ui->menu_W, &QMenu::aboutToShow, this, &MainWindow::addSubWndListMenu);

    m_WndMapper = new QSignalMapper(this);
    connect(m_WndMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    //保证互斥性，一次只能选择一种
    QActionGroup *alignGroup = new QActionGroup(this);
    alignGroup->addAction(ui->leftAlignAction);
    alignGroup->addAction(ui->centerAction);
    alignGroup->addAction(ui->rightAlignAction);
    alignGroup->addAction(ui->justifyAction);

}

void MainWindow::docNew()
{
    childWnd *childwnd = new childWnd;
    ui->mdiArea->addSubWindow(childwnd);
    connect(childwnd, SIGNAL(copyAvailable(bool)),ui->cutAction, SLOT(setEnable(bool)));
    connect(childwnd, SIGNAL(copyAvailable(bool)), ui->copyAction, SLOT(setEnable(bool)));

    childwnd->newDoc();
    childwnd->show();
    formatEnabled();
}

void MainWindow::docOpen()
{
    QString docName = QFileDialog::getOpenFileName(this,
                                 "打开文档",
                                 "",
                                 "文本文件(*.txt);;"
                                 "HTML文件(*.html *.htm);;"
                                 "所有文件(*.*)");
    if(!docName.isEmpty())
    {
        QMdiSubWindow *existWnd = findChildWnd(docName);
        if(existWnd)
            ui->mdiArea->setActiveSubWindow(existWnd);
        childWnd *childwnd = new childWnd;
        ui->mdiArea->addSubWindow(childwnd);
        connect(childwnd, SIGNAL(copyAvailable(bool)), ui->cutAction, SLOT(setEnabled(bool)));
        connect(childwnd, SIGNAL(copyAvailabel(bool)), ui->copyAction, SLOT(setEnabled(bool)));

        if(childwnd->loadDoc(docName))
        {
            statusBar()->showMessage("文档已打开",3000);
            childwnd->show();
            formatEnabled();
        }else{
            childwnd->close();
        }
    }

}

void MainWindow::docSave()
{
    if(activateChildWnd() && activateChildWnd()->saveDoc())
        statusBar()->showMessage("保存成功", 3000);
}

void MainWindow::docSaveAs()
{
    if(activateChildWnd() && activateChildWnd()->saveAsDoc())
        statusBar()->showMessage("保存成功", 3000);
}

void MainWindow::docPrint()
{
    QPrinter pter(QPrinter::HighResolution);
    QPrintDialog * ddlg = new QPrintDialog(&pter, this);
    if(activateChildWnd())
        ddlg->setOption(QAbstractPrintDialog::PrintSelection, true);
     ddlg->setWindowTitle("打印文档");
     childWnd *childwnd = activateChildWnd();
     if(ddlg->exec() == QDialog::Accepted)
         childwnd->print(&pter);
     delete ddlg;
}

void MainWindow::docPrintView()
{
    QPrinter pter;
    QPrintPreviewDialog preview(&pter, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void MainWindow::formatEnabled()
{
    ui->boldAction->setEnabled(true);
    ui->italicAction->setEnabled(true);
    ui->underlineAction->setEnabled(true);
    ui->leftAlignAction->setEnabled(true);
    ui->centerAction->setEnabled(true);
    ui->rightAlignAction->setEnabled(true);
    ui->justifyAction->setEnabled(true);
    ui->colorAction->setEnabled(true);
}

QMdiSubWindow *MainWindow::findChildWnd(const QString &docName)
{
    QString strFile = QFileInfo(docName).canonicalFilePath();
    foreach(QMdiSubWindow* subWnd, ui->mdiArea->subWindowList())
    {
        childWnd* childwnd = qobject_cast<childWnd*>(subWnd->widget());
        if(childwnd->m_CurDocPath == strFile)
            return subWnd;
        return 0;
    }

}

void MainWindow::on_newAction_triggered()
{
    docNew();
}

void MainWindow::refreshMenus()
{
    bool hasChild = false;
    hasChild = (activateChildWnd() != 0);

    ui->saveAction->setEnabled(hasChild);
    ui->saveAsAction->setEnabled(hasChild);
    ui->printAction->setEnabled(hasChild);
    ui->printPreviewAction->setEnabled(hasChild);
    ui->pasteAction->setEnabled(hasChild);
    ui->closeAction->setEnabled(hasChild);
    ui->closeAllAction->setEnabled(hasChild);
    ui->titleAction->setEnabled(hasChild);
    ui->cascadeAction->setEnabled(hasChild);
    ui->newAction->setEnabled(hasChild);
    ui->previousAction->setEnabled(hasChild);

    //文档打开且右内容选中
    bool hasSelect = (activateChildWnd() && activateChildWnd()->textCursor().hasSelection());

    ui->cutAction->setEnabled(hasSelect);
    ui->copyAction->setEnabled(hasSelect);
    ui->boldAction->setEnabled(hasSelect);
    ui->italicAction->setEnabled(hasSelect);
    ui->underlineAction->setEnabled(hasSelect);
    ui->leftAlignAction->setEnabled(hasSelect);
    ui->centerAction->setEnabled(hasSelect);
    ui->rightAlignAction->setEnabled(hasSelect);
    ui->justifyAction->setEnabled(hasSelect);
    ui->colorAction->setEnabled(hasSelect);
}

childWnd *MainWindow::activateChildWnd()
{
    QMdiSubWindow* actWnd = ui->mdiArea->activeSubWindow();
    if(actWnd)
        return qobject_cast<childWnd*>(actWnd->widget());
    else {
        return 0;
    }
}

void MainWindow::addSubWndListMenu()
{
    ui->menu_W->clear();
    ui->menu_W->addAction(ui->closeAction);
    ui->menu_W->addAction(ui->closeAllAction);
    ui->menu_W->addSeparator();
    ui->menu_W->addAction(ui->titleAction);
    ui->menu_W->addAction(ui->cascadeAction);
    ui->menu_W->addSeparator();
    ui->menu_W->addAction(ui->nextAction);
    ui->menu_W->addAction(ui->previousAction);
    QList<QMdiSubWindow*> wnds = ui->mdiArea->subWindowList();
    if(!wnds.empty())
        ui->menu_W->addSeparator();
    for(int i = 0; i < wnds.size(); ++i)
    {
        childWnd* childwnd = qobject_cast<childWnd*>(wnds.at(i)->widget());
        QString menuitem_text;
        menuitem_text = QString("%1 %2")
                .arg(i+1)
                .arg(childwnd->getcurDocName());
        QAction* menuitem_act = ui->menu_W->addAction(menuitem_text);
        menuitem_act->setCheckable(true);
        menuitem_act->setChecked(childwnd == activateChildWnd());

        connect(menuitem_act, SIGNAL(triggered(bool)), m_WndMapper, SLOT(map()));

        m_WndMapper->setMapping(menuitem_act, wnds.at(i));
    }

    formatEnabled();
}

void MainWindow::on_closeAction_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::on_closeAllAction_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_titleAction_triggered()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_cascadeAction_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_nextAction_triggered()
{
    ui->mdiArea->activateNextSubWindow();
}


void MainWindow::on_previousAction_triggered()
{
    ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::setActiveSubWindow(QWidget *wnd)
{
    if(!wnd)return;
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(wnd));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if(ui->mdiArea->currentSubWindow())
        event->ignore();        //忽略此事件
    else {
        event->accept();        //接受此事件
    }
}



void MainWindow::on_openAction_triggered()
{
    docOpen();
}

void MainWindow::on_saveAction_triggered()
{
    docSave();
}

void MainWindow::on_saveAsAction_triggered()
{
    docSaveAs();
}

void MainWindow::docUndo()
{
    if(activateChildWnd())
        activateChildWnd()->undo();
}

void MainWindow::docRedo()
{
    if(activateChildWnd())
        activateChildWnd()->redo();
}
void MainWindow::docCut()
{
    if(activateChildWnd())
        activateChildWnd()->cut();
}
void MainWindow::docPaste()
{
    if(activateChildWnd())
        activateChildWnd()->paste();
}

void MainWindow::docCopy()
{
    if(activateChildWnd())
        activateChildWnd()->copy();
}

void MainWindow::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->boldAction->isChecked()? QFont::Bold : QFont::Normal);
    if(activateChildWnd())
        activateChildWnd()->setFormatOnSelectWord(fmt);
}

void MainWindow::textUnderLine()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->underlineAction->isChecked());
    if(activateChildWnd())
        activateChildWnd()->setFormatOnSelectWord(fmt);
}

void MainWindow::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->italicAction->isChecked());
    if(activateChildWnd())
        activateChildWnd()->setFormatOnSelectWord(fmt);
}

void MainWindow::textFamily(const QString &fmly)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(fmly);
    if(activateChildWnd())
        activateChildWnd()->setFormatOnSelectWord(fmt);
}

void MainWindow::TextSize(const QString &ps)
{
    qreal pointSize = ps.toFloat();
    if(ps.toFloat() > 0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        if(activateChildWnd())
            activateChildWnd()->setFormatOnSelectWord(fmt);

    }
}

void MainWindow::paraStyle(int nStyle)
{
    if(activateChildWnd())
        activateChildWnd()->setParaStyle(nStyle);
}



void MainWindow::on_undoAction_triggered()
{
    docUndo();
}

void MainWindow::on_redoAction_triggered()
{
    docRedo();
}

void MainWindow::on_cutAction_triggered()
{
    docCut();
}

void MainWindow::on_copyAction_triggered()
{
    docCopy();
}

void MainWindow::on_pasteAction_triggered()
{
    docPaste();
}

void MainWindow::on_boldAction_triggered()
{
    textBold();
}

void MainWindow::on_italicAction_triggered()
{
    textItalic();
}


void MainWindow::on_underlineAction_triggered()
{
    textUnderLine();
}

void MainWindow::on_sizeComboBox_activated(const QString &arg1)
{
    TextSize(arg1);
}

void MainWindow::on_fontComboBox_activated(const QString &arg1)
{
    textFamily(arg1);
}

void MainWindow::on_leftAlignAction_triggered()
{
    if(activateChildWnd())
        activateChildWnd()->setAlignofDocumentText(1);
}

void MainWindow::on_centerAction_triggered()
{
    if(activateChildWnd())
        activateChildWnd()->setAlignofDocumentText(3);
}

void MainWindow::on_rightAlignAction_triggered()
{
    if(activateChildWnd())
        activateChildWnd()->setAlignofDocumentText(2);
}


void MainWindow::on_justifyAction_triggered()
{
    if(activateChildWnd())
        activateChildWnd()->setAlignofDocumentText(4);
}

void MainWindow::on_comboBox_activated(int index)
{
    paraStyle(index);
}

void MainWindow::on_printAction_triggered()
{
    docPrint();
}

void MainWindow::printPreview(QPrinter *printer)
{
    activateChildWnd()->print(printer);
}

void MainWindow::on_printPreviewAction_triggered()
{
    docPrintView();
}
