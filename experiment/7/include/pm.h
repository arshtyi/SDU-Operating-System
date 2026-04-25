#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <sys/types.h>

struct MapItem {
    std::uint64_t start = 0;
    std::uint64_t end = 0;
    std::uint64_t offset = 0;
    std::size_t size_kb = 0;
    std::string perms;
    std::string dev;
    unsigned long inode = 0;
    std::string path;

    long rss_kb = 0;
    long pss_kb = 0;
    long shared_clean_kb = 0;
    long shared_dirty_kb = 0;
    long private_clean_kb = 0;
    long private_dirty_kb = 0;
    long referenced_kb = 0;
    long anonymous_kb = 0;
    long anon_huge_pages_kb = 0;
    long swap_kb = 0;
    long kernel_page_size_kb = 0;
    long mmu_page_size_kb = 0;
    long locked_kb = 0;
};

enum class ViewMode {
    Basic,
    X,
    XUpper
};

bool read_maps(pid_t pid, std::vector<MapItem>& out, std::string& err);
bool read_smaps(pid_t pid, std::vector<MapItem>& items, std::string& err);
void print_maps(pid_t pid, const std::vector<MapItem>& items, ViewMode mode);
