#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


void show_histogram(string const& name, Mat1b const& image)
{
	//Set histogram bins count
	int bins = 256;
	int histSize[] = { bins };
	//Set ranges for histogram bins
	float lranges[] = { 0, 256 };
	const float* ranges[] = { lranges };
	//Create matrix for histogram
	Mat hist;
	int channels[] = { 0 };

	//Create matrix for histogram visualization
	int const hist_height = 256;
	Mat3b hist_image = Mat3b::zeros(hist_height, bins);

	calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);

	double max_val = 0;
	minMaxLoc(hist, 0, &max_val);

	//Visualize each bin
	for (int b = 0; b < bins; b++) {
		float const binVal = hist.at<float>(b);
		int   const height = cvRound(binVal * hist_height / max_val);
		line
		(hist_image
			, Point(b, hist_height - height), Point(b, hist_height)
			, Scalar::all(255)
		);
	}
	imshow(name, hist_image);
}

int main()
{
	Mat frame;
	VideoCapture capture("barriers.avi");
	VideoWriter output("barriers_new.avi", CV_FOURCC('M', 'J', 'P', 'G'), 50, Size(640, 480));
	bool preview = true;

	namedWindow("Video", WINDOW_AUTOSIZE);
	namedWindow("Histogram", WINDOW_AUTOSIZE);

	int brightnessTrackbar = 50;
	createTrackbar("Brightness", "Video", &brightnessTrackbar, 100);
	int contrastTrackbar = 50;
	createTrackbar("Contrast", "Video", &contrastTrackbar, 100);
	
	//Play video
	for (;;)
	{
		capture.read(frame);
		if (frame.empty())
		{
			if (preview)
			{
				capture.set(CV_CAP_PROP_POS_FRAMES, 0); //If no more frames, reset to frame 0
				capture.read(frame);
			}
			else
			{
				cout << "Save complete." << endl;
				break;
			}
		}

		if (brightnessTrackbar != 50 || contrastTrackbar != 50)
		{
			float adjustBrightness = (brightnessTrackbar - 50) * 2;
			float adjustContrast = contrastTrackbar - 50;

			adjustContrast /= 50;
			adjustContrast += 1;

			//Shift brightness values so histogram is centered
			adjustBrightness -= (256 * adjustContrast) / 2 - 128;

			//Multiply pixels by contrast and add brightness
			frame.convertTo(frame, -1, adjustContrast, adjustBrightness);
		}
		
		if (preview)
		{
			putText(frame, "Press 'S' key to save", Point(5, 20), FONT_HERSHEY_PLAIN, 1.0, Scalar(255, 255, 255));
			imshow("Video", frame);
			show_histogram("Histogram", frame);

			char key = (char)waitKey(10);
			if (key == 's' || key == 'S')
			{
				cout << "Saving video..." << endl;
				capture.set(CV_CAP_PROP_POS_FRAMES, 0);
				preview = false;
			}
			else if (key > 0)
				break;
		}
		else
			output.write(frame);
	}
	capture.release();
	output.release();
	return 0;
}