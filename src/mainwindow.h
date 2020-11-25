#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void updateInfoLabel();

    void on_vdpEdit_edited();

    void on_vdpRegCmdEdit_edited();

    void on_ramEdit_edited();
    void on_ramResult_edited();

    void on_srEdit_edited();
    void on_srResult_edited();

  private:
    Ui::MainWindow *ui;
    std::vector<QPushButton *> mBinary;
};

#endif // MAINWINDOW_H
