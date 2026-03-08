#include <iostream>
#include <string>
#include "list.hpp"
namespace sogdanov {
  template<class T>
  LIter<T> list_append(List<T>& list, LIter<T> last, T val) {
    if (list.empty()) {
      list.push_front(std::move(val));
      return list.begin();
    }
    return list.insert_after(last, std::move(val));
  }
  using List_pair = List<std::pair<std::string, List<size_t>>>;
  void input(std::istream& in, List_pair& list) {
    LIter<std::pair<std::string, List<size_t>>> last;
    std::string name;
    while (in >> name) {
      List<size_t> nums;
      LIter<size_t> nums_last;
      char ch;
      while (in.get(ch)) {
        if (ch == '\n') {
          break;
        }
        if (ch == ' ') {
          continue;
        }
        in.unget();
        size_t val;
        in >> val;
        nums_last = list_append(nums, nums_last, val);
      }
      last = list_append(list, last,std::pair<std::string , List<size_t>>{name, \
                                                                     std::move(nums)});
    }
  }
  void transpose(const List_pair& list, List<List<size_t>>& data) {
    size_t max_size = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
      if (it->second.size() > max_size) {
        max_size = it->second.size();
      }
    }
    LIter<List<size_t>> data_last;
    for (size_t i = 0; i < max_size; ++i) {
      List<size_t> row;
      LIter<size_t> row_last;
      for (auto it - list.begin(); it != list.end(); ++it) {
        auto elem = it->second.begin();
        for (size_t j = 0; j < i; ++j) {
          ++elem;
        }
        if (e != it->second.end()) {
          row_last = list_append(row, row_last, *elem);
        }
      }
      data_last = list_append(data, data_last, std::move(row));
    }
  }
  void output(std::ostream& o, const List_pair& names, const List<List<size_t>>& transp);

}
int main() {
  using namespace sogdanov;
}
