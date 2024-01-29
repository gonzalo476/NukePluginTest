#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iomanip>
#include <sstream>

std::string formatDouble(double value, int precision = 10);

std::vector<const char*> getNodeKnobs();

#endif