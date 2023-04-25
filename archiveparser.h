#ifndef ARCHIVEPARSER_H
#define ARCHIVEPARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "test_case_types.h"

namespace fs = std::filesystem;

class archiveParser
{
public:
    archiveParser();
    archiveParser(std::string archiveDirectoryPath);
    ~archiveParser();

    void setDirectoryPath(const std::string archiveDirectoryPath);
    std::string getDirectoryPath() const;
    std::vector<group> parse();

private:
    std::string archiveDirectoryPath;
private:
    test_result check_test(fs::path test);
    std::vector<test_result> check_test_case(fs::path folder);
};

#endif // ARCHIVEPARSER_H
