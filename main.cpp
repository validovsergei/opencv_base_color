#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>

void multiplication(int n, void * im)
{
      cv::Mat * image = static_cast<cv::Mat*>(im);
      cv::Mat new_image = cv::Mat::zeros(image->size(), image->type());
      for( int y = 0; y < image->rows; y++ ) 
      {
            for( int x = 0; x < image->cols; x++ ) 
            {
                  for( int c = 0; c < image->channels(); c++ ) 
                  {
                        new_image.at<cv::Vec3b>(y,x)[c] =
                        cv::saturate_cast<uchar>( image->at<cv::Vec3b>(y,x)[c] * n);
                  }
            }
      }
      cv::imshow("Change image brighntness", new_image);
}

void additional(int n, void * im)
{
      cv::Mat * image = static_cast<cv::Mat*>(im);
      cv::Mat  new_image = cv::Mat::zeros( image->size(), image->type());
      for( int y = 0; y < image->rows; y++ ) 
      {
            for( int x = 0; x < image->cols; x++ ) 
            {
                  for( int c = 0; c < image->channels(); c++ ) 
                  {
                        new_image.at<cv::Vec3b>(y,x)[c] =
                        cv::saturate_cast<uchar>( image->at<cv::Vec3b>(y,x)[c] + n);
                  }
            }
      }
      cv::imshow("Change image brighntness", new_image);
}

void Calculation_hist(const cv::Mat* images, cv::Mat& hist, int hist_w, int hist_h, cv::Mat& hist_image)
{
      int histSize = 256;
      float range[] = {0,256};
      const float * histRange[] = {range};
      cv::calcHist(images, 1, 0, cv::Mat(), hist, 1, &histSize, histRange, true, false );
      cv::normalize(hist, hist, 0, hist_image.rows, cv::NORM_MINMAX, -1, cv::Mat());
}

void draw_hist(const std::vector<std::pair<cv::Mat, std::pair<cv::Mat, cv::Scalar>>>&  windowhist, int hist_h, int hist_w)
{
      int histSize = 256;
      int bin_w = cvRound( (double) hist_w/histSize );
      for( int i = 1; i < histSize; i++ )
      {
            for(auto whc = windowhist.begin(); whc != windowhist.end(); ++whc)
            {
                  cv::line(whc->first, cv::Point(bin_w*(i-1), hist_h - cvRound(whc->second.first.at<float>(i-1)) ),
                  cv::Point(bin_w * (i), hist_h - cvRound(whc->second.first.at<float>(i)) ),
                  whc->second.second, 2, 8, 0 );
            }
      }
}

void draw_histog(cv::Mat& histimage, cv::Mat& hist, int hist_h, int hist_w, cv::Scalar sc)
{
      int histSize = 256;
      int bin_w = cvRound( (double) hist_w/histSize );
      for( int i = 1; i < histSize; i++ ) {
      cv::line( histimage, cv::Point(bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1))),
      cv::Point(bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ), sc, 2, 8, 0);
}
}


int main(int argc, char **argv)
{
      int regime = 1;

      if(regime == 1)
      { 
            std::string path = "video.mp4";
            cv::VideoCapture cap(path); 
            cv::Mat src_image;
            bool check = cap.read(src_image);
            while(check)
            {
                  std::vector<cv::Mat> bgr;
                  cv::split(src_image, bgr);
                  cv::Mat b_hist;
                  cv::Mat g_hist;
                  cv::Mat r_hist;
                  cv::Mat gray_hist;
                  cv::Mat src_gray;
                  cv::cvtColor(src_image, src_gray, cv::COLOR_BGR2GRAY);
                  int hist_w = 512, hist_h = 400;
                  cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0));
                  cv::Mat histGrayImage(hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0));
                  Calculation_hist(&src_gray, gray_hist, hist_w, hist_h, histGrayImage);
                  Calculation_hist(&bgr[0], b_hist, hist_w, hist_h, histImage);
                  Calculation_hist(&bgr[1], g_hist, hist_w, hist_h, histImage);
                  Calculation_hist(&bgr[2], r_hist, hist_w, hist_h, histImage);
                  std::vector<std::pair<cv::Mat, std::pair<cv::Mat, cv::Scalar>>> winhist;
                  winhist.push_back(std::make_pair(histImage, std::make_pair(b_hist, cv::Scalar(255, 0, 0))));
                  winhist.push_back(std::make_pair(histImage, std::make_pair(g_hist, cv::Scalar( 0, 255, 0))));
                  winhist.push_back(std::make_pair(histImage, std::make_pair(r_hist, cv::Scalar( 0, 0, 255))));
                  winhist.push_back(std::make_pair(histGrayImage, std::make_pair(gray_hist, cv::Scalar(255, 255, 255))));
                  draw_hist(winhist, hist_h, hist_w);
                  cv::imshow("Source image", src_image);
                  cv::imshow("Histogramm for processed image", histImage);
                  cv::imshow("Gray Histogram for processed image", histGrayImage);
                  check = cap.read(src_image);
                  cv::waitKey(1000);
            }
      }
      else if (regime == 2)
      {
            std::string path;
            std::cout << "Input path to picture file\n";
            std::cin >> path;
            cv::Mat src_image = cv::imread(path);
            cv::namedWindow("Change image brighntness", cv::WINDOW_AUTOSIZE);
            int st_pos = 1;
            cv::createTrackbar("Multiplication coefficient", "Change image brighntness", &st_pos, 10, multiplication, &src_image);
            cv::createTrackbar("Addition coefficient", "Change image brighntness", 0, 255, additional, &src_image);
            cv::imshow("Change image brighntness", src_image);
            cv::waitKey();
      }
      else
      {
            break;
      }
}