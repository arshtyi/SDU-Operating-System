#ifndef RAIL_H
#define RAIL_H

#include <condition_variable>
#include <mutex>

enum class Dir { NORTH = 0, SOUTH = 1 };

class RailMon {
public:
    explicit RailMon(int max_batch);
    void enter(Dir dir, int id);
    void leave(Dir dir, int id);

private:
    static int idx(Dir dir);
    static const char* name(Dir dir);
    bool can_enter(Dir dir) const;
    void wake_open_dir_unlocked();

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_[2];
    int waiting_[2];

    int on_track_;
    int active_dir_;   // -1 idle, 0 north, 1 south
    int phase_count_;  // how many trains entered in this phase
    int max_batch_;
    int prefer_dir_;   // preferred dir when both sides are waiting
};

#endif
