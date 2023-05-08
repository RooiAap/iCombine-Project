
#ifndef VERSION_H
#define VERSION_H

#include <QVersionNumber>

#define VERSION_MAJOR 1
#define VERSION_MINOR 4
#define VERSION_PATCH 0

// 1.0.1: Added user input validation
// 1.1.0: Added test grouping for summary data
// 1.1.1: Added summary data
// 1.1.2: Fixed test pass criteria
// 1.2.0: Added file inspection functionality
// 1.3.0: Added export to csv functionality
// 1.3.1: Minor UI fixes
// 1.3.2: Improved test grouping
// 1.3.3: Finalized data export
// 1.4.0: Added summary data visualization

QVersionNumber version(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

#endif // VERSION_H
