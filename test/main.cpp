#include <iostream>
#include <sstream>
#include <ptcsv.hpp>

void test_from_file() {
	std::cout << __func__ << std::endl;

	mtk::ptcsv ptcsv;

	ptcsv.load_csv("test.csv");

	for(const auto& row : ptcsv.get_rows()) {
		for(const auto &item : row) {
			std::printf("%15s : %s\n", item.first.c_str(), item.second.c_str());
		}
	}
}

void test_from_stream() {
	std::cout << __func__ << std::endl;

	std::stringstream ss;
	ss << "A,B,C" << std::endl;
	ss << "1,2,3" << std::endl;
	ss << "6,7,8" << std::endl;

	mtk::ptcsv ptcsv;

	ptcsv.load_csv(ss);

	for(const auto& row : ptcsv.get_rows()) {
		for(const auto &item : row) {
			std::printf("%15s : %s\n", item.first.c_str(), item.second.c_str());
		}
	}
}

int main() {
	test_from_file();
	test_from_stream();
}
