#include "mainwindow.h"
#include "common.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindow();

    connect(rootPathButton,SIGNAL(clicked(bool)),this,SLOT(selectDiractory()));
    connect(templateList,SIGNAL(currentIndexChanged(QString)),templateControl,SLOT(loadTemplate(QString)));
    connect(templateView,SIGNAL(itemClickedView(QStandardItem*)),propertyView,SLOT(setCurrentItem(QStandardItem*)));
    connect(templateView,SIGNAL(itemDeleted()),propertyView,SLOT(resetAllData()));
    connect(cancelButton,SIGNAL(clicked()),qApp,SLOT(quit()));

//    connect(testButton,SIGNAL(clicked(bool)),this,SLOT(testSlot()));

}

MainWindow::~MainWindow()
{

}

void MainWindow::setWindow()
{
    createActions();
    createMenus();
    createStatusBar();
    createCentralWidget();
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New project"),this);
    connect(newAct,SIGNAL(triggered(bool)),this,SLOT(newProject()));

    saveTemplateAct = new QAction(tr("Save Template"));
    connect(saveTemplateAct,SIGNAL(triggered(bool)),SLOT(saveTemplate()));

    deleteTemplateAct = new QAction(tr("delete Template"));

    exportTemplateAct = new QAction(tr("export Template"));

    importTemplateAct = new QAction(tr("import Template"));

    createAct = new QAction(tr("createFolder"),this);
    connect(createAct,SIGNAL(triggered(bool)),this,SLOT(createFolder()));

    deleteAct = new QAction(tr("deleteFolder"),this);
    connect(deleteAct,SIGNAL(triggered(bool)),this,SLOT(deleteFolder()));
}

void MainWindow::createMenus()
{

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveTemplateAct);
    fileMenu->addAction(deleteTemplateAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exportTemplateAct);
    fileMenu->addAction(importTemplateAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(createAct);
    editMenu->addAction(deleteAct);

    menuBar()->setNativeMenuBar(0);
}


void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("ready"));
}

void MainWindow::createCentralWidget()
{
    QWidget *centerWidget = new QWidget();
    QVBoxLayout *baseLayout = new QVBoxLayout();
    QHBoxLayout *topLineLayout = new QHBoxLayout();

    QLabel *rootPathLabel = new QLabel(tr("rootPath"));
    rootPathEdit = new QLineEdit();
    rootPathButton = new QPushButton(tr("..."));
    templateList = new QComboBox();
    templateList->addItem(tr("<select template>"));

    QHBoxLayout *viewLayout = new QHBoxLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();


    templateView = new TemplateView();
    preView = new PreView(templateView);
    templateView->connectPreView(preView);
    QSplitter *viewerSplitter = new QSplitter();
    propertyView = new PropertyView();

    templateControl = new TemplateControl(templateView);
    templateList->addItems(templateControl->readTemplateList());
    templateControl->setFolderIcon(templateView->icon());


    makeTreeButton = new QPushButton(tr("Make"));
    cancelButton = new QPushButton(tr("cancel"));

    topLineLayout->addWidget(rootPathLabel);
    topLineLayout->addWidget(rootPathEdit);
    topLineLayout->addWidget(rootPathButton);
    topLineLayout->addWidget(templateList);

    viewerSplitter->addWidget(templateView);
    viewerSplitter->addWidget(preView);

    viewLayout->addWidget(viewerSplitter);
    viewLayout->addWidget(propertyView);


    buttonLayout->addWidget(makeTreeButton);
    buttonLayout->addWidget(cancelButton);

    baseLayout->addLayout(topLineLayout);
    baseLayout->addLayout(viewLayout);
    baseLayout->addLayout(buttonLayout);

    centerWidget->setLayout(baseLayout);
    setCentralWidget(centerWidget);


    rootPathEdit->setReadOnly(true);

}

void MainWindow::selectDiractory()
{
    QString folderName = QFileDialog::getExistingDirectory(this);
    if(folderName!=""){
        rootPathEdit->setText(folderName);
        QDir dirName(folderName);
        templateView->setRootFolderName(dirName.dirName());
//        rootPathEdit->setReadOnly(true);
    }

//    QPalette *palette = new QPalette();
//    palette->setColor(QPalette::Base,Qt::gray);
//    palette->setColor(QPalette::Text,Qt::darkGray);
//    rootPathEdit->setPalette(*palette);

}

void MainWindow::newProject()
{
    templateView->selectAll();
    templateView->deleteFolder();
    rootPathEdit->setText("");
    templateView->setRootFolderName("/");
}

void MainWindow::saveTemplate()
{
    bool ok;
    int templateNameIndex;
    QString templateName = QInputDialog::getText(this,"Save Template","TemplateName",QLineEdit::Normal,"template",&ok);
     if (ok && !templateName.isEmpty())
     {
         if(templateList->findText(templateName)==-1)
         {
             disconnect(templateList,SIGNAL(currentIndexChanged(QString)),templateControl,SLOT(loadTemplate(QString)));
             if(templateControl->saveTemplate(templateName))
             {
                 templateList->addItem(templateName);
                 templateNameIndex = templateList->findData(templateName,Qt::DisplayRole);
                 templateList->setCurrentIndex(templateNameIndex);
 //                templateControl = new TemplateControl(this);
             }else{
                 QMessageBox::information(this,"","Save fales",QMessageBox::Yes);
             }
             connect(templateList,SIGNAL(currentIndexChanged(QString)),templateControl,SLOT(loadTemplate(QString)));
         }else{
             QMessageBox::information(this,"","already have the same template name",QMessageBox::Yes);
         }
     }
}

void MainWindow::createFolder()
{
    templateView->insertFolder();
}

void MainWindow::deleteFolder()
{
    templateView->deleteFolder();
}

void MainWindow::testSlot()
{
    QModelIndex index = templateView->currentIndex();
    QString data = templateView->data(index,Qt::DisplayRole).toString();
    QRegExp rx("\\[[a-zA-Z0-9]+\\]");
    QStringList list =data.split(rx);
    int pos = rx.indexIn(data);
    QStringList pattern = rx.capturedTexts();

    if(isSequencName(data).count()!=0)
    {
        QMessageBox::information(this,"",data,QMessageBox::Yes);
    }

//    QString str = "offsets: 1.23 .50 71.00 6.00";
//    QRegExp rx("\\d*\\.\\d+");    // primitive floating point matching
//    int count = 0;
//    int pos = 0;
//    while ((pos = rx.indexIn(str, pos)) != -1) {
//        ++count;
//        QStringList aa = rx.capturedTexts();
//        pos += rx.matchedLength();
//    }
}
