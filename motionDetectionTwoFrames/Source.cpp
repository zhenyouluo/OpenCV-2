#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const static int SENSITIVITY_VALUE = 20;
const static int BLUR_SIZE = 10;

bool objectDetected = false;
bool debugMode = false;
bool trackingMode = false;
bool pause = false;

Mat frame1, frame2, grayImg1, grayImg2, differenceImg, thresholdImg, blurredImg, thresholdImg2;
VideoCapture capture;

void trackObjects(Mat thresh, Mat &output) {

	Mat temp;
	thresh.copyTo(temp);
	int xpos, ypos;

	// two vectors for the contours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// find all contours
	findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	if (contours.size() > 0) objectDetected = true;
	else objectDetected = false;

	if (objectDetected){
		// we assume the object we're tracking is the largest contour
		// which is found at the end of the detected contours
		int idx = 0;
		for (; idx >= 0; idx = hierarchy[idx][0])
		{
			Scalar color( 255, 0, 240);
			drawContours(output, contours, idx, color, LINE_4, 8, hierarchy);
		}
		vector< vector<Point> > largetContour;
		largetContour.push_back(contours.at(contours.size() - 1));

		Rect objectBoundingRectangle = boundingRect(largetContour.at(0));

		xpos = objectBoundingRectangle.x + objectBoundingRectangle.width / 2;
		ypos = objectBoundingRectangle.y + objectBoundingRectangle.height / 2;

	}
	// circle(output, Point(xpos, ypos), 20, Scalar(0, 255, 0), 2);


}
int main(){

	// looping to read video over and over
	while (true){

		// opening video stream
		capture.open("D:/bouncingBall.avi");

		// checking if there is an error with the capture
		if (!capture.isOpened()) {
			cout << "Error getting video feed" << endl;
			getchar();
			return -1;
		}

		// we check that we didn't reach the last frame because we read 2 frames at a time
		while (capture.get(CV_CAP_PROP_POS_FRAMES) < capture.get(CV_CAP_PROP_FRAME_COUNT) - 1){

			// reading first frame
			capture.read(frame1);
			// converting to gray
			cvtColor(frame1, grayImg1, COLOR_BGR2GRAY);

			// reading second frame
			capture.read(frame2);
			// converting to gray
			cvtColor(frame2, grayImg2, COLOR_BGR2GRAY);

			// difference between the 2 gray frames, outputs intensity image
			absdiff(grayImg1, grayImg2, differenceImg);

			// applying threshold on intensity image with a certain sensitivity value
			threshold(differenceImg, thresholdImg, SENSITIVITY_VALUE, 255, THRESH_BINARY);

			// to get rid of noise, we use the blur, outputs intensity image
			blur(thresholdImg, blurredImg, Size(BLUR_SIZE, BLUR_SIZE));
			// threshold again to get a binary image from the intensity img obtained from blur
			threshold(blurredImg, thresholdImg2, SENSITIVITY_VALUE, 255, THRESH_BINARY);

			if (debugMode){
				imshow("Difference Image", differenceImg);
				imshow("Threshold Image", thresholdImg);
				imshow("Final threshold Image", thresholdImg2);
			}
			else {
				destroyWindow("Difference Image");
				destroyWindow("Threshold Image");
				destroyWindow("Final threshold Image");
			}

			if (trackingMode){
				trackObjects(thresholdImg2, frame2);
			}

			imshow("Tracking", frame2);

			switch (waitKey(10)){

			case 27: //'esc' key has been pressed, exit program.
				return 0;
			case 116: //'t' has been pressed. this will toggle tracking mode
				trackingMode = !trackingMode;
				if (trackingMode == false) cout << "Tracking disabled." << endl;
				else cout << "Tracking enabled." << endl;
				break;
			case 100: //'d' has been pressed. this will toggle debug mode
				debugMode = !debugMode;
				if (debugMode == false) cout << "Debug mode disabled." << endl;
				else cout << "Debug mode enabled." << endl;
				break;
			case 112: //'p' has been pressed. this will pause/resume the code.
				pause = !pause;
				if (pause == true){
					cout << "Code paused, press 'p' again to resume" << endl;
					while (pause == true){
						//stay in this loop until 
						switch (waitKey()){
						case 112:
							//change pause back to false
							pause = false;
							cout << "Code Resumed" << endl;
							break;
						}
					}
				}

			}
		}

		// release capture to open it again
		capture.release();

	}
	return 0;
}

