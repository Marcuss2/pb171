#pragma once

#include "concepts.hpp"
#include "timers.hpp"

namespace StopWatch {

enum Resolution {
        MICROS,
        MILLIS,
        SECONDS,
    };

template <unsigned_integral T = uint32_t>
class StopWatch {
    T _time = 0;
    uint32_t _last_measure;
    Resolution _resolution;
    bool _running = false;
    
    uint32_t get_current_time() {
        switch (_resolution) {
            case Resolution::MICROS:
                return hal::micros();
            case Resolution::MILLIS:
            case Resolution::SECONDS:
                return hal::millis();
        }
        return 0;
    }
public:
    StopWatch(Resolution res = Resolution::MILLIS) : _resolution(res) {}
    
    void start() { 
        _running = true;
        _last_measure = get_current_time();
    }
    
    void stop() { _running = false; }
    
    bool isRunning() { return _running; }
    
    Resolution resolution() { return _resolution; }
    
    /*
     * Updates internal timer to prevent potential overflows
     * Call at least once per 70 minutes for micros and 50 days for millis/seconds
     */
    void poll() {
        if (!_running) return;
        auto curr = get_current_time();
        auto saved = curr;
        // Overflow happened, we need to compensate for it
        if (_last_measure > curr) {
            curr += __UINT32_MAX__ - _last_measure;
        } else {
            curr -= _last_measure;
        }
        if (_resolution == Resolution::SECONDS) {
            
            if (curr < 1000) {
                return;
            }
            // Forced integer multiplication
            _last_measure += (curr / 1000) * 1000;
            curr /= 1000;
        } else {
            _last_measure = saved;
        }
        _time += curr;
    }
    
    T elapsed() {
        poll();
        return _time;
    }
    
    void reset() {
        _time = 0;
        _last_measure = get_current_time();
    }
};

}
