/////////////////////////////////////////////////////////////////////////////
//
// COMS30121 - noentry_viola.cpp
//
/////////////////////////////////////////////////////////////////////////////

// header inclusion
#include <stdio.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

//#pragma warning(disable : 4996)

using namespace std;
using namespace cv;


/** Function Headers */
vector<string> split(const string& line, char delimiter);
vector<Rect> readFile(string image_nr);
float findIOU(Rect found, Rect truth);
void detectAndDisplay(Mat frame, string image_nr);
float findF1Score(float TP, float FP, float FN);


/** Global variables */
String cascade_name = "NoEntrycascade/cascade.xml";
CascadeClassifier cascade;


/** @function main */
int main(int argc, const char** argv) {
	// 1. Read Input Image
	string image_nr = argv[1];
	string file = "No_entry/NoEntry" + image_nr + ".bmp";
	Mat frame = imread(file, CV_LOAD_IMAGE_COLOR);

	// 2. Load the Strong Classifier in a structure called `Cascade'
	if (!cascade.load(cascade_name)) { printf("--(!)Error loading\n"); return -1; };

	// 3. Detect Signs and Display Result
	detectAndDisplay(frame, image_nr);

	// 4. Save Result Image
	imwrite("detected_signs/detected" + image_nr + ".jpg", frame);

	return 0;
}


// Computer Graphics CW
vector<string> split(const string& line, char delimiter) {
	auto haystack = line;
	vector<string> tokens;
	size_t pos;

	while ((pos = haystack.find(delimiter)) != string::npos) {
		tokens.push_back(haystack.substr(0, pos));
		haystack.erase(0, pos + 1);
	}
	tokens.push_back(haystack);

	return tokens;
}


vector<Rect> readFile(string image_nr) {
	string file_name = "signs_coord/" + image_nr + ".txt";
	string line;
	ifstream inFile(file_name);
	std::vector<Rect> true_signs;

	while (getline(inFile, line)) {
		vector<string> tokens = split(line, ',');
		true_signs.push_back(Rect(stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3])));
	}
	inFile.close();

	return true_signs;
}


//void SobelX(cv::Mat& input, int size, cv::Mat& blurredOutput) {
//	Mat kernel = (Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
//	blurredOutput.create(input.size(), CV_64F);
//
//	int kernelRadiusX = (kernel.size[0] - 1) / 2;
//	int kernelRadiusY = (kernel.size[1] - 1) / 2;
//
//	for (int m = -kernelRadiusX; m <= kernelRadiusX; m++) {
//		for (int n = -kernelRadiusY; n <= kernelRadiusY; n++) {
//			kernel.at<double>(m + kernelRadiusX, n + kernelRadiusY) = kernel.at<double>(m + kernelRadiusX, n + kernelRadiusY);
//		}
//	}
//
//	cv::Mat paddedInput;
//	cv::copyMakeBorder(input, paddedInput,
//		kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY,
//		cv::BORDER_REPLICATE);
//
//	// now we can do the convoltion
//	for (int i = 0; i < input.rows; i++) {
//		for (int j = 0; j < input.cols; j++) {
//			double sum = 0.0;
//
//			for (int m = -kernelRadiusX; m <= kernelRadiusX; m++) {
//				for (int n = -kernelRadiusY; n <= kernelRadiusY; n++) {
//					// find the correct indices we are using
//					int imagex = i + m + kernelRadiusX;
//					int imagey = j + n + kernelRadiusY;
//					int kernelx = m + kernelRadiusX;
//					int kernely = n + kernelRadiusY;
//
//					// get the values from the padded image and the kernel
//					int imageval = (int)paddedInput.at<uchar>(imagex, imagey);
//					double kernalval = kernel.at<double>(kernelx, kernely);
//
//					// do the multiplication
//					sum += imageval * kernalval;
//				}
//			}
//			// set the output value as the sum of the convolution
//			if (sum > 255) {
//				sum = sum / 255.0;
//			}
//			if (sum < 0) {
//				sum = sum / (-255.0);
//			}
//			blurredOutput.at<double>(i, j) = (double)sum;
//		}
//	}
//}


//void SobelY(cv::Mat& input, int size, cv::Mat& blurredOutput) {
//	Mat kernel = (Mat_<double>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
//
//	// intialise the output using the input
//	blurredOutput.create(input.size(), CV_64F);
//
//	int kernelRadiusX = (kernel.size[0] - 1) / 2;
//	int kernelRadiusY = (kernel.size[1] - 1) / 2;
//
//	// SET KERNEL VALUES
//	for (int m = -kernelRadiusX; m <= kernelRadiusX; m++) {
//		for (int n = -kernelRadiusY; n <= kernelRadiusY; n++)
//			kernel.at<double>(m + kernelRadiusX, n + kernelRadiusY) = kernel.at<double>(m + kernelRadiusX, n + kernelRadiusY);
//	}
//
//	cv::Mat paddedInput;
//	cv::copyMakeBorder(input, paddedInput,
//		kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY,
//		cv::BORDER_REPLICATE);
//
//	for (int i = 0; i < input.rows; i++) {
//		for (int j = 0; j < input.cols; j++) {
//			double sum = 0.0;
//
//			for (int m = -kernelRadiusX; m <= kernelRadiusX; m++) {
//				for (int n = -kernelRadiusY; n <= kernelRadiusY; n++) {
//					// find the correct indices we are using
//					int imagex = i + m + kernelRadiusX;
//					int imagey = j + n + kernelRadiusY;
//					int kernelx = m + kernelRadiusX;
//					int kernely = n + kernelRadiusY;
//					// get the values from the padded image and the kernel
//					//cout << "wwwwwwww" << "\n";
//					int imageval = (int)paddedInput.at<uchar>(imagex, imagey);
//					//cout << imageval << "  ";
//					double kernalval = kernel.at<double>(kernelx, kernely);
//					// do the multiplication
//					sum += imageval * kernalval;
//				}
//			}
//
//			// set the output value as the sum of the convolution
//			if (sum > 255) {
//				sum = sum / 255.0;
//			}
//			if (sum < 0) {
//				sum = sum / (-255.0);
//			}
//			blurredOutput.at<double>(i, j) = (double)sum;
//		}
//	}
//}


