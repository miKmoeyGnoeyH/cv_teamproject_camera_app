#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <opencv2/opencv.hpp>

#include "../editdialog/editdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void displayImage(const cv::Mat &img);

private slots:
    void selectImage();
    void startOrCapture();
    void updateFrame();
    void saveImage();
    void editImage();

private:
    Ui::MainWindow *ui;
    cv::Mat currentImage;
    cv::VideoCapture cap;
    QTimer *timer;
    bool isCapturing;
};

#endif // MAINWINDOW_H
