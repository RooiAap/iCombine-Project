
#ifndef COLLECTIONTREEWIDGETITEM_H
#define COLLECTIONTREEWIDGETITEM_H

#include <QTreeWidgetItem>

#include <QWidget>

#include "test_case_types.h"

class collectionTreeWidgetItem : public QTreeWidgetItem
{
public:
    collectionTreeWidgetItem(const card_collection &c, QTreeWidgetItem *parent);

    card_collection getCollection() const;

    int getTotalTests() const;
    int getTestsPassed() const;
    int getTestsFailed() const;

private:
    card_collection collection;

    int total_tests;
    int tests_passed;
    int tests_failed;
};

#endif // COLLECTIONTREEWIDGETITEM_H
