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
String cascade_name = "frontalface.xml";
CascadeClassifier cascade;


/** @function main */
int main(int argc, const char** argv) {
	// 1. Read Input Image
	string image_nr = argv[1];
	string file = "No_entry/NoEntry" + image_nr + ".bmp";
	Mat frame = imread(file, CV_LOAD_IMAGE_COLOR);

	// 2. Load the Strong Classifier in a structure called `Cascade'
	if (!cascade.load(cascade_name)) { printf("--(!)Error loading\n"); return -1; };

	// 3. Detect Faces and Display Result
	detectAndDisplay(frame, image_nr);

	// 4. Save Result Image
	imwrite("detected_faces/detected" + image_nr + ".jpg", frame);

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
	string file_name = "faces_coord/" + image_nr + ".txt";
	string line;
	ifstream inFile(file_name);
	std::vector<Rect> true_faces;

	while (getline(inFile, line)) {
		vector<string> tokens = split(line, ',');
		true_faces.push_back(Rect(stoi(tokens[0]), stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3])));
	}
	inFile.close();

	return true_faces;
}


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
	vector<Rect> true_faces = readFile(image_nr);
	std::vector<Rect> faces;
	Mat frame_gray;

	// 1. Prepare Image by turning it into Grayscale and normalising lighting
	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// 2. Perform Viola-Jones Object Detection 
	cascade.detectMultiScale(frame_gray, faces, 1.1, 1, 0 | CV_HAAR_SCALE_IMAGE, Size(10, 10), Size(300, 300));

	// 3. Print number of Faces found
	std::cout << faces.size() << std::endl;
	std::cout << true_faces.size() << std::endl;

	// 4. Draw box around faces found
	for (int i = 0; i < faces.size(); i++) {
		rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar(0, 255, 0), 2);
	}

	// 5. Draw box around true faces found
	for (int j = 0; j < true_faces.size(); j++) {
		rectangle(frame, Point(true_faces[j].x, true_faces[j].y), Point(true_faces[j].x + true_faces[j].width, true_faces[j].y + true_faces[j].height), Scalar(0, 0, 255), 2);
	}

	float iou_threshold = 0.5;
	float true_positives = 0;

	for (int x = 0; x < true_faces.size(); x++) {
		for (int y = 0; y < faces.size(); y++) {
			if ((findIOU(faces[y], true_faces[x])) > iou_threshold) {
				true_positives++;
				break;
			}
		}
	}

	// true positive rate
	float tpr;
	cout << "image: " << image_nr << endl;

	if (true_faces.size() > 0) {
		tpr = true_positives / (float)true_faces.size();
	}
	else {
		tpr = 0;
	}
	cout << "TPR: " << tpr << endl;

	float false_positives = faces.size() - true_positives;
	cout << "false positives: " << false_positives << endl;

	float false_negatives = true_faces.size() - true_positives;
	cout << "false negatives: " << false_negatives << endl;

	float f1_score = findF1Score(true_positives, false_positives, false_negatives);
	cout << "F1-SCORE: " << f1_score << endl;
}
