#pragma once
#include <string>

namespace Color {
const std::string reset{"\033[0m"};
const std::string black{"\033[30m"};
const std::string red{"\033[31m"};
const std::string green{"\033[32m"};
const std::string yellow{"\033[33m"};
const std::string blue{"\033[34m"};
const std::string magenta{"\033[35m"};
const std::string cyan{"\033[36m"};
const std::string white{"\033[37m"};
const std::string golden{"\033[93m"};

const std::string bgBlack{"\033[40m"};
const std::string bgRed{"\033[41m"};
const std::string bgGreen{"\033[42m"};
const std::string bgYellow{"\033[43m"};
const std::string bgBlue{"\033[44m"};
const std::string bgMagenta{"\033[45m"};
const std::string bgCyan{"\033[46m"};
const std::string bgWhite{"\033[47m"};

const std::string bold{"\033[1m"};
const std::string underline{"\033[4m"};
const std::string italic{"\033[3m"};
}