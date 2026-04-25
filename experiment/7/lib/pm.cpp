#include "pm.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

std::string ltrim(const std::string& s) {
    std::size_t i = 0;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])) != 0) {
        ++i;
    }
    return s.substr(i);
}

bool parse_range(const std::string& token, std::uint64_t& start, std::uint64_t& end) {
    const std::size_t p = token.find('-');
    if (p == std::string::npos) {
        return false;
    }
    const std::string a = token.substr(0, p);
    const std::string b = token.substr(p + 1);
    char* pa = nullptr;
    char* pb = nullptr;
    start = std::strtoull(a.c_str(), &pa, 16);
    end = std::strtoull(b.c_str(), &pb, 16);
    return pa != nullptr && pb != nullptr && *pa == '\0' && *pb == '\0' && end >= start;
}

bool parse_kb_line(const std::string& line, const char* key, long& dst) {
    const std::size_t klen = std::strlen(key);
    if (line.size() < klen || line.compare(0, klen, key) != 0) {
        return false;
    }
    std::istringstream iss(line.substr(klen));
    long v = 0;
    iss >> v;
    if (!iss.fail()) {
        dst = v;
        return true;
    }
    return false;
}

bool looks_like_header(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    if (!(iss >> token)) {
        return false;
    }
    std::uint64_t s = 0;
    std::uint64_t e = 0;
    return parse_range(token, s, e);
}

void print_basic_row(const MapItem& it) {
    std::printf("%016llx %8zu %4s %08llx %5s %s\n",
                static_cast<unsigned long long>(it.start),
                it.size_kb,
                it.perms.c_str(),
                static_cast<unsigned long long>(it.offset),
                it.dev.c_str(),
                it.path.empty() ? "[ anon ]" : it.path.c_str());
}

void print_x_row(const MapItem& it) {
    const long dirty = it.shared_dirty_kb + it.private_dirty_kb;
    std::printf("%016llx %8zu %8ld %8ld %4s %s\n",
                static_cast<unsigned long long>(it.start),
                it.size_kb,
                it.rss_kb,
                dirty,
                it.perms.c_str(),
                it.path.empty() ? "[ anon ]" : it.path.c_str());
}

void print_X_row(const MapItem& it) {
    std::printf("%016llx %8zu %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %8ld %4s %s\n",
                static_cast<unsigned long long>(it.start),
                it.size_kb,
                it.rss_kb,
                it.pss_kb,
                it.shared_clean_kb,
                it.shared_dirty_kb,
                it.private_clean_kb,
                it.private_dirty_kb,
                it.referenced_kb,
                it.anonymous_kb,
                it.anon_huge_pages_kb,
                it.swap_kb,
                it.kernel_page_size_kb,
                it.mmu_page_size_kb,
                it.locked_kb,
                it.perms.c_str(),
                it.path.empty() ? "[ anon ]" : it.path.c_str());
}

} 

bool read_maps(pid_t pid, std::vector<MapItem>& out, std::string& err) {
    std::ostringstream path;
    path << "/proc/" << pid << "/maps";
    std::ifstream fin(path.str().c_str());
    if (!fin.is_open()) {
        err = "cannot open " + path.str();
        return false;
    }

    out.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string range;
        MapItem it;
        if (!(iss >> range >> it.perms)) {
            continue;
        }

        std::string off;
        if (!(iss >> off >> it.dev >> it.inode)) {
            continue;
        }

        std::uint64_t start = 0;
        std::uint64_t end = 0;
        if (!parse_range(range, start, end)) {
            continue;
        }
        char* poff = nullptr;
        it.offset = std::strtoull(off.c_str(), &poff, 16);
        if (poff == nullptr || *poff != '\0') {
            continue;
        }

        std::string rest;
        std::getline(iss, rest);
        it.path = ltrim(rest);
        it.start = start;
        it.end = end;
        it.size_kb = static_cast<std::size_t>((end - start) / 1024ULL);
        out.push_back(it);
    }
    return true;
}

