#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), isCapturing(false) {
    ui->setupUi(this);

    timer = new QTimer(this);

    // 시그널과 슬롯 연결
    connect(ui->selectImageButton, &QPushButton::clicked, this, &MainWindow::selectImage);
    connect(ui->captureImageButton, &QPushButton::clicked, this, &MainWindow::startOrCapture);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    connect(ui->saveImageButton, &QPushButton::clicked, this, &MainWindow::saveImage);
    connect(ui->editImageButton, &QPushButton::clicked, this, &MainWindow::editImage);
}

MainWindow::~MainWindow() {
    if (cap.isOpened()) {
        cap.release();
    }
    delete ui;
}

void MainWindow::selectImage() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("이미지 선택"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty()) {
        currentImage = cv::imread(fileName.toStdString());
        if (!currentImage.empty()) {
            displayImage(currentImage);
        } else {
            QMessageBox::warning(this, tr("오류"), tr("이미지를 불러올 수 없습니다."));
        }
    }
}

void MainWindow::startOrCapture() {
    if (!isCapturing) {
        // 웹캠 열기
        cap.open(0);
        if (!cap.isOpened()) {
            QMessageBox::warning(this, tr("오류"), tr("웹캠을 열 수 없습니다."));
            return;
        }
        isCapturing = true;
        ui->captureImageButton->setText("Capture");
        timer->start(30);
    } else {
        // 사진 촬영
        isCapturing = false;
        timer->stop();
        ui->captureImageButton->setText("Take New Photo");
        if (!currentImage.empty()) {
            displayImage(currentImage);
        }
        cap.release();
    }
}

void MainWindow::updateFrame() {
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) {
        return;
    }
    flip(frame, frame, 1);
    currentImage = frame;
    displayImage(currentImage);
}

void MainWindow::displayImage(const cv::Mat &img) {
    cv::Mat rgb;
    cv::cvtColor(img, rgb, cv::COLOR_BGR2RGB);
    QImage qimg(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
    ui->imageLabel->setPixmap(QPixmap::fromImage(qimg).scaled(
            ui->imageLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
    ));
}

void MainWindow::saveImage() {
    if (currentImage.empty()) {
        QMessageBox::warning(this, tr("오류"), tr("저장할 이미지가 없습니다."));
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("이미지 저장"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty()) {
        cv::imwrite(fileName.toStdString(), currentImage);
    }
}

void MainWindow::editImage() {
    if (currentImage.empty()) {
        QMessageBox::warning(this, tr("오류"), tr("편집할 이미지가 없습니다."));
        return;
    }

    EditDialog editDialog(currentImage, this);
    if (editDialog.exec() == QDialog::Accepted) {
        currentImage = editDialog.getEditedImage();
        displayImage(currentImage);
    }
}
