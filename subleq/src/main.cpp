#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <algorithm>

int subleq(
	std::vector<int> &memory,
	int &programCounter,
	const int operand1Address,
	const int operand2Address,
	const int jmpAddress
);

const std::string dumpMemory(const std::vector<int> &memory);

int main(int argc, char* argv[]) {
	bool debug = false, verbose = true;
	if (argc <= 1) {
		std::cout << "Usage: subleq program.sl" << std::endl;
		return 0;
	}
	else if (argc > 2) {
		for (int i = 2; i < argc; ++i) {
			std::string arg(argv[i]);

			if (arg == "-debug") {
				debug = true;
			}
			else if (arg == "-nowarn") {
				verbose = false;
			}
		}
	}

	std::ifstream source;
	source.open(argv[1]);

	if (!source.is_open()) {
		std::cerr << "Unable to open " << argv[1] << ". Please ensure the file exists and is readable." << std::endl;
		return -1;
	}

	std::vector<int> memory;

	source.exceptions(std::ios::failbit);

	int byte = 0;

	while (!source.eof()) {
		try {
			source >> byte;
			memory.push_back(byte);
		}
		catch (std::ios::failure &) {
			//Probably a comment
			source.clear();
			source.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}
	source.close();

	std::cout << "Initial state: " << dumpMemory(memory) << std::endl;

	int programCounter = 0;
	while (true) {
		try {
			if (debug) {
				std::cout << "Program Counter: " << programCounter << std::endl
				          << "Operands: (" << memory.at(programCounter) << ", " << memory.at(programCounter + 1) << ", " << memory.at(programCounter + 2) << ")" << std::endl;
			}

			subleq(memory, programCounter, memory.at(programCounter), memory.at(programCounter + 1), memory.at(programCounter + 2));

			if (debug) {
				if (programCounter > 0 && programCounter + 2 < static_cast<int>(memory.size())) {
					std::cout << "Current state: " << dumpMemory(memory) << std::endl;
				}
			}
		}
		catch (std::out_of_range &) {
			std::cout << "Final state: " << dumpMemory(memory) << std::endl
			          << "Program execution halted." << std::endl;
			return 0;
		}
	}
}

int subleq(
	std::vector<int> &memory,
	int &programCounter,
	const int operand1Address,
	const int operand2Address,
	const int jmpAddress
) {
	if (operand1Address > static_cast<int>(memory.size()) || operand2Address > static_cast<int>(memory.size())) {
		memory.resize(std::max(operand1Address, operand2Address) + 1);
	}

	memory.at(operand2Address) -= memory.at(operand1Address);

	if (memory.at(operand2Address) <= 0) {
		programCounter = jmpAddress;
	}
	else {
		programCounter += 3;
	}

	return programCounter;
}

const std::string dumpMemory(const std::vector<int> &memory) {
	std::string memString;

	std::for_each(memory.begin(), memory.end(), [&memString](const int byte) { memString += " " + std::to_string(byte); });

	return "[" + memString.substr(1) + "]";
}