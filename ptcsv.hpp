#ifndef __PTCSV_HPP__
#define __PTCSV_HPP__
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <sstream>

#define PTCSV_AS_SET(type, func) template <> inline type as<type>(const std::string str) {return func(str.c_str());}

namespace mtk {
namespace utils {
template <class T>
inline T as(const std::string str);
template <> inline std::string as<std::string>(const std::string str) {return str;}
PTCSV_AS_SET(int, std::stoi);
PTCSV_AS_SET(unsigned long, std::stoul);
PTCSV_AS_SET(unsigned long long, std::stoull);
PTCSV_AS_SET(double, std::stod);
PTCSV_AS_SET(long double, std::stold);
PTCSV_AS_SET(float, std::stof);

inline std::vector<std::string> split(const std::string str) {
	std::vector<std::string> splited;
	std::stringstream ss(str);

	while(ss.good()) {
		std::string buffer;
		std::getline(ss, buffer, ',');
		splited.push_back(buffer);
	}

	return splited;
}
} // namespace utils
class ptcsv {
private:
	std::map<std::string, std::vector<std::string>> data;
	std::size_t num_data;
	std::vector<std::string> col_names;
public:
	inline ptcsv() : num_data(0lu) {};
	inline ptcsv(const std::string filename) : ptcsv() {
		load_csv(filename);
	}

	inline void load_csv(const std::string filename, const char comment_head = '\0') {
		std::string buffer;
		std::ifstream ifs(filename);

		if(!ifs) {
			throw std::runtime_error("No such a file : " + filename);
		}

		while(std::getline(ifs, buffer) && buffer.data()[0] == comment_head);
		col_names = utils::split(buffer);
		for(const auto& col_name : col_names) {
			data.insert(std::make_pair(col_name, std::vector<std::string>{}));
		}

		while(std::getline(ifs, buffer)) {
			if(buffer.data()[0] == comment_head) {
				continue;
			}
			const auto row_values = utils::split(buffer);
			for(std::size_t i = 0; i < data.size() && i < row_values.size(); i++) {
				data.at(col_names[i]).push_back(row_values[i]);
			}
			num_data++;
		}
	}

	inline std::size_t get_num_rows() const {
		return num_data;
	}

	inline std::size_t get_num_cols() const {
		return col_names.size();
	}

	template <class T>
	inline std::vector<T> get_col_as(const std::string col_name) const {
		std::vector<T> t_col(get_num_rows());
		for(std::size_t i = 0; i < get_num_rows(); i++) {
			t_col[i] = utils::as<T>(data.at(col_name)[i]);
		}
		return t_col;
	}

	std::map<std::string, std::string> get_row_at(const std::size_t r) const {
		std::map<std::string, std::string> row;
		for(const auto& col_name : col_names) {
			row.insert(std::make_pair(col_name, data.at(col_name)[r]));
		}
		return row;
	}
};
} // namespace mtk

#endif /* end of include guard */

