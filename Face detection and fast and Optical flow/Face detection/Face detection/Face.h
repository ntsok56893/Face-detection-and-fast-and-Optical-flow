#pragma once

#include <iostream>
#include <string>
#include <ctime>

#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/tracking.hpp>

using namespace std;
using namespace cv;

namespace Face_detection {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btn_detection;
	protected:

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->btn_detection = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// btn_detection
			// 
			this->btn_detection->Location = System::Drawing::Point(99, 194);
			this->btn_detection->Name = L"btn_detection";
			this->btn_detection->Size = System::Drawing::Size(75, 23);
			this->btn_detection->TabIndex = 0;
			this->btn_detection->Text = L"Detection";
			this->btn_detection->UseVisualStyleBackColor = true;
			this->btn_detection->Click += gcnew System::EventHandler(this, &MyForm::btn_detection_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 261);
			this->Controls->Add(this->btn_detection);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void btn_detection_Click(System::Object^  sender, System::EventArgs^  e) {

		Mat frame;
		Mat grayFrame;
		Mat faceM;
		Mat preFace;
		char svfileName[10000];

		VideoCapture capture(0);
		string cascadeName = "haarcascade_frontalface_alt2.xml";
		CascadeClassifier face_cascade;

		// vector of keyPoints
		std::vector<KeyPoint> keyPoints;
		std::vector<Point2f> curkeyPoints;
		std::vector<Point2f> prekeyPoints;

		Boolean contr = false;

		vector<uchar> status;
		vector<float> err;

		// construction of the fast feature detector object
		Ptr<FeatureDetector> fast = FastFeatureDetector::create(40);

		if (!capture.isOpened()) {
			cout << "Error loading video" << endl;
			return;
		}
		if (!face_cascade.load(cascadeName)) {
			cout << "Error loading cascade file" << endl;
		}

		clock_t start = clock();
		int frame_cout = 0;
		int frameCount = 0;

		//選定視訊裝置，0為自動偵測
		//capture = cvCaptureFromCAM(0);
		while (true) {
			//擷取視訊
			capture.read(frame);
			//轉換灰階
			grayFrame = Mat(frame.size(), CV_8U);
			cvtColor(frame, grayFrame, CV_BGR2GRAY);

			vector<Rect> face;
			face_cascade.detectMultiScale(grayFrame, face, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(10, 10));
			
			if (frame_cout >= 15) {

				if (face.empty()) {
					contr = false;
				}
				else {
					contr = true;
				}

				for (size_t i = 0; i < face.size() && i <1; i++) {
					cv::Point pt1(face[i].x + face[i].width, face[i].y + face[i].height);
					cv::Point pt2(face[i].x, face[i].y);
					faceM = frame(Rect(face[i].x, face[i].y, face[i].width, face[i].height));

					// feature point detection
					fast->detect(faceM, keyPoints);

					if(!keyPoints.empty())
						curkeyPoints.clear();

					for (int i = 0; i < keyPoints.size(); i++) {
						curkeyPoints.push_back(keyPoints[i].pt);
					}

					for (int i = 0; i < curkeyPoints.size(); i++) {
						curkeyPoints[i].x = curkeyPoints[i].x + face[0].x;
						curkeyPoints[i].y = curkeyPoints[i].y + face[0].y;
					}

					
					preFace = frame.clone();
					prekeyPoints.clear();
					for (int i = 0; i < curkeyPoints.size(); i++) {
						prekeyPoints.push_back(curkeyPoints[i]);
					}
					
					drawKeypoints(faceM, keyPoints, faceM, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);

					for (int i = 0; i < prekeyPoints.size(); i++) {
						circle(frame, prekeyPoints[i], 5, CV_RGB(0, 255, 255), 1);
					}
					frame_cout = 0;
				}
			}

			if (!preFace.empty() && !curkeyPoints.empty() && contr) {
				calcOpticalFlowPyrLK(preFace, frame, prekeyPoints, curkeyPoints, status, err);

				circle(frame, curkeyPoints[curkeyPoints.size()/2], sqrtf(powf((curkeyPoints[curkeyPoints.size()-1].x - curkeyPoints[0].x), 2) + powf((curkeyPoints[curkeyPoints.size()-1].y - curkeyPoints[0].y), 2)), CV_RGB(0, 255, 0), 5);

				for (int i = 0; i < curkeyPoints.size(); i++) {
					circle(frame, curkeyPoints[i], 5, CV_RGB(0, 255, 0), 1);
				}
				for (int i = 0; i < curkeyPoints.size(); i++) {
					line(frame, prekeyPoints[i], curkeyPoints[i], CV_RGB(255, 0, 0), 1);
				}
			}
			
			frame_cout++;

			imshow("window", frame);

			if (waitKey(33) >= 0)
			{
				break;
			}
		}
		capture.release();
	}
	};
}
