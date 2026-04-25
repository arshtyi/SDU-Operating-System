#include "rail.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

struct TrainArgs {
    int id;
    Dir dir;
    RailMon* mon;
    int arrive_ms_min;
    int arrive_ms_max;
    int pass_ms_min;
    int pass_ms_max;
};

static void run_train(TrainArgs args) {
    std::mt19937 rng(static_cast<unsigned int>(
        std::chrono::steady_clock::now().time_since_epoch().count() + args.id * 997));
    std::uniform_int_distribution<int> arrive_dist(args.arrive_ms_min, args.arrive_ms_max);
    std::uniform_int_distribution<int> pass_dist(args.pass_ms_min, args.pass_ms_max);

    std::this_thread::sleep_for(std::chrono::milliseconds(arrive_dist(rng)));
    args.mon->enter(args.dir, args.id);
    std::this_thread::sleep_for(std::chrono::milliseconds(pass_dist(rng)));
    args.mon->leave(args.dir, args.id);
}

int main(int argc, char* argv[]) {
    int north_n = 8;
    int south_n = 8;
    int max_batch = 3;

    if (argc > 1) {
        north_n = std::atoi(argv[1]);
    }
    if (argc > 2) {
        south_n = std::atoi(argv[2]);
    }
    if (argc > 3) {
        max_batch = std::atoi(argv[3]);
    }

    if (north_n < 0) north_n = 0;
    if (south_n < 0) south_n = 0;
    if (max_batch < 1) max_batch = 1;

    std::cout << "rail monitor demo: north=" << north_n << " south=" << south_n
              << " max_batch=" << max_batch << "\n";

    RailMon mon(max_batch);
    std::vector<std::thread> ts;
    ts.reserve(static_cast<size_t>(north_n + south_n));

    int id = 1;
    for (int i = 0; i < north_n; ++i) {
        TrainArgs args{id++, Dir::NORTH, &mon, 50, 600, 180, 650};
        ts.emplace_back(run_train, args);
    }
    for (int i = 0; i < south_n; ++i) {
        TrainArgs args{id++, Dir::SOUTH, &mon, 50, 600, 180, 650};
        ts.emplace_back(run_train, args);
    }

    for (size_t i = 0; i < ts.size(); ++i) {
        ts[i].join();
    }

    std::cout << "all trains passed\n";
    return 0;
}
