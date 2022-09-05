//
// Created by JinHai on 2022/9/5.
//

#include <sstream>

#include "profiler.h"

namespace infinity {

void
Profiler::Begin() {
    finished_ = false;
    begin_ts_ = Now();
}

void
Profiler::End() {
    end_ts_ = Now();
    finished_ = true;
}

std::chrono::nanoseconds
Profiler::ElapsedInternal() const {
    auto now = finished_ ? end_ts_ : Now();
    return now - begin_ts_;
}

std::string
Profiler::ElapsedToString() const {
    auto duration = this->ElapsedInternal();
    std::stringstream ss;
    if(duration.count() <= 1000) {
        ss << duration.count() << "ns";
    } else if(duration.count() <= 1000*1000) {
        ss << std::chrono::duration_cast<std::chrono::microseconds>(duration).count() << "us";
    } else if(duration.count() <= 1000*1000*1000) {
        ss << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms";
    } else {
        ss << std::chrono::duration_cast<std::chrono::seconds>(duration).count() << "s";
    }
    return ss.str();
}

}