#include <iostream>
#include <ptcsv.hpp>

int main() {
	mtk::ptcsv ptcsv;

	ptcsv.load_csv("test.csv");

	for(const auto& row : ptcsv.get_rows()) {
		for(const auto &item : row) {
			std::printf("%15s : %s\n", item.first.c_str(), item.second.c_str());
		}
	}
}
