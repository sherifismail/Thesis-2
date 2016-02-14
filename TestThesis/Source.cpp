
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <sstream>
#include <string>
#include <iostream>
#include "players.h"
#include <vector>
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 800;
const int FRAME_HEIGHT = 600;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 40 * 40;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";
void on_trackbar(int, void*)
{//This function gets called whenever a
	// trackbar position is changed
}
string intToString(int number){
	std::stringstream ss;
	ss << number;
	return ss.str();
}
void createTrackbars(){
	//create window for trackbars


	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);


}
void checkPos(int XH, int YH, int XA, int YA){

		if ((XA > XH) && (YA > YH))
			cout << "Offside detected";

}
void drawObject(vector<players> theplayers, Mat &frame){
	int Xhome, Yhome, Xaway, Yaway;
	for (int i = 0; i < theplayers.size(); i++){
		//int Xhome = 1000000, Yhome = 100000, Xaway = 100000, Yaway = 1000000;

		cv::circle(frame, cv::Point(theplayers.at(i).getxPos(), theplayers.at(i).getyPos()), 10, cv::Scalar(0, 0, 255));
		cv::putText(frame, intToString(theplayers.at(i).getxPos()) + " , " + intToString(theplayers.at(i).getyPos()), cv::Point(theplayers.at(i).getxPos(), theplayers.at(i).getyPos() + 20), 1, 1, Scalar(0, 255, 0));
		putText(frame, theplayers.at(i).getType(), cv::Point(theplayers.at(i).getxPos(), theplayers.at(i).getyPos() - 30), 1, 2, theplayers.at(i).getcolor());
	
		if (theplayers.at(i).getType() == "Home player"){
			Xhome = theplayers.at(i).getxPos();
			Yhome = theplayers.at(i).getyPos();
			Xaway = 0;
			Yaway = 0;
		}

		if (theplayers.at(i).getType() == "Away player"){
			Xaway = theplayers.at(i).getxPos();
			Yaway = theplayers.at(i).getyPos();
			Xhome = 10000;
			Yhome = 10000;
		}
		checkPos(Xhome, Yhome, Xaway, Yaway);
	}
}
void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);


}
void trackFilteredObject(players theplayer, Mat threshold, Mat HSV, Mat &cameraFeed){   //to write the type of the detected object

	vector<players> hplayer;

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA){
					players object;
					object.setxPos(moment.m10 / area);
					object.setyPos(moment.m01 / area);
					object.setType(theplayer.getType());
					object.setcolor(theplayer.getcolor());

					hplayer.push_back(object);


					objectFound = true;

				}
				else objectFound = false;


			}
			//let user know you found an object
			if (objectFound == true){
				//draw object location on screen
				drawObject(hplayer, cameraFeed);
			}

		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

void trackFilteredObject(Mat threshold, Mat HSV, Mat &cameraFeed){


	vector<players> hplayer;

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA){
					players object;
					object.setxPos(moment.m10 / area);
					object.setyPos(moment.m01 / area);

					hplayer.push_back(object);


					objectFound = true;

				}
				else objectFound = false;


			}
			//let user know you found an object
			if (objectFound == true){
				//draw object location on screen
				drawObject(hplayer, cameraFeed);
			}

		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

int main(int argc, char* argv[])
{
	//if we would like to calibrate our filter values, set to true.
	bool calibrationMode = false;

	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	Mat threshold;
	Mat HSV;

	if (calibrationMode){
		//create slider bars for HSV filtering
		createTrackbars();
	}
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(0);
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while (1){
		//store image to matrix
		capture.read(cameraFeed);
		if (!cameraFeed.data)
			continue;
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);

		if (calibrationMode == true){
			//if in calibration mode, we track objects based on the HSV slider values.
			cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);
			morphOps(threshold);
			imshow(windowName2, threshold);
			trackFilteredObject(threshold, HSV, cameraFeed);
		}
		else{

			players hplayer("Home player"), aplayer("Away player"), ball("Ball");


			cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, hplayer.getHSVmin(), hplayer.getHSVmax(), threshold);
			morphOps(threshold);
			trackFilteredObject(hplayer, threshold, HSV, cameraFeed);


			cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, aplayer.getHSVmin(), aplayer.getHSVmax(), threshold);
			morphOps(threshold);
			trackFilteredObject(aplayer, threshold, HSV, cameraFeed);

			cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, ball.getHSVmin(), ball.getHSVmax(), threshold);
			morphOps(threshold);
			trackFilteredObject(ball, threshold, HSV, cameraFeed);

		}

		//show frames 
		//imshow(windowName2,threshold);

		imshow(windowName, cameraFeed);
		//imshow(windowName1,HSV);


		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(20);
	}

	return 0;
}

