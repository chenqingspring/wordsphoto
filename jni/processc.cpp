#include "processc.h"
#include <jni.h>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

using namespace cv;
jboolean wordPhotoProcess(JNIEnv *env, jobject obj, jobject matrixList){

	jintArray JcolorImgbuf;
	int w;
	int h;
	jintArray JfontImgbuf;
	int fontW;
	int fontH;

	//class ArrayList
	jclass cls_arraylist = env->GetObjectClass(matrixList);
	//method in class ArrayList
	jmethodID arraylist_get = env->GetMethodID(cls_arraylist, "get",
			"(I)Ljava/lang/Object;");

	jmethodID arraylist_size = env->GetMethodID(cls_arraylist, "size", "()I");

	jint len = env->CallIntMethod(matrixList, arraylist_size);

	//for(int i=0;i<len;i++){
	jstring jInputImagePath = (jstring)env->CallObjectMethod(matrixList, arraylist_get, 0);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "%s, jInputImagePath==%s",__FUNCTION__,jInputImagePath);
	jboolean isCopy;
	const char *inputImagePath = env->GetStringUTFChars(jInputImagePath, false);


	Mat colorImg = imread(inputImagePath);

	int zoomlever = 2;

	Mat colorZoomImg(colorImg.cols * zoomlever, colorImg.rows * zoomlever, CV_8UC4);

	resize(colorImg, colorZoomImg,
			Size(round(colorZoomImg.cols), round(colorZoomImg.rows)), 0, 0,
			INTER_NEAREST);

	//colorZoomImg = colorImg.clone();

	Mat grayImg = colorZoomImg.clone();
	//Mat fourChannelsGrayImg = colorImg.clone();

	cvtColor(grayImg, grayImg, COLOR_BGRA2GRAY, 4);

	for (int n = 0; n < grayImg.rows; n++)
		for (int m = 0; m < grayImg.cols; m++) {
			int temp = (grayImg.data + grayImg.step * n)[m * grayImg.channels()];

			if (temp >= 0 && temp < 128) {

				(grayImg.data + grayImg.step * n)[m * grayImg.channels()] = 0;

			}

			if (temp >= 128 && temp <= 255) {

				(grayImg.data + grayImg.step * n)[m * grayImg.channels()] = 100;

			}
			/*if (temp>=180&&temp<=255) {

			 (grayImg.data + grayImg.step * n)[m * grayImg.channels()]  = 180;

			 }
			 /*if (temp>=150&&temp<200) {

			 (grayImg.data + grayImg.step * n)[m * grayImg.channels()] = 150;

			 }
			 if (temp>=200&&temp<255) {

			 (grayImg.data + grayImg.step * n)[m * grayImg.channels()] = 200;

			 }*/

		}

	Canny(grayImg, grayImg, 80, 90, 7);

	int fontNumber = 7;

	Mat *fontGrayList = new Mat[(len - 1) * fontNumber];

	Mat *fontMaskList = new Mat[(len - 1) * fontNumber];

	jint *fontImgbuf;

	for (int i = 1; i < len; i++) {

		jstring jFontsPath = (jstring)env->CallObjectMethod(matrixList, arraylist_get, i);

		const char *fontsPath = env->GetStringUTFChars(jFontsPath, false);

		Mat fontGray = imread(fontsPath);
		cvtColor(fontGray, fontGray, CV_RGB2GRAY);

		Mat fontMask = fontGray.clone();

		cvtColor(fontGray, fontGray, COLOR_BGRA2GRAY, 4);

		for (int n = 0; n < fontGray.rows; n++)
			for (int m = 0; m < fontGray.cols; m++) {

				(fontGray.data + fontGray.step * n)[m * fontGray.channels()] =
						255
								- (fontGray.data + fontGray.step * n)[m
										* fontGray.channels()];

			}
		//adaptiveThreshold(fontGray, fontMask, 10, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 0);

		threshold(fontGray, fontMask, 10, 90, CV_THRESH_BINARY);

		Mat fontGraytemp = fontGray.clone();

		Mat fontMasktemp = fontGray.clone();

		fontGrayList[(i - 1) * fontNumber] = fontGray.clone();
		fontMaskList[(i - 1) * fontNumber] = fontMask.clone();

		double zoomrate = 0.7;

		for (int n = 1; n < fontNumber; n++) {

			resize(fontGraytemp, fontGraytemp,
					Size(round(zoomrate * fontGraytemp.cols),
							round(zoomrate * fontGraytemp.rows)), 0, 0,
					INTER_NEAREST);

			threshold(fontGraytemp, fontMasktemp, 10, 90, CV_THRESH_BINARY);

			fontGrayList[(i - 1) * fontNumber + n] = fontGraytemp.clone();
			fontMaskList[(i - 1) * fontNumber + n] = fontMasktemp.clone();

			if (n == 2)
				zoomrate = 0.8;
			if (n == 3)
				zoomrate = 0.7;
			if (n == 4)
				zoomrate = 0.5;
			if (n == 5)
				zoomrate = 0.5;
			if (n == 6)
				zoomrate = 0.4;

		}

		fontGraytemp.release();
		fontMasktemp.release();
		fontGray.release();
		fontMask.release();

	}

	//jint *grayImgbuf;
	//jint *fourChannelsGraybuf;

	//grayImgbuf = env->GetIntArrayElements(JcolorImgbuf, false);
	//fourChannelsGraybuf = env->GetIntArrayElements(JcolorImgbuf, false);

	for (int i = 0; i < colorZoomImg.rows; i++)
		for (int j = 0; j < colorZoomImg.cols; j++) {
			int temp_R = (colorZoomImg.data + colorZoomImg.step * i)[j
					* colorZoomImg.channels() + 0];
			int temp_G = (colorZoomImg.data + colorZoomImg.step * i)[j
					* colorZoomImg.channels() + 1];
			int temp_B = (colorZoomImg.data + colorZoomImg.step * i)[j
					* colorZoomImg.channels() + 2];

			double val_R;
			double val_G;
			double val_B;

			val_R = 128 + (temp_R - 128) * 1.5;

			if (val_R > 255) {
				val_R = 255;
			}
			if (val_R < 0) {
				val_R = 0;
			}

			(colorZoomImg.data + colorZoomImg.step * i)[j
					* colorZoomImg.channels() + 0] = val_R;

			val_G = 128 + (temp_G - 128) * 1.5;

			if (val_G > 255) {
				val_G = 255;
			}
			if (val_G < 0) {
				val_G = 0;
			}

			(colorZoomImg.data + colorZoomImg.step * i)[j
					* colorZoomImg.channels() + 1] = val_G;

			val_B = 128 + (temp_B - 128) * 1.5;

			if (val_B > 255) {
				val_B = 255;
			}
			if (val_B < 0) {
				val_B = 0;
			}

			(colorZoomImg.data + colorZoomImg.step * i)[j
					* colorZoomImg.channels() + 2] = val_B;

		}

	/*Mat colorImg_larger = colorImg.clone();

	 resize(colorImg_larger,colorImg_larger, Size(round(3*colorImg.cols), round(3*colorImg.rows)), 0, 0, INTER_LINEAR );

	 Mat blackBackGround = colorImg_larger.clone();

	 Mat cannyImg = colorImg_larger.clone();

	 //cvtColor(cannyImg , cannyImg,COLOR_BGRA2GRAY, 4);

	 Canny(cannyImg, cannyImg, 80, 90, 3);*/

	//Mat blackBackGround = colorZoomImg.clone();
	Mat blackBackGround(colorZoomImg.size(),
			CV_MAKETYPE(colorZoomImg.depth(), 4));

	for (int n = 0; n < blackBackGround.rows; n++)
		for (int m = 0; m < blackBackGround.cols; m++) {

			(blackBackGround.data + blackBackGround.step * n)[m
					* blackBackGround.channels() + 0] = 0.6
					* (colorZoomImg.data + colorZoomImg.step * n)[m
							* colorZoomImg.channels() + 0];
			(blackBackGround.data + blackBackGround.step * n)[m
					* blackBackGround.channels() + 1] = 0.6
					* (colorZoomImg.data + colorZoomImg.step * n)[m
							* colorZoomImg.channels() + 1];
			(blackBackGround.data + blackBackGround.step * n)[m
					* blackBackGround.channels() + 2] = 0.6
					* (colorZoomImg.data + colorZoomImg.step * n)[m
							* colorZoomImg.channels() + 2];

		}

	double bufferRed, bufferGreen, bufferBlue;
	double sumbufferRed, sumbufferGreen, sumbufferBlue;
	double averR, averG, averB;
	double bufferFont;

	//Mat fontGray = fontGrayList[10].clone();
	// Mat fontMask = fontMaskList[10].clone();

	srand((unsigned) time( NULL));

	int a;
	int caseNumber;
	int step_length;

	for (int k = 1; k < fontNumber; k++) {

		if (1 == k) {

			a = rand() % (len - 1);
			caseNumber = fontNumber * a;
			step_length = 15;

		}

		if (2 == k) {
			a = rand() % (len - 1);

			caseNumber = fontNumber * a + 1;

			step_length = 13;

		}
		if (3 == k) {
			a = rand() % (len - 1);

			caseNumber = fontNumber * a + 2;

			step_length = 10;

		}
		if (4 == k) {
			a = rand() % (len - 1);

			caseNumber = fontNumber * a + 3;

			step_length = 8;

		}
		if (5 == k) {
			a = rand() % (len - 1);

			caseNumber = fontNumber * a + 4;

			step_length = 5;

		}
		if (6 == k) {
			a = rand() % (len - 1);

			caseNumber = fontNumber * a + 5;

			step_length = 3;

		}

		for (int i = 0; i < grayImg.rows - fontGrayList[caseNumber].rows;
				i = i + step_length)

			for (int j = 0; j < grayImg.cols - fontGrayList[caseNumber].cols;
					j = j + step_length) {

				int fontWidth = fontGrayList[caseNumber].cols;
				int fontHeight = fontGrayList[caseNumber].rows;
				int totalArea = fontWidth * fontHeight;

				int grayValue = 0;
				int area = 0;
				//countNum = 0;

				sumbufferRed = 0;
				sumbufferGreen = 0;
				sumbufferBlue = 0;

				for (int n = i; n < fontHeight + i; n++)

					for (int m = j; m < fontWidth + j; m++)

					{

						grayValue = (grayImg.data + grayImg.step * n)[m
								* grayImg.channels()];

						//                  if (grayValue == 255 || grayValue == -1) {
						//                      NSLog( @"grayValue=%d", grayValue );
						//                  }
						//

						if (grayValue == 0) {

							bufferRed = (colorZoomImg.data
									+ colorZoomImg.step * n)[m
									* colorZoomImg.channels() + 0];
							bufferGreen = (colorZoomImg.data
									+ colorZoomImg.step * n)[m
									* colorZoomImg.channels() + 1];
							bufferBlue = (colorZoomImg.data
									+ colorZoomImg.step * n)[m
									* colorZoomImg.channels() + 2];

							sumbufferRed += bufferRed;
							sumbufferGreen += bufferGreen;
							sumbufferBlue += bufferBlue;

							area = area + 1;

						} else {
							//countNum++;
							//if (countNum>30) {

							m = fontWidth + j;
							n = fontHeight + i;
							//}

						}
					}

				if (area == totalArea) {

					averR = sumbufferRed / area;
					averG = sumbufferGreen / area;
					averB = sumbufferBlue / area;

					for (int n = i; n < fontHeight + i; n++)
						for (int m = j; m < fontWidth + j; m++) {

							(grayImg.data + grayImg.step * n)[m
									* grayImg.channels()] =
									(fontMaskList[caseNumber].data
											+ fontMaskList[caseNumber].step
													* (n - i))[(m - j)
											* fontMaskList[caseNumber].channels()];

							//((uchar*)(img2->imageData + img2->widthStep*n))[m]= 90;

							bufferFont =
									(fontGrayList[caseNumber].data
											+ fontGrayList[caseNumber].step
													* (n - i))[(m - j)
											* fontGrayList[caseNumber].channels()];

							if ((fontMaskList[caseNumber].data
									+ fontMaskList[caseNumber].step * (n - i))[(m
									- j) * fontMaskList[caseNumber].channels()]
									== 90) {

								(blackBackGround.data + blackBackGround.step * n)[m
										* blackBackGround.channels() + 0] =
										(int) averR * bufferFont / 255;
								(blackBackGround.data + blackBackGround.step * n)[m
										* blackBackGround.channels() + 1] =
										(int) averG * bufferFont / 255;
								(blackBackGround.data + blackBackGround.step * n)[m
										* blackBackGround.channels() + 2] =
										(int) averB * bufferFont / 255;

							}

						}
				}

				if (1 == k) {
					a = rand() % (len - 1);
					caseNumber = fontNumber * a;

				}

				if (2 == k) {
					a = rand() % (len - 1);
					caseNumber = fontNumber * a + 1;

				}
				if (3 == k) {
					a = rand() % (len - 1);
					caseNumber = fontNumber * a + 2;

				}
				if (4 == k) {
					a = rand() % (len - 1);
					caseNumber = fontNumber * a + 3;

				}
				if (5 == k) {
					a = rand() % (len - 1);
					caseNumber = fontNumber * a + 4;

				}
				if (6 == k) {
					a = rand() % (len - 1);
					caseNumber = fontNumber * a + 5;

				}

			}

	}

	a = rand() % (len - 1);
	caseNumber = fontNumber * a + 6;

	for (int i = 0; i < grayImg.rows - fontGrayList[caseNumber].rows; i = i + 2)

		for (int j = 0; j < grayImg.cols - fontGrayList[caseNumber].cols;
				j = j + 2) {

			int fontWidth = fontGrayList[caseNumber].cols;
			int fontHeight = fontGrayList[caseNumber].rows;
			int totalArea = fontWidth * fontHeight;

			int grayValue = 0;
			int area = 0;
			//countNum = 0;

			sumbufferRed = 0;
			sumbufferGreen = 0;
			sumbufferBlue = 0;

			for (int n = i; n < fontHeight + i; n++)

				for (int m = j; m < fontWidth + j; m++)

				{

					grayValue = (grayImg.data + grayImg.step * n)[m
							* grayImg.channels()];

					//                  if (grayValue == 255 || grayValue == -1) {
					//                      NSLog( @"grayValue=%d", grayValue );
					//                  }
					//

					if (grayValue == 0 || grayValue == 255) {

						bufferRed =
								(colorZoomImg.data + colorZoomImg.step * n)[m
										* colorZoomImg.channels() + 0];
						bufferGreen =
								(colorZoomImg.data + colorZoomImg.step * n)[m
										* colorZoomImg.channels() + 1];
						bufferBlue =
								(colorZoomImg.data + colorZoomImg.step * n)[m
										* colorZoomImg.channels() + 2];

						sumbufferRed += bufferRed;
						sumbufferGreen += bufferGreen;
						sumbufferBlue += bufferBlue;

						area = area + 1;

					} else {
						//countNum++;
						//if (countNum>30) {

						m = fontWidth + j;
						n = fontHeight + i;
						//}

					}
				}

			if (area == totalArea) {

				averR = sumbufferRed / area;
				averG = sumbufferGreen / area;
				averB = sumbufferBlue / area;

				for (int n = i; n < fontHeight + i; n++)
					for (int m = j; m < fontWidth + j; m++) {

						(grayImg.data + grayImg.step * n)[m * grayImg.channels()] =
								(fontMaskList[caseNumber].data
										+ fontMaskList[caseNumber].step
												* (n - i))[(m - j)
										* fontMaskList[caseNumber].channels()];

						//((uchar*)(img2->imageData + img2->widthStep*n))[m]= 90;

						bufferFont = (fontGrayList[caseNumber].data
								+ fontGrayList[caseNumber].step * (n - i))[(m
								- j) * fontGrayList[caseNumber].channels()];

						if ((fontMaskList[caseNumber].data
								+ fontMaskList[caseNumber].step * (n - i))[(m
								- j) * fontMaskList[caseNumber].channels()]
								== 90) {

							(blackBackGround.data + blackBackGround.step * n)[m
									* blackBackGround.channels() + 0] =
									(int) averR * bufferFont / 255;
							(blackBackGround.data + blackBackGround.step * n)[m
									* blackBackGround.channels() + 1] =
									(int) averG * bufferFont / 255;
							(blackBackGround.data + blackBackGround.step * n)[m
									* blackBackGround.channels() + 2] =
									(int) averB * bufferFont / 255;

						}

					}
			}
			a = rand() % (len - 1);
			caseNumber = fontNumber * a + 6;

		}

	for (int i = 0; i < colorZoomImg.rows; i++) {
		for (int j = 0; j < colorZoomImg.cols; j++) {
			(colorZoomImg.data + colorZoomImg.step * i)[j
					* colorZoomImg.channels() + 0] = (int) (blackBackGround.data
					+ blackBackGround.step * i)[j * blackBackGround.channels()
					+ 0];

			(colorZoomImg.data + colorZoomImg.step * i)[j
					* colorZoomImg.channels() + 1] = (int) (blackBackGround.data
					+ blackBackGround.step * i)[j * blackBackGround.channels()
					+ 1];

			(colorZoomImg.data + colorZoomImg.step * i)[j
					* colorZoomImg.channels() + 2] = (int) (blackBackGround.data
					+ blackBackGround.step * i)[j * blackBackGround.channels()
					+ 2];

		}
	}

	blackBackGround.release();
	grayImg.release();
	colorImg.release();

	delete[] fontGrayList;          //http://www.chinaunix.net/jh/23/311058.html
	delete[] fontMaskList;


	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "%s, JcolorZoomImgbuf==%d",__FUNCTION__,time(0));
		bool isWriteSuccess = imwrite("/sdcard/test.jpg",colorZoomImg);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "%s, JcolorZoomImgbuf==%d",__FUNCTION__,time(0));



	env->ReleaseIntArrayElements(JfontImgbuf, fontImgbuf, 0);

	return isWriteSuccess;

}
