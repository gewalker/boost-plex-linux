//  (C) Copyright David Abrahams 2000. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  The author gratefully acknowleges the support of Dragon Systems, Inc., in
//  producing this work.
//
//  This file generated for 5-argument member functions and 5-argument free
//  functions by gen_caller.py

#ifndef CALLER_DWA05090_H_
# define CALLER_DWA05090_H_

# include "pyconfig.h"
# include "wrap_python.h"
# include <boost/config.hpp>
# include "signatures.h"
# include "none.h"

namespace py {

// Calling C++ from Python
template <class R>
struct Caller
{
    template <class T>
    static PyObject* call(R (T::*pmf)(), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        if (!PyArg_ParseTuple(args, const_cast<char*>("O"), &self))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)());
    }

    template <class T, class A1>
    static PyObject* call(R (T::*pmf)(A1), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OO"), &self, &a1))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)(from_python(a1, Type<A1>())));
    }

    template <class T, class A1, class A2>
    static PyObject* call(R (T::*pmf)(A1, A2), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOO"), &self, &a1, &a2))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>())));
    }

    template <class T, class A1, class A2, class A3>
    static PyObject* call(R (T::*pmf)(A1, A2, A3), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOO"), &self, &a1, &a2, &a3))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>())));
    }

    template <class T, class A1, class A2, class A3, class A4>
    static PyObject* call(R (T::*pmf)(A1, A2, A3, A4), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOOO"), &self, &a1, &a2, &a3, &a4))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>())));
    }

    template <class T, class A1, class A2, class A3, class A4, class A5>
    static PyObject* call(R (T::*pmf)(A1, A2, A3, A4, A5), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        PyObject* a5;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOOOO"), &self, &a1, &a2, &a3, &a4, &a5))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>()),
                        from_python(a5, Type<A5>())));
    }


    template <class T>
    static PyObject* call(R (T::*pmf)() const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        if (!PyArg_ParseTuple(args, const_cast<char*>("O"), &self))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)());
    }

    template <class T, class A1>
    static PyObject* call(R (T::*pmf)(A1) const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OO"), &self, &a1))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)(from_python(a1, Type<A1>())));
    }

    template <class T, class A1, class A2>
    static PyObject* call(R (T::*pmf)(A1, A2) const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOO"), &self, &a1, &a2))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>())));
    }

    template <class T, class A1, class A2, class A3>
    static PyObject* call(R (T::*pmf)(A1, A2, A3) const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOO"), &self, &a1, &a2, &a3))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>())));
    }

    template <class T, class A1, class A2, class A3, class A4>
    static PyObject* call(R (T::*pmf)(A1, A2, A3, A4) const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOOO"), &self, &a1, &a2, &a3, &a4))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>())));
    }

    template <class T, class A1, class A2, class A3, class A4, class A5>
    static PyObject* call(R (T::*pmf)(A1, A2, A3, A4, A5) const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        PyObject* a5;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOOOO"), &self, &a1, &a2, &a3, &a4, &a5))
            return 0;
        T& target = from_python(self, Type<T&>());
        return to_python((target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>()),
                        from_python(a5, Type<A5>())));
    }

    // Free functions
    static PyObject* call(R (*f)(), PyObject* args, PyObject* /* keywords */ ) {
        if (!PyArg_ParseTuple(args, const_cast<char*>("")))
            return 0;
        return to_python(f());
    }

    template <class A1>
    static PyObject* call(R (*f)(A1), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* a1;
        if (!PyArg_ParseTuple(args, const_cast<char*>("O"), &a1))
            return 0;
        return to_python(f(from_python(a1, Type<A1>())));
    }

    template <class A1, class A2>
    static PyObject* call(R (*f)(A1, A2), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* a1;
        PyObject* a2;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OO"), &a1, &a2))
            return 0;
        return to_python(f(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>())));
    }

    template <class A1, class A2, class A3>
    static PyObject* call(R (*f)(A1, A2, A3), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOO"), &a1, &a2, &a3))
            return 0;
        return to_python(f(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>())));
    }

    template <class A1, class A2, class A3, class A4>
    static PyObject* call(R (*f)(A1, A2, A3, A4), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOO"), &a1, &a2, &a3, &a4))
            return 0;
        return to_python(f(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>())));
    }

    template <class A1, class A2, class A3, class A4, class A5>
    static PyObject* call(R (*f)(A1, A2, A3, A4, A5), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        PyObject* a5;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOOO"), &a1, &a2, &a3, &a4, &a5))
            return 0;
        return to_python(f(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>()),
                        from_python(a5, Type<A5>())));
    }

};
                           
