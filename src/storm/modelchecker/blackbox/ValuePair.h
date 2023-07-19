
#ifndef STORM_VALUEPAIR_H
#define STORM_VALUEPAIR_H

#include <cstddef>
#include <iterator>
#include <ostream>
#include <utility>
#include "boost/container_hash/hash_fwd.hpp"
#include "storm/utility/rationalfunction.h"

namespace storm::utility {

template<typename ValueType> class ValuePair;
template<typename ValueType> class ValuePairIterator;

//Iterator to enable hashing
template<typename ValueType> class ValuePairIterator
{
   public:
    using iterator_category = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = ValueType;
    using pointer           = ValueType*;
    using reference         = ValueType&;
    explicit ValuePairIterator(ValuePair<ValueType> v);

    ValuePairIterator();

    reference operator*();
    ValuePairIterator& operator++();
    ValuePairIterator operator++(int);

    bool isSecond() const;
    bool isAnEnd() const;

   private:
    ValuePair<ValueType> v;
    bool second;
    bool end;
};

template<typename V> bool operator!=(const ValuePairIterator<V>& a, const ValuePairIterator<V>& b)
{
    return a.isSecond() != b.isSecond() || a.isAnEnd() != b.isAnEnd();
}


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
    explicit ValuePair(std::pair<ValueType, ValueType>&& v) ;
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

    ValuePairIterator<ValueType> begin() const;
    ValuePairIterator<ValueType> end() const;

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

}


//Trait to enable hashing
namespace boost::container_hash {
template<> struct is_range <storm::utility::ValuePair<double>> {
    static constexpr bool value = true;
};
}


#endif  // STORM_VALUEPAIR_H
