
#ifndef matrix_h
#define matrix_h


#include <iostream>
#include <vector>


class ProxyRow
{
    size_t _cols;
    std::vector<int> _row;
    
public:
    ProxyRow (size_t cols = 0) : _cols(cols) {
        _row = std::vector<int> (cols);
    }
    
    int& operator[] (size_t idx)
    {
        if (idx < 0 || idx >= _cols)
            throw std::out_of_range("");
        return _row[idx];
    }
    
    const int& operator[] (size_t idx) const
    {
        if (idx < 0 || idx >= _cols)
            throw std::out_of_range("");
        return _row[idx];
    }
};



class Matrix
{
    const size_t _rows, _cols;
    std::vector<ProxyRow> _data;
    
public:
    Matrix (size_t rows, size_t cols) : _rows (rows), _cols (cols)
    {
        _data = std::vector<ProxyRow> (rows, ProxyRow(_cols));
    }
    
    ProxyRow& operator [] (size_t row)
    {
        if (row < 0 || row >= _rows)
            throw std::out_of_range("");
        
        // первый оператор [] возвращает ссылку на нужную строку
        return _data[row];
    }
    
    const ProxyRow& operator [] (size_t row) const
    {
        if (row < 0 || row >= _rows)
            throw std::out_of_range("");
        return _data[row];
    }

    size_t getRows() const
    {
        return _rows;
    }
    
    size_t getColumns() const
    {
        return _cols;
    }
    
    Matrix& operator *= (int multiplier)
    {
        for (size_t i = 0; i < _rows; i++)
            for (size_t j = 0; j <  _cols; j++)
                _data[i][j] *= multiplier;
        return *this;
    }
    
    bool operator == (const Matrix& m2) // тк sizeof(Matrix) (40) > sizeof(Matrix*) (8)
    {
        if (_rows != m2.getRows() || _cols != m2.getColumns())
            return false;
        for (size_t i = 0; i < _rows; i++)
            for (size_t j = 0; j <  _cols; j++)
                if (_data[i][j] != m2[i][j])
                    return false;
        return true;
    }
    
    bool operator != (const Matrix& m2)
    {
        return !(*this == m2);
    }
    
};


#endif

