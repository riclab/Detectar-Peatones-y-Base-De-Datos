#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTime>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/cudaobjdetect.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>

#include "mythread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void SeleccionarVideo();
    void ProcesarVideo(bool checked);
    void nms(const std::vector<cv::Rect> &srcRects, std::vector<cv::Rect> &resRects, float thresh);
    unsigned int pea=0;

private slots:
    void on_toolButton_clicked();
    void on_actionAbrir_Video_triggered();
    void on_play_toggled(bool checked);


private:
    Ui::MainWindow *ui;
    cv::VideoCapture cap;
    MyThread thread1;
};

#endif // MAINWINDOW_H
