#include "rail.h"

#include <chrono>
#include <iostream>
#include <thread>

RailMon::RailMon(int max_batch)
    : waiting_{0, 0},
      on_track_(0),
      active_dir_(-1),
      phase_count_(0),
      max_batch_(max_batch < 1 ? 1 : max_batch),
      prefer_dir_(0) {}

int RailMon::idx(Dir dir) {
    return static_cast<int>(dir);
}

const char* RailMon::name(Dir dir) {
    return dir == Dir::NORTH ? "NORTH->SOUTH" : "SOUTH->NORTH";
}

bool RailMon::can_enter(Dir dir) const {
    const int d = idx(dir);
    const int o = 1 - d;

    if (on_track_ > 0) {
        if (active_dir_ != d) {
            return false;
        }
        if (waiting_[o] > 0 && phase_count_ >= max_batch_) {
            return false;
        }
        return true;
    }

    if (active_dir_ == -1) {
        if (waiting_[0] > 0 && waiting_[1] > 0) {
            return d == prefer_dir_;
        }
        return true;
    }

    if (active_dir_ != d) {
        return false;
    }
    if (waiting_[o] > 0 && phase_count_ >= max_batch_) {
        return false;
    }
    return true;
}

void RailMon::wake_open_dir_unlocked() {
    if (active_dir_ == -1) {
        cv_[0].notify_all();
        cv_[1].notify_all();
        return;
    }
    cv_[active_dir_].notify_all();
}

void RailMon::enter(Dir dir, int id) {
    const int d = idx(dir);
    const int o = 1 - d;
    std::unique_lock<std::mutex> lk(mtx_);
    ++waiting_[d];

    while (!can_enter(dir)) {
        cv_[d].wait(lk);
    }

    --waiting_[d];
    if (on_track_ == 0) {
        if (active_dir_ == -1) {
            active_dir_ = d;
            phase_count_ = 0;
        } else if (active_dir_ != d) {
            active_dir_ = d;
            phase_count_ = 0;
        }
    }
    ++on_track_;
    ++phase_count_;

    std::cout << "[ENTER] train#" << id << " " << name(dir)
              << " on_track=" << on_track_ << " waiting=(" << waiting_[0] << ","
              << waiting_[1] << ")\n";

    if (waiting_[o] > 0 && phase_count_ >= max_batch_) {
        cv_[d].notify_all();
    }
}

void RailMon::leave(Dir dir, int id) {
    const int d = idx(dir);
    const int o = 1 - d;
    std::unique_lock<std::mutex> lk(mtx_);

    --on_track_;
    std::cout << "[LEAVE] train#" << id << " " << name(dir)
              << " on_track=" << on_track_ << " waiting=(" << waiting_[0] << ","
              << waiting_[1] << ")\n";

    if (on_track_ == 0) {
        if (waiting_[0] == 0 && waiting_[1] == 0) {
            active_dir_ = -1;
            phase_count_ = 0;
        } else if (waiting_[d] == 0 && waiting_[o] > 0) {
            active_dir_ = o;
            phase_count_ = 0;
            prefer_dir_ = d;
        } else if (waiting_[d] > 0 && waiting_[o] == 0) {
            active_dir_ = d;
            phase_count_ = 0;
            prefer_dir_ = o;
        } else {
            if (phase_count_ >= max_batch_) {
                active_dir_ = o;
            } else {
                active_dir_ = prefer_dir_;
            }
            phase_count_ = 0;
            prefer_dir_ = 1 - active_dir_;
        }
    }

    wake_open_dir_unlocked();
}
