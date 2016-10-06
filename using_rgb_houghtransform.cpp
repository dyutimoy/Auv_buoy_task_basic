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
Mat rgb_detect(Mat img, int _rmax_y, int _rmin_y, int _gmax_y, int _gmin_y, int _bmax_y, int _bmin_y, int _rmax_r, int _rmin_r, int _gmax_r, int _gmin_r, int _bmax_r, int _bmin_r)
{

	//Mat rgb(img.rows, img.cols, CV_8UC1, Scalar(255));
	

        Mat yellow;
        Mat red;
	inRange(img, Scalar(_bmin_y,_gmin_y, _rmin_y), Scalar(_bmax_y,_gmax_y,_rmax_y), yellow);
	inRange(img, Scalar(_bmin_r,_gmin_r, _rmin_r), Scalar(_bmax_r,_gmax_r,_rmax_r), red);

	// Combine the above two images
	Mat output;
	addWeighted(yellow, 0.25, red, 0.75, 0.0, output);

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
    int rmax_y=0,rmin_y=0,gmax_y=0,gmin_y=0,bmax_y=0,bmin_y=0,rmax_r=0,rmin_r=0,gmax_r=0,gmin_r=0,bmax_r=0,bmin_r=0,dil = 0,ero = 0;
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
    createTrackbar("rmax_y", "src_gray", &rmax_y, 255);
    createTrackbar("rmin_y", "src_gray", &rmin_y, 255);
    createTrackbar("gmax_y", "src_gray", &gmax_y, 255);
    createTrackbar("gmin_y", "src_gray", &gmin_y, 255);
    createTrackbar("bmax_y", "src_gray", &bmax_y, 255);
    createTrackbar("bmin_y", "src_gray", &bmin_y, 255);
    createTrackbar("rmax_r", "noise_reduce", &rmax_r, 255);
    createTrackbar("rmin_r", "noise_reduce", &rmin_r, 255);
    createTrackbar("gmax_r", "noise_reduce", &gmax_r, 255);
    createTrackbar("gmin_r", "noise_reduce", &gmin_r, 255);
    createTrackbar("bmax_r", "noise_reduce", &bmax_r, 255);
    createTrackbar("bmin_r", "noise_reduce", &bmin_r, 255);
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
        {   hsv_img = src.clone();
        
            src_gray = rgb_detect(hsv_img,rmax_y,rmin_y,gmax_y,gmin_y,bmax_y,bmin_y,rmax_r,rmin_r,gmax_r,gmin_r,bmax_r,bmin_r);
            
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
