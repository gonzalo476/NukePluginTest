#include "DDImage/Iop.h"
#include "DDImage/Knobs.h"
#include "Utils.h"
#include <vector>

std::string formatDouble(double value, int precision) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

std::vector<const char*> getNodeKnobs() {
    std::vector<const char*> translation = {
        "translate", 
        "rotate", 
        "scaling"
    };
    return translation;
}