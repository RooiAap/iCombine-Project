
#ifndef VERSION_H
#define VERSION_H

#include <QVersionNumber>

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 2

// 1.0.1: Added user input validation
// 1.1.0: Added test grouping for summary data
// 1.1.1: Added summary data
// 1.1.2: Fixed test pass criteria

QVersionNumber version(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

#endif // VERSION_H
