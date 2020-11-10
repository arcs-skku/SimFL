# SimFL
*SimFL(**Sim**ple **F**PGA **L**anguage)* is a programming model for developing FPGA host programs.

## Requirements
1. OS
   - Linux
   - Tested on Ubuntu 16.04
2. Language
   - Requires C++14 or higher
   - Tested on gcc(g++) 5.4.0
3. Drivers
   - OpenCL platform drivers
   - Tested on Xilinx Runtim(xrt) 2019.1 ~ 2020.1
   
## How to use
`$ source setup.sh` <br>
`#include "simfl.hpp"` in your host code<br>
`$ g++  -I${SIMFL}/include   ...   -std=c++14 -o TARGET SOURCE   ...   -L${SIMFL} -lSimFL -lOpenCL   ...`

## Public Member Functions
```cpp
simfl::Context::Context (  const std::string			deviceName, 
                      	   const std::string			bitstream
                         )
```
Constructs a `simfl::Context` object.
It reads information from the bitstream and automatically sets the number of CUs.
<br>
```cpp
simfl::Context::Context (  const std::string			deviceName, 
                      	   const std::string			bitstream, 
                       	   const std::vector<std::string>	kernelNames
                         )
```
Constructs a `simfl::Context` object including a list of multiple *OpenCL* kernels from a specified device name and bitstream(kernel binary file) path.
<br>
```cpp
simfl::Context::Context (  const std::string		deviceName,
			   const std::string		bitstream,
			   const std::string		kernelName,
			   const int			numOfCUs
                         ) 
```
Constructs a `simfl::Context` object from a specified device name, bitstream path, and kernel name.   
`numOfCUs` has to be set 1 or more which be used the number of *Compute Unit*s.

<br><br><br>

```cpp
template <typename T>
simfl::Context& simfl::Context::arg (  const int	argIndex,
				       T		scalarArg, 
                                       const int	id = 0
                                     )
```
Sets a scalar value as a kernel argument with `argIndex` which is same as kernel's argument index.   
Scalar value means a variable that holds one value at a time.   
If your `simfl::Context` has multiple *OpenCL* kernel objects, you should specify an index of kernel.   
<br>
```cpp
template <typename T>
simfl::Context& simfl::Context::arg (  const int        argIndex,
				       T                argPtr, 
				       const int        bufferFlag,
				       const size_t     dataSize,
				       const int        id = 0
                                     )
```
Sets non-scalar such as array, vector, etc. 
`argPtr` must be pointer which is indicating first element of argument.   
`dataSize` is the number of elements.   
<br>
Default of `id` is 0, don't have to sepcify when `simfl::Context` has 1 kernel(CU).   
This function can be also used when you need to iteratively update argument and call the kernel.   
<br>
```cpp
template <typename T>
simfl::Context& simfl::Context::argSplit (  const int	   argIndex,
					    T		   scalarArg 
                                      	  )
```
Sets a scalar value by spliting according to the number of CUs.   
<br>
```cpp
template <typename T>
simfl::Context& simfl::Context::argSplit (  const int	     argIndex,
					    T		     argPtr, 
					    const int	     bufferFlag,
					    const size_t     dataSize
                                          )
```
Sets non-scalar data by spliting according to the number of CUs.   
*Note*: The data must be able to be divided sequentially.   
*e.g.*　□□□□□□□□□□□□□□□□□□□□　　→　　 □□□□□□□□□□　|　□□□□□□□□□□   
　　└────────┬───────┘　　　 　 └───┬───┘　　└───┬───┘   
　　　　　　　`dataSize`　　　　　　　　　　　`dataSize/numOfCUs` respectively   
If you need to divide data irregularly, you must specify the data using `arg` for each CU instead of `argSplit`.


<br><br><br>

```cpp
void simfl::Context::run (  const int	   id = ALL )
```
This function calls your device kernel.   
If you want to run only the specified kernel,`id` value is needed.   
Default of `id` is `ALL`, don't have to specify when running all device kernels.

<br><br><br>

```cpp
void simfl::Context::await (  const int	   id = ALL )
```
This function let the host wait for kernel to finish.   
Default of `id` is `ALL`, don't have to specify when waiting all device kernels.

<br><br><br>

## Examples
* basic
```cpp
/* Single device & Single coumpute unit */
int main() {

	...
	
	simfl::Context context("xilinx", "vadd.xclbin", {"vadd"});
	/***	OR
	simfl::Context context("xilinx", "vadd.xclbin", "vadd", 1);		
	***/	
	context.arg(0, &input1[0], R, data_size)
	       .arg(1, &input2[0], R, data_size)
	       .arg(2, &output[0], W, data_size)
	       .arg(3, data_size);
	context.run();
	context.await();

	...

}
```

<br><br>

* multi CUs
```cpp
/* Single device & Multi compute unit */
int main() {

	...
	
	int num_cu = 2;
	simfl::Context context("xilinx", "vadd.xclbin", "vadd", num_cu);
	/***	OR
	simfl::Context context("xilinx", "vadd.xclbin", {"vadd", "vadd"});
	***/
	
	context.argSplit(0, &input1[0],      R, data_size)
	       .argSplit(1, &input2[0],      R, data_size)
	       .argSplit(2, &output[0],      W, data_size)
	       .argSplit(3, data_size);			/***	OR
	       						int offset = data_size / num_cu;
							context.arg(0, &input1[0],      R, offset, 0)
	      						       .arg(1, &input2[0],      R, offset, 0)
	      						       .arg(2, &output[0],      W, offset, 0)
	      						       .arg(3, offset, 0);
							context.arg(0, &input1[offset], R, offset, 1)
	      						       .arg(1, &input2[offset], R, offset, 1)
	      						       .arg(2, &output[offset], W, offset, 1)
	      						       .arg(3, offset, 1);
							***/
	context.run();		/***	OR
				context.run(0);	
				context.run(1);	
				***/
	context.await();   	/***	OR
				context.await(0);	
				context.await(1);
				***/
	...

}
```

