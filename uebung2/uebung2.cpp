#include <CL/cl.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>


using namespace std;

void printAllPlatformAttribute(cl_platform_id const & _id, size_t const & _bufferSize);
void printDeviceAttribute(cl_device_id const & _id, cl_device_info const & _attribute, size_t const & _bufferSize);
cl_device_id getPlatformModelDevices(cl_platform_id const & _id);
cl_platform_id * getPlatforms();
string readFile(string fileName);
void executeMathKernel(cl_context const & context, cl_command_queue & commandQueue, cl_kernel const & kernel);

int main() {

	int bufferSize = 1000000;

	//Platforms
	cl_platform_id * platforms = getPlatforms();
	
	//Ausgabe der Platform-Eigenschaften
	//printAllPlatformAttribute(platforms[0], bufferSize);
	//printAllPlatformAttribute(platforms[1], bufferSize);

	//Platform Model-Devices
	cl_device_id deviceID = getPlatformModelDevices(platforms[0]);
	
	//Print attributes from Devices
	//printDeviceAttribute(deviceID, CL_DEVICE_TYPE, bufferSize);
	//printDeviceAttribute(deviceID, CL_DEVICE_MAX_COMPUTE_UNITS, bufferSize);
	//printDeviceAttribute(deviceID, CL_DEVICE_MAX_WORK_GROUP_SIZE, bufferSize);

	//Create CONTEXT
	cl_device_id const currentDeviceID[] = { deviceID };
	cl_context const myContext = clCreateContext(NULL, 1, currentDeviceID, NULL, NULL, NULL);

	//Create Command-Queue
	cl_command_queue myCommandQueue = clCreateCommandQueueWithProperties(myContext, currentDeviceID[0], NULL, NULL);

	//Read kernel data 
	string kernelContent = readFile("kernel.cl");

	//Create kernel program
	char const * source = kernelContent.c_str(); 
	cl_int programError = CL_SUCCESS;
	cl_program myClProgram = clCreateProgramWithSource(myContext, 1, &source, NULL, &programError);
	if (CL_SUCCESS == programError) cout << "Kernel created" << endl;
	else cerr << "Couldn't create kernel program. Error code: " << programError<< endl;

	//Compile kernel program
	cl_int error = clBuildProgram(myClProgram, 1, currentDeviceID, "-w", NULL , NULL);
	if (CL_SUCCESS == error) cout << "Kernel compiled" << endl;
	else cerr << "Couldn't compile kernel program. Error code: " << error << endl;

	size_t size = 10000;
	char * attribute_01 = new char[size];
	cl_int buildError = clGetProgramBuildInfo(myClProgram, currentDeviceID[0], CL_PROGRAM_BUILD_LOG, size, attribute_01, NULL);
	if (CL_SUCCESS == buildError) cout << "Get build infos" << endl;
	else cerr << "Couldn't run build infos. Error code: " << buildError << endl;

	cout << "FEHLER: " << attribute_01 << endl;


	//Create kernel 
	cl_int kernelError = CL_SUCCESS;
	cl_kernel const myKernel = clCreateKernel(myClProgram, "matrixVectorMulti", &kernelError);
	if (CL_SUCCESS == kernelError) cout << "Kernel created"<< endl;
	else cerr << "Couldn't created kernel: " << kernelError<< endl;

	//Execute kernel
	//size_t const global_work_size = 1;
	//size_t const local_work_size = 1;
	//cl_int const executeError = clEnqueueNDRangeKernel(myCommandQueue, myKernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
	//if (CL_SUCCESS == executeError) cout << "Kernel executed" << endl;
	//else cerr << "Couldn't execute kernel: " << executeError <<  endl;
	
	

	executeMathKernel(myContext, myCommandQueue, myKernel);

	clFinish(myCommandQueue);
}

string readFile(string path) {
	ifstream input(path);
	string text = "";
		
	for (std::string line; getline(input, line);)
		text += line;

	return text;
}

void printDeviceAttribute(cl_device_id const & _id, cl_device_info const & _attribute, size_t const & _bufferSize)
{
	char * attribute = new char[_bufferSize];
	size_t unusedReturnedSize = 0;
	cl_int const error = clGetDeviceInfo(_id, _attribute, _bufferSize, attribute, &unusedReturnedSize);
	if (CL_SUCCESS != error)
	{
		cerr << "Couldn't get device attribute " << _attribute << "." << endl;
		delete[] attribute;
	}
	else
	{
		cout << (cl_uint)attribute << endl;
		delete[] attribute;
	}
}

