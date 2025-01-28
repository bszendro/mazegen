#pragma once

#include <string.h>
#include <assert.h>

class MatrixRow
{
public:
    char& operator[](int j)
    {
        assert(0 <= j && j < cols_);
        return m_[cols_ * row_ + j];
    }

private:
    friend class Matrix;
    MatrixRow(char *m, int cols, int row): m_(m), cols_(cols), row_(row) {}

    char* m_;
    int cols_;
    int row_;
};

class ConstMatrixRow
{
public:
    char operator[](int j) const
    {
        assert(0 <= j && j < cols_);
        return m_[cols_ * row_ + j];
    }

private:
    friend class Matrix;
    ConstMatrixRow(const char *m, int cols, int row): m_(m), cols_(cols), row_(row) {}

    const char* m_;
    int cols_;
    int row_;
};

class Matrix
{
public:
    Matrix(const Matrix&) = delete;
    Matrix& operator=(const Matrix&) = delete;

    Matrix(int rows, int cols, char init_value): rows_(rows), cols_(cols)
    {
        assert(rows > 0);
        assert(cols > 0);
        m_ = new char[rows * cols];
        memset(m_, init_value, rows * cols);
    }

    ~Matrix()
    {
        if (m_ != nullptr) {
            delete[] m_;
            m_ = nullptr;
        }
    }

    ConstMatrixRow operator[](int i) const
    {
        assert(0 <= i && i < rows_);
        return {m_, cols_, i};
    }

    MatrixRow operator[](int i)
    {
        assert(0 <= i && i < rows_);
        return {m_, cols_, i};
    }

    int rows() const { return rows_; }
    int cols() const { return cols_; }

private:
    int rows_;
    int cols_;
    char* m_;
};
