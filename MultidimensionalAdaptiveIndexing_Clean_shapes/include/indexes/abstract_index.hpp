#ifndef ABSTRACT_INDEX_H
#define ABSTRACT_INDEX_H

#include "measurements.hpp"
#include "query.hpp"
#include "table.hpp"
#include <string>
#include <cassert>

class AbstractIndex
{
protected:
    // Table with copy of the data
    std::unique_ptr<Table> table;
    int64_t n_tuples_scanned_before_adapting;
    std::unique_ptr<Table> table_max;
public:
    // Class to keep track of the time/index measurements
    std::unique_ptr<Measurements> measurements;

    AbstractIndex()
    : n_tuples_scanned_before_adapting(0),
      measurements(std::make_unique<Measurements>()){}
    virtual ~AbstractIndex(){}
    virtual void initialize(Table *table_to_copy) = 0;
    virtual void initialize_max_table(Table *table_to_copy);
    virtual void adapt_index(Query &query) = 0;
    // virtual std::unique_ptr<Table> range_query(Query &query) = 0;
    virtual std::unique_ptr<Table> range_query(Query &query, Query &original_query) = 0;
    virtual std::string name() = 0;
    virtual void draw_index(std::string){}
    virtual bool sanity_check(){return true;}
};

inline void AbstractIndex::initialize_max_table(Table *table_to_copy){
        table_max = std::make_unique<Table>(table_to_copy);
    }

#endif // ABSTRACT_INDEX_H
