#ifndef ARCHIVEPARSER_H
#define ARCHIVEPARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

typedef struct test_result
{
    std::string test_date;
    std::string test_time;
    std::string test_outcome;
    bool valid = true;
}test_result;

typedef struct test
{
    std::string test_name;
    std::vector<test_result> results;
}test;

class archiveParser
{
public:
    archiveParser();
    archiveParser(std::string archiveDirectoryPath);
    ~archiveParser();

    void setDirectoryPath(const std::string archiveDirectoryPath);
    std::string getDirectoryPath() const;
    std::vector<test> parse();

private:
    std::string archiveDirectoryPath;
private:
    test_result check_test(fs::path test);
    std::vector<test_result> check_test_case(fs::path folder);
};

#endif // ARCHIVEPARSER_H
