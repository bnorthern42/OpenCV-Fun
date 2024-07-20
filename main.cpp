#include "mainwindow.h"

#include <QApplication>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<iostream>
using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{
    Mat frame;//Declaring a matrix to load the frames//
    VideoCapture cap;
    int deviceID = 0; // 0 = open default camera
    int apiID = cv::CAP_V4L2; // 0 = autodetect default API


    // open selected camera using selected API
    cap.open(deviceID, apiID);
    if (!cap.isOpened()){ //This section prompt an error message if no video stream is found//
        cerr << "No video stream detected" << endl;
        system("pause");
        return-1;
    }
    cout << "information\n"
         << "Backend: " + cap.getBackendName() + "\n";
   // Mat img = imread("/home/bradn4/Projects/opencv/cvqt/cvqt/blackSpecksTest.jpeg");
  //  namedWindow("Image", WINDOW_NORMAL);
   // imshow("Image", img);
    // Wait for a key press to close the window
  //  waitKey(0);
    cout << "Start grabbing" << endl
         << "Press any key to terminate" << endl;

    namedWindow("Control"); //create a window called "Control"
    int iLowH = 0;
    int iHighH = 179;

    int iLowS = 0;
    int iHighS = 255;

    int iLowV = 0;
    int iHighV = 255;

    //Create trackbars in "Control" window
    createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
    createTrackbar("HighH", "Control", &iHighH, 179);

    createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
    createTrackbar("HighS", "Control", &iHighS, 255);

    createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
    createTrackbar("HighV", "Control", &iHighV, 255);
    int iLastX = -1;
    int iLastY = -1;
    Mat imgLines = Mat::zeros( frame.size(), CV_8UC3 );;
    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        // show live and wait for a key with timeout long enough to show images
        imshow("Control", frame);
        if (waitKey(5) >= 0)
            break;

        Mat imgHSV;
        cvtColor(frame, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        Mat imgThresholded;

        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

        //morphological opening (remove small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        //morphological closing (fill small holes in the foreground)
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        Moments oMoments = moments(imgThresholded);

        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;
        if (dArea > 10000)
        {
            //calculate the position of the object
            int posX = dM10 / dArea;
            int posY = dM01 / dArea;

            if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
            {
                //Draw a red line from the previous point to the current point
                line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 2);
            }

            iLastX = posX;
            iLastY = posY;
        }


        imshow("Thresholded Image", imgThresholded); //show the thresholded image


    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    cap.release();//Releasing the buffer memory//
    // Clean up
    destroyAllWindows();
    //QApplication a(argc, argv);
   // MainWindow w;
  //  w.show();
  //  return a.exec();
}
