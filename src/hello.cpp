#include "hello.h"
#include "version.h"

#include <cstdlib>
#include <regex>
#include <string>

#include <iostream>

int version::version() {
  const std::string version(PROJECT_VERSION);
  std::regex rgx("([0-9]+)$");
  std::smatch match;

  int ver = 0;
  if (std::regex_search(version.begin(), version.end(), match, rgx))
    ver = stoi(match[match.size() - 1]);

  return ver;
}
