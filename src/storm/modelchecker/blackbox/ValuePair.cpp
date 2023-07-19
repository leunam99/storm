#include "storm/modelchecker/blackbox/ValuePair.h"


namespace storm::utility {

template<typename ValueType> ValuePair<ValueType>::ValuePair(std::pair<ValueType, ValueType> v) : valuePair{v} {}
template<typename ValueType> ValuePair<ValueType>::ValuePair(std::pair<ValueType, ValueType>&& v) : valuePair{std::move(v)} {}
template<typename ValueType> ValuePair<ValueType>::ValuePair(double p) : valuePair{std::make_pair(p, p)} {}
template<typename ValueType> ValuePair<ValueType>::ValuePair() : ValuePair(0) {}
template<typename ValueType> ValuePair<ValueType>::ValuePair(double p, double q) : valuePair{std::make_pair(p, q)} {}

template<typename ValueType>   ValueType  ValuePair<ValueType>::getLBound() const {
       return valuePair.first;
   }
template<typename ValueType> ValueType ValuePair<ValueType>::getUBound() const {
       return valuePair.second;
   }
template<typename ValueType>   ValueType&  ValuePair<ValueType>::getLBoundRef(){
   return valuePair.first;
}
template<typename ValueType> ValueType& ValuePair<ValueType>::getUBoundRef(){
   return valuePair.second;
}


template<typename ValueType> ValuePair<ValueType> ValuePair<ValueType>::operator+(ValuePair other) const{
   return ValuePair(getLBound() + other.getLBound(), getUBound() + other.getUBound());
}

template<typename ValueType>
ValuePair<ValueType> ValuePair<ValueType>::operator/(const ValuePair other) const{
   return ValuePair(getLBound() / other.getLBound(), getUBound() / other.getUBound());
}
template<typename ValueType> ValuePair<ValueType> ValuePair<ValueType>::operator-(ValuePair other) const{
    return ValuePair(getLBound() - other.getLBound(), getUBound() - other.getUBound());
}

template<typename ValueType> ValuePair<ValueType> ValuePair<ValueType>::operator*(ValuePair other) const{
    return ValuePair(getLBound() * other.getLBound(), getUBound() * other.getUBound());
}

template<typename ValueType> ValuePair<ValueType> ValuePair<ValueType>::operator-() const{
    return ValuePair(-valuePair.first, -valuePair.second);
}

template<typename ValueType>
void ValuePair<ValueType>::operator+=(ValuePair other) {
    valuePair.first += other.getLBound();
    valuePair.second += other.getUBound();
}

template<typename ValueType>
void ValuePair<ValueType>::operator-=(ValuePair other) {
    valuePair.first -= other.getLBound();
    valuePair.second -= other.getUBound();
}

template<typename ValueType>
void ValuePair<ValueType>::operator/=(ValuePair other) {
    valuePair.first /= other.getLBound();
    valuePair.second /= other.getUBound();
}

template<typename ValueType>
void ValuePair<ValueType>::operator*=(ValuePair other) {
    valuePair.first *= other.getLBound();
    valuePair.second *= other.getUBound();
}

template<typename ValueType>
bool ValuePair<ValueType>::operator==(const ValuePair other) const {
    return getLBound() == other.getLBound() && getUBound() == other.getUBound();
}

//compare lower bound first
template<typename ValueType>
bool ValuePair<ValueType>::operator<(const ValuePair other) const {
    return getLBound() == other.getLBound() ? getUBound() < other.getUBound() : getLBound() < other.getLBound() ;
}
template<typename ValueType>
bool ValuePair<ValueType>::operator>(const ValuePair other) const {
    return getLBound() == other.getLBound() ? getUBound() > other.getUBound() : getLBound() > other.getLBound() ;
}


template<typename ValueType>
bool ValuePair<ValueType>::operator!=(ValuePair other) const {
    return !(*this == other);
}

template<typename ValueType>
ValuePairIterator<ValueType> ValuePair<ValueType>::begin() const{
    return ValuePairIterator<ValueType>(*this);
}

template<typename ValueType>
ValuePairIterator<ValueType> ValuePair<ValueType>::end() const{
    return ValuePairIterator<ValueType>();
}
template<typename ValueType>
ValuePair<ValueType> ValuePair<ValueType>::operator*(ValueType other) const {
    return *this * ValuePair(other, other);
}

template<typename ValueType>
ValuePair<ValueType>::operator ValueType() const {
    return getLBound();
}

template<class T>
std::ostream& operator<<(std::ostream& os, const storm::utility::ValuePair<T>& vp) {
   (os << "[") << vp.getLBound() << "," << vp.getUBound() << "]";
   return os;
}

template<class V> V& operator+=(V& a, const ValuePair<V>& b){
    a += b.getLBound();
    return a;
}

template<class V> V operator+(V a, const ValuePair<V>& b){
    return a + b.getLBound();
}

template<class V> V operator*(V a, const ValuePair<V>& b){
    return a * b.getLBound();
}

template<typename ValueType>
ValuePairIterator<ValueType>::ValuePairIterator(ValuePair<ValueType> v)  : v(v), second(false), end(false) {}

template<typename ValueType>
ValueType& ValuePairIterator<ValueType>::operator*(){
    if(second)
        return v.getUBoundRef() ;
    else
        return v.getLBoundRef();
}
template<typename ValueType>
ValuePairIterator<ValueType>& ValuePairIterator<ValueType>::operator++() {
    if(second)
        end = true;
    else
        second = true;

    return *this;
}

template<typename ValueType>
ValuePairIterator<ValueType> ValuePairIterator<ValueType>::operator++(int) {
    ValuePairIterator<ValueType> tmp = *this; ++(*this); return tmp;
}
template<typename ValueType>
ValuePairIterator<ValueType>::ValuePairIterator() : second(false), end(true) {}

template<typename ValueType>
bool ValuePairIterator<ValueType>::isSecond() const {
    return second;
}
template<typename ValueType>
bool ValuePairIterator<ValueType>::isAnEnd() const {
    return end;
}



template class ValuePair<double>;
template class ValuePairIterator<double>;
template std::ostream& operator<< <double>(std::ostream& os, const storm::utility::ValuePair<double>& vp);
template double& operator+=(double& a, const ValuePair<double>& b);
template double operator+(double a, const ValuePair<double>& b);
template double operator*(double a, const ValuePair<double>& b);

}

