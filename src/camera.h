#ifndef CAMERA_H
#define CAMERA_H

#include "opencv2/opencv.hpp"
using namespace cv;

#include <QLabel>
#include <QPixmap>
#include <QTimer>

class Camera : public QLabel
{
    Q_OBJECT
public:
    Camera(QWidget *parent = 0);

public slots:
    void displayCamera();
    void findFists();

signals:
    void sRotate();
    void sTranslateL();
    void sTranslateR();

private:
    VideoCapture * webCam_;
    QTimer *timer_cam_;
    QTimer *timer_fists_;
    CascadeClassifier *fist_cascade = new CascadeClassifier;


};


#endif // CAMERA_H
