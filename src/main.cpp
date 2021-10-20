#include "hello.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class ILogger {
public:
  ILogger() = default;
  virtual ~ILogger() = default;
  virtual void log(const std::string &message) = 0;
};

class ConsoleLogger : public ILogger {
public:
  ConsoleLogger() = default;
  ~ConsoleLogger() = default;

  void log(const std::string &message) override;
};

void ConsoleLogger::log(const std::string &message) {
  std::cout << message << std::endl;
}

class FileLogger : public ILogger {
public:
  FileLogger();
  ~FileLogger() = default;

  void log(const std::string &message) override;

private:
  std::time_t m_time;
};

FileLogger::FileLogger() : m_time(std::time(nullptr)) {}

void FileLogger::log(const std::string &message) {
  std::string timeStr = std::to_string(static_cast<long int>(m_time));
  std::string filename = "bulk" + timeStr + ".log";
  std::ofstream file(filename);
  file << message << std::endl;

  m_time = std::time(nullptr);
}

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
  int m_nestingLvl = 0;
  std::vector<std::unique_ptr<ILogger>> m_apLoggers;
};

Bulk::Bulk(size_t buffer_size) : m_buffer_size(buffer_size) {
  m_cmds << "bulk:";
  m_apLoggers.emplace_back(std::make_unique<ConsoleLogger>());
  m_apLoggers.emplace_back(std::make_unique<FileLogger>());
}

void Bulk::log() {
  if (m_nCmds > 0) {
    std::string message = m_cmds.str();
    for (auto &pLogger : m_apLoggers)
      pLogger->log(message);
  }

  m_nestingLvl = 0;
  m_nCmds = 0;
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
