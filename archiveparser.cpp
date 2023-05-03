#include "archiveparser.h"

#include <QDebug>
#include <QString>
#include <QDir>

#include <fstream>

#include <set>
#include <regex>

archiveParser::archiveParser()
{
    this->archiveDirectoryPath = "";
}

archiveParser::archiveParser(std::string archiveDirectoryPath)
{
    this->archiveDirectoryPath = archiveDirectoryPath;
}

archiveParser::~archiveParser()
{

}

void archiveParser::setDirectoryPath(const std::string archiveDirectoryPath)
{
    this->archiveDirectoryPath = archiveDirectoryPath;
}

std::string archiveParser::getDirectoryPath() const
{
    return this->archiveDirectoryPath;
}

std::vector<group> archiveParser::parse()
{
    std::vector<test> test_cases;
    for (const auto& entry : fs::directory_iterator(fs::path(this->archiveDirectoryPath + "\\BttFiles\\BttTestResults\\reports"))) {
        if (!isalpha(entry.path().filename().string()[0]) || !fs::is_directory(entry.path())) {
            continue;
        }
        if(entry.path().filename().string() == "ApduInterpretation"){
            continue;
        }
        test t;
        t.test_name = entry.path().filename().string();
        try
        {
            t.results = check_test_case(entry.path());
            test_cases.push_back(t);
        }
        catch (const char* e)
        {
            std::cerr << e << '\n';
        }
    }

    for(const auto &t: test_cases){
        qDebug().noquote() << QString::fromStdString(t.test_name);
    }

    group mastercard;
    mastercard.group_name = "MASTERCARD";
    mastercard.contact.collection_name = "Contact";
    mastercard.contactless.collection_name = "Contactless";
    std::regex mastercard_contact_pattern("(M-TIP)");
    std::regex mastercard_contactless_pattern("(MCD|MCM|MSI|COM)");
    for(auto &t: test_cases){
        if(!t.used){
            if(std::regex_search(t.test_name, mastercard_contact_pattern)){
                mastercard.contact.tests.push_back(t);
                t.used = true;
            }else if(std::regex_search(t.test_name, mastercard_contactless_pattern)){
                mastercard.contactless.tests.push_back(t);
                t.used = true;
            }
        }else{
            continue;
        }
    }

    group visa;
    visa.group_name = "VISA";
    visa.contact.collection_name = "Contact";
    visa.contactless.collection_name = "Contactless";
    std::regex visa_contact_pattern("(TC\.0)");
    std::regex visa_contactless_pattern("(TC\.1)");
    for(auto &t: test_cases){
        if(!t.used){
            if(std::regex_search(t.test_name, visa_contact_pattern)){
                visa.contact.tests.push_back(t);
                t.used = true;
            }else if(std::regex_search(t.test_name, visa_contactless_pattern)){
                visa.contactless.tests.push_back(t);
                t.used = true;
            }
        }else{
            continue;
        }
    }

    group amex;
    amex.group_name = "AMEX";
    amex.contact.collection_name = "Contact";
    amex.contactless.collection_name = "Contactless";
    std::regex amex_contact_pattern("(AXP_EMV|AXP_RCP|AXP_MAG)");
    std::regex amex_contactless_pattern("(AXP_EP)");
    for(auto &t: test_cases){
        if(!t.used){
            if(std::regex_search(t.test_name, amex_contact_pattern)){
                amex.contact.tests.push_back(t);
                t.used = true;
            }else if(std::regex_search(t.test_name, amex_contactless_pattern)){
                amex.contactless.tests.push_back(t);
                t.used = true;
            }
        }else{
            continue;
        }
    }

    group diners;
    diners.group_name = "DINERS";
    diners.contact.collection_name = "Contact";
    diners.contactless.collection_name = "Contactless";
    std::regex diners_contact_pattern("(DGN_Connect_L3_CT)");
    std::regex diners_contactless_pattern("(DGN_Connect_L3_CL)");
    for(auto &t: test_cases){
        if(!t.used){
            if(std::regex_search(t.test_name, diners_contact_pattern)){
                diners.contact.tests.push_back(t);
                t.used = true;
            }else if(std::regex_search(t.test_name, diners_contactless_pattern)){
                diners.contactless.tests.push_back(t);
                t.used = true;
            }
        }else{
            continue;
        }
    }

    group jcb;
    jcb.group_name = "JCB";
    jcb.contact.collection_name = "Contact";
    jcb.contactless.collection_name = "Contactless";
    std::regex jcb_contact_pattern("TCI_CT");
    std::regex jcb_contactless_pattern("TCI_CL");
    for(auto &t: test_cases){
        if(!t.used){
            if(std::regex_search(t.test_name, jcb_contact_pattern)){
                jcb.contact.tests.push_back(t);
                t.used = true;
            }else if(std::regex_search(t.test_name, jcb_contactless_pattern)){
                jcb.contactless.tests.push_back(t);
                t.used = true;
            }
        }else{
            continue;
        }
    }

    std::vector<group> groups = {mastercard, visa, amex, diners, jcb};

    return groups;
}

