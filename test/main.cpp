#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
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

void test_filter() {
	std::cout << __func__ << std::endl;

	std::stringstream ss;
	ss << "A,B,C,D,E" << std::endl;
	ss << "1,2,3,y,y" << std::endl;
	ss << "6,7,8,y,n" << std::endl;
	ss << "4,2,9,n,n" << std::endl;

	mtk::ptcsv ptcsv;

	ptcsv.load_csv(ss);

	// filter
	std::map<std::string, mtk::ptcsv::filter_func_t> filter_map;
	filter_map.insert(std::make_pair("B", [](const std::string str){return (std::stoll(str) < 6);}));
	filter_map.insert(std::make_pair("E", [](const std::string str){return str == "n";}));

	for(const auto& row : ptcsv.get_rows(filter_map)) {
		for(const auto &item : row) {
			std::printf("%5s : %s, ", item.first.c_str(), item.second.c_str());
		}
		std::printf("\n");
	}
}

int main() {
	test_from_file();
	test_from_stream();
	test_filter();
}
