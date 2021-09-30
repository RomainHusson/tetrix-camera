#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <GL/glu.h>

#include "camera.h"

#include <QWidget>
#include <cstdio>
#include <iostream>



using namespace cv;
using namespace std;


Camera::Camera(QWidget *parent) :
    QLabel(parent)
{
    // Loading the xml file in order to detect fists
    if( !fist_cascade->load( "D:/Programmes/Qt/resources/fist_v3.xml" ) )
    {
        cerr<<"Error loading fist_v3.xml"<<endl;
    }

    // Size of the camera widget
    int frameWidth=640;
    int frameHeight=480;

    webCam_=new VideoCapture(0); // open the default camera

    webCam_->set(CV_CAP_PROP_FRAME_WIDTH,frameWidth);
    webCam_->set(CV_CAP_PROP_FRAME_HEIGHT,frameHeight);
    if(!webCam_->isOpened())  // check if we succeeded
    {
        cerr<<"Error openning the default camera"<<endl;
    }

    timer_cam_ = new QTimer(this);
    timer_cam_->start(16);
    timer_fists_ = new QTimer(this);
    timer_fists_->start(100);


    connect(timer_cam_,SIGNAL(timeout()),this,SLOT(displayCamera()));
    connect(timer_fists_,SIGNAL(timeout()),this,SLOT(findFists()));


//    displayCamera();
}

void Camera::displayCamera()
{
    Mat image;
    if (webCam_->read(image)) {   // Capture a frame
           // Flip to get a mirror effect
           flip(image,image,1);
           // Invert Blue and Red color channels
           cvtColor(image,image,CV_BGR2RGB);
           // Convert to Qt image
           QImage img= QImage((const unsigned char*)(image.data),image.cols,image.rows,QImage::Format_RGB888);
           // Display on label
           this->setPixmap(QPixmap::fromImage(img));
           // Resize the label to fit the image
           this->resize(this->pixmap()->size());
   }
}

void Camera::findFists()
{
    waitKey(5);

    Mat frame,frame_gray;
    std::vector<Rect> fists;
    // Get frame
    *webCam_ >> frame;
    // Mirror effect
    cv::flip(frame,frame,1);
    // Convert to gray
    cv::cvtColor(frame,frame_gray,COLOR_BGR2GRAY);
    // Equalize graylevels
//        equalizeHist( frame_gray, frame_gray );
    //-- Detect fists
    fist_cascade->detectMultiScale( frame_gray, fists, 1.1, 4, 0|CV_HAAR_SCALE_IMAGE, Size(60, 60) );
    if (fists.size()>0)
    {
        // Draw green rectangle
        for (int i=0;i<(int)fists.size();i++)
        {
            rectangle(frame,fists[i],Scalar(0,255,0),2);

            // Rotation
            int seuilR = 20;
            if ((int)fists.size() == 2)
            {
                if (abs(fists[i].x + fists[i].width - fists[i+1].x) < seuilR)
                {
                    cout << "Rotation" << endl;
                    emit sRotate();
                }
            }

            // Translation
            int seuilT = 200;
            if ((int)fists.size() == 2)
            {
                if  ((fists[i].x != fists[i+1].x) && (fists[i].x == min(fists[i].x,fists[i+1].x)))
                {
                    if (fists[i].y - fists[i+1].y > seuilT)
                    {
                        cout << "Left" << endl;
                        emit sTranslateL();
                    }
                    else if (fists[i].y - fists[i+1].y < -seuilT)
                    {
                        cout << "Right" << endl;
                        emit sTranslateR();
                    }
                }
            }
        }
    }

}
