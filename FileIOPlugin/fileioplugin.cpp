#include <QDebug>
#include "fileioplugin.h"

FileIOPlugin::FileIOPlugin()
{
}

FileIOPlugin::~FileIOPlugin()
{

}

QString FileIOPlugin::description()
{
    return "FileIO plugin: this plugin was used to deal with files and graphics";
}

void FileIOPlugin::Initilization(QMainWindow *window)
{
    QToolBar* toolBar = createToolBar();
    window->addToolBar(toolBar);
    connect(this, &FileIOPlugin::fileOpen, [this,window](){openFile(window);});
    connect(this, &FileIOPlugin::fileSave, [this,window](){saveFile(window);});
}

QGraphicsScene *FileIOPlugin::getScene(QMainWindow *window)
{
    QGraphicsView* view = dynamic_cast<QGraphicsView *>(window->centralWidget());
    return view->scene();
}

QGraphicsView *FileIOPlugin::getView(QMainWindow *window)
{
    QGraphicsView *view = dynamic_cast<QGraphicsView *>(window->centralWidget());
    return view;
}

void FileIOPlugin::openFile(QMainWindow *window)
{
    QFileDialog openFileDialog(window);
    openFileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    openFileDialog.setMimeTypeFilters(
                QStringList() << "image/svg+xml"
                << "image/svg+xml-compressed"
                <<"image/png"
                <<"image/jpeg");
    while (openFileDialog.exec() == QDialog::Accepted &&
           !loadFile(QString(openFileDialog.selectedFiles().constFirst()), window))
        ;
}

bool FileIOPlugin::loadFile(QString &fileName, QMainWindow *window)
{
    if (!QFileInfo::exists(fileName)){
            return false;
    }
    QFileInfo fileInfo = QFileInfo(fileName);
    QGraphicsScene* scene = getScene(window);
    if( fileInfo.suffix().toLower() == "svg")
        openSvgFile(fileName, scene);
    return true;
}

void FileIOPlugin::openSvgFile(QString &fileName, QGraphicsScene *scene)
{
    QGraphicsSvgItem* svgItem = new QGraphicsSvgItem(fileName);
    if(!svgItem->renderer()->isValid())
        return;
    scene->clear();

    svgItem->setFlag(QGraphicsItem::ItemIsSelectable);
    svgItem->setFlag(QGraphicsItem::ItemIsMovable);
    scene->addItem(svgItem);
    scene->setSceneRect(svgItem->boundingRect());
    qDebug()<<"svgItem->boundingRect()";
}

void FileIOPlugin::saveFile(QMainWindow *window)
{
    QString newPath = QFileDialog::getSaveFileName(window, tr("Save File"),QString(),"SVG (*.svg)");
    QGraphicsView *view = getView(window);
    saveSvgFile(newPath,view);
}

void FileIOPlugin::saveSvgFile(QString &fileName, QGraphicsView *view)
{
    QSvgGenerator generator;
    generator.setFileName(fileName);
    generator.setSize(QSize(200, 200));
    generator.setViewBox(QRect(0, 0, 200, 200));
    generator.setTitle(tr("SVG File"));
    generator.setDescription(tr("An SVG drawing created by the QGraphicsMagic"));

    QPainter painter;
    painter.begin(&generator);
    view->render(&painter);
    painter.end();
}

QToolBar *FileIOPlugin::createToolBar()
{
    QToolBar* toolBar =  new QToolBar;

    QIcon openFileIcon;
    openFileIcon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_DirOpenIcon),
                           QIcon::Normal, QIcon::On);
    QIcon saveFileIcon;
    saveFileIcon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_DialogSaveButton),
                           QIcon::Normal, QIcon::On);
    QAction* openFileAction = toolBar->addAction(openFileIcon, tr("&Open"));
    QAction* saveFileAction = toolBar->addAction(saveFileIcon, tr("&Save"));

    connect(openFileAction, &QAction::triggered, [this](){emit fileOpen();});
    connect(saveFileAction, &QAction::triggered, [this](){emit fileSave();});
    return toolBar;
}


