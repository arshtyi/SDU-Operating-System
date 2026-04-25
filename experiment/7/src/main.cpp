#include "pm.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

namespace {

void usage(const char* prog) {
    std::cerr << "usage: " << prog << " [-x|-X] [pid]\n";
}

bool parse_pid(const std::string& s, pid_t& pid) {
    char* end = nullptr;
    long v = std::strtol(s.c_str(), &end, 10);
    if (end == nullptr || *end != '\0' || v <= 0) {
        return false;
    }
    pid = static_cast<pid_t>(v);
    return true;
}

}

int main(int argc, char* argv[]) {
    ViewMode mode = ViewMode::Basic;
    pid_t pid = getpid();

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    for (std::size_t i = 0; i < args.size(); ++i) {
        const std::string& a = args[i];
        if (a == "-x") {
            mode = ViewMode::X;
            continue;
        }
        if (a == "-X") {
            mode = ViewMode::XUpper;
            continue;
        }
        if (!parse_pid(a, pid)) {
            usage(argv[0]);
            return 1;
        }
    }

    std::vector<MapItem> items;
    std::string err;
    if (!read_maps(pid, items, err)) {
        std::cerr << err << ": " << std::strerror(errno) << "\n";
        return 1;
    }

    if (mode != ViewMode::Basic) {
        std::string smap_err;
        if (!read_smaps(pid, items, smap_err)) {
            std::cerr << smap_err << ": " << std::strerror(errno) << "\n";
            return 1;
        }
    }

    print_maps(pid, items, mode);
    return 0;
}
