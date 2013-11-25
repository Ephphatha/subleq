#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <algorithm>

int subleq(std::vector<int> &memory, unsigned int &programCounter, const unsigned int operand1Address, const unsigned int operand2Address, const unsigned int jmpAddress);

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
		std::cerr << "Unable to open " << argv[0] << ". Please ensure the file exists and is readable." << std::endl;
		return -1;
	}

	std::vector<int> memory;
	std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> program;

	std::string line;
	std::stringstream lineStream;

	if (!source.eof()) {
		std::getline(source, line);

		lineStream.str(line);
		lineStream.exceptions(std::ios::failbit);

		int byte = 0;
		while (!lineStream.eof()) {
			try {
				lineStream >> byte;
				memory.push_back(byte);
			}
			catch (std::ios::failure &e) {
				std::cerr << "Invalid memory value: " << e.what() << std::endl;
				source.close();
				return -1;
			}
		}
	}

	while (!source.eof()) {
		std::getline(source, line);

		lineStream.clear();
		lineStream.str(line);
		lineStream.exceptions(std::ios::failbit);

		try {
			unsigned int operand1Address = 0, operand2Address = 0, jmpAddress = 1;

			lineStream >> operand1Address >> operand2Address;

			try {
				lineStream >> jmpAddress;
			}
			catch (std::ios::failure &) {
				jmpAddress = program.size() + 1;
				if (verbose) {
					std::cout << "Warning: Instruction " << program.size() << " does not specify a jmp address, assuming " << jmpAddress << "." << std::endl;
				}
			}

			std::tuple<unsigned int, unsigned int, unsigned int> instruction (operand1Address, operand2Address, jmpAddress);
			program.push_back(instruction);
		}
		catch (std::ios::failure &e) {
			std::cerr << "Invalid or incomplete instruction: " << e.what() << std::endl;
			source.close();
			return -1;
		}
	}
	source.close();

	std::cout << "Initial state:" << std::endl
		      << dumpMemory(memory) << std::endl;

	std::tuple<unsigned int, unsigned int, unsigned int> instruction;
	unsigned int programCounter = 0;
	while (true) {
		try {
			instruction = program.at(programCounter);
		}
		catch (std::out_of_range &) {
			std::cout << "Final state:" << std::endl
				      << dumpMemory(memory) << std::endl
				      << "Program execution halted." << std::endl;
			return 0;
		}

		if (debug) {
			std::cout << dumpMemory(memory) << std::endl
			          << "(" << std::get<0>(instruction) << ", " << std::get<1>(instruction) << ", " << std::get<2>(instruction) << ")" << std::endl;
		}

		subleq(memory, programCounter, std::get<0>(instruction), std::get<1>(instruction), std::get<2>(instruction));
	}
}

int subleq(std::vector<int> &memory, unsigned int &programCounter, const unsigned int operand1Address, const unsigned int operand2Address, const unsigned int jmpAddress) {
	if (memory.size() <= operand1Address || memory.size() <= operand2Address) {
		memory.resize(std::max(operand1Address, operand2Address) + 1);
	}

	memory[operand1Address] -= memory[operand2Address];

	if (memory[operand1Address] <= 0) {
		programCounter = jmpAddress;
	}
	else {
		++programCounter;
	}

	return programCounter;
}

const std::string dumpMemory(const std::vector<int> &memory) {
	std::string memString;

	std::for_each(memory.begin(), memory.end(), [&memString](const int byte) { memString += " " + std::to_string(byte); });

	return "[" + memString.substr(1) + "]";
}