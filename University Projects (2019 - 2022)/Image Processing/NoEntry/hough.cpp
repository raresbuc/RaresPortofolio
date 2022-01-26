#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>    
#include <math.h>

using namespace cv;
using namespace std;


#define MAXRADIUS 100
#define MINRADIUS 0

#define PI 3.14

void houghMain(string image_nr);

void Sobel(Mat& input, Mat& result_x, Mat& result_y, Mat& result_mag, Mat& result_dir);

void GaussianBlur(Mat& input, int size, Mat& blurredOutput);

Mat threshold(Mat& input, int TH);

void hough_circles(Mat& input, Mat& gradient, Mat& dir, int*** hSpace, int radius);

int*** malloc3dArray(int dim1, int dim2, int dim3) {
    int i, j, k;
    int*** array = (int***)malloc(dim1 * sizeof(int**));

    for (i = 0; i < dim1; i++) {
        array[i] = (int**)malloc(dim2 * sizeof(int*));
        for (j = 0; j < dim2; j++) {
            array[i][j] = (int*)malloc(dim3 * sizeof(int));
        }
    }

    return array;
}

int** malloc2dArray(int dim1, int dim2) {
    int i, j;
    int** array = (int**)malloc(dim1 * sizeof(int*));

    for (i = 0; i < dim1; i++) {
        array[i] = (int*)malloc(dim2 * sizeof(int));
    }

    return array;
}


int main(int argc, char** argv) {
    // loadinhg the image
    char* image_nr = argv[1];
    Mat image;
    image = imread(image_nr, 1);

    Mat gray_image;
    cvtColor(image, gray_image, CV_BGR2GRAY);

    Mat blurred;
    GaussianBlur(gray_image, 8, blurred);
    imwrite("blurred.jpg", blurred);

    Mat SobelX(image.rows, image.cols, CV_32FC1);
    Mat SobelY(image.rows, image.cols, CV_32FC1);
    Mat SobelMag(image.rows, image.cols, CV_32FC1);
    Mat SobelDir(image.rows, image.cols, CV_32FC1);
    Sobel(blurred, SobelX, SobelY, SobelMag, SobelDir);

    Mat resultX(image.rows, image.cols, CV_8UC1);
    Mat resultY(image.rows, image.cols, CV_8UC1);
    Mat resultMag(image.rows, image.cols, CV_8UC1);
    Mat resultDir(image.rows, image.cols, CV_8UC1);

    normalize(SobelX, resultX, 0, 255, NORM_MINMAX, CV_8UC1);
    normalize(SobelY, resultY, 0, 255, NORM_MINMAX, CV_8UC1);
    normalize(SobelMag, resultMag, 0, 255, NORM_MINMAX);
    normalize(SobelDir, resultDir, 0, 255, NORM_MINMAX);
    imwrite("result_x.jpg", resultX);
    imwrite("result_y.jpg", resultY);
    imwrite("result_mag.jpg", resultMag);
    imwrite("result_dir.jpg", resultDir);

    int*** hSpace = malloc3dArray(image.rows, image.cols, MAXRADIUS);

    //loading magnitude
    Mat Mag = imread("result_mag.jpg", 1);
    Mat Mag_gray;
    cvtColor(Mag, Mag_gray, CV_BGR2GRAY);

    // thresholding magnitude
    Mat TH = threshold(Mag_gray, 60);

    // finding circles 
    hough_circles(image, TH, SobelDir, hSpace, MAXRADIUS - MINRADIUS);

    return 0;
}


