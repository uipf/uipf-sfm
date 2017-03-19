#include "Foerstner.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace cv;
using namespace std;

// calculates the average value of pixels in an image
float average(Mat& inp)
{
	float sum = 0;
	int cnt = 0;
	for(int r = 0; r < inp.rows; ++r) {
		for(int c = 0; c < inp.cols; ++c) {
			sum += inp.at<float>(r,c);
			++cnt;
		}
	}
	return sum / cnt;
}

// uses structure tensor to define interest points (foerstner)
void Foerstner::getInterestPoints(const Mat& img, double sigma, vector<KeyPoint>& points){

	// sobel gradients
	Mat Gx, Gy;
	// void Sobel(InputArray src, OutputArray dst, int ddepth, int dx, int dy, int ksize=3, double scale=1, double delta=0, int borderType=BORDER_DEFAULT )¶
	// http://docs.opencv.org/modules/imgproc/doc/filtering.html#void%20Sobel%28InputArray%20src,%20OutputArray%20dst,%20int%20ddepth,%20int%20dx,%20int%20dy,%20int%20ksize,%20double%20scale,%20double%20delta,%20int%20borderType%29

	Mat kernelX = createFstDevKernel(sigma);
	Mat kernelY;
	kernelX.copyTo(kernelY);
	transpose(kernelY,kernelY);
	flip(kernelY,kernelY,1);
	filter2D(img, Gx, -1 , kernelX);
	filter2D(img, Gy, -1 , kernelY);

	Mat Gxx = Gx.clone(); // Gradient_X ^2
	Mat Gyy = Gy.clone(); // Gradient_Y ^2
	Mat Gxy = Gx.clone(); // Gradient_XY
	for(int r = 0; r < img.rows; r++) {
		for(int c = 0; c < img.cols; c++) {
			// create square matrixes
			Gxx.at<float>(r,c) = Gx.at<float>(r,c) * Gx.at<float>(r,c);
			Gyy.at<float>(r,c) = Gy.at<float>(r,c) * Gy.at<float>(r,c);
			Gxy.at<float>(r,c) = Gx.at<float>(r,c) * Gy.at<float>(r,c);
		}
	}

	// Gausian blur on the gradient matrixes
	// void GaussianBlur(InputArray src, OutputArray dst, Size ksize, double sigmaX, double sigmaY=0, int borderType=BORDER_DEFAULT )
	GaussianBlur(Gxx, Gxx, Size(5,5), 1, 1);
	GaussianBlur(Gyy, Gyy, Size(5,5), 1, 1);
	GaussianBlur(Gxy, Gxy, Size(5,5), 1, 1);

	Mat trA = Gx.clone(); // trace of the structure tensor
	Mat detA = Gx.clone(); // determinant of the structure tensor
	Mat w = Gx.clone(); // weight:   w = det(A)/tr(A)
	Mat q = Gx.clone(); // isotropy: q = 4*det(A)/tr(A)²
	for(int r = 0; r < img.rows; r++) {
		for(int c = 0; c < img.cols; c++) {
			// calculate trace of structure tensor
			trA.at<float>(r,c) = Gxx.at<float>(r,c) + Gyy.at<float>(r,c);
			// calculate determinant of structure tensor
			detA.at<float>(r,c) = Gxx.at<float>(r,c) * Gyy.at<float>(r,c) - Gxy.at<float>(r,c) * Gxy.at<float>(r,c);
			// calculate weight
			w.at<float>(r,c) = detA.at<float>(r,c) / trA.at<float>(r,c);
			// calculate isotropy
			q.at<float>(r,c) = 4 * detA.at<float>(r,c) / (trA.at<float>(r,c) * trA.at<float>(r,c));
		}
	}
//    showImage(Gxx, "Sobel in X²", 0, true, true);
//    showImage(Gyy, "Sobel in Y²", 0, true, true);
//    showImage(Gxy, "Sobel in X*Y", 0, true, true);
//	showImage(trA, "trace of structure tensor", 0, true, true);
//	showImage(detA, "determinant of structure tensor", 0, true, true);

//    showImage(w, "weight", 0, true, true);
//    showImage(q, "isotropy", 0, true, true);

	// weight non-max + thresholding
	Mat nonMaxW = nonMaxSuppression(w);

	// isotropy non-max + thresholding
	Mat nonMaxQ = nonMaxSuppression(q);

//	showImage(nonMaxW, "weightNM", 0, true, true);
//	showImage(nonMaxQ, "isotropyNM", 0, true, true);

	float wAvg = average(nonMaxW);

	// extract points
	for(int r = 0; r < img.rows; r++) {
		for(int c = 0; c < img.cols; c++) {
			if (nonMaxW.at<float>(r,c) > 0.5 * wAvg && nonMaxQ.at<float>(r,c) > 0.5) {
				points.push_back(KeyPoint(c,r,1));
			}
		}
	}

}

// creates kernel representing fst derivative of a Gaussian kernel in x-direction
/*
sigma	standard deviation of the Gaussian kernel
return	the calculated kernel
*/
Mat Foerstner::createFstDevKernel(double sigma){

	int kSize = (int) ceil(sigma/0.3413);
	Mat kernel(kSize,kSize,CV_32FC1);

	// Maximal value at centre
	const float  MU=kSize/2;

	// must integrate to one, so we need the sum
	float sum = 0;

	// loop through kernel
	for(int i = 0 ; i < kSize ; ++i) {
		for(int j = 0 ; j < kSize ; ++j) {
			double val = -((j-MU)/(sigma*sigma))*(1.0/(2.0*M_PI*sigma*sigma) * exp( -( (j-MU)*(j-MU)/(2*sigma*sigma) + (i-MU)*(i-MU)/(2*sigma*sigma) )));
			kernel.at<float>(i,j) = (float) val;
			sum += val;
		}
	}
	return kernel;
}

// function calls processing function
/*
in		:  input image
points	:	detected keypoints
*/
void Foerstner::run(const Mat& in, vector<KeyPoint>& points){
	this->getInterestPoints(in, this->sigma, points);
}

// non-maxima suppression
// if any of the pixel at the 4-neighborhood is greater than current pixel, set it to zero
Mat Foerstner::nonMaxSuppression(Mat& img){

	Mat out = img.clone();

	for(int x=1; x<out.cols-1; x++){
		for(int y=1; y<out.rows-1; y++){
			if ( img.at<float>(y-1, x) >= img.at<float>(y, x) ){
				out.at<float>(y, x) = 0;
				continue;
			}
			if ( img.at<float>(y, x-1) >= img.at<float>(y, x) ){
				out.at<float>(y, x) = 0;
				continue;
			}
			if ( img.at<float>(y, x+1) >= img.at<float>(y, x) ){
				out.at<float>(y, x) = 0;
				continue;
			}
			if ( img.at<float>( y+1, x) >= img.at<float>(y, x) ){
				out.at<float>(y, x) = 0;
				continue;
			}
		}
	}
	return out;
}

// Function displays image (after proper normalization)
/*
win   :  Window name
img   :  Image that shall be displayed
cut   :  whether to cut or scale values outside of [0,255] range
*/
void Foerstner::showImage(Mat& img, const char* win, int wait, bool show, bool save){

	Mat aux = img.clone();

	// scale and convert
	if (img.channels() == 1)
		normalize(aux, aux, 0, 255, CV_MINMAX);
	aux.convertTo(aux, CV_8UC1);
	// show
	if (show){
		imshow( win, aux);
		waitKey(wait);
	}
	// save
	if (save)
		imwrite( (string(win)+string(".png")).c_str(), aux);
}
