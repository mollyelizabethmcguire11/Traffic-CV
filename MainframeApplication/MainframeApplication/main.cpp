/********************************************************************************/
/*                                                                              */
/* Program: Mainframe Thermal Image Processor                                   */
/*                                                                              */
/* File: main.cpp                                                               */
/*                                                                              */
/* Author(s): Alex Gribov, Alexander Senckowski, Molly McGuire and Kevin Smida  */
/*                                                                              */
/* All Right Reserved (c) 2017                                                  */
/*                                                                              */
/********************************************************************************/

#include "globals.h"
#include "videoHelper.h"
#include "GUI.h"
#include "threshold.h"

void processVideo(VideoCapture* pCap);
int thresholding(Mat &frame, Mat* p_frame_threshold);

int main(int argc, char* argv[]) {
	const string sampleStreamAddress = "http://66.193.157.18/mjpg/video.mjpg";
	VideoCapture* pCap;

	initializeGUI();

	//create Background Subtractor objects
	pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach

	//initializeVideo(sampleStreamAddress.c_str());
	pCap = initializeVideo("http://66.193.157.18/mjpg/video.mjpg");

	processVideo(pCap);

	//thresholding("http://66.193.157.18/mjpg/video.mjpg");

	//destroy GUI windows
	destroyAllWindows();
	return EXIT_SUCCESS;
}

/************************** AZS Start Subtraction Method *************************/
void processVideo(VideoCapture* pCap) {
	
	//read input data. ESC or 'q' for quitting
	keyboard = 0;

	Mat frame, threshFrame; //current frame

	while (keyboard != 'q' && keyboard != 27) {

		//read the current frame
		if (!(*pCap).read(frame)) {
			cerr << "Unable to read next frame." << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_FAILURE);
		}

		//update the background model
		pMOG2->apply(frame, fgMaskMOG2);

		//get the frame number and write it on the current frame
		stringstream ss;
		rectangle(frame, cv::Point(10, 2), cv::Point(100, 20),
			cv::Scalar(255, 255, 255), -1);
		ss << (*pCap).get(CAP_PROP_POS_FRAMES);
		string frameNumberString = ss.str();
		putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
			FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));

		//Run Thresholding
		thresholding(frame, &threshFrame);

		//show the current frame and the fg masks
		imshow("Frame", frame);
		imshow("FG Mask MOG 2", fgMaskMOG2);

		//get the input from the keyboard
		keyboard = (char)waitKey(30);
	}
	//delete capture object
	releaseVideo(pCap);
}
/************************** AZS End Subtraction Method ***************************/

/************************** AZS Start Thresholding *******************************/
int thresholding(Mat &frame, Mat* p_frame_threshold)	//AZS Modified
{
	//int low_r = 30, low_g = 30, low_b = 30;
	//int high_r = 100, high_g = 100, high_b = 100;

	if (frame.empty())
		return -1;
	
	//-- Detect the object based on RGB Range Values
	inRange(frame, Scalar(low_b, low_g, low_r), Scalar(high_b, high_g, high_r), (*p_frame_threshold));

	//-- Show the frames
	imshow("Video Capture", frame);
	imshow("Object Detection", (*p_frame_threshold));

	return 0;
}


/************************** AZS END Thresholding *********************************/

//int main(int argc, char* argv[]) {
//
//	// FIXME: In the future all declarations will be in file globals.h, but I left this here for now so everyone can see how the stream works.
//	VideoCapture cap;		// Number is the ID of video device.
//
//	//*****************************************************//
//	// BEGIN SETUP                                         //
//	//*****************************************************//
//
//	// FIXME: Future home of FLIR ONE address
//	const std::string videoStreamAddress = "http://192.168.0.117:8080/cam_1.mjpg";
//	// Address of sample video feed
//	const std::string sampleStreamAddress = "http://66.193.157.18/mjpg/video.mjpg";
//
//	// Load the videostrem into VideoCapture object.
//	cap.open(sampleStreamAddress);
//	if (!cap.isOpened())  // If not success, exit program
//	{
//		cout << "Cannot open the video cam" << endl;
//		return -1;
//	}
//
//	// Not yet needed, but may be useful
//	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
//	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
//
//	cout << "Frame size : " << dWidth << " x " << dHeight << endl;
//
//	// Create output windows for displaying video
//	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
//	namedWindow("MyNegativeVideo", CV_WINDOW_AUTOSIZE);
//
//	//*****************************************************//
//	// BEGIN SETUP                                         //
//	//*****************************************************//
//
//	//*****************************************************//
//	// BEGIN EXECUTION                                     //
//	//*****************************************************//
//
//	// While loop is main program body -- Gets image from stream, operates on it, then repeats
//	// Loop breaks when user hits "esc" key
//	while (1) {
//		Mat frame;
//		Mat contours;
//
//		bool bSuccess = cap.read(frame); // Read a new frame from video
//
//		if (!bSuccess) { // If not success, break loop
//			cout << "Cannot read a frame from video stream" << endl;
//			break;
//		}
//
//		flip(frame, frame, 1);
//		imshow("MyVideo", frame); //show the frame in "MyVideo" window
//
//		// Sample image processing
//		Canny(frame, contours, 500, 1000, 5, true);
//		imshow("MyNegativeVideo", contours);
//
//		if (waitKey(30) == 27) {//wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
//			cout << "esc key is pressed by user" << endl;
//			break;
//		}
//	}
//	return 0;
//}

//// TEST FUNCTION, IGNORE THIS:
//void connectToCamera() {
//	VideoCapture stream(0);		// Number is the ID of video device.
//	stream.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
//	stream.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
//	stream.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
//
//	if (!stream.isOpened()) { //check if video device has been initialised
//		cout << "cannot open camera";
//	}
//
//	//unconditional loop
//	while (true) {
//		Mat cameraFrame;
//		stream.read(cameraFrame);
//		imshow("cam", cameraFrame);
//		if (waitKey(30) >= 0)
//			break;
//	}
//}