//void GaussianBlur(cv::Mat& input, int size, cv::Mat& blurredOutput)
//{
//	// intialise the output using the input
//	blurredOutput.create(input.size(), input.type());
//
//	// create the Gaussian kernel in 1D 
//	cv::Mat kX = cv::getGaussianKernel(size, -1);
//	cv::Mat kY = cv::getGaussianKernel(size, -1);
//
//	// make it 2D multiply one by the transpose of the other
//	cv::Mat kernel = kX * kY.t();
//
//	//CREATING A DIFFERENT IMAGE kernel WILL BE NEEDED
//	//TO PERFORM OPERATIONS OTHER THAN GUASSIAN BLUR!!!
//
//	// we need to create a padded version of the input
//	// or there will be border effects
//	int kernelRadiusX = (kernel.size[0] - 1) / 2;
//	int kernelRadiusY = (kernel.size[1] - 1) / 2;
//
//	cv::Mat paddedInput;
//	cv::copyMakeBorder(input, paddedInput,
//		kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY,
//		cv::BORDER_REPLICATE);
//
//	// now we can do the convoltion
//	for (int i = 0; i < input.rows; i++)
//	{
//		for (int j = 0; j < input.cols; j++)
//		{
//			double sum = 0.0;
//			for (int m = -kernelRadiusX; m <= kernelRadiusX; m++)
//			{
//				for (int n = -kernelRadiusY; n <= kernelRadiusY; n++)
//				{
//					// find the correct indices we are using
//					int imagex = i + m + kernelRadiusX;
//					int imagey = j + n + kernelRadiusY;
//					int kernelx = m + kernelRadiusX;
//					int kernely = n + kernelRadiusY;
//
//					// get the values from the padded image and the kernel
//					int imageval = (int)paddedInput.at<uchar>(imagex, imagey);
//					double kernalval = kernel.at<double>(kernelx, kernely);
//
//					// do the multiplication
//					sum += imageval * kernalval;
//				}
//			}
//			// set the output value as the sum of the convolution
//			blurredOutput.at<uchar>(i, j) = (uchar)sum;
//		}
//	}
//}


// https://www.pyimagesearch.com/2016/11/07/intersection-over-union-iou-for-object-detection/
float findIOU(Rect found, Rect truth) {
	float width = min(found.x + found.width, truth.x + truth.width) - max(found.x, truth.x);
	float height = min(found.y + found.height, truth.y + truth.height) - max(found.y, truth.y);

	if (width <= 0 || height <= 0) {
		return 0;
	}

	float intersectionArea = width * height;
	float unionArea = (found.width * found.height) + (truth.width * truth.height) - intersectionArea;

	return intersectionArea / unionArea;
}


// https://towardsdatascience.com/the-f1-score-bec2bbc38aa6 
float findF1Score(float TP, float FP, float FN) {
	if (TP == 0 && FP == 0 && FN == 0) {
		return 0;
	}

	float f1_score = TP / (TP + 0.5 * (FP + FN));

	return f1_score;
}


/** @function detectAndDisplay */
void detectAndDisplay(Mat frame, string image_nr) {
	vector<Rect> true_signs = readFile(image_nr);
	std::vector<Rect> signs;
	Mat frame_gray;

	// 1. Prepare Image by turning it into Grayscale and normalising lighting
	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// 2. Perform Viola-Jones Object Detection 
	cascade.detectMultiScale(frame_gray, signs, 1.1, 1, 0 | CV_HAAR_SCALE_IMAGE, Size(10, 10), Size(300, 300));

	// 3. Print number of Signs found
	std::cout << signs.size() << std::endl;
	std::cout << true_signs.size() << std::endl;

	// 4. Draw box around signs found
	for (int i = 0; i < signs.size(); i++) {
		rectangle(frame, Point(signs[i].x, signs[i].y), Point(signs[i].x + signs[i].width, signs[i].y + signs[i].height), Scalar(0, 255, 0), 2);
	}

	// 5. Draw box around true signs found
	for (int j = 0; j < true_signs.size(); j++) {
		rectangle(frame, Point(true_signs[j].x, true_signs[j].y), Point(true_signs[j].x + true_signs[j].width, true_signs[j].y + true_signs[j].height), Scalar(0, 0, 255), 2);
	}

	float iou_threshold = 0.5;
	float true_positives = 0;

	for (int x = 0; x < true_signs.size(); x++) {
		for (int y = 0; y < signs.size(); y++) {
			if ((findIOU(signs[y], true_signs[x])) > iou_threshold) {
				true_positives++;
				break;
			}
		}
	}

	// true positive rate
	float tpr;
	cout << "image: " << image_nr << endl;

	if (true_signs.size() > 0) {
		tpr = true_positives / (float)true_signs.size();
	}
	else {
		tpr = 0;
	}
	cout << "TPR: " << tpr << endl;

	float false_positives = signs.size() - true_positives;
	cout << "false positives: " << false_positives << endl;

	float false_negatives = true_signs.size() - true_positives;
	cout << "false negatives: " << false_negatives << endl;

	float f1_score = findF1Score(true_positives, false_positives, false_negatives);
	cout << "F1-SCORE: " << f1_score << endl;
}