void Sobel(Mat& input, Mat& result_x, Mat& result_y, Mat& result_mag, Mat& result_dir) {
    Mat kernelDX = (Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    Mat kernelDY = (Mat_<double>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);

    int kernelRadiusX = (kernelDX.size[0] - 1) / 2;
    int kernelRadiusY = (kernelDX.size[1] - 1) / 2;

    Mat paddedInput;
    copyMakeBorder(input, paddedInput, kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY, BORDER_REPLICATE);

    // now we can do the convoltion
    for (int i = 0; i < input.rows; i++) {
        for (int j = 0; j < input.cols; j++) {
            float sumDX = 0.0;
            float sumDY = 0.0;
            for (int m = -kernelRadiusX; m <= kernelRadiusX; m++) {
                for (int n = -kernelRadiusY; n <= kernelRadiusY; n++) {
                    // find the correct indices we are using
                    int imagex = i + m + kernelRadiusX;
                    int imagey = j + n + kernelRadiusY;
                    int kernelx = m + kernelRadiusX;
                    int kernely = n + kernelRadiusY;
                    int imageval = (int)paddedInput.at<uchar>(imagex, imagey);
                    double kernelvalDX = kernelDX.at<double>(kernelx, kernely);
                    double kernelvalDY = kernelDY.at<double>(kernelx, kernely);
                    // do the multiplication
                    sumDX += imageval * kernelvalDX;
                    sumDY += imageval * kernelvalDY;
                }
            }
            // set the output value as the sum of the convolution
            result_x.at<float>(i, j) = (float)sumDX;
            result_y.at<float>(i, j) = (float)sumDY;
            result_mag.at<float>(i, j) = (float)sqrt((sumDX * sumDX) + (sumDY * sumDY));
            result_dir.at<float>(i, j) = (float)atan2(sumDY, sumDX);
        }
    }
}


void GaussianBlur(cv::Mat& input, int size, Mat& result)
{
    // intialise the output using the input
    result.create(input.size(), input.type());

    // create the Gaussian kernel in 1D 
    cv::Mat kX = cv::getGaussianKernel(size, -1);
    cv::Mat kY = cv::getGaussianKernel(size, -1);

    // make it 2D multiply one by the transpose of the other
    cv::Mat kernel = kX * kY.t();

    //CREATING A DIFFERENT IMAGE kernel WILL BE NEEDED
    //TO PERFORM OPERATIONS OTHER THAN GUASSIAN BLUR!!!

    // we need to create a padded version of the input
    // or there will be border effects
    int kernelRadiusX = (kernel.size[0] - 1) / 2;
    int kernelRadiusY = (kernel.size[1] - 1) / 2;

    cv::Mat paddedInput;
    cv::copyMakeBorder(input, paddedInput, kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY, cv::BORDER_REPLICATE);

    // now we can do the convoltion
    for (int i = 0; i < input.rows; i++) {
        for (int j = 0; j < input.cols; j++) {
            double sum = 0.0;
            for (int m = -kernelRadiusX; m <= kernelRadiusX; m++) {
                for (int n = -kernelRadiusY; n <= kernelRadiusY; n++) {
                    // find the correct indices we are using
                    int imagex = i + m + kernelRadiusX;
                    int imagey = j + n + kernelRadiusY;
                    int kernelx = m + kernelRadiusX;
                    int kernely = n + kernelRadiusY;
                    // get the values from the padded image and the kernel
                    int imageval = (int)paddedInput.at<uchar>(imagex, imagey);
                    double kernelval = kernel.at<double>(kernelx, kernely);
                    // do the multiplication
                    sum += imageval * kernelval;
                }
            }
            // set the output value as the sum of the convolution
            result.at<uchar>(i, j) = (uchar)sum;
        }
    }
}


Mat threshold(Mat& input, int TH) {
    Mat thresh(input.rows, input.cols, CV_8UC1);

    for (int i = 0; i < input.rows; i++) {
        for (int j = 0; j < input.cols; j++) {
            int imageval = (int)input.at<uchar>(i, j);

            if (imageval > TH) {
                thresh.at<uchar>(i, j) = 255;
            }
            else {
                thresh.at<uchar>(i, j) = 0;
            }
        }
    }

    imwrite("TH.jpg", thresh);
    return thresh;
}


void hough_circles(Mat& input, Mat& gradient, Mat& dir, int*** hSpace, int radius) {
    for (int i = 0; i < gradient.rows; i++) {
        for (int j = 0; j < gradient.cols; j++) {
            for (int k = 0; k < MAXRADIUS; k++) {
                hSpace[i][j][k] = 0;
            }
        }
    }

    for (int x = 0; x < gradient.rows; x++) {
        for (int y = 0; y < gradient.cols; y++) {
            if (gradient.at<uchar>(x, y) == 255) {
                for (int r = MINRADIUS; r < MAXRADIUS; r++) {
                    for (int i = -1; i < 2; i += 2) {
                        for (int j = -1; j < 2; j += 2) {
                            int y0 = (int)(y + i * r * cos(dir.at<float>(x, y)));
                            int x0 = (int)(x + j * r * sin(dir.at<float>(x, y)));
                            if ((y0 > 0 && y0 < gradient.cols) && (x0 > 0 && x0 < gradient.rows)) {
                                hSpace[x0][y0][r] += 1;
                            }
                        }
                    }
                }
            }
        }
    }

    Mat result_hSpace(gradient.rows, gradient.cols, CV_32FC1);

    for (int x = 0; x < gradient.rows; x++) {
        for (int y = 0; y < gradient.cols; y++) {
            for (int r = MINRADIUS; r < MAXRADIUS; r++) {
                result_hSpace.at<float>(x, y) += hSpace[x][y][r];
            }
        }
    }

    normalize(result_hSpace, result_hSpace, 0, 255, NORM_MINMAX);

    int circlesCount = 0;
    vector<Point> circles;
    vector<int> radiuses;

    for (int y = 0; y < gradient.rows; y++) {
        for (int x = 0; x < gradient.cols; x++) {
            if (result_hSpace.at<float>(y, x) > 20) {
                Point p;
                Scalar color(255, 255, 255);
                p.x = x;
                p.y = y;
                bool ok = true;
                for (int i = 0; i < circles.size(); i++) {
                    if (abs(p.x - circles[i].x) < 50 && abs(p.y - circles[i].y) < 50) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    circlesCount++;
                    circles.push_back(p);
                }
            }
        }
    }

    for (int i = 0; i < circles.size(); i++) {
        int maxi = -1;
        int bestR = -1;
        Point p = circles[i];
        for (int r = MINRADIUS; r < MAXRADIUS; r++) {
            if (hSpace[p.y][p.x][r] > maxi) {
                maxi = hSpace[p.y][p.x][r];
                bestR = r;
            }
        }
        radiuses.push_back(bestR);
        circle(input, p, bestR, Scalar(255, 0, 0), 3, CV_AA);
    }
    cout << circles.size() << " " << "circles";

    imwrite("result_hough_circles.jpg", result_hSpace);
    imwrite("outputCircles.jpg", input);
}