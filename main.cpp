#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap("/home/jeremymboe/cmpt130/belajarOpenCVCPP/2.video/VideoAsli.avi");

    if (!cap.isOpened()) {
        return -1;
    }

    Mat frame, hsvFrame, mask;

    double x_robot = 0.0, y_robot = 0.0;
    double prev_ballX_cm = 0.0, prev_ballY_cm = 0.0;  
    const float px_ke_cm = 10.0;  

    bool cek_firstFrame = true; 
    double min_area_ball = 40.0;  
    double max_area_ball = 5000.0;  

    Point2f centerField(cap.get(CAP_PROP_FRAME_WIDTH) / 2, cap.get(CAP_PROP_FRAME_HEIGHT) / 2);
    int exclusionRadius = 100;  

    while (true) {
        cap >> frame;  
        if (frame.empty()) {
            break;  
        }

        cvtColor(frame, hsvFrame, COLOR_BGR2HSV);

        Scalar lower_orange(5, 150, 150); 
        Scalar upper_orange(15, 255, 255);

        inRange(hsvFrame, lower_orange, upper_orange, mask);

        circle(mask, centerField, exclusionRadius, Scalar(0), -1);

        vector<vector<Point>> contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        if (!contours.empty()) {
            double maxArea = 0;
            int largestContourIndex = -1;

            for (int i = 0; i < contours.size(); i++) {
                double area = contourArea(contours[i]);
                if (area > maxArea && area >= min_area_ball && area <= max_area_ball) {
                    maxArea = area;
                    largestContourIndex = i;
                }
            }

            if (largestContourIndex != -1) {
               
                RotatedRect rect = minAreaRect(contours[largestContourIndex]);
               
                Point2f rect_points[4];
                rect.points(rect_points);
                Point2f center = rect.center;

                double object_x_cm = center.x * px_ke_cm;
                double object_y_cm = center.y * px_ke_cm;

                if (!cek_firstFrame) {
                    x_robot += object_x_cm - prev_ballX_cm;
                    y_robot += object_y_cm - prev_ballY_cm;
                }

                prev_ballX_cm = object_x_cm;
                prev_ballY_cm = object_y_cm;

                cek_firstFrame = false;

                for (int j = 0; j < 4; j++) {
                    line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0), 2);
                }

                circle(frame, Point(center.x - 30, center.y - 30), 5, Scalar(255, 0, 0), -1);
                circle(frame, Point(center.x + 30, center.y - 30), 5, Scalar(255, 0, 0), -1);
                circle(frame, Point(center.x - 30, center.y + 30), 5, Scalar(255, 0, 0), -1);
                circle(frame, Point(center.x + 30, center.y + 30), 5, Scalar(255, 0, 0), -1);

                std::string positionText = "Posisi robot (" + std::to_string(x_robot) + ", " + std::to_string(y_robot) + ") ";
                putText(frame, positionText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
            }
        }

        imshow("Video Robot", frame);

        if (waitKey(33) == 'q') {
            break;
        }
    }

    return 0;
}
