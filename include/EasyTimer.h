#include <chrono>
#include <iostream>
typedef std::chrono::time_point<std::chrono::steady_clock> TimeType;
typedef std::chrono::duration<double> DurationType;

namespace easytime{
    TimeType getPresent();
}

class EasyTimer {
	std::chrono::time_point<std::chrono::steady_clock> timer;
public:
	EasyTimer();
	~EasyTimer();
	double tick();
};
