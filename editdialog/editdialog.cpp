#include "editdialog.h"
#include "../filter/filter.h"
#include <QDebug>
#include <QInputDialog>

// EditDialog 생성자 내부
EditDialog::EditDialog(const cv::Mat &image, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::EditDialog),
        originalImage(image.clone()),
        editedImage(image.clone()) {
    ui->setupUi(this);

    // 다이얼로그 최소 크기 설정
    this->setMinimumSize(800, 600);

    // imageLabel의 크기 조정 정책 설정
    ui->imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 적용된 이미지 표시
    cv::Mat rgb;
    cv::cvtColor(editedImage, rgb, cv::COLOR_BGR2RGB);
    QImage qimg(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);

    ui->imageLabel->setPixmap(QPixmap::fromImage(qimg).scaled(
            ui->imageLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
    ));
}

EditDialog::~EditDialog() {}

cv::Mat EditDialog::getEditedImage() const {
    return editedImage;
}

void EditDialog::on_autoBrightnessButton_clicked() {
    editedImage = auto_brightness_adjust(editedImage);
    displayImage();
}

void EditDialog::on_filterButton_clicked() {
    QStringList filters;
    filters << "GrayScale" << "Reverse" << "Warm" << "Cool" << "Cartoon";
    bool ok;
    QString filter = QInputDialog::getItem(this, tr("Select Filter"), tr("Select Filter:"), filters, 0, false, &ok);

    if (ok && !filter.isEmpty()) {
        if (filter == "GrayScale") {
            editedImage = filter_gray(editedImage);
        } else if (filter == "Reverse") {
            editedImage = filter_reverse(editedImage);
        } else if (filter == "Warm") {
            editedImage = filter_warm(editedImage);
        } else if (filter == "Cool") {
            editedImage = filter_cool(editedImage);
        } else {
            editedImage = filter_cartoon(editedImage);
        }

        displayImage();
    }
}

void EditDialog::displayImage() {
    if (!editedImage.empty()) {
        cv::Mat rgb;
        cv::cvtColor(editedImage, rgb, cv::COLOR_BGR2RGB);
        QImage qimg(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);

        ui->imageLabel->setPixmap(QPixmap::fromImage(qimg).scaled(
                ui->imageLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
        ));
    }
}

void EditDialog::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);
    displayImage(); // 이미지 표시 함수 호출
}

void EditDialog::on_completeButton_clicked() {
    accept(); // 다이얼로그 닫기
}
