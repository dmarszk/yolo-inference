#pragma once


#include "yolov5.h"


//LetterBox����
void LetterBox(cv::Mat& image, cv::Mat& outImage, cv::Size& newShape = cv::Size(640, 640), cv::Scalar& color = cv::Scalar(114, 114, 114));

//NMS
void nms(std::vector<cv::Rect>& boxes, std::vector<float>& scores, float score_threshold, float nms_threshold, std::vector<int>& indices);

//box���ŵ�ԭͼ�ߴ�
void scale_boxes(cv::Rect& box, cv::Size size);

//���ӻ�����
void draw_result(cv::Mat& image, std::string label, cv::Rect box);