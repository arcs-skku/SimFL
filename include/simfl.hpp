#pragma once

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define	CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <cstring>
#include <boost/algorithm/string/find.hpp>
#include <boost/tokenizer.hpp>
#include <CL/cl2.hpp>
//#include <CL/cl_ext_xilinx.h>

#define S			0		// Scalar value (constant value> do not make buffer
#define R			1		// Read buffer
#define H2G		1		// 			"
#define W			2		// Write buffer
#define G2H		2		// 			"
#define RW		3		// Read & Write buffer
#define WR		3		//  				"
#define ALL		-1	// for run|await all kernels

namespace simfl {

	template <typename T>
		struct fk_allocator {
			using value_type = T;
			T* allocate(std::size_t num) {
				void* ptr = nullptr;
				if (posix_memalign(&ptr, 4096, num * sizeof(T)))
					throw std::bad_alloc();
				return reinterpret_cast<T*>(ptr);
			}
			void deallocate(T* p, std::size_t num) {
				free(p);
			}
		};

	template <typename T>
		T* fk_alloc(size_t num) {
			void* ptr = nullptr;
			posix_memalign(&ptr, 4096, num * sizeof(T));
			return reinterpret_cast<T*>(ptr);
		}

	class Context {
		cl_int err;
		int cuNum;
		int deviceId;
		std::string deviceName;
		std::vector<int> argumentCount;
		std::vector<std::string> kernelNames;

		cl::Context clContext;
		cl::CommandQueue clCommandQueue;
		std::vector<cl::Device> usingDevice;
		std::vector<cl::Kernel> clKernel;

		std::vector<std::vector<cl::Event>> revent;
		std::vector<std::vector<cl::Event>> tevent;

		std::vector<std::vector<cl::Memory>> buffers;
		std::vector<std::vector<cl::Memory>> readBuffers;
		std::vector<std::vector<cl::Memory>> writeBuffers;

		std::vector<std::vector<void*>> argPtr;
		std::vector<std::vector<void*>> resultPtr;
		std::vector<std::vector<size_t>> sizeVector;

		public:
		Context(const std::string, const std::string);
		Context(const std::string, const std::string, const std::vector<std::string>);
		Context(const std::string, const std::string, const std::string, const int);

		template <typename T> Context& arg(const int, T, const int kid=0);
		template <typename T> Context& arg(const int, T, const int, size_t, const int kid=0);
		template <typename T> T* argMap(const int, const int, size_t, const int kid=0);

		template <typename T> Context& argSplit(const int, T);
		template <typename T> Context& argSplit(const int, T, const int, size_t);

		void run(const int kid=-1);
		void await(const int kid=-1);


		private:
		std::vector<unsigned char> readBitstream(const std::string);
		bool find_istr(const std::string, const std::string);
		void parseDeviceName(const std::string);
		void parseKernelName(const std::string);
		void getDevices();
		cl::Device selectDevice();
		void migrateResult(const int);
		void clearBuffer(const int);
	
		template <typename T> Context& arg_impl(const int, T, const int);
		template <typename T> Context& arg_impl(const int, T, const int, size_t, const int);
		template <typename T> T* argMap_impl(const int, const int, size_t, const int);
		template <typename T> Context& argSplit_impl(const int, T);
		template <typename T> Context& argSplit_impl(const int, T, const int, size_t);
	};
}

