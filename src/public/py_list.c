#include "pocketpy/pocketpy.h"

#include "pocketpy/common/utils.h"
#include "pocketpy/objects/object.h"
#include "pocketpy/interpreter/vm.h"

typedef c11_vector List;

void py_newlist(py_Ref out) {
    pk_VM* vm = pk_current_vm;
    PyObject* obj = pk_ManagedHeap__gcnew(&vm->heap, tp_list, 0, sizeof(List));
    List* userdata = PyObject__userdata(obj);
    c11_vector__ctor(userdata, sizeof(py_TValue));
    out->type = tp_list;
    out->is_ptr = true;
    out->_obj = obj;
}

void py_newlistn(py_Ref out, int n) {
    py_newlist(out);
    List* userdata = py_touserdata(out);
    c11_vector__reserve(userdata, n);
    userdata->count = n;
}

py_Ref py_list__getitem(const py_Ref self, int i) {
    List* userdata = py_touserdata(self);
    return c11__at(py_TValue, userdata, i);
}

void py_list__setitem(py_Ref self, int i, const py_Ref val) {
    List* userdata = py_touserdata(self);
    c11__setitem(py_TValue, userdata, i, *val);
}

void py_list__delitem(py_Ref self, int i) {
    List* userdata = py_touserdata(self);
    c11_vector__erase(py_TValue, userdata, i);
}

int py_list__len(const py_Ref self) {
    List* userdata = py_touserdata(self);
    return userdata->count;
}

void py_list__append(py_Ref self, const py_Ref val) {
    List* userdata = py_touserdata(self);
    c11_vector__push(py_TValue, userdata, *val);
}

void py_list__clear(py_Ref self) {
    List* userdata = py_touserdata(self);
    c11_vector__clear(userdata);
}

void py_list__insert(py_Ref self, int i, const py_Ref val) {
    List* userdata = py_touserdata(self);
    c11_vector__insert(py_TValue, userdata, i, *val);
}

////////////////////////////////
static bool _py_list__len__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(1);
    py_i64 res = py_list__len(py_arg(0));
    py_newint(py_retval(), res);
    return true;
}

static bool _py_list__eq__(int argc, py_Ref argv) {
    PY_CHECK_ARGC(2);
    py_Ref _0 = py_arg(0);
    py_Ref _1 = py_arg(1);
    if(py_istype(_1, tp_list)) {
        int length = py_list__len(_0);
        if(length != py_list__len(_1)) {
            py_newbool(py_retval(), false);
            return true;
        }
        for(int i = 0; i < length; i++) {
            py_Ref a = py_list__getitem(_0, i);
            py_Ref b = py_list__getitem(_1, i);
            int res = py_eq(a, b);
            if(res == -1) return false;
            if(res == 0) {
                py_newbool(py_retval(), false);
                return true;
            }
        }
        py_newbool(py_retval(), true);
    } else {
        py_newnotimplemented(py_retval());
    }
    return true;
}

static bool _py_list__ne__(int argc, py_Ref argv) {
    bool ok = _py_list__eq__(argc, argv);
    if(!ok) return false;
    py_Ref retval = py_retval();
    py_newbool(retval, !py_tobool(retval));
    return true;
}

py_Type pk_list__register() {
    pk_VM* vm = pk_current_vm;
    py_Type type = pk_VM__new_type(vm, "list", tp_object, NULL, false);
    pk_TypeInfo* ti = c11__at(pk_TypeInfo, &vm->types, type);
    ti->dtor = (void (*)(void*))c11_vector__dtor;

    py_bindmagic(type, __len__, _py_list__len__);
    py_bindmagic(type, __eq__, _py_list__eq__);
    py_bindmagic(type, __ne__, _py_list__ne__);
    return type;
}