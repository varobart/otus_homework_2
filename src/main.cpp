#include "hello.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Bulk {
public:
  Bulk(size_t buffer_size);
  ~Bulk() = default;

  void process(const std::string &cmd);
  void flush();

private:
  void log();

  std::stringstream m_cmds;

  size_t m_buffer_size = 0;
  size_t m_nCmds = 0;
  std::time_t m_time;
  int m_nestingLvl = 0;
};

Bulk::Bulk(size_t buffer_size)
    : m_time(std::time(nullptr)), m_buffer_size(buffer_size) {
  m_cmds << "bulk:";
}

void Bulk::log() {
  if (m_nCmds > 0) {
    std::string timeStr = std::to_string(static_cast<long int>(m_time));
    std::string filename = "bulk" + timeStr + ".log";
    std::ofstream file(filename);
    file << m_cmds.str() << std::endl;
    std::cout << m_cmds.str() << std::endl;
  }

  m_nestingLvl = 0;
  m_nCmds = 0;
  m_time = std::time(nullptr);
  m_cmds = std::stringstream();
  m_cmds << "bulk:";
}

void Bulk::flush() {
  if (m_nestingLvl == 0)
    log();
}

void Bulk::process(const std::string &cmd) {
  if (cmd == "{") {
    if (m_nestingLvl == 0)
      flush();

    ++m_nestingLvl;
  } else if (cmd == "}") {
    --m_nestingLvl;
    if (m_nestingLvl == 0)
      flush();
  } else {
    if (m_nCmds > 0)
      m_cmds << ",";
    else
      m_time = std::time(nullptr);

    m_cmds << " " << cmd;
    ++m_nCmds;

    if (m_nCmds == m_buffer_size && m_nestingLvl == 0)
      flush();
  }
}

int main(int argc, char const *argv[]) {

  if (argc < 2) {
    std::cerr << "buffer size wasn't passed.\n";
    return 0;
  }

  std::string cmd;
  Bulk bulk(atoi(argv[1]));
  while (std::getline(std::cin, cmd))
    bulk.process(cmd);

  bulk.flush();
  return 0;
}
