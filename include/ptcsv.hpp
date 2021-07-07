#ifndef __PTCSV_HPP__
#define __PTCSV_HPP__
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <sstream>
#include <functional>
#include <exception>

#define PTCSV_AS_SET(type, func) template <> inline type as<type>(const std::string str) {return func(str.c_str());}
#define PTCSV_CATCH_EXCEPTION(op, message) try{(op);} catch(const std::exception& e) {std::fprintf(stderr, "[PTCSV ERROR] %s [%s] @(%s l.%d in %s)\n", e.what(), (message).c_str(), __func__, __LINE__, __FILE__); throw e;}

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
public:
	using filter_func_t = std::function<bool(const std::string&)>;
private:
	std::map<std::string, std::vector<std::string>> data;
	std::size_t num_data;
	std::vector<std::string> col_names;
public:
	inline ptcsv() : num_data(0lu) {};
	inline ptcsv(const std::string filename) : ptcsv() {
		load_csv(filename);
	}

	inline void load_csv(std::istream &ss, const char comment_head = '\0') {
		std::string buffer;

		while(std::getline(ss, buffer) && buffer.data()[0] == comment_head);
		col_names = utils::split(buffer);
		for(const auto& col_name : col_names) {
			data.insert(std::make_pair(col_name, std::vector<std::string>{}));
		}

		while(std::getline(ss, buffer)) {
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

	inline void load_csv(const std::string filename, const char comment_head = '\0') {
		std::ifstream ifs(filename);

		if(!ifs) {
			throw std::runtime_error("No such a file : " + filename);
		}

		load_csv(ifs, comment_head);

		ifs.close();
	}

	inline std::size_t get_num_rows() const {
		return num_data;
	}

	inline std::size_t get_num_cols() const {
		return col_names.size();
	}

	inline std::vector<std::string> get_col_names() const {
		return col_names;
	}

	ptcsv filter(
			const std::map<std::string, filter_func_t>& filter_func_map
			) const {
		ptcsv new_ptcsv;

		new_ptcsv.col_names.resize(col_names.size());
		for (unsigned i = 0; i < col_names.size(); i++) {
			new_ptcsv.col_names[i] = col_names[i];
			new_ptcsv.data.insert(std::make_pair(col_names[i], std::vector<std::string>{}));
		}

		std::size_t num_added = 0;
		for (std::size_t i = 0; i < num_data; i++) {
			bool add = true;
			for (const auto& filter : filter_func_map) {
				const auto filter_col = filter.first;
				const auto filter_func = filter.second;

				PTCSV_CATCH_EXCEPTION(add &= filter_func(data.at(filter_col)[i]), std::string("filter_col = ") + filter_col);
			}

			if (add) {
				for (const auto& col : col_names) {
					PTCSV_CATCH_EXCEPTION(new_ptcsv.data.at(col).push_back(data.at(col)[i]), std::string("col = ") + col);
				}
				num_added++;
			}
		}

		new_ptcsv.num_data = num_added;

		return new_ptcsv;
	}

	template <class T>
	inline std::vector<T> get_col_as(
			const std::string col_name,
			const std::map<std::string, filter_func_t>& filter_func_map = std::map<std::string, filter_func_t>{}
			) const {
		const auto filtered_data = get_rows(filter_func_map);
		std::vector<T> t_col(filtered_data.size());
		for(std::size_t i = 0; i < t_col.size(); i++) {
			PTCSV_CATCH_EXCEPTION(t_col[i] = utils::as<T>(filtered_data[i].at(col_name)), std::string("col_name = ") + col_name);
		}
		return t_col;
	}

	std::map<std::string, std::string> get_row_at(const std::size_t r) const {
		std::map<std::string, std::string> row;
		for(const auto& col_name : col_names) {
			PTCSV_CATCH_EXCEPTION(row.insert(std::make_pair(col_name, data.at(col_name)[r])), std::string("col_name = ") + col_name);
		}
		return row;
	}

	inline std::vector<std::map<std::string, std::string>> get_rows(
			const std::map<std::string, filter_func_t>& filter_func_map = std::map<std::string, filter_func_t>{}
			) const {
		std::vector<std::map<std::string, std::string>> rows;
		for(std::size_t i = 0; i < num_data; i++) {
			const auto value = get_row_at(i);

			bool add = true;
			for (const auto& filter : filter_func_map) {
				const auto& filter_col = filter.first;
				const filter_func_t filter_func = filter.second;

				PTCSV_CATCH_EXCEPTION(add &= filter_func(value.at(filter_col)), std::string("filter_col = ") + filter_col);
			}

			if (add) {
				rows.push_back(value);
			}
		}
		return rows;
	}

	// print
	inline void print(FILE* const out = stdout) const {
		std::map<std::string, unsigned> max_str_length;
		for (unsigned i = 0; i < col_names.size(); i++) {
			max_str_length.insert(std::make_pair(col_names[i], col_names[i].length()));
		}

		for (const auto& row : get_rows()) {
			for (unsigned i = 0; i < col_names.size(); i++) {
				max_str_length.at(col_names[i]) =
					std::max(
							max_str_length.at(col_names[i]),
							static_cast<unsigned>(row.at(col_names[i]).length())
							);
			}
		}

		for (unsigned i = 0; i < col_names.size(); i++) {
			std::fprintf(out, "%*s", static_cast<int>(max_str_length.at(col_names[i])), col_names[i].c_str());
			if (i != col_names.size() - 1) {
				std::printf(", ");
			}
		}
		std::fprintf(out, "\n");
		for (const auto& row : get_rows()) {
			for (unsigned i = 0; i < col_names.size(); i++) {
				std::fprintf(out, "%*s", static_cast<int>(max_str_length.at(col_names[i])), row.at(col_names[i]).c_str());
				if (i != col_names.size() - 1) {
					std::printf(", ");
				}
			}
			std::fprintf(out, "\n");
		}
	}
};
} // namespace mtk

#endif /* end of include guard */