<br><br>

* multi devices
```cpp
/* Multi devices & Single compute unit per device */
int main() {

	...
	
	int num_device = 2;
	int offset = data_size / num_device;
	std::vector<simfl::Context> contexts;
	contexts.push_back(simfl::Context("xilinx[0]", "vadd.xclbin", "vadd", 1));
	contexts.push_back(simfl::Context("xilinx[1]", "vadd.xclbin", "vadd", 1));
	for (int d = 0; d < num_device; d++) {
		contexts[d].arg(0, &input1[d*offset], R, offset)
			   .arg(1, &input2[d*offset], R, offset)
			   .arg(2, &output[d*offset], W, offset)
			   .arg(3, offset);
	}
	for (int d = 0; d < num_device; d++) {
		contexts[d].run();
	}
	for (int d = 0; d < num_device; d++) {
		contexts[d].await();
	}
	
	...
	
}
```

<br><br>

* Iterative kernel
```cpp
/* Iterative kernel call */
int main() {

	...
	
	simfl::Context context("xilinx", "Gradients.xclbin", "gradients", 1);
	context.arg(0, labels, R, label_size)
	       .arg(1, features, R, feature_size)
	       .arg(2, weights, R, weight_size)
	       .arg(3, gradients, R, gradient_size)
	       .arg(4, num_classes)
               .arg(5, num_features)
	       .arg(6, num_examples);
	for (int i = 0; i < iter; i++) {
		if ( i != 0 )
			context.arg(2, weights, R, weight_size);
		context.run();
		context.await();
		/* Update weights */
		
		...
		
	}
	
	...
	
}
```
<br><br><br>
## Data Parallelism
* OpenCL example
```cpp
int main(){

	...

	cl::Kernel kernel1(program, "vadd", &err);
	cl::Kernel kernel2(program, "vadd", &err);

	cl::Buffer kernel1_in (context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  dataSize/2, &in[0],  &err);
	cl::Buffer kernel1_out(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, dataSize/2, &out[0], &err);
	cl::Buffer kernel2_in (context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  dataSize/2, &in[dataSize/2],  &err);
	cl::Buffer kernel2_out(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, dataSize/2, &out[dataSize/2], &err);

	kernel1.setArg(0, kernel1_in);
	kernel1.setArg(1, kernel1_out);
	kernel2.setArg(0, kernel2_in);
	kernel2.setArg(1, kernel2_out);

	...
	
}

```
<br><br>
* SimFL example
```cpp
int main(){

	...

	simfl::Context context("xilinx", "vadd.xclbin", "vadd", 2);
	context.argSplit(0, &in[0], R, dataSize).argSplit(1, &out[0], W, dataSize);

	...

}
```

<br><br><br>
## Appendix
### SimFL vs. OpenCL
* simfl::Context()
```cpp
	/***	SimFL	***/
	simfl::Context context("platform", "bitstream.xclbin", {"kernel"});
```
```cpp
	/***	OpenCL	***/
	cl_int err = CL_SUCCESS;
	
	std::vector<cl::Platform> platforms;						// Xilinx provides function: get_xil_deivces()
	err = cl::Platform::get(&platforms);
	cl::Platform platform;
	for (int i = 0; i < platforms.size(); i++) {
		platform = platforms[i];
		std::string platformName = platform.getInfo<CL_PLATFORM_NAME>(&err);
		if (platformName == "vendor_name") break;
	}
	std::vector<cl::Device> devices
	err = platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices)			// std::vector<cl::Device> devices = xcl::get_xil_devices();
	
	cl::Context context(devices[0], NULL, NULL, NULL, &err);			/*** NULLs
											properties : use in special case (e.g., specify platform, use OpenGL, etc.)
											callback function : use to report
											callback func's argument	***/
	cl::CommandQueue queue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &err);
	
	vector<unsigned char> fileBuf = xcl::read_binary_file("bitstream.xclbin");	// just file IO (non-opencl)
	cl::Program::Binaries bins{{fileBuf.data)_. fileBuf.size()}};
	devices.resize(1);								// if multiple devices, error occurs
	cl::Program program(context, devices, bins);
	cl::Kernel kernel(program, "kernel", &err);
```
**Problems**<br><br>
When using multiple CUs attached to different banks, only the method using `std::vector<std::string> kernelNames` is safe. <br><br>
*correct* :<br>
```cpp
simfl::Context context("platform", "bitstream.xclbin", {"kernel:{kernel_1}, kernel:{kernel_2}"};
```
*may be wrong* :<br>
```cpp
simfl::Context context("platform", "bitstream.xclbin");
simfl::Context context("platform", "bitstream.xclbin", "kernel", 2};
```
<br>

* simfl::Context::arg()
```cpp
	/*** SimFL ***/
	context.arg(0, &input[0], R, dataSize).arg(1, &output[0], W, dataSize).arg(2, dataSize);
```
```cpp
	/*** OpenCL ***/
	cl::Buffer buf_in(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(T) * dataSize, &input[0], &err);
	cl::Buffer buf_out(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(T) * dataSize, &output[0], &err);
	context.setArg(0, buf_in);
	context.setArg(1, buf_out);
	context.setArg(2, dataSize);
```
**Problems**<br><br>
To satisfy the 4K aligned memory, automatic data distribution `simfl::Context::argSplit()` needs an internal memcpy. <br>
Performance is good in the order of *direct allocation of aligned data*, *internal memcpy*, and *allocation of non-aligned data*. <br>
