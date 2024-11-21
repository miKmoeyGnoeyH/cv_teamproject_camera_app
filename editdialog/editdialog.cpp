#include "editdialog.h"
#include <QDebug>
#include <QInputDialog>

// EditDialog 생성자 내부
EditDialog::EditDialog(const cv::Mat &image, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::EditDialog),
        originalImage(image.clone()),
        editedImage(image.clone())
{
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

cv::Mat EditDialog::getEditedImage() const
{
    return editedImage;
}

void EditDialog::on_autoBrightnessButton_clicked()
{
    // 자동 밝기 조절 기능은 추후 구현 예정
    qDebug() << "Auto Brightness Adjustment clicked";
}

void EditDialog::on_filterButton_clicked()
{
    // 필터 목록 표시
    QStringList filters;
    filters << "Cartoon" << "Worm" << "Cool";
    bool ok;
    QString filter = QInputDialog::getItem(this, tr("Select Filter"), tr("Select Filter:"), filters, 0, false, &ok);

    if (ok && !filter.isEmpty()) {
        if (filter == "Cartoon") {
            // 카툰 필터 적용 (예시)
            cv::Mat gray, edges;
            cv::cvtColor(editedImage, gray, cv::COLOR_BGR2GRAY);
            cv::medianBlur(gray, gray, 7);
            cv::Laplacian(gray, edges, CV_8U, 5);
            cv::threshold(edges, edges, 80, 255, cv::THRESH_BINARY_INV);
            cv::cvtColor(edges, editedImage, cv::COLOR_GRAY2BGR);
        } else if (filter == "Worm") {
            // 웜톤 필터 적용 (예시)
            cv::Mat warmImage = editedImage + cv::Scalar(0, 20, 20);
            editedImage = warmImage;
        } else if (filter == "Cool") {
            // 쿨톤 필터 적용 (예시)
            cv::Mat coolImage = editedImage + cv::Scalar(20, 20, 0);
            editedImage = coolImage;
        }

        // 적용된 이미지 표시
        displayImage();
//        cv::Mat rgb;
//        cv::cvtColor(editedImage, rgb, cv::COLOR_BGR2RGB);
//        QImage qimg(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
//        ui->imageLabel->setPixmap(QPixmap::fromImage(qimg).scaled(
//                ui->imageLabel->size(),
//                Qt::KeepAspectRatio,
//                Qt::SmoothTransformation
//        ));
    }
}

void EditDialog::displayImage()
{
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

void EditDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    displayImage(); // 이미지 표시 함수 호출
}

void EditDialog::on_completeButton_clicked()
{
    accept(); // 다이얼로그 닫기
}
