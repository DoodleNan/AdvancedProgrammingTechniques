// Implementation of the templated Vector class
// ECE4893/8893 lab 3
// YOUR NAME HERE

#include <iostream> // debugging
#include "Vector.h"

// Your implementation here
// Fill in all the necessary functions below
using namespace std;

// Default constructor
template <typename T>
Vector<T>::Vector(){
    elements = NULL;
    count = 0;
    reserved = 0;
}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& rhs){
    count = rhs.count;
    reserved = rhs.reserved;
    elements = (T*)malloc(sizeof(T)*reserved);
    for (size_t i = 0; i<rhs.count; i++) {
        new (&elements[i]) T(rhs[i]);
    }
}

// Assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& rhs){
    for (size_t i = 0; i<count; i++) {
        elements[i].~T();
    }
    free(elements);
    count = rhs.count;
    reserved = rhs.reserved;
    elements = (T*)malloc(sizeof(T)*reserved);
    for (size_t i = 0; i<count; i++) {
        new (&elements[i]) T(rhs[i]);
    }
}

#ifdef GRAD_STUDENT
// Other constructors
template <typename T>
Vector<T>::Vector(size_t nReserved){ // Initialize with reserved memory
    reserved = nReserved;
    elements = (T*)malloc(sizeof(T)*nReserved);
    count = 0;
}

template <typename T>
Vector<T>::Vector(size_t n, const T& t){ // Initialize with "n" copies of "t"
    count = n;
    reserved = n;
    elements = (T*)malloc(sizeof(T)*count);
    for (size_t i = 0; i<n; i++) {
        new (&elements[i]) T(t);
    }
}

template <typename T>
void Vector<T>::Reserve(size_t n){ // Reserve extra memory
    reserved = n;
    T* cur = (T*)malloc(sizeof(T)*reserved);
    for (size_t i = 0; i<count; i++) {
        new (&cur[i]) T(elements[i]);
        elements[i].~T();
    }
    free(elements);
    elements = cur;
}

#endif

// Destructor
template <typename T>
Vector<T>::~Vector(){
    for (size_t i = 0; i<count; i++) {
        elements[i].~T();
    }
    free(elements);
    elements = NULL;
    count = 0;
    reserved = 0;
}

// Add and access front and back
template <typename T>
void Vector<T>::Push_Back(const T& rhs){
    if(count >= reserved){
        Reserve(reserved+1);
    }
    new (&elements[count]) T(rhs);
    count++;
}

template <typename T>
void Vector<T>::Push_Front(const T& rhs){
    if (count < reserved) {
        for (size_t i = count; count>0; i--) {
            elements[i] = elements[i-1];
        }
        new (&elements[0]) T(rhs);
        //element[0] = T(rhs);
        count++;
    }else{
        T* cur = (T*)malloc(sizeof(T)*(reserved+1));
        new (&cur[0]) T(rhs);
        for (size_t i = 0; i<count; i++) {
            new (&cur[i+1]) T(elements[i]);
            elements[i].~T();
        }
        free(elements);
        elements = cur;
        reserved++;
        count++;
    }
}

template <typename T>
void Vector<T>::Pop_Back(){ // Remove last element
    if(count>0){
        count--;
        elements[count].~T();
    }else{
        cout<<"Error!!!!!Vector empty!!!!!Can not pop back!!!!!"<<endl;
    }
}

template <typename T>
void Vector<T>::Pop_Front(){ // Remove first element
    if (count>0) {
        for (size_t i = 0; i<count-1; i++) {
            elements[i].~T();
            new (&elements[i]) T(elements[i+1]);
        }
        Pop_Back();
    }else{
        cout<<"Error!!!!!Vector empty!!!!!Can not pop front!!!!!"<<endl;
    }
}

// Element Access
template <typename T>
T& Vector<T>::Front() const{
    return elements[0];
}

// Element Access
template <typename T>
T& Vector<T>::Back() const{
    return elements[count-1];
}

template <typename T>
const T& Vector<T>::operator[](size_t i) const{ // const element access
    return elements[i];
}

template <typename T>
T& Vector<T>::operator[](size_t i){//nonconst element access
    return elements[i];
}

template <typename T>
size_t Vector<T>::Size() const{
    return count;
}

