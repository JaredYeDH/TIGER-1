#include "Generator.hpp"

//   std::vector<block_t> blocks_;
//   std::vector<std::vector<std::string>> vars_;
//   std::map<variable_t, live_range_t> live_ranges_;
//   std::map<variable_t, reg_t> regs_;

void GenCFG::find_blocks(std::vector<std::string>& ir) {
  std::vector<size_t> line_num;
  for (size_t i = 0; i < ir_.size(); ++i) {
    std::vector<std::string> tokens;
    tokens = cvt2tokens(i);
    if (tokens[0].find(":") != std::string::npos ||
        tokens[0].find("label") != std::string::npos) {
      line_num.push_back(i);
    }
  }
  for (size_t i = 0; i < line_num.size(); ++i) {
    block_t block = {line_num[i] + 1, line_num[i + 1] - 1};
    blocks_.push_back(block);
  }
}

void GenCFG::analyse_live() {
  vars_.reserve(blocks_.size());

  // store vars in each blocks into vars_
  for (size_t i = 0; i < blocks_.size(); ++i) {
    size_t beg_pos = blocks_[i].start_pos_;
    size_t end_pos = blocks_[i].end_pos_;
    std::vector<std::string> tokens;
    for (size_t j = beg_pos; j < end_pos; ++j) {
      tokens = cvt2tokens(j);
      size_t i = 0;
      for (auto& token : tokens) {
        if (i++ == 0) continue;
        if (tokens[0] == "call" && i == 2) continue;
        if (tokens[0] == "callr" && i == 3) continue;
        if (token.find(":") != std::string::npos) continue;
        if (token.find("label") != std::string::npos) continue;
        // if it's a variable
        if (!is_num(token)) {
          vars_[i].push_back(token);
        }
      }
    }
  }

  // analyse live range
  size_t live_beg, live_end;
  auto detect_live_range = [&](size_t id, std::string var_name) {
    size_t beg_pos = blocks_[id].start_pos_;
    size_t end_pos = blocks_[id].end_pos_;
    size_t times = 0;
    for (size_t j = beg_pos; j < end_pos; ++j) {
      auto& line = ir_[j];
      if (line.find(var_name) != std::string::npos) {
        if (times++ == 0) {
          live_beg = j;
        } else {
          live_end = j;
        }
      }
    }
  };

  for (size_t i = 0; i < vars_.size(); ++i) {
    for (size_t j = 0; j < vars_[i].size(); ++j) {
      detect_live_range(i, vars_[i][j]);
      live_ranges_[{i, vars_[i][j]}] = {live_beg, live_end};
    }
  }

	// interference graph and graph coloring
}