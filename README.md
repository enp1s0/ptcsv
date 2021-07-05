# PtCSV

A tiny C++ CSV loading library

## Sample
```cpp
#include <iostream>
#include <sstream>
#include <string>
#include <ptcsv.hpp>

int main() {
  std::stringstream ss;
  ss << "A,B,C,D,E" << std::endl;
  ss << "1,2,3,y,y" << std::endl;
  ss << "6,7,8,y,n" << std::endl;
  ss << "4,2,9,n,n" << std::endl;
  
  mtk::ptcsv ptcsv;
  
  // load from stringstream
  ptcsv.load_csv(ss);
  
  // filter
  std::map<std::string, mtk::ptcsv::filter_func_t> filter_map;
  filter_map.insert(std::make_pair("B", [](const std::string str){return (std::stoll(str) < 6);}));
  filter_map.insert(std::make_pair("E", [](const std::string str){return str == "n";}));
  
  // output
  for(const auto& row : ptcsv.get_rows(filter_map)) {
    for(const auto &item : row) {
      std::printf("%5s : %s, ", item.first.c_str(), item.second.c_str());
    }
    std::printf("\n");
  }
}
```

## License

MIT
