#ifndef TABLE_H
#define TABLE_H

#include "column.hpp"
#include <vector>
#include <cstdint>
#include <iostream>
#include <memory>

class Table{
    public:
        std::vector<std::unique_ptr<Column>> columns;
        Table(size_t number_of_columns);

        // this is used when the first query fully copies the data (e.g., Quasii, Cracking KD-Tree)
        Table(Table *table_to_copy);

        // This just allocates the memory, no copy is done (e.g., PI)
        Table(size_t number_of_columns, size_t number_of_rows);

        inline void push_back(double value);

        inline void maybe_push_back(double value,size_t column_idx, int maybe);
        ~Table();

        // File handling methods
        static std::unique_ptr<Table> read_file(std::string path);
        static std::unique_ptr<Table> read_file_dinos(std::string path, int dataSize, int dimensions);

        void save_file(std::string path);

        // Cracking methods
        std::pair<size_t, size_t> CrackTableInThree(size_t low, size_t high, float key_left, float key_right, size_t c);

        size_t CrackTable(size_t low, size_t high, float element, size_t c);

        std::unique_ptr<float[]> materialize_row(size_t row_index);

        void append(float* row);

        size_t row_count() const;

        size_t col_count() const;

        void exchange(size_t index1, size_t index2);
};

#endif // TABLE_H
