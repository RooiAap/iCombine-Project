
#ifndef TEST_CASE_TYPES_H
#define TEST_CASE_TYPES_H

#include <string>
#include <vector>

typedef struct test_result
{
    std::string file;
    std::string test_date;
    std::string test_time;
    std::string test_outcome;
    bool valid = true;
}test_result;

typedef struct test
{
    std::string test_name;
    std::vector<test_result> results;
    bool used = false;
}test;

typedef struct group
{
    std::string group_name;
    std::vector<test> tests;
}group;

#endif // TEST_CASE_TYPES_H
