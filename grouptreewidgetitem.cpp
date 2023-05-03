
#include "grouptreewidgetitem.h"

#include <QString>

groupTreeWidgetItem::groupTreeWidgetItem(const group &g, QTreeWidget *parentTree): QTreeWidgetItem(parentTree, nullptr, 1)
{
    this->card_group.group_name = g.group_name;
    this->card_group.contact = g.contact;
    this->card_group.contactless = g.contactless;

    this->total_tests = this->tests_passed = this->tests_failed = 0;

    for(const auto &t: card_group.contact.tests){
        this->total_tests++;
        if(t.results.back().test_outcome == "passed"){
            this->tests_passed++;
        }else if(t.results.back().test_outcome == "failed"){
            this->tests_failed++;
        }
    }
    for(const auto &t: card_group.contactless.tests){
        this->total_tests++;
        if(t.results.back().test_outcome == "passed"){
            this->tests_passed++;
        }else if(t.results.back().test_outcome == "failed"){
            this->tests_failed++;
        }
    }

    this->setText(0, QString::fromStdString(this->card_group.group_name));
    this->setText(3, (QString::number(this->total_tests) + " Total Tests"));
    this->setText(4, (QString::number(this->tests_passed) + " Tests Passed"));
    this->setText(5, (QString::number(this->tests_failed) + " Tests Failed"));

    this->setExpanded(false);
}

groupTreeWidgetItem::~groupTreeWidgetItem()
{

}

group groupTreeWidgetItem::getCardGroup() const
{
    return this->card_group;
}

int groupTreeWidgetItem::getTotalTests() const
{
    return this->total_tests;
}

int groupTreeWidgetItem::getTestsPassed() const
{
    return this->tests_passed;
}

int groupTreeWidgetItem::getTestsFailed() const
{
    return this->tests_failed;
}


