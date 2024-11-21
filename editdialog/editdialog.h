#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <opencv2/opencv.hpp>
#include "ui_editdialog.h" // ui 헤더 파일 포함

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(const cv::Mat &image, QWidget *parent = nullptr);
    ~EditDialog();
    cv::Mat getEditedImage() const;

protected:
    void displayImage();
    void showEvent(QShowEvent *event) override;

private slots:
    void on_autoBrightnessButton_clicked();
    void on_filterButton_clicked();
    void on_completeButton_clicked();

private:
    Ui::EditDialog *ui; // 포인터가 아닌 객체로 선언
    cv::Mat originalImage;
    cv::Mat editedImage;
};

#endif // EDITDIALOG_H
