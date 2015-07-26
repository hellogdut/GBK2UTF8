#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>
#include <QCheckBox>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    void reset();
    void enumFileType(const QString& path);
    void transform(const QString& curr_folder,const QString& tar_folder);
private:
    Ui::MainWindow *ui;

    QString folder;
    QSet<QString> set;
    QVector<QCheckBox*> checkboxs;
    QSet<QString> selected_set;
};

#endif // MAINWINDOW_H
