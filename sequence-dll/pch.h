// pch.h: это предварительно скомпилированный заголовочный файл.
// Перечисленные ниже файлы компилируются только один раз, что ускоряет последующие сборки.
// Это также влияет на работу IntelliSense, включая многие функции просмотра и завершения кода.
// Однако изменение любого из приведенных здесь файлов между операциями сборки приведет к повторной компиляции всех(!) этих файлов.
// Не добавляйте сюда файлы, которые планируете часто изменять, так как в этом случае выигрыша в производительности не будет.

// pch.h: это предварительно скомпилированный заголовочный файл.
// Перечисленные ниже файлы компилируются только один раз, что ускоряет последующие сборки.
// Это также влияет на работу IntelliSense, включая многие функции просмотра и завершения кода.
// Однако изменение любого из приведенных здесь файлов между операциями сборки приведет к повторной компиляции всех(!) этих файлов.
// Не добавляйте сюда файлы, которые планируете часто изменять, так как в этом случае выигрыша в производительности не будет.

#ifndef PCH_H
#define PCH_H

// Добавьте сюда заголовочные файлы для предварительной компиляции
#include "framework.h"
#include <iostream>
#include <omp.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <vector>

struct Picture {
	std::vector<std::vector<double>> ptrDst;
	std::vector<std::vector<double>> ptrDstCopy;
	int cols;
	int rows;
};

Picture GetPicture(std::string path)
{
	cv::Mat dst;
	cv::Mat img_grayscale = cv::imread(path, 0);
	cv::Mat binaryMat(img_grayscale.size(), img_grayscale.type());
	cv::threshold(img_grayscale, binaryMat, 100, 255, cv::THRESH_BINARY);


	binaryMat.convertTo(dst, CV_64F, 1, 0);

	Picture picture;


	picture.ptrDst = std::vector<std::vector<double>>(dst.rows);
	picture.ptrDstCopy = std::vector<std::vector<double>>(dst.rows);

	for (int i = 0; i < dst.rows; i++)
	{
		dst.row(i).reshape(1, 1).copyTo(picture.ptrDst[i]);
		dst.row(i).reshape(1, 1).copyTo(picture.ptrDstCopy[i]);
	}

	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			picture.ptrDst[i][j] = (picture.ptrDst[i][j] == 255 ? 1 : 0);
			picture.ptrDstCopy[i][j] = (picture.ptrDstCopy[i][j] == 255 ? 1 : 0);
		}
	}

	picture.cols = binaryMat.cols;
	picture.rows = binaryMat.rows;

	return picture;
}

extern "C"
__declspec(dllexport)
int Cols(char* path) {
	auto picture = GetPicture(path);

	return picture.cols;
}

extern "C"
__declspec(dllexport)
int Rows(char* path) {
	auto picture = GetPicture(path);

	return picture.rows;
}

int* ToResult(std::vector<std::vector<double>> input, int rows, int cols)
{
	int* out = new int[cols * rows + 1];

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			out[i * cols + j] = input[i][j];
		}
	}

	return out;
}

extern "C"
__declspec(dllexport)
int* Dilatation(int kernel, char* path)
{
	int WHITE = 1;
	int BLACK = 0;
	auto begin = std::chrono::steady_clock::now();
	Picture picture = GetPicture(path);

	for (int i = 0; i < picture.rows; i++) {
		for (int j = 0; j < picture.cols; j++) {

			if (picture.ptrDst[i][j] == BLACK) {

				for (int k = i - kernel / 2; k < i + kernel / 2; k++)
				{
					for (int l = j - kernel / 2; l < j + kernel / 2; l++)
					{
						if (k >= 0 && l >= 0 && k + kernel / 2 < picture.rows && l + kernel / 2 < picture.cols)
							picture.ptrDstCopy[k][l] = BLACK;
					}
				}

			}
		}
	}

	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	std::cout << elapsed_ms.count()/1000000000.;

	return ToResult(picture.ptrDstCopy, picture.rows, picture.cols);
}

extern "C"
__declspec(dllexport)
int* Erosion(int kernel, char* path)
{
	int WHITE = 1;
	int BLACK = 0;
	auto begin = std::chrono::steady_clock::now();
	Picture picture = GetPicture(path);

	for (int i = 0; i < picture.rows; i++) {
		for (int j = 0; j < picture.cols; j++) {

			if (picture.ptrDst[i][j] == WHITE) {

				for (int k = i - kernel / 2; k < i + kernel / 2; k++)
				{
					for (int l = j - kernel / 2; l < j + kernel / 2; l++)
					{
						if (k >= 0 && l >= 0 && k + kernel / 2 < picture.rows && l + kernel / 2 < picture.cols)
							picture.ptrDstCopy[k][l] = WHITE;
					}
				}

			}
		}
	}

	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	std::cout << elapsed_ms.count();

	return ToResult(picture.ptrDstCopy, picture.rows, picture.cols);
}
#endif //PCH_H