test_result archiveParser::check_test(fs::path test)
{
    std::vector<fs::path> items;
    for (const auto& entry : fs::directory_iterator(test)) {
        items.push_back(entry);
    }

    if (items.size() == 0) {
        test_result t;
        t.valid = false;
        return t;
    }

    std::vector<fs::path> logs;
    for (auto i = items.cbegin(); i != items.cend(); i++) {
        if (i->extension().string() == ".log") {
            if (i->filename() == "TestSuite.log") {
                continue;
            }
            logs.push_back(*i);
        }
    }

    std::string filename = logs.at(0).string();

    std::string file_data;
    std::ifstream log_file(filename);
    if (log_file.is_open()) {
        std::getline(log_file, file_data);
    }
    else {
        throw "Error: Unable to open file!";
    }

    std::stringstream stream(file_data);
    std::string segment;
    std::vector<std::string> data_segments;
    while (std::getline(stream, segment, '\t')) {
        data_segments.push_back(segment);
    }

    test_result r;    
    std::string date_time = data_segments.at(4);
    std::stringstream stream2(date_time);
    std::vector<std::string> date_time_segments;
    std::string seg;
    while(std::getline(stream2, seg, ' ')){
        date_time_segments.push_back(seg);
    }
    r.file = fs::absolute(filename).lexically_normal().string();
    r.test_date = date_time_segments.at(0);
    r.test_time = date_time_segments.at(1);
    r.test_outcome = data_segments.at(2);

    return r;
}


std::vector<test_result> archiveParser::check_test_case(fs::path folder)
{
    if (!fs::exists(folder) || !fs::is_directory(folder)) {
        throw "Error: Invalid test case path";
    }

    int item_count = 0;
    std::vector<fs::path> tests;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (fs::is_directory(entry.path())) {
            item_count++;
            if (isdigit(entry.path().filename().string()[0])) {
                tests.push_back(entry);
            }
        }
    }

    std::vector<test_result> results;
    if (item_count != tests.size()) {
        std::vector<fs::path> internal_branches;
        for (const auto& entry : fs::directory_iterator(folder)) {
            if (fs::is_directory(entry.path())) {
                if (std::find(tests.begin(), tests.end(), entry.path().filename()) != tests.end()) {
                    continue;
                }
                else {
                    internal_branches.push_back(entry);
                }
            }
        }

        for (const auto& branch : internal_branches) {
            std::vector<test_result> branch_results = check_test_case(branch);
            results.insert(results.end(), branch_results.begin(), branch_results.end());
        }
    }

    for (const auto& test : tests) {
        if (fs::is_directory(test)) {
            test_result result = check_test(test);
            if (result.valid) results.push_back(result);
        }
    }

    return results;
}
