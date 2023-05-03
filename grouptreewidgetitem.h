
#ifndef GROUPTREEWIDGETITEM_H
#define GROUPTREEWIDGETITEM_H

#include <QTreeWidgetItem>

#include <QWidget>

#include "test_case_types.h"

class groupTreeWidgetItem : public QTreeWidgetItem
{
public:
    groupTreeWidgetItem(const group &g, QTreeWidget *parentTree=nullptr);
    ~groupTreeWidgetItem();

    group getCardGroup() const;
    int getTotalTests() const;
    int getTestsPassed() const;
    int getTestsFailed() const;

private:
    group card_group;

    int total_tests;
    int tests_passed;
    int tests_failed;
};

#endif // GROUPTREEWIDGETITEM_H