bool read_smaps(pid_t pid, std::vector<MapItem>& items, std::string& err) {
    std::ostringstream path;
    path << "/proc/" << pid << "/smaps";
    std::ifstream fin(path.str().c_str());
    if (!fin.is_open()) {
        err = "cannot open " + path.str();
        return false;
    }

    std::string line;
    std::size_t idx = static_cast<std::size_t>(-1);
    while (std::getline(fin, line)) {
        if (looks_like_header(line)) {
            if (idx == static_cast<std::size_t>(-1)) {
                idx = 0;
            } else {
                ++idx;
            }
            continue;
        }
        if (idx >= items.size()) {
            continue;
        }

        MapItem& it = items[idx];
        if (parse_kb_line(line, "Rss:", it.rss_kb)) continue;
        if (parse_kb_line(line, "Pss:", it.pss_kb)) continue;
        if (parse_kb_line(line, "Shared_Clean:", it.shared_clean_kb)) continue;
        if (parse_kb_line(line, "Shared_Dirty:", it.shared_dirty_kb)) continue;
        if (parse_kb_line(line, "Private_Clean:", it.private_clean_kb)) continue;
        if (parse_kb_line(line, "Private_Dirty:", it.private_dirty_kb)) continue;
        if (parse_kb_line(line, "Referenced:", it.referenced_kb)) continue;
        if (parse_kb_line(line, "Anonymous:", it.anonymous_kb)) continue;
        if (parse_kb_line(line, "AnonHugePages:", it.anon_huge_pages_kb)) continue;
        if (parse_kb_line(line, "Swap:", it.swap_kb)) continue;
        if (parse_kb_line(line, "KernelPageSize:", it.kernel_page_size_kb)) continue;
        if (parse_kb_line(line, "MMUPageSize:", it.mmu_page_size_kb)) continue;
        if (parse_kb_line(line, "Locked:", it.locked_kb)) continue;
    }
    return true;
}

void print_maps(pid_t pid, const std::vector<MapItem>& items, ViewMode mode) {
    std::printf("pid = %d\n", static_cast<int>(pid));

    std::size_t total_size = 0;
    long total_rss = 0;
    long total_pss = 0;
    long total_sc = 0;
    long total_sd = 0;
    long total_pc = 0;
    long total_pd = 0;
    long total_ref = 0;
    long total_anon = 0;
    long total_ahp = 0;
    long total_swap = 0;
    long total_kps = 0;
    long total_mps = 0;
    long total_lock = 0;

    if (mode == ViewMode::Basic) {
        std::printf("Address           Kbytes Perm Offset   Device Mapping\n");
    } else if (mode == ViewMode::X) {
        std::printf("Address           Kbytes      RSS    Dirty Mode Mapping\n");
    } else {
        std::printf("Address           Kbytes      RSS      PSS   SharedC   SharedD  PrivateC  PrivateD Referenced Anonymous AnonHuge     Swap   KPage   MPage   Locked Mode Mapping\n");
    }

    for (std::size_t i = 0; i < items.size(); ++i) {
        const MapItem& it = items[i];
        total_size += it.size_kb;
        total_rss += it.rss_kb;
        total_pss += it.pss_kb;
        total_sc += it.shared_clean_kb;
        total_sd += it.shared_dirty_kb;
        total_pc += it.private_clean_kb;
        total_pd += it.private_dirty_kb;
        total_ref += it.referenced_kb;
        total_anon += it.anonymous_kb;
        total_ahp += it.anon_huge_pages_kb;
        total_swap += it.swap_kb;
        total_kps += it.kernel_page_size_kb;
        total_mps += it.mmu_page_size_kb;
        total_lock += it.locked_kb;

        if (mode == ViewMode::Basic) {
            print_basic_row(it);
        } else if (mode == ViewMode::X) {
            print_x_row(it);
        } else {
            print_X_row(it);
        }
    }

    if (mode == ViewMode::Basic) {
        std::printf("total %zu\n", total_size);
    } else if (mode == ViewMode::X) {
        std::printf("total kB %zu rss %ld dirty %ld\n", total_size, total_rss, total_sd + total_pd);
    } else {
        std::printf("total kB=%zu rss=%ld pss=%ld shared_clean=%ld shared_dirty=%ld private_clean=%ld private_dirty=%ld referenced=%ld anonymous=%ld anon_huge=%ld swap=%ld kernel_ps=%ld mmu_ps=%ld locked=%ld\n",
                    total_size,
                    total_rss,
                    total_pss,
                    total_sc,
                    total_sd,
                    total_pc,
                    total_pd,
                    total_ref,
                    total_anon,
                    total_ahp,
                    total_swap,
                    total_kps,
                    total_mps,
                    total_lock);
    }
}