template <>
struct Caller<void>
{
    template <class T>
    static PyObject* call(void (T::*pmf)(), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        if (!PyArg_ParseTuple(args, const_cast<char*>("O"), &self))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)();
        return none();
    }

    template <class T, class A1>
    static PyObject* call(void (T::*pmf)(A1), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OO"), &self, &a1))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)(from_python(a1, Type<A1>()));
        return none();
    }

    template <class T, class A1, class A2>
    static PyObject* call(void (T::*pmf)(A1, A2), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOO"), &self, &a1, &a2))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()));
        return none();
    }

    template <class T, class A1, class A2, class A3>
    static PyObject* call(void (T::*pmf)(A1, A2, A3), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOO"), &self, &a1, &a2, &a3))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()));
        return none();
    }

    template <class T, class A1, class A2, class A3, class A4>
    static PyObject* call(void (T::*pmf)(A1, A2, A3, A4), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOOO"), &self, &a1, &a2, &a3, &a4))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>()));
        return none();
    }

    template <class T, class A1, class A2, class A3, class A4, class A5>
    static PyObject* call(void (T::*pmf)(A1, A2, A3, A4, A5), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        PyObject* a5;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOOOO"), &self, &a1, &a2, &a3, &a4, &a5))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>()),
                        from_python(a5, Type<A5>()));
        return none();
    }


    template <class T>
    static PyObject* call(void (T::*pmf)() const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        if (!PyArg_ParseTuple(args, const_cast<char*>("O"), &self))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)();
        return none();
    }

    template <class T, class A1>
    static PyObject* call(void (T::*pmf)(A1) const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OO"), &self, &a1))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)(from_python(a1, Type<A1>()));
        return none();
    }

    template <class T, class A1, class A2>
    static PyObject* call(void (T::*pmf)(A1, A2) const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOO"), &self, &a1, &a2))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()));
        return none();
    }

    template <class T, class A1, class A2, class A3>
    static PyObject* call(void (T::*pmf)(A1, A2, A3) const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOO"), &self, &a1, &a2, &a3))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()));
        return none();
    }

    template <class T, class A1, class A2, class A3, class A4>
    static PyObject* call(void (T::*pmf)(A1, A2, A3, A4) const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOOO"), &self, &a1, &a2, &a3, &a4))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>()));
        return none();
    }

    template <class T, class A1, class A2, class A3, class A4, class A5>
    static PyObject* call(void (T::*pmf)(A1, A2, A3, A4, A5) const, PyObject* args, PyObject* /* keywords */ ) {
        PyObject* self;
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        PyObject* a5;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOOOO"), &self, &a1, &a2, &a3, &a4, &a5))
            return 0;
        T& target = from_python(self, Type<T&>());
        (target.*pmf)(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>()),
                        from_python(a5, Type<A5>()));
        return none();
    }


    // Free functions
    static PyObject* call(void (*f)(), PyObject* args, PyObject* /* keywords */ ) {
        if (!PyArg_ParseTuple(args, const_cast<char*>("")))
            return 0;
        f();
        return none();
    }

    template <class A1>
    static PyObject* call(void (*f)(A1), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* a1;
        if (!PyArg_ParseTuple(args, const_cast<char*>("O"), &a1))
            return 0;
        f(from_python(a1, Type<A1>()));
        return none();
    }

    template <class A1, class A2>
    static PyObject* call(void (*f)(A1, A2), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* a1;
        PyObject* a2;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OO"), &a1, &a2))
            return 0;
        f(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()));
        return none();
    }

    template <class A1, class A2, class A3>
    static PyObject* call(void (*f)(A1, A2, A3), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOO"), &a1, &a2, &a3))
            return 0;
        f(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()));
        return none();
    }

    template <class A1, class A2, class A3, class A4>
    static PyObject* call(void (*f)(A1, A2, A3, A4), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOO"), &a1, &a2, &a3, &a4))
            return 0;
        f(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>()));
        return none();
    }

    template <class A1, class A2, class A3, class A4, class A5>
    static PyObject* call(void (*f)(A1, A2, A3, A4, A5), PyObject* args, PyObject* /* keywords */ ) {
        PyObject* a1;
        PyObject* a2;
        PyObject* a3;
        PyObject* a4;
        PyObject* a5;
        if (!PyArg_ParseTuple(args, const_cast<char*>("OOOOO"), &a1, &a2, &a3, &a4, &a5))
            return 0;
        f(from_python(a1, Type<A1>()),
                        from_python(a2, Type<A2>()),
                        from_python(a3, Type<A3>()),
                        from_python(a4, Type<A4>()),
                        from_python(a5, Type<A5>()));
        return none();
    }

};

}

#endif