void printAllPlatformAttribute(cl_platform_id const & _id, size_t const & _bufferSize)
{
	char * attribute_01 = new char[_bufferSize];
	char * attribute_02 = new char[_bufferSize];
	char * attribute_03 = new char[_bufferSize];
	char * attribute_04 = new char[_bufferSize];
	char * attribute_05 = new char[_bufferSize];

	size_t returnedSize_01 = 0, returnedSize_02 = 0, returnedSize_03 = 0, returnedSize_04 = 0, returnedSize_05 = 0;
	
	cl_int const error1 = clGetPlatformInfo(_id, CL_PLATFORM_PROFILE, _bufferSize, attribute_01, &returnedSize_01);
	cl_int const error2 = clGetPlatformInfo(_id, CL_PLATFORM_VERSION, _bufferSize, attribute_02, &returnedSize_02);
	cl_int const error3 = clGetPlatformInfo(_id, CL_PLATFORM_NAME, _bufferSize, attribute_03, &returnedSize_03);
	cl_int const error4 = clGetPlatformInfo(_id, CL_PLATFORM_VENDOR, _bufferSize, attribute_04, &returnedSize_04);
	cl_int const error5 = clGetPlatformInfo(_id, CL_PLATFORM_EXTENSIONS, _bufferSize, attribute_05, &returnedSize_05);
	if (CL_SUCCESS != error1 || CL_SUCCESS != error2 || CL_SUCCESS != error3 || CL_SUCCESS != error4 || CL_SUCCESS != error5) 
		cerr << "Couldn't get platform attribute " << "." << endl;
	else
	{
		string const string1(attribute_01, returnedSize_01);
		string const string2(attribute_02, returnedSize_02);
		string const string3(attribute_03, returnedSize_03);
		string const string4(attribute_04, returnedSize_04);
		string const string5(attribute_05, returnedSize_05);

		cout << "CL_PLATFORM_PROFILE:    " << string1.c_str() << endl;
		cout << "CL_PLATFORM_VERSION:    " << string2.c_str() << endl;
		cout << "CL_PLATFORM_NAME:       " << string3.c_str() << endl;
		cout << "CL_PLATFORM_VENDOR:     " << string4.c_str() << endl;
		cout << "CL_PLATFORM_EXTENSIONS: " << string5.c_str() << endl;
		cout << "--------------------------------" << endl;
	}

	delete[] attribute_01;
	delete[] attribute_02;
	delete[] attribute_03;
	delete[] attribute_04;
	delete[] attribute_05;
}

cl_device_id getPlatformModelDevices(cl_platform_id const & _id) {
	cl_uint numberOfDevices = 0;
	cl_uint const MAX_SEARCH_DEVICES = 30;
	cl_device_id devices[MAX_SEARCH_DEVICES];
	cl_int error = clGetDeviceIDs(_id, CL_DEVICE_TYPE_ALL, MAX_SEARCH_DEVICES, devices, &numberOfDevices);

	if (CL_SUCCESS == error) cout << "Detected devices: " << numberOfDevices << endl;
		else cerr << "Couldn't detect OpenCL devices. Error code: " << error << endl;

	return devices[0];
}

cl_platform_id* getPlatforms() {
	cl_uint numberOfPlatforms = 0;
	cl_uint const SEARCH_PLATFORMS = 5;
	static cl_platform_id platforms[SEARCH_PLATFORMS];
	cl_int error = clGetPlatformIDs(SEARCH_PLATFORMS, platforms, &numberOfPlatforms);

	if (CL_SUCCESS == error) cout << "Detected OpenCL platforms: " << numberOfPlatforms << endl;
		else cerr << "Couldn't detect OpenCL platforms. Error code: " << error << endl;

	return platforms;
}

void executeMathKernel(cl_context const & context, cl_command_queue & commandQueue, cl_kernel const & kernel) {
	cl_uint const elements = 5;
	cl_uint const matrixBytes = elements * elements * sizeof(cl_int);
	cl_uint const vectorBytes = elements * sizeof(cl_int);


	cl_int vectorA[] = {5,10,15,20,25,5,10,15,20,25, 5,10,15,20,25, 5,10,15,20,25, 5,10,15,20,25};
	cl_int vectorAError = CL_SUCCESS;
	cl_mem const kernelVectorA = clCreateBuffer(context, CL_MEM_READ_ONLY| CL_MEM_COPY_HOST_PTR, matrixBytes, vectorA, &vectorAError);
	if (CL_SUCCESS == vectorAError) cout << "vectorA laeuft" << endl;
	else cerr << "vectorAError: " << vectorAError << endl;

	cl_int vectorB[] = { 1,2,3,4,5 };
	cl_int vectorBError = CL_SUCCESS;
	cl_mem const kernelVectorB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, vectorBytes, vectorB, &vectorBError);
	if (CL_SUCCESS == vectorBError) cout << "vectorB laeuft" << endl;
	else cerr << "vectorBError: " << vectorBError << endl;

	cl_int resultVectorError = CL_SUCCESS;
	cl_mem const resultKernelVector = clCreateBuffer(context, CL_MEM_WRITE_ONLY, vectorBytes, NULL, &resultVectorError);
	if (CL_SUCCESS == resultVectorError) cout << "Result kernel laeuft" << endl;
	else cerr << "resultVectorError : " << resultVectorError << endl;

	cl_int const setSizeError = clSetKernelArg(kernel, 0, sizeof(cl_uint), &elements);
	if (CL_SUCCESS == setSizeError) cout << "1" << endl;
	else cerr << "setSize: " << setSizeError<< endl;

	cl_int const setVectorAError = clSetKernelArg(kernel, 1, sizeof(cl_mem), &kernelVectorA);
	if (CL_SUCCESS == setVectorAError) cout << "2" << endl;
	else cerr << "setVectorAError: " << setVectorAError<< endl;

	cl_int const setVectorBError = clSetKernelArg(kernel, 2, sizeof(cl_mem), &kernelVectorB);
	if (CL_SUCCESS == setVectorBError) cout << "3" << endl;
	else cerr << "setVectorBError: " << setVectorBError<< endl;

	cl_int const setResultVectorError = clSetKernelArg(kernel, 3, sizeof(cl_mem), &resultKernelVector); 
	if (CL_SUCCESS == setResultVectorError) cout << "4" << endl;
	else cerr << "setResultVectorError: " << setResultVectorError << endl;

	size_t const global_work_size = 1;
	size_t const local_work_size = 1;

	cl_int const executeError = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);

	cl_int const finishError = clFinish(commandQueue);

	cl_int resultVector[] = { 0,0,0,0,0 };
	cl_int const readResultError = clEnqueueReadBuffer(commandQueue, resultKernelVector, true, 0, vectorBytes, resultVector, NULL, NULL, NULL);

	for (int i = 0; i < elements; i++) {
		cout << resultVector[i] << " ";
	}
}