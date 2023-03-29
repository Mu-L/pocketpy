#pragma once

#include "ceval.h"

namespace pkpy{

class RangeIter : public BaseIter {
    i64 current;
    Range r;
public:
    RangeIter(VM* vm, PyObject* _ref) : BaseIter(vm, _ref) {
        this->r = OBJ_GET(Range, _ref);
        this->current = r.start;
    }

    bool _has_next(){
        return r.step > 0 ? current < r.stop : current > r.stop;
    }

    PyObject* next(){
        if(!_has_next()) return nullptr;
        current += r.step;
        return VAR(current-r.step);
    }
};

template <typename T>
class ArrayIter : public BaseIter {
    size_t index = 0;
    const T* p;
public:
    ArrayIter(VM* vm, PyObject* _ref) : BaseIter(vm, _ref) { p = &OBJ_GET(T, _ref);}
    PyObject* next(){
        if(index == p->size()) return nullptr;
        return p->operator[](index++); 
    }
};

class StringIter : public BaseIter {
    int index = 0;
    Str* str;
public:
    StringIter(VM* vm, PyObject* _ref) : BaseIter(vm, _ref) {
        str = &OBJ_GET(Str, _ref);
    }

    PyObject* next() {
        if(index == str->u8_length()) return nullptr;
        return VAR(str->u8_getitem(index++));
    }
};

inline PyObject* Generator::next(){
    if(state == 2) return nullptr;
    vm->callstack.push(std::move(frame));
    PyObject* ret = vm->_exec();
    if(ret == vm->_py_op_yield){
        frame = std::move(vm->callstack.top());
        vm->callstack.pop();
        state = 1;
        return frame->pop_value(vm);
    }else{
        state = 2;
        return nullptr;
    }
}

inline void BaseIter::_mark() {
    if(_ref != nullptr) OBJ_MARK(_ref);
    if(loop_var != nullptr) OBJ_MARK(loop_var);
}

inline void Generator::_mark(){
    BaseIter::_mark();
    if(frame!=nullptr) frame->_mark();
}

template<typename T>
void _mark(T& t){
    if constexpr(std::is_base_of_v<BaseIter, T>){
        t._mark();
    }
}

} // namespace pkpy