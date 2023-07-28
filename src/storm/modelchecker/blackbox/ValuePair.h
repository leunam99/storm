#pragma once

#include <cstddef>
#include <iterator>
#include <ostream>
#include <utility>
#include "boost/container_hash/hash_fwd.hpp"
#include "storm/utility/rationalfunction.h"
#include "storm/utility/ConstantsComparatorForward.h"

namespace storm::utility {

template<typename ValueType> class ValuePair;

/*!
* Wrapper around a pair to add to it the necessary methods for using it as value in SparseMatrix
 * Implementations are somewhat sensible but as some operations (e.g. < ) are not well defined for intervals,
 * they should be taken with care
 */
template<typename ValueType>
class ValuePair {
   private:
    std::pair<ValueType, ValueType> valuePair;

   public:
    explicit ValuePair(std::pair<ValueType, ValueType> v);
    explicit ValuePair(double p);
    ValuePair();
    ValuePair(double p, double q);

    ValueType getLBound() const;
    ValueType getUBound() const;
    ValueType& getLBoundRef();
    ValueType& getUBoundRef();
    ValuePair operator+(const ValuePair other) const;
    ValuePair<ValueType> operator/(const ValuePair other) const;
    ValuePair<ValueType> operator-(const ValuePair other) const;
    ValuePair<ValueType> operator*(const ValuePair other) const;
    ValuePair<ValueType> operator*(const ValueType other) const;

    operator ValueType() const;

    ValuePair<ValueType> operator-() const;


    void operator+=(ValuePair other);
    void operator-=(ValuePair other);
    void operator*=(ValuePair other);
    void operator/=(ValuePair other);


    bool operator==(ValuePair other) const;
    bool operator!=(ValuePair other) const;
    bool operator<(ValuePair other) const;
    bool operator>(ValuePair other) const;

};

template<class V> V& operator+=(V& a, const ValuePair<V>& b);
template<class V> V operator+(const V a, const ValuePair<V>& b);
template<class V> V operator*(const V a, const ValuePair<V>& b);

/**
 * Prints the value pair to the stream
 * @return a reference to the stream
 */
template<class T>
std::ostream& operator<<(std::ostream& os, const ValuePair<T>& vp);

/*
 * This is needed because there are some checks done if a row sums to 1,
 *  using these functions
 */
template<typename ValueType>
class ConstantsComparator<ValuePair<ValueType>> {
   public:
    ConstantsComparator() = default;
    bool isOne(ValuePair<ValueType> const& value) const;
    bool isZero(ValuePair<ValueType> const& value) const;
    bool isEqual(ValuePair<ValueType> const& value1, ValuePair<ValueType> const& value2) const;
    bool isConstant(ValuePair<ValueType> const& value) const;
    bool isInfinity(ValuePair<ValueType> const& value) const;
    bool isLess(ValuePair<ValueType> const& value1, ValuePair<ValueType> const& value2) const;
};

template<typename ValueType>
std::size_t hash_value(ValuePair<ValueType> const& b);

}

