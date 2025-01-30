#pragma once

#include <string.h>
#include <assert.h>

template< typename T >
class MatrixRow
{
public:
    MatrixRow(T *m, int cols, int row): m_(m), cols_(cols), row_(row) {}

    T& operator[](int j)
    {
        assert(0 <= j && j < cols_);
        return m_[cols_ * row_ + j];
    }

private:
    T* m_;
    int cols_;
    int row_;
};

template< typename T >
class ConstMatrixRow
{
public:
    ConstMatrixRow(const T *m, int cols, int row): m_(m), cols_(cols), row_(row) {}

    T operator[](int j) const
    {
        assert(0 <= j && j < cols_);
        return m_[cols_ * row_ + j];
    }

private:
    const T* m_;
    int cols_;
    int row_;
};

template< typename T >
class Matrix
{
public:
    Matrix(const Matrix&) = delete;
    Matrix& operator=(const Matrix&) = delete;

    Matrix(int rows, int cols): rows_(rows), cols_(cols)
    {
        assert(rows > 0);
        assert(cols > 0);
        m_ = new T[rows * cols];
        memset(m_, 0, rows * cols * sizeof(T));
    }

    ~Matrix()
    {
        if (m_ != nullptr) {
            delete[] m_;
            m_ = nullptr;
        }
    }

    ConstMatrixRow<T> operator[](int i) const
    {
        assert(0 <= i && i < rows_);
        return {m_, cols_, i};
    }

    MatrixRow<T> operator[](int i)
    {
        assert(0 <= i && i < rows_);
        return {m_, cols_, i};
    }

    int rows() const { return rows_; }
    int cols() const { return cols_; }

private:
    int rows_;
    int cols_;
    T* m_;
};
