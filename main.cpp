//
//  main.cpp
//  Assignment 1B
//
//  Created by Ujjwal Sankhwar on 05/03/21.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>

using namespace std;
using namespace cv;


int main(int argc, const char * argv[]) {
    VideoCapture video("trafficvideo.mp4");
    Mat image =imread("clear.jpg");
    Mat imagegray,matrix,imageCrop;
    UMat  flow, prevgray;
    cvtColor(image, imagegray, COLOR_BGR2GRAY);
    if(video.isOpened()==false){
        cout << "Cannot open the video file" <<endl;
        return -1;
    }
    
    vector<Point2f> des;
    des.push_back(Point2f(472,52));
    des.push_back(Point2f(472,830));
    des.push_back(Point2f(800,830));
    des.push_back(Point2f(800,52));
    vector<Point2f> src;
    src.push_back(Point2f(973, 221));
    src.push_back(Point2f(238, 1070));
    src.push_back(Point2f(1546, 1073));
    src.push_back(Point2f(1273, 220));
    matrix = getPerspectiveTransform(src, des);
    warpPerspective(imagegray, imageCrop, matrix,image.size());
    Rect rec(472,52,328,778);
    float total=255184;
    float framecount=1;
    float countdensity=0;
    imageCrop = imageCrop(rec);
    GaussianBlur(imageCrop, imageCrop, Size(5,5), 0,0);
    ofstream MyFile("Density.csv");
    MyFile <<"Time(in seconds),Queue Density,Dynamic Density"<<endl;
    int c=0;
    while(true){
        Mat frame,framegray,framecrop,diff,diffdensity,flowtemp;
        bool success = video.read(frame);
        if(success==false){
            cout << "Finished" <<endl;
            break;
        }
        if(c%3==0){
            c=0;
            cvtColor(frame, framegray, COLOR_BGR2GRAY);
            warpPerspective(framegray, framecrop, matrix,image.size());
            framecrop = framecrop(rec);
            framecrop.copyTo(diffdensity);
            GaussianBlur(framecrop ,diff , Size(5,5), 0,0);
            absdiff(diff, imageCrop, diff);
            threshold(diff, diff, 25, 255, THRESH_BINARY);
            float countqueue=countNonZero(diff);
            if(prevgray.empty()==false){
                calcOpticalFlowFarneback(prevgray, framecrop , flow, 0.4, 1, 12, 2, 8, 1.2, 0);
                flow.copyTo(flowtemp);
                for (int y = 0; y < diffdensity.rows; y += 1) {
                    for (int x = 0; x < diffdensity.cols; x += 1) {
                        const Point2f flowatxy = flowtemp.at<Point2f>(y, x) * 2;
                        line(diffdensity, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(255, 0, 0));
                        circle(diffdensity, Point(x, y), 1, Scalar(0, 0, 0), -1);
                    }
                }
                threshold(diffdensity, diffdensity, 20, 255, THRESH_BINARY);
                countdensity = countNonZero(diffdensity);
                framecrop.copyTo(prevgray);
                imshow("prew", frame);
            }
            else{
                framecrop.copyTo(prevgray);
            }
            MyFile << framecount/5<<","<<countqueue/total<<","<<countdensity/total<<endl;
            framecount++;
        }
        if(waitKey(1)==27){
            cout << "Stopped" <<endl;
            break;
        }
        c++;
    }
    MyFile.close();
    return 0;
}
