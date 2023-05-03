
#include "collectiontreewidgetitem.h"

collectionTreeWidgetItem::collectionTreeWidgetItem(const card_collection &c, QTreeWidgetItem *parent): QTreeWidgetItem(parent, nullptr, 2)
{
    this->collection.collection_name = c.collection_name;
    this->collection.tests = c.tests;

    this->total_tests = this->tests_passed = this->tests_failed = 0;

    for(const auto &t: this->collection.tests){
        this->total_tests++;
        if(t.results.back().test_outcome == "passed"){
            this->tests_passed++;
        }else if(t.results.back().test_outcome == "failed"){
            this->tests_failed++;
        }
    }

    this->setText(0, QString::fromStdString(this->collection.collection_name));
    this->setText(3, (QString::number(this->total_tests) + " Total Tests"));
    this->setText(4, (QString::number(this->tests_passed) + " Tests Passed"));
    this->setText(5, (QString::number(this->tests_failed) + " Tests Failed"));

    this->setExpanded(false);
}

card_collection collectionTreeWidgetItem::getCollection() const
{
    return this->collection;
}
