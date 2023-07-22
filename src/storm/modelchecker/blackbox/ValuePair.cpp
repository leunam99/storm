#include "storm/modelchecker/blackbox/ValuePair.h"
#include "settings/SettingsManager.h"
#include "settings/modules/GeneralSettings.h"
#include "utility/constants.h"

namespace storm::utility {

template<typename ValueType> ValuePair<ValueType>::ValuePair(std::pair<ValueType, ValueType> v) : valuePair{v} {}
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
bool ConstantsComparator<ValuePair<ValueType>>::isConstant(const ValuePair<ValueType>& value) const {
    return true;
}

template<typename ValueType>
bool ConstantsComparator<ValuePair<ValueType>>::isEqual(const ValuePair<ValueType>& value1, const ValuePair<ValueType>& value2) const {
    return value1 == value2;
}

template<typename ValueType>
bool ConstantsComparator<ValuePair<ValueType>>::isInfinity(const ValuePair<ValueType>& value) const {
    return false;
}

template<typename ValueType>
bool ConstantsComparator<ValuePair<ValueType>>::isLess(const ValuePair<ValueType>& value1, const ValuePair<ValueType>& value2) const {
    return value1 < value2;
}
template<typename ValueType>
bool ConstantsComparator<ValuePair<ValueType>>::isOne(const ValuePair<ValueType>& value) const {
    return value.getUBound() >= 1 - storm::utility::convertNumber<ValueType>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

template<typename ValueType>
bool ConstantsComparator<ValuePair<ValueType>>::isZero(const ValuePair<ValueType>& value) const {
    return value.getLBound() <= storm::utility::convertNumber<ValueType>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

template<typename ValueType>
std::size_t hash_value(ValuePair<ValueType> const& b)
{
    boost::hash<ValueType> hasher;
    std::size_t seed = hasher(b.getLBound());
    boost::hash_combine(seed, b.getUBound());
    return seed;
}

template class ValuePair<double>;
template std::ostream& operator<< <double>(std::ostream& os, const storm::utility::ValuePair<double>& vp);
template double& operator+=(double& a, const ValuePair<double>& b);
template double operator+(double a, const ValuePair<double>& b);
template double operator*(double a, const ValuePair<double>& b);

template class ConstantsComparator<ValuePair<double>>;
template std::size_t hash_value(ValuePair<double> const& b);

}

