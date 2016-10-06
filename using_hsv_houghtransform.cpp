#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int erosion_elem = 0;
int dilation_elem = 0;

namespace
{
    // windows and trackbars name
    const std::string windowName = "Hough Circle Detection Demo";
    const std::string cannyThresholdTrackbarName = "Canny threshold";
    const std::string accumulatorThresholdTrackbarName = "Accumulator Threshold";
    const std::string usage = "Usage : tutorial_HoughCircle_Demo <path_to_input_image>\n";

    // initial and max values of the parameters of interests.
    const int cannyThresholdInitialValue = 200;
    const int accumulatorThresholdInitialValue = 50;
    const int maxAccumulatorThreshold = 200;
    const int maxCannyThreshold = 255;

    void HoughDetection(const Mat& src_gray, const Mat& src_display, int cannyThreshold, int accumulatorThreshold)
    {
        // will hold the results of the detection
        std::vector<Vec3f> circles;
        // runs the actual detection
        HoughCircles( src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, cannyThreshold, accumulatorThreshold, 0, 0 );

        // clone the colour, input image for displaying purposes
        Mat display = src_display.clone();
        for( size_t i = 0; i < circles.size(); i++ )
        {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // circle center
            circle( display, center, 3, Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            circle( display, center, radius, Scalar(0,0,255), 3, 8, 0 );
        }

        // shows the results
        imshow( windowName, display);
    }
}
Mat rgb_detect(Mat img, int _hmax_y, int _hmin_y, int _smax_y, int _smin_y, int _vmax_y, int _vmin_y, int _hmax_r, int _hmin_r, int _smax_r, int _smin_r, int _vmax_r, int _vmin_r)
{

	//Mat rgb(img.rows, img.cols, CV_8UC1, Scalar(255));
	

        Mat yellow;
        Mat red;
	inRange(img, Scalar(_hmin_y,_smin_y, _vmin_y), Scalar(_hmax_y,_smax_y,_vmax_y), yellow);
	inRange(img, Scalar(_hmin_r,_smin_r, _vmin_r), Scalar(_hmax_r,_smax_r,_vmax_r), red);

	// Combine the above two images
	Mat output;
	addWeighted(yellow, 0.5, red, 0.5, 0.0, output);

	//imshow("output_hsv_img",hsv);
	return output;
}
Mat erosion(Mat main_image,int erosion_size)
{
	int erosion_type;
	Mat erosion_dst;
	if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
	else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
	else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement( erosion_type,
	Size( 2*erosion_size + 1, 2*erosion_size+1 ),
	Point( erosion_size, erosion_size ) );
	/// Apply the erosion operation
	erode( main_image, erosion_dst, element );
	return erosion_dst;


}
Mat dilation(Mat main_image,int dilation_size )
{
	int dilation_type;
	Mat dilation_dst;
	if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
	else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
	else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement( dilation_type,
	Size( 2*dilation_size + 1, 2*dilation_size+1 ),
	Point( dilation_size, dilation_size ) );
	/// Apply the dilation operation
	dilate( main_image, dilation_dst, element );
	return dilation_dst;
}


int main(int argc, char** argv)
{
    Mat src,src_gray,hsv_img,noise_reduce;
    int hmax_y=0,hmin_y=0,smax_y=0,smin_y=0,vmax_y=0,vmin_y=0,hmax_r=0,hmin_r=0,smax_r=0,smin_r=0,vmax_r=0,vmin_r=0,dil = 0,ero = 0;
/*  
    if (argc < 2)
    {
        std::cerr<<"No input image specified\n";
        std::cout<<usage;
        return -1;
    }
*/
    // Read the image
    VideoCapture vid(argv[1]);
    
    
/*
    if( src.empty() )
    {
        std::cerr<<"Invalid input image\n";
        std::cout<<usage;
        return -1;
    }
*/
   

    //declare and initialize both parameters that are subjects to change
    int cannyThreshold = cannyThresholdInitialValue;
    int accumulatorThreshold = accumulatorThresholdInitialValue;

    // create the main window, and attach the trackbars
    namedWindow( windowName, WINDOW_AUTOSIZE );
    //namedWindow("hsv_output", CV_WINDOW_AUTOSIZE);
    namedWindow("src_gray",CV_WINDOW_AUTOSIZE);
    namedWindow("noise_reduce",CV_WINDOW_AUTOSIZE);
    createTrackbar(cannyThresholdTrackbarName, windowName, &cannyThreshold,maxCannyThreshold);
    createTrackbar(accumulatorThresholdTrackbarName, windowName, &accumulatorThreshold, maxAccumulatorThreshold);
    createTrackbar("hmax_y", "src_gray", &hmax_y, 255);
    createTrackbar("hmin_y", "src_gray", &hmin_y, 255);
    createTrackbar("smax_y", "src_gray", &smax_y, 255);
    createTrackbar("smin_y", "src_gray", &smin_y, 255);
    createTrackbar("vmax_y", "src_gray", &vmax_y, 255);
    createTrackbar("vmin_y", "src_gray", &vmin_y, 255);
    createTrackbar("hmax_r", "noise_reduce", &hmax_r, 255);
    createTrackbar("hmin_r", "noise_reduce", &hmin_r, 255);
    createTrackbar("smax_r", "noise_reduce", &vmax_r, 255);
    createTrackbar("smin_r", "noise_reduce", &vmin_r, 255);
    createTrackbar("vmax_r", "noise_reduce", &smax_r, 255);
    createTrackbar("vmin_r", "noise_reduce", &smin_r, 255);
    createTrackbar("dilation", "noise_reduce", &dil, 10);
    createTrackbar("erosion", "noise_reduce", &ero, 10);
    // infinite loop to display
    // and refresh the content of the output image
    // until the user presses q or Q
   for (; vid.get(CV_CAP_PROP_POS_FRAMES) <vid.get(CV_CAP_PROP_FRAME_COUNT);)
   {  
        vid >> src;
        
        int key = 0 ;
        while(key != 'q' && key != 'Q')
        {   cvtColor(src,hsv_img, CV_BGR2HSV);
        
            src_gray = rgb_detect(hsv_img,hmax_y,hmin_y,smax_y,smin_y,vmax_y,vmin_y,hmax_r,hmin_r,smax_r,smin_r,vmax_r,vmin_r);
            
            // Reduce the noise so we avoid false circle detectio
            medianBlur(src_gray, src_gray, 3);
            GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );
            src_gray=erosion(src_gray,ero);
            src_gray=dilation(src_gray,dil);
        

            // those paramaters cannot be =0
            // so we must check here
            cannyThreshold = std::max(cannyThreshold, 1);
            accumulatorThreshold = std::max(accumulatorThreshold, 1);

            //runs the detection, and update the display
            HoughDetection(src_gray, src, cannyThreshold, accumulatorThreshold);
            
            imshow("src_gray",src_gray);
            
             key = waitKey(10);
        }    
        
            //only to pause at frame 1
            int k = 0;
            cout << "give value of k";
            cin >>  k ;
           /* if (k !=1)
            {     
                return 0;
            }*/
            // get user key
            int iKey = waitKey(10);
		if (iKey == 27)break;
          
    }

    return 0; 
}
