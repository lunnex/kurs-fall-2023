// pch.h: это предварительно скомпилированный заголовочный файл.
// Перечисленные ниже файлы компилируются только один раз, что ускоряет последующие сборки.
// Это также влияет на работу IntelliSense, включая многие функции просмотра и завершения кода.
// Однако изменение любого из приведенных здесь файлов между операциями сборки приведет к повторной компиляции всех(!) этих файлов.
// Не добавляйте сюда файлы, которые планируете часто изменять, так как в этом случае выигрыша в производительности не будет.

#ifndef PCH_H
#define PCH_H
#pragma warning(disable : 4996)

// Добавьте сюда заголовочные файлы для предварительной компиляции
#include "framework.h"
#include<iostream>
#include<CL/cl.h>
#include<iostream>
#include<fstream>
#include <string.h>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;

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



cl_device_id* configureDevices() {
	cl_uint numberOfPlatforms;
	cl_platform_id platform = NULL;
	cl_int status = clGetPlatformIDs(0, NULL,
		&numberOfPlatforms);
	if (numberOfPlatforms > 0)
	{
		cl_platform_id* platforms = new
			cl_platform_id[numberOfPlatforms];
		status = clGetPlatformIDs(numberOfPlatforms,
			platforms, NULL);
		platform = platforms[0];
		delete[] platforms;
	}
	cl_uint numberOfDevices = 0;
	cl_device_id* devices;
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0,
		NULL, &numberOfDevices);
	if (status != CL_SUCCESS)
	{
		cout << " Не найдено устройство, поддерживающее OpenCL" << endl;
		exit(1);
	}
	devices = new cl_device_id[numberOfDevices];
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL,
		numberOfDevices, devices, NULL);
	if (status != CL_SUCCESS)
	{
		cout << "Не найдено устройство, поддерживающее OpenCL" << endl;
		exit(1);
	}
	return devices;
}

int convertToString(const char* fileName, string& s)
{
	size_t size;
	char* str;
	fstream f(fileName, (std::fstream::in | std::fstream::binary));
	if (f.is_open())
	{
		size_t fileSize;
		f.seekg(0, fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, fstream::beg);
		str = new char[size + 1];
		if (!str) {
			f.close();
			return 0;
		}
		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return 0;
	}
	return -1;
}

void disposeDevices(cl_device_id* devices)
{
	delete[] devices;
}

int* dilatation_ocl(cl_device_id* devices, int size, string path)
{
#pragma region program compilation
	cl_context context = clCreateContext(NULL, 1, devices,
		NULL, NULL, NULL);
	cl_command_queue commandQueue =
		clCreateCommandQueue(context, devices[0], 0, NULL);

	const char* fileName = "E:\\kurs-fall-2023\\ocl-dll\\source.cl";
	std::string sourceStr;
	cl_int status = convertToString(fileName, sourceStr);
	const char* source = sourceStr.c_str();

	size_t sizeOfSource[] = { strlen(source) };
	cl_program program = clCreateProgramWithSource(context, 1,
		&source, sizeOfSource, &status);

	status = clBuildProgram(program, 1, devices, NULL, NULL,
		NULL);

	//cout << "status";
	cout << status;

	if (status != CL_SUCCESS) {
		static char log[65536];
		memset(log, 0, sizeof(log));
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(log) - 1, log, NULL);
		cout << log[10];
		cout << log;
	}


