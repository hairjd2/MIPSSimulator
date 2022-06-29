#include "Register.h"
// Register Constructor
template <class T>
Register<T>::Register() {
    m_inUse = false;
    m_value = 0;
}
// Setter method for in-use value
template <class T>
void Register<T>::setInUse(bool inUse) {
    m_inUse = inUse;
}
// Setter method for register value
template <class T>
void Register<T>::setValue(T value) {
    m_value = value;
}
// Getter method for in-use value
template <class T>
bool Register<T>::getInUse() {
    return m_inUse;
}
// Getter method for register value
template <class T>
T Register<T>::getValue() {
    return m_value;
}