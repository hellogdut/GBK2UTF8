#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include <QDebug>
#include <QCheckBox>
#include <QTextStream>
#include <QMessageBox>
#include <algorithm>
#include <QDesktopServices>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("GBK To UTF8");
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::reset()
{
    folder = "";
    set.clear();
    selected_set.clear();
    for(auto i : checkboxs)
    {
        ui->type_layout->removeWidget(i);
        i -> close();
    }
    ui->type_layout->update();
    checkboxs.clear();


}

void MainWindow::on_pushButton_clicked()
{
   reset();
   folder = QFileDialog::getExistingDirectory(this);
   //qDebug() << folder << endl;
   enumFileType(folder);

   int n = set.size();
   QVector<QString> vec;
   for(auto s: set)
       vec.push_back(s);
   std::sort(vec.begin(),vec.end(),[](const QString& a,const QString& b){return a.size() < b.size();});

   int i = 0;
   for(auto &str : vec)
   {
       QCheckBox* b = new QCheckBox(str);
       if(str == "c" || str == "cpp" || str == "h" || str == "hpp")
           b->setChecked(true);
       checkboxs.push_back(b);
       ui->type_layout->addWidget(b,i/8,i%8);
       i++;
   }
}
void MainWindow::enumFileType(const QString& path)
{
    QDir dir(path);
    if (!dir.exists())
        return;
    dir.setFilter(QDir::Dirs|QDir::Files);// 除了目录或文件，其他的过滤
    dir.setSorting(QDir::DirsFirst);// 优先显示目录
    QFileInfoList list = dir.entryInfoList();// 获取文件信息列表

    int i = 0;
    bool bIsDir;
    do{
       QFileInfo fileInfo = list.at(i);
       if(fileInfo.fileName()=="."||fileInfo.fileName()=="..")
       {
            i++;
           continue;
       }
       bIsDir = fileInfo.isDir();
       if (!bIsDir)
       {
           if(!set.contains(fileInfo.suffix()))
               set.insert(fileInfo.suffix());
       }
       else
       {
           enumFileType(fileInfo.filePath());
           //qDebug() << fileInfo.filePath();
       }
       i++;
    }while(i<list.size());

}


void MainWindow::on_pushButton_2_clicked()
{

    for(auto i :checkboxs)
    {
        if(i -> isChecked())
            selected_set.insert(i -> text());
    }
    for(auto i : selected_set)
        qDebug() << i;


    // 构造新文件夹
    QFileInfo info(folder);

    QString tar_folder = info.absolutePath() + "/___" + info.fileName();

    QDir dir(tar_folder);
    if(!dir.exists())
        if(!dir.mkdir(tar_folder))
            qDebug() << "dir.mkdir " << tar_folder << " failed" << endl;

    transform(folder,tar_folder);
    QMessageBox::about(this,"提示","转换成功");

}
void MainWindow::transform(const QString& curr_folder,const QString& tar_folder)
{
    QDir dir(curr_folder);
    if (!dir.exists())
        return;
    dir.setFilter(QDir::Dirs|QDir::Files);// 除了目录或文件，其他的过滤
    dir.setSorting(QDir::DirsFirst);// 优先显示目录
    QFileInfoList list = dir.entryInfoList();// 获取文件信息列表

    int i = 0;
    bool bIsDir;
    do{
       QFileInfo fileInfo = list.at(i);
       if(fileInfo.fileName()=="."||fileInfo.fileName()=="..")
       {
            i++;
           continue;
       }
       bIsDir = fileInfo.isDir();
       if (!bIsDir)
       {
           // copy 无关文件
           if(!selected_set.contains(fileInfo.suffix()))
           {
               if(ui->copyBox->isChecked())
                    QFile::copy(fileInfo.filePath(),tar_folder + "/" + fileInfo.fileName());
           }
           // 改为指定编码
           else
           {
               QFile inFile(fileInfo.filePath());
               QFile outFile(tar_folder + "/" + fileInfo.fileName());
               if (!inFile.open(QIODevice::ReadOnly)) {
                   qDebug() << "Cannot open file for writing: ";
                   qDebug() << fileInfo.filePath();
                   return;
               }
               if (!outFile.open(QIODevice::WriteOnly)) {
                   qDebug() << "Cannot open file for writing: ";
                   qDebug() << tar_folder + "/" + fileInfo.fileName();
                   return;
               }


               QTextStream in(&inFile);
               QTextStream out(&outFile);

               in.setCodec("GBK");
               out.setCodec("UTF-8");

               QString s = in.readAll();
               qDebug() << s;
               out << s;
               inFile.close();
               outFile.close();

           }
       }
       else
       {
           QString tar = tar_folder + "/" + fileInfo.fileName();
           QDir dir(tar);
           if(!dir.exists())
                if(!dir.mkdir(tar))
                    qDebug() << "mkdir " << tar << " failed";
           transform(fileInfo.filePath(),tar);
       }
       i++;
    }while(i<list.size());
    ui->pushButton_3->setEnabled(true);
}

void MainWindow::on_pushButton_3_clicked()
{
    QString path = folder;
    path = QString(folder.begin(),folder.lastIndexOf("/"));
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
