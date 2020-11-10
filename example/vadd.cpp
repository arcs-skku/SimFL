#include <iostream>
#include <vector>
#include "simfl.hpp"

using namespace std;

int main() {
	
	int dataSize = 4096;
	vector<int> output(dataSize);
	vector<int> input1(dataSize, 1);
	vector<int> input2(dataSize, 2);		

	simfl::Context context("u250", "vadd.xclbin", {"vadd:{vadd_1}", "vadd:{vadd_2}"});
	
	context.argSplit(2, &output[0], W, dataSize)
	.argSplit(0, &input1[0], R, dataSize)
	.argSplit(1, &input2[0], R, dataSize)
	.argSplit(3, dataSize);
	
	context.run();
	context.await();

	string result = "TRUE";
	for (int i = 0; i < dataSize; i++) {
		if (output[i] != 3)
			result = "FALSE";
	}
	cout << result << " " << output[0] << endl;

	return 0;
}