template <typename T>
bool Vector<T>::Empty() const{
    return count == 0;
}

// Implement clear
template <typename T>
void Vector<T>::Clear(){
    for (size_t i = 0; i<count; i++) {
        elements[i].~T();
    }
    count = 0;
}

// Iterator access functions
template <typename T>
VectorIterator<T> Vector<T>::Begin() const{
    return VectorIterator<T>(elements);
}

template <typename T>
VectorIterator<T> Vector<T>::End() const{
    return VectorIterator<T>(elements+count);
}

#ifdef GRAD_STUDENT
// Erase and insert
template <typename T>
void Vector<T>::Erase(const VectorIterator<T>& it){
    T* curVector = (T*)malloc(sizeof(T)*reserved);
    size_t erasePos = 0;
    for (VectorIterator<T> iter = Begin(); iter != it; iter++) {
        erasePos++;
    }
    for (size_t i = 0; i<erasePos; i++) {
        new (&curVector[i]) T(elements[i]);
        //elements[i].~T();
    }
    //elements[erasePos].~T();
    for (size_t i = erasePos+1; i<count; i++) {
        new (&curVector[i-1]) T(elements[i]);
        //elements[i].~T();
    }
    for (size_t i = 0; i<count; i++) {
        elements[i].~T();
    }
    free(elements);
    elements = curVector;
    count--;

   
}

template <typename T>
void Vector<T>::Insert(const T& rhs, const VectorIterator<T>& it){
//    if (count >= reserved) {
//        reserved++;
//    }
//    T* curVector = (T*)malloc(sizeof(T)*reserved);
//    size_t insertPos = 0;
//    for (VectorIterator<T> iter = Begin(); iter != it; ++iter) {
//        insertPos++;
//    }
//    for (size_t i = 0; i<insertPos; ++i) {
//        new (&curVector[i]) T(elements[i]);
//        elements[i].~T();
//    }
//    new (&curVector[insertPos]) T(rhs);
//    for (size_t i = insertPos; i<count; ++i) {
//        new (&curVector[i+1]) T(elements[i]);
//        elements[i].~T();
//    }
//    free(elements);
//    count++;
//    elements = curVector;
    size_t index;
    
    for (size_t i = 0; i < count; ++i)
    {
        if (&elements[i] == it.current)
        {
            index = i;
            break;
        }
    }
    
    if (count < reserved)
    {
        for (size_t i = count; i > index; --i)
        {
            elements[i] = elements[i-1];
        }
        new (&elements[index]) T(rhs);
        count++;
    }
    else
    {
        T* new_elements = (T*)malloc(sizeof(T) * (count+1));
        for (size_t i = 0; i < index; ++i)
        {
            new (&new_elements[i]) T(elements[i]);
            elements[i].~T();
        }
        
        for (size_t i = count; i > index; --i)
        {
            new (&new_elements[i]) T(elements[i-1]);
            elements[i-1].~T();
        }
        free(elements);
        elements = new_elements;
        new (&elements[index]) T(rhs);
        count++;
        reserved = count;
    }
}
#endif

// Implement the iterators

// Constructors
template <typename T>
VectorIterator<T>::VectorIterator(){
    current = NULL;
}

template <typename T>
VectorIterator<T>::VectorIterator(T* c){
    current = c;
}

// Copy constructor
template <typename T>
VectorIterator<T>::VectorIterator(const VectorIterator<T>& rhs){
    current = rhs.current;
}

// Iterator defeferencing operator
template <typename T>
T& VectorIterator<T>::operator*() const{
    return *current;
}

// Prefix increment
template <typename T>
VectorIterator<T>  VectorIterator<T>::operator++(){
    return VectorIterator<T>(++current);
//    current++;
//    return *this;
}

// Postfix increment
template <typename T>
VectorIterator<T> VectorIterator<T>::operator++(int){
    return VectorIterator<T>(current++);
}

// Comparison operators
template <typename T>
bool VectorIterator<T>::operator !=(const VectorIterator<T>& rhs) const{
    return current != rhs.current;
}

template <typename T>
bool VectorIterator<T>::operator ==(const VectorIterator<T>& rhs) const{
    return current == rhs.current;
}