#pragma endregion
	auto picture = GetPicture(path);


	int* regionSize = new int[3] {size, picture.rows, picture.cols};
	cl_mem regionSizeBuffer = clCreateBuffer(context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(int) * 3, regionSize, &status);


	int* input = new int[picture.rows * picture.cols];

	for (int i = 0; i < picture.rows; i++) {
		for (int j = 0; j < picture.cols; j++) {
			input[i * picture.rows + j] = picture.ptrDst[i][j];
		}
	}

	cl_mem inputBuffer = clCreateBuffer(context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(int) * picture.rows * picture.cols, input, &status);

	int* output = new int[picture.rows * picture.cols];
	cl_mem outputBuffer = clCreateBuffer(context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(int) * picture.rows * picture.cols, output, &status);

	cl_kernel kernel = clCreateKernel(program, "Dilatation", &status);

	status = clSetKernelArg(kernel, 0, sizeof(cl_mem),
		(void*)&regionSizeBuffer);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem),
		(void*)&inputBuffer);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem),
		(void*)&outputBuffer);

	const size_t global_work_size[1] = { picture.rows};



	auto begin = std::chrono::steady_clock::now();

	status = clEnqueueNDRangeKernel(commandQueue, kernel, 1,
		NULL, global_work_size, NULL, 0, NULL, NULL);

	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	std::cout << elapsed_ms.count();




	clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0,
		sizeof(int) * picture.rows * picture.cols, output, 0, NULL, NULL);


	clReleaseKernel(kernel);
	clReleaseMemObject(regionSizeBuffer);
	clReleaseMemObject(inputBuffer);
	clReleaseMemObject(outputBuffer);
	clReleaseProgram(program);
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);

	return output;
}

int* erode_ocl(cl_device_id* devices, int size, string path)
{
#pragma region program compilation
	cl_context context = clCreateContext(NULL, 1, devices,
		NULL, NULL, NULL);
	cl_command_queue commandQueue =
		clCreateCommandQueue(context, devices[0], 0, NULL);

	const char* fileName = "C:\\Users\\ilyak\\source\\repos\\kurs-fall-2023\\ocl-dll\\source.cl";
	std::string sourceStr;
	cl_int status = convertToString(fileName, sourceStr);
	const char* source = sourceStr.c_str();

	size_t sizeOfSource[] = { strlen(source) };
	cl_program program = clCreateProgramWithSource(context, 1,
		&source, sizeOfSource, &status);

	status = clBuildProgram(program, 1, devices, NULL, NULL,
		NULL);

	//cout << "status";
	cout << status;

	if (status != CL_SUCCESS) {
		static char log[65536];
		memset(log, 0, sizeof(log));
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(log) - 1, log, NULL);
		cout << log[10];
		cout << log;
	}


#pragma endregion
	auto picture = GetPicture(path);


	int* regionSize = new int[3] {size, picture.rows, picture.cols};
	cl_mem regionSizeBuffer = clCreateBuffer(context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(int) * 3, regionSize, &status);


	int* input = new int[picture.rows * picture.cols];

	for (int i = 0; i < picture.rows; i++) {
		for (int j = 0; j < picture.cols; j++) {
			input[i * picture.rows + j] = picture.ptrDst[i][j];
		}
	}

	cl_mem inputBuffer = clCreateBuffer(context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(int) * picture.rows * picture.cols, input, &status);

	int* output = new int[picture.rows * picture.cols];
	cl_mem outputBuffer = clCreateBuffer(context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(int) * picture.rows * picture.cols, output, &status);

	cl_kernel kernel = clCreateKernel(program, "Erosion", &status);

	status = clSetKernelArg(kernel, 0, sizeof(cl_mem),
		(void*)&regionSizeBuffer);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem),
		(void*)&inputBuffer);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem),
		(void*)&outputBuffer);

	const size_t global_work_size[1] = { picture.rows };



	auto begin = std::chrono::steady_clock::now();

	status = clEnqueueNDRangeKernel(commandQueue, kernel, 1,
		NULL, global_work_size, NULL, 0, NULL, NULL);

	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	std::cout << elapsed_ms.count();




	clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0,
		sizeof(int) * picture.rows * picture.cols, output, 0, NULL, NULL);


	clReleaseKernel(kernel);
	clReleaseMemObject(regionSizeBuffer);
	clReleaseMemObject(inputBuffer);
	clReleaseMemObject(outputBuffer);
	clReleaseProgram(program);
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);

	return output;
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


extern "C"
__declspec(dllexport)
int* Dilatation(int size, char* path)
{
	cl_device_id* devices = configureDevices();
	int* result =  dilatation_ocl(devices, size, path);
	disposeDevices(devices);
	return result;
}

extern "C"
__declspec(dllexport)
int* Erosion(int size, char* path)
{
	cl_device_id* devices = configureDevices();
	int* result = erode_ocl(devices, size, path);
	disposeDevices(devices);
	return result;
}
#endif //PCH_H