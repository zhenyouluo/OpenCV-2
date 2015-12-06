#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay(Mat frame);

/** Global variables */
String body_cascade_name = "haarcascade_fullbody.xml";
CascadeClassifier body_cascade;
String window_name = "Capture - full body detection";

/** @function main */
int main(void)
{
	VideoCapture capture;
	Mat frame;

	//-- 1. Load the cascades
	if (!body_cascade.load(body_cascade_name)){ printf("--(!)Error loading body cascade\n"); getchar();  return -1; };

	//-- 2. Read the video stream
	capture.open("D:/cctv.mp4");
	if (!capture.isOpened()) { printf("--(!)Error opening video capture\n"); return -1; }

	while (capture.read(frame))
	{
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

		//-- 3. Apply the classifier to the frame
		detectAndDisplay(frame);

		int c = waitKey(10);
		if ((char)c == 27) { break; } // escape
	}
	return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay(Mat frame)
{
	std::vector<Rect> bodies;
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect bodies
	body_cascade.detectMultiScale(frame_gray, bodies, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < bodies.size(); i++)
	{
		Point center(bodies[i].x + bodies[i].width / 2, bodies[i].y + bodies[i].height / 2);
		ellipse(frame, center, Size(bodies[i].width / 2, bodies[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
	}
	//-- Show what you got
	imshow(window_name, frame);
}