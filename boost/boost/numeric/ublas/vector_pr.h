//
//  Copyright (c) 2000-2002
//  Joerg Walter, Mathias Koch
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  The authors make no representations
//  about the suitability of this software for any purpose.
//  It is provided "as is" without express or implied warranty.
//
//  The authors gratefully acknowledge the support of
//  GeNeSys mbH & Co. KG in producing this work.
//

#ifndef NUMERICS_VECTOR_PR_H
#define NUMERICS_VECTOR_PR_H

#include <boost/numeric/ublas/config.h>
#include <boost/numeric/ublas/vector_et.h>

// Iterators based on ideas of Jeremy Siek

namespace boost { namespace numerics {

    // Vector based range class
    template<class V>
    class vector_range:
        public vector_expression<vector_range<V> > {
    public:      
        typedef const V const_vector_type;
        typedef V vector_type;
        typedef typename V::size_type size_type;
        typedef typename V::difference_type difference_type;
        typedef typename V::value_type value_type;
        typedef typename V::const_reference const_reference;
        typedef typename V::reference reference;
        typedef typename V::const_pointer const_pointer;
        typedef typename V::pointer pointer;
#ifdef NUMERICS_ET_CLOSURE_REFERENCE
        typedef const vector_const_reference<const vector_range<vector_type> > const_closure_type;
        typedef vector_reference<vector_range<vector_type> > closure_type;
#endif
#ifdef NUMERICS_ET_CLOSURE_VALUE
        typedef const vector_range<vector_type> const_closure_type;
        typedef vector_range<vector_type> closure_type;
#endif
        typedef typename V::const_iterator const_iterator_type;
        typedef typename V::iterator iterator_type;
        typedef typename storage_restrict_traits<typename V::storage_category,
                                                 dense_proxy_tag>::storage_category storage_category;

        // Construction and destruction
        NUMERICS_INLINE
        vector_range (): 
            data_ (nil_), r_ () {}
        NUMERICS_INLINE
        vector_range (vector_type &data, const range &r): 
            data_ (data), r_ (r) {}
#ifdef NUMERICS_DEPRECATED
        NUMERICS_INLINE
        vector_range (vector_type &data, size_type start, size_type stop): 
            data_ (data), r_ (start, stop) {}
#endif

        // Accessors
        NUMERICS_INLINE
        size_type start () const { 
            return r_.start (); 
        }
        NUMERICS_INLINE
        size_type size () const { 
            return r_.size (); 
        }
        NUMERICS_INLINE
        const_vector_type &data () const {
            return data_;
        }
        NUMERICS_INLINE
        vector_type &data () {
            return data_;
        }

        // Resetting
        NUMERICS_INLINE
        void reset (vector_type &data) {
            // data_ = data;
            data_.reset (data);
        }
        NUMERICS_INLINE
        void reset (vector_type &data, const range &r) {
            // data_ = data;
            data_.reset (data);
            r_ = r;
        }

        // Element access
        NUMERICS_INLINE
        value_type operator () (size_type i) const {
            return data () (r_ (i));
        }
        NUMERICS_INLINE
        reference operator () (size_type i) {
            return data () (r_ (i)); 
        }

        NUMERICS_INLINE
        value_type operator [] (size_type i) const { 
            return (*this) (i); 
        }
        NUMERICS_INLINE
        reference operator [] (size_type i) { 
            return (*this) (i); 
        }

        NUMERICS_INLINE
        vector_range<const_vector_type> project (const range &r) const {
            return vector_range<const_vector_type> (data (), r_.composite (r));
        }
        NUMERICS_INLINE
        vector_range<vector_type> project (const range &r) {
            return vector_range<vector_type> (data (), r_.composite (r));
        }

        // Assignment
        NUMERICS_INLINE
        vector_range &operator = (const vector_range &vr) {
            // FIXME: the ranges could be differently sized.
            // std::copy (vr.begin (), vr.end (), begin ());
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (vr));
            return *this;
        }
        NUMERICS_INLINE
        vector_range &assign_temporary (vector_range &vr) {
            // FIXME: this is suboptimal.
            // return *this = vr;
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vr);
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_range &operator = (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (ae));
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_range &assign (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, NUMERICS_TYPENAME AE::value_type> > () (*this, ae);
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_range &operator += (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (*this + ae));
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_range &plus_assign (const vector_expression<AE> &ae) {
            vector_assign<scalar_plus_assign<value_type, NUMERICS_TYPENAME AE::value_type> > () (*this, ae);
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_range &operator -= (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (*this - ae));
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_range &minus_assign (const vector_expression<AE> &ae) {
            vector_assign<scalar_minus_assign<value_type, NUMERICS_TYPENAME AE::value_type> > () (*this, ae);
            return *this;
        }
        template<class AT>
        NUMERICS_INLINE
        vector_range &operator *= (const AT &at) {
            vector_assign_scalar<scalar_multiplies_assign<value_type, AT> > () (*this, at);
            return *this;
        }
        template<class AT>
        NUMERICS_INLINE
        vector_range &operator /= (const AT &at) {
            vector_assign_scalar<scalar_divides_assign<value_type, AT> > () (*this, at);
            return *this;
        }

        // Swapping
        NUMERICS_INLINE
        void swap (vector_range &vr) {
            // Too unusual semantic.
            // check (this != &vr, external_logic ());
            if (this != &vr) {
                check (size () == vr.size (), bad_size ());
                std::swap_ranges (begin (), end (), vr.begin ());
            }
        }
#ifdef NUMERICS_FRIEND_FUNCTION
        NUMERICS_INLINE
        friend void swap (vector_range &vr1, vector_range &vr2) {
            vr1.swap (vr2);
        }
#endif

#ifdef NUMERICS_USE_INDEXED_ITERATOR
        typedef indexed_iterator<vector_range<vector_type>, 
                                 NUMERICS_TYPENAME vector_type::iterator::iterator_category> iterator;
        typedef indexed_const_iterator<vector_range<vector_type>, 
                                       NUMERICS_TYPENAME vector_type::const_iterator::iterator_category> const_iterator;
#else
        class const_iterator;
        class iterator;
#endif

        // Element lookup
        NUMERICS_INLINE
        const_iterator find_first (size_type i) const {
            const_iterator_type it (data ().find_first (start () + i));
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return const_iterator (*this, it.index ());
#else
            return const_iterator (*this, it);
#endif
        }
        NUMERICS_INLINE
        iterator find_first (size_type i) {
            iterator_type it (data ().find_first (start () + i));
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return iterator (*this, it.index ());
#else
            return iterator (*this, it);
#endif
        }
        NUMERICS_INLINE
        const_iterator find_last (size_type i) const {
            const_iterator_type it (data ().find_last (start () + i));
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return const_iterator (*this, it.index ());
#else
            return const_iterator (*this, it);
#endif
        }
        NUMERICS_INLINE
        iterator find_last (size_type i) {
            iterator_type it (data ().find_last (start () + i));
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return iterator (*this, it.index ());
#else
            return iterator (*this, it);
#endif
        }

        // Iterators simply are pointers.

#ifndef NUMERICS_USE_INDEXED_ITERATOR
        class const_iterator:
            public container_const_reference<vector_range>,
#ifdef NUMERICS_USE_ITERATOR_BASE_TRAITS
            public iterator_base_traits<typename V::const_iterator::iterator_category>::template
                        iterator_base<const_iterator, value_type>::type {
#else
            public random_access_iterator_base<typename V::const_iterator::iterator_category,
                                               const_iterator, value_type> {
#endif
        public:
            typedef typename V::const_iterator::iterator_category iterator_category;
#ifndef BOOST_MSVC_STD_ITERATOR
            typedef typename V::const_iterator::difference_type difference_type;
            typedef typename V::const_iterator::value_type value_type;
            typedef typename V::const_iterator::value_type reference;
            typedef typename V::const_iterator::pointer pointer;
#endif

            // Construction and destruction
            NUMERICS_INLINE
            const_iterator ():
                container_const_reference<vector_range> (), it_ () {}
            NUMERICS_INLINE
            const_iterator (const vector_range &vr, const const_iterator_type &it):
                container_const_reference<vector_range> (vr), it_ (it) {}
#ifndef NUMERICS_QUALIFIED_TYPENAME
            NUMERICS_INLINE
            const_iterator (const iterator &it):
                container_const_reference<vector_range> (it ()), it_ (it.it_) {}
#else
            NUMERICS_INLINE
            const_iterator (const typename vector_range::iterator &it):
                container_const_reference<vector_range> (it ()), it_ (it.it_) {}
#endif

            // Arithmetic
            NUMERICS_INLINE
            const_iterator &operator ++ () {
                ++ it_;
                return *this;
            }
            NUMERICS_INLINE
            const_iterator &operator -- () {
                -- it_;
                return *this;
            }
            NUMERICS_INLINE
            const_iterator &operator += (difference_type n) {
                it_ += n;
                return *this;
            }
            NUMERICS_INLINE
            const_iterator &operator -= (difference_type n) {
                it_ -= n;
                return *this;
            }
            NUMERICS_INLINE
            difference_type operator - (const const_iterator &it) const {
                return it_ - it.it_;
            }

            // Dereference
            NUMERICS_INLINE
            value_type operator * () const {
                check (index () < (*this) ().size (), bad_index ());
                return *it_;
            }

            // Index
            NUMERICS_INLINE
            size_type index () const {
                return it_.index () - (*this) ().start ();
            }

            // Assignment 
            NUMERICS_INLINE
            const_iterator &operator = (const const_iterator &it) {
                container_const_reference<vector_range>::assign (&it ());
                it_ = it.it_;
                return *this;
            }

            // Comparison
            NUMERICS_INLINE
            bool operator == (const const_iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ == it.it_;
            }
            NUMERICS_INLINE
            bool operator < (const const_iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ < it.it_;
            }

        private:
            const_iterator_type it_;
        };
#endif

        NUMERICS_INLINE
        const_iterator begin () const {
            return find_first (0);
        }
        NUMERICS_INLINE
        const_iterator end () const {
            return find_last (size ());
        }

#ifndef NUMERICS_USE_INDEXED_ITERATOR
        class iterator:
            public container_reference<vector_range>,
#ifdef NUMERICS_USE_ITERATOR_BASE_TRAITS
            public iterator_base_traits<typename V::iterator::iterator_category>::template
                        iterator_base<iterator, value_type>::type {
#else
            public random_access_iterator_base<typename V::iterator::iterator_category,
                                               iterator, value_type> {
#endif
        public:
            typedef typename V::iterator::iterator_category iterator_category;
#ifndef BOOST_MSVC_STD_ITERATOR
            typedef typename V::iterator::difference_type difference_type;
            typedef typename V::iterator::value_type value_type;
            typedef typename V::iterator::reference reference;
            typedef typename V::iterator::pointer pointer;
#endif

            // Construction and destruction
            NUMERICS_INLINE
            iterator ():
                container_reference<vector_range> (), it_ () {}
            NUMERICS_INLINE
            iterator (vector_range &vr, const iterator_type &it):
                container_reference<vector_range> (vr), it_ (it) {}

            // Arithmetic
            NUMERICS_INLINE
            iterator &operator ++ () {
                ++ it_;
                return *this;
            }
            NUMERICS_INLINE
            iterator &operator -- () {
                -- it_;
                return *this;
            }
            NUMERICS_INLINE
            iterator &operator += (difference_type n) {
                it_ += n;
                return *this;
            }
            NUMERICS_INLINE
            iterator &operator -= (difference_type n) {
                it_ -= n;
                return *this;
            }
            NUMERICS_INLINE
            difference_type operator - (const iterator &it) const {
                return it_ - it.it_;
            }

            // Dereference
            NUMERICS_INLINE
            reference operator * () const {
                check (index () < (*this) ().size (), bad_index ());
                return *it_;
            }

            // Index
            NUMERICS_INLINE
            size_type index () const {
                return it_.index () - (*this) ().start ();
            }

            // Assignment 
            NUMERICS_INLINE
            iterator &operator = (const iterator &it) {
                container_reference<vector_range>::assign (&it ());
                it_ = it.it_;
                return *this;
            }

            // Comparison
            NUMERICS_INLINE
            bool operator == (const iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ == it.it_;
            }
            NUMERICS_INLINE
            bool operator < (const iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ < it.it_;
            }

        private:
            iterator_type it_;

            friend class const_iterator;
        };
#endif

        NUMERICS_INLINE
        iterator begin () {
            return find_first (0);
        }
        NUMERICS_INLINE
        iterator end () {
            return find_last (size ());
        }

        // Reverse iterator

#ifdef BOOST_MSVC_STD_ITERATOR
        typedef reverse_iterator<const_iterator, value_type, value_type> const_reverse_iterator;
#else
        typedef reverse_iterator<const_iterator> const_reverse_iterator;
#endif

        NUMERICS_INLINE
        const_reverse_iterator rbegin () const {
            return const_reverse_iterator (end ());
        }
        NUMERICS_INLINE
        const_reverse_iterator rend () const {
            return const_reverse_iterator (begin ());
        }

#ifdef BOOST_MSVC_STD_ITERATOR
        typedef reverse_iterator<iterator, value_type, reference> reverse_iterator;
#else
        typedef reverse_iterator<iterator> reverse_iterator;
#endif

        NUMERICS_INLINE
        reverse_iterator rbegin () {
            return reverse_iterator (end ());
        }
        NUMERICS_INLINE
        reverse_iterator rend () {
            return reverse_iterator (begin ());
        }

    private:
        vector_type &data_;
        range r_;
        static vector_type nil_;
    };

    template<class V>
    typename vector_range<V>::vector_type vector_range<V>::nil_;

    // Projections
#ifdef NUMERICS_DEPRECATED
    template<class V>
    NUMERICS_INLINE
    vector_range<V> project (V &data, std::size_t start, std::size_t stop) {
        return vector_range<V> (data, start, stop);
    }
#endif
    template<class V>
    NUMERICS_INLINE
    vector_range<V> project (V &data, const range &r) {
        return vector_range<V> (data, r);
    }
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template<class V>
    NUMERICS_INLINE
    const vector_range<const V> project (const V &data, const range &r) {
        return vector_range<const V> (data, r);
    }
    template<class V>
    NUMERICS_INLINE
    vector_range<V> project (vector_range<V> &data, const range &r) {
        return data.project (r);
    }
    template<class V>
    NUMERICS_INLINE
    const vector_range<const V> project (const vector_range<const V> &data, const range &r) {
        return data.project (r);
    }
#endif

    // Vector based slice class
    template<class V>
    class vector_slice:
        public vector_expression<vector_slice<V> > {
    public:
        typedef const V const_vector_type;
        typedef V vector_type;
        typedef typename V::size_type size_type;
        typedef typename V::difference_type difference_type;
        typedef typename V::value_type value_type;
        typedef typename V::const_reference const_reference;
        typedef typename V::reference reference;
        typedef typename V::const_pointer const_pointer;
        typedef typename V::pointer pointer;
#ifdef NUMERICS_ET_CLOSURE_REFERENCE
        typedef const vector_const_reference<const vector_slice<vector_type> > const_closure_type;
        typedef vector_reference<vector_slice<vector_type> > closure_type;
#endif
#ifdef NUMERICS_ET_CLOSURE_VALUE
        typedef const vector_slice<vector_type> const_closure_type;
        typedef vector_slice<vector_type> closure_type;
#endif
        typedef slice::const_iterator const_iterator_type;
        typedef slice::const_iterator iterator_type;
        typedef typename storage_restrict_traits<typename V::storage_category,
                                                 dense_proxy_tag>::storage_category storage_category;

        // Construction and destruction
        NUMERICS_INLINE
        vector_slice ():
            data_ (nil_), s_ () {}
        NUMERICS_INLINE
        vector_slice (vector_type &data, const slice &s):
            data_ (data), s_ (s) {}
#ifdef NUMERICS_DEPRECATED
        NUMERICS_INLINE
        vector_slice (vector_type &data, size_type start, difference_type stride, size_type size):
            data_ (data), s_ (start, stride, size) {}
#endif

        // Accessors
        NUMERICS_INLINE
        size_type start () const { 
            return s_.start (); 
        }
        NUMERICS_INLINE
        difference_type stride () const { 
            return s_.stride (); 
        }
        NUMERICS_INLINE
        size_type size () const { 
            return s_.size (); 
        }
        NUMERICS_INLINE
        const_vector_type &data () const {
            return data_;
        }
        NUMERICS_INLINE
        vector_type &data () {
            return data_;
        }

        // Resetting
        NUMERICS_INLINE
        void reset (vector_type &data) {
            // data_ = data;
            data_.reset (data);
        }
        NUMERICS_INLINE
        void reset (vector_type &data, const slice &s) {
            // data_ = data;
            data_.reset (data);
            s_ = s;
        }

        // Element access
        NUMERICS_INLINE
        value_type operator () (size_type i) const {
            return data () (s_ (i)); 
        }
        NUMERICS_INLINE
        reference operator () (size_type i) {
            return data () (s_ (i)); 
        }

        NUMERICS_INLINE
        value_type operator [] (size_type i) const { 
            return (*this) (i); 
        }
        NUMERICS_INLINE
        reference operator [] (size_type i) {
            return (*this) (i); 
        }

        NUMERICS_INLINE
        vector_slice<const_vector_type> project (const range &r) const {
            return vector_slice<const_vector_type>  (data (), s_.composite (r));
        }
        NUMERICS_INLINE
        vector_slice<vector_type> project (const range &r) {
            return vector_slice<vector_type>  (data (), s_.composite (r));
        }
        NUMERICS_INLINE
        vector_slice<const_vector_type> project (const slice &s) const {
            return vector_slice<const_vector_type>  (data (), s_.composite (s));
        }
        NUMERICS_INLINE
        vector_slice<vector_type> project (const slice &s) {
            return vector_slice<vector_type>  (data (), s_.composite (s));
        }

        // Assignment
        NUMERICS_INLINE
        vector_slice &operator = (const vector_slice &vs) { 
            // FIXME: the slices could be differently sized.
            // std::copy (vs.begin (), vs.end (), begin ());
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (vs));
            return *this;
        }
        NUMERICS_INLINE
        vector_slice &assign_temporary (vector_slice &vs) {
            // FIXME: this is suboptimal.
            // return *this = vs;
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vs);
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_slice &operator = (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (ae));
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_slice &assign (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, NUMERICS_TYPENAME AE::value_type> > () (*this, ae);
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_slice &operator += (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (*this + ae));
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_slice &plus_assign (const vector_expression<AE> &ae) {
            vector_assign<scalar_plus_assign<value_type, NUMERICS_TYPENAME AE::value_type> > () (*this, ae);
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_slice &operator -= (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (*this - ae));
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_slice &minus_assign (const vector_expression<AE> &ae) {
            vector_assign<scalar_minus_assign<value_type, NUMERICS_TYPENAME AE::value_type> > () (*this, ae);
            return *this;
        }
        template<class AT>
        NUMERICS_INLINE
        vector_slice &operator *= (const AT &at) {
            vector_assign_scalar<scalar_multiplies_assign<value_type, AT> > () (*this, at);
            return *this;
        }
        template<class AT>
        NUMERICS_INLINE
        vector_slice &operator /= (const AT &at) {
            vector_assign_scalar<scalar_divides_assign<value_type, AT> > () (*this, at);
            return *this;
        }

        // Swapping
        NUMERICS_INLINE
        void swap (vector_slice &vs) {
            // Too unusual semantic.
            // check (this != &vs, external_logic ());
            if (this != &vs) {
                check (size () == vs.size (), bad_size ());
                std::swap_ranges (begin (), end (), vs.begin ());
            }
        }
#ifdef NUMERICS_FRIEND_FUNCTION
        NUMERICS_INLINE
        friend void swap (vector_slice &vs1, vector_slice &vs2) {
            vs1.swap (vs2);
        }
#endif

#ifdef NUMERICS_USE_INDEXED_ITERATOR
        typedef indexed_iterator<vector_slice<vector_type>, 
                                 NUMERICS_TYPENAME vector_type::iterator::iterator_category> iterator;
        typedef indexed_const_iterator<vector_slice<vector_type>,
                                       NUMERICS_TYPENAME vector_type::const_iterator::iterator_category> const_iterator;
#else
        class const_iterator;
        class iterator;
#endif

        // Element lookup
        NUMERICS_INLINE
        const_iterator find_first (size_type i) const {
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return const_iterator (*this, i);
#else
            return const_iterator (data (), s_.begin () + i);
#endif
        }
        NUMERICS_INLINE
        iterator find_first (size_type i) {
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return iterator (*this, i);
#else
            return iterator (data (), s_.begin () + i);
#endif
        }
        NUMERICS_INLINE
        const_iterator find_last (size_type i) const {
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return const_iterator (*this, i);
#else
            return const_iterator (data (), s_.begin () + i);
#endif
        }
        NUMERICS_INLINE
        iterator find_last (size_type i) {
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return iterator (*this, i);
#else
            return iterator (data (), s_.begin () + i);
#endif
        }

        // Iterators simply are indices.

#ifndef NUMERICS_USE_INDEXED_ITERATOR
        class const_iterator:
            public container_const_reference<vector_type>,
#ifdef NUMERICS_USE_ITERATOR_BASE_TRAITS
            public iterator_base_traits<typename V::const_iterator::iterator_category>::template
                        iterator_base<const_iterator, value_type>::type {
#else
            public random_access_iterator_base<typename V::const_iterator::iterator_category,
                                               const_iterator, value_type> {
#endif                                               
        public:
            typedef typename V::const_iterator::iterator_category iterator_category;
#ifndef BOOST_MSVC_STD_ITERATOR
            typedef typename V::const_iterator::difference_type difference_type;
            typedef typename V::const_iterator::value_type value_type;
            typedef typename V::const_iterator::value_type reference;
            typedef typename V::const_iterator::pointer pointer;
#endif

            // Construction and destruction
            NUMERICS_INLINE
            const_iterator ():
                container_const_reference<vector_type> (), it_ () {}
            NUMERICS_INLINE
            const_iterator (const vector_type &v, const const_iterator_type &it):
                container_const_reference<vector_type> (v), it_ (it) {}
#ifndef NUMERICS_QUALIFIED_TYPENAME
            NUMERICS_INLINE
            const_iterator (const iterator &it):
                container_const_reference<vector_type> (it ()), it_ (it.it_) {}
#else
            NUMERICS_INLINE
            const_iterator (const typename vector_slice::iterator &it):
                container_const_reference<vector_type> (it ()), it_ (it.it_) {}
#endif

            // Arithmetic
            NUMERICS_INLINE
            const_iterator &operator ++ () {
                ++ it_;
                return *this;
            }
            NUMERICS_INLINE
            const_iterator &operator -- () {
                -- it_;
                return *this;
            }
            NUMERICS_INLINE
            const_iterator &operator += (difference_type n) {
                it_ += n;
                return *this;
            }
            NUMERICS_INLINE
            const_iterator &operator -= (difference_type n) {
                it_ -= n;
                return *this;
            }
            NUMERICS_INLINE
            difference_type operator - (const const_iterator &it) const {
                return it_ - it.it_;
            }

            // Dereference
            NUMERICS_INLINE
            value_type operator * () const {
                check (index () < (*this) ().size (), bad_index ());
                return (*this) () (*it_);
            }

            // Index
            NUMERICS_INLINE
            size_type index () const {
                return it_.index ();
            }

            // Assignment 
            NUMERICS_INLINE
            const_iterator &operator = (const const_iterator &it) {
                container_const_reference<vector_type>::assign (&it ());
                it_ = it.it_;
                return *this;
            }

            // Comparison
            NUMERICS_INLINE
            bool operator == (const const_iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ == it.it_;
            }
            NUMERICS_INLINE
            bool operator < (const const_iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ < it.it_;
            }

        private:
            const_iterator_type it_;
        };
#endif

        NUMERICS_INLINE
        const_iterator begin () const {
            return find_first (0);
        }
        NUMERICS_INLINE
        const_iterator end () const {
            return find_last (size ());
        }

#ifndef NUMERICS_USE_INDEXED_ITERATOR
        class iterator:
            public container_reference<vector_type>,
#ifdef NUMERICS_USE_ITERATOR_BASE_TRAITS
            public iterator_base_traits<typename V::iterator::iterator_category>::template
                        iterator_base<iterator, value_type>::type {
#else
            public random_access_iterator_base<typename V::iterator::iterator_category,
                                               iterator, value_type> {
#endif
        public:
            typedef typename V::iterator::iterator_category iterator_category;
#ifndef BOOST_MSVC_STD_ITERATOR
            typedef typename V::iterator::difference_type difference_type;
            typedef typename V::iterator::value_type value_type;
            typedef typename V::iterator::reference reference;
            typedef typename V::iterator::pointer pointer;
#endif

            // Construction and destruction
            NUMERICS_INLINE
            iterator ():
                container_reference<vector_type> (), it_ () {}
            NUMERICS_INLINE
            iterator (vector_type &v, const iterator_type &it):
                container_reference<vector_type> (v), it_ (it) {}

            // Arithmetic
            NUMERICS_INLINE
            iterator &operator ++ () {
                ++ it_;
                return *this;
            }
            NUMERICS_INLINE
            iterator &operator -- () {
                -- it_;
                return *this;
            }
            NUMERICS_INLINE
            iterator &operator += (difference_type n) {
                it_ += n;
                return *this;
            }
            NUMERICS_INLINE
            iterator &operator -= (difference_type n) {
                it_ -= n;
                return *this;
            }
            NUMERICS_INLINE
            difference_type operator - (const iterator &it) const {
                return it_ - it.it_;
            }

            // Dereference
            NUMERICS_INLINE
            reference operator * () const {
                check (index () < (*this) ().size (), bad_index ());
                return (*this) () (*it_);
            }

            // Index
            NUMERICS_INLINE
            size_type index () const {
                return it_.index ();
            }

            // Assignment 
            NUMERICS_INLINE
            iterator &operator = (const iterator &it) {
                container_reference<vector_type>::assign (&it ());
                it_ = it.it_;
                return *this;
            }

            // Comparison
            NUMERICS_INLINE
            bool operator == (const iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ == it.it_;
            }
            NUMERICS_INLINE
            bool operator < (const iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ < it.it_;
            }

        private:
            iterator_type it_;

            friend class const_iterator;
        };
#endif

        NUMERICS_INLINE
        iterator begin () {
            return find_first (0);
        }
        NUMERICS_INLINE
        iterator end () {
            return find_last (size ());
        }

        // Reverse iterator

#ifdef BOOST_MSVC_STD_ITERATOR
        typedef reverse_iterator<const_iterator, value_type, value_type> const_reverse_iterator;
#else
        typedef reverse_iterator<const_iterator> const_reverse_iterator;
#endif

        NUMERICS_INLINE
        const_reverse_iterator rbegin () const {
            return const_reverse_iterator (end ());
        }
        NUMERICS_INLINE
        const_reverse_iterator rend () const {
            return const_reverse_iterator (begin ());
        }

#ifdef BOOST_MSVC_STD_ITERATOR
        typedef reverse_iterator<iterator, value_type, reference> reverse_iterator;
#else
        typedef reverse_iterator<iterator> reverse_iterator;
#endif

        NUMERICS_INLINE
        reverse_iterator rbegin () {
            return reverse_iterator (end ());
        }
        NUMERICS_INLINE
        reverse_iterator rend () {
            return reverse_iterator (begin ());
        }

    private:
        vector_type &data_;
        slice s_;
        static vector_type nil_;
    };

    template<class V>
    typename vector_slice<V>::vector_type vector_slice<V>::nil_;

    // Projections
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template<class V>
    NUMERICS_INLINE
    vector_slice<V> project (vector_slice<V> &data, const range &r) {
        return data.project (r);
    }
    template<class V>
    NUMERICS_INLINE
    const vector_slice<const V> project (const vector_slice<const V> &data, const range &r) {
        return data.project (r);
    }
#endif
    template<class V>
    NUMERICS_INLINE
    vector_slice<V> project (V &data, const slice &s) {
        return vector_slice<V> (data, s);
    }
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template<class V>
    NUMERICS_INLINE
    const vector_slice<const V> project (const V &data, const slice &s) {
        return vector_slice<const V> (data, s);
    }
    template<class V>
    NUMERICS_INLINE
    vector_slice<V> project (vector_slice<V> &data, const slice &s) {
        return data.project (s);
    }
    template<class V>
    NUMERICS_INLINE
    const vector_slice<const V> project (const vector_slice<const V> &data, const slice &s) {
        return data.project (s);
    }
#endif

    // Vector based indirection class
    // Contributed by Toon Knapen.
    template<class V>
    class vector_indirect:
        public vector_expression<vector_indirect<V> > {
    public:
        typedef const V const_vector_type;
        typedef V vector_type;
        typedef typename V::size_type size_type;
        typedef typename V::difference_type difference_type;
        typedef typename V::value_type value_type;
        typedef typename V::const_reference const_reference;
        typedef typename V::reference reference;
        typedef typename V::const_pointer const_pointer;
        typedef typename V::pointer pointer;
#ifdef NUMERICS_ET_CLOSURE_REFERENCE
        typedef const vector_const_reference<const vector_indirect<vector_type> > const_closure_type;
        typedef vector_reference<vector_indirect<vector_type> > closure_type;
#endif
#ifdef NUMERICS_ET_CLOSURE_VALUE
        typedef const vector_indirect<vector_type> const_closure_type;
        typedef vector_indirect<vector_type> closure_type;
#endif
        typedef indirect_array<>::const_iterator const_iterator_type;
        typedef indirect_array<>::const_iterator iterator_type;
        typedef typename storage_restrict_traits<typename V::storage_category,
                                                 dense_proxy_tag>::storage_category storage_category;

        // Construction and destruction
        NUMERICS_INLINE
        vector_indirect ():
            data_ (nil_), ia_ () {}
        NUMERICS_INLINE
        vector_indirect (vector_type &data, const indirect_array<> &ia):
            data_ (data), ia_ (ia) {}

        // Accessors
        NUMERICS_INLINE
        size_type size () const {
            return ia_.size ();
        }
        NUMERICS_INLINE
        const_vector_type &data () const {
            return data_;
        }
        NUMERICS_INLINE
        vector_type &data () {
            return data_;
        }

        // Resetting
        NUMERICS_INLINE
        void reset (vector_type &data) {
            // data_ = data;
            data_.reset (data);
        }
        NUMERICS_INLINE
        void reset (vector_type &data, const indirect_array<> &ia) {
            // data_ = data;
            data_.reset (data);
            ia_ = ia;
        }

        // Element access
        NUMERICS_INLINE
        value_type operator () (size_type i) const {
            return data () (ia_ (i));
        }
        NUMERICS_INLINE
        reference operator () (size_type i) {
            return data () (ia_ (i));
        }

        NUMERICS_INLINE
        value_type operator [] (size_type i) const {
            return (*this) (i);
        }
        NUMERICS_INLINE
        reference operator [] (size_type i) {
            return (*this) (i);
        }

        NUMERICS_INLINE
        vector_indirect<const_vector_type> project (const range &r) const {
            return vector_indirect<const_vector_type>  (data (), ia_.composite (r));
        }
        NUMERICS_INLINE
        vector_indirect<vector_type> project (const range &r) {
            return vector_indirect<vector_type>  (data (), ia_.composite (r));
        }
        NUMERICS_INLINE
        vector_indirect<const_vector_type> project (const slice &s) const {
            return vector_indirect<const_vector_type>  (data (), ia_.composite (s));
        }
        NUMERICS_INLINE
        vector_indirect<vector_type> project (const slice &s) {
            return vector_indirect<vector_type>  (data (), ia_.composite (s));
        }
        NUMERICS_INLINE
        vector_indirect<const_vector_type> project (const indirect_array<> &ia) const {
            return vector_indirect<const_vector_type>  (data (), ia_.composite (ia));
        }
        NUMERICS_INLINE
        vector_indirect<vector_type> project (const indirect_array<> &ia) {
            return vector_indirect<vector_type>  (data (), ia_.composite (ia));
        }

        // Assignment
        NUMERICS_INLINE
        vector_indirect &operator = (const vector_indirect &vi) {
            // FIXME: the indirect_arrays could be differently sized.
            // std::copy (vi.begin (), vi.end (), begin ());
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (vi));
            return *this;
        }
        NUMERICS_INLINE
        vector_indirect &assign_temporary (vector_indirect &vi) {
            // FIXME: this is suboptimal.
            // return *this = vi;
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vi);
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_indirect &operator = (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (ae));
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_indirect &assign (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, NUMERICS_TYPENAME AE::value_type> > () (*this, ae);
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_indirect &operator += (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (*this + ae));
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_indirect &plus_assign (const vector_expression<AE> &ae) {
            vector_assign<scalar_plus_assign<value_type, NUMERICS_TYPENAME AE::value_type> > () (*this, ae);
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_indirect &operator -= (const vector_expression<AE> &ae) {
            vector_assign<scalar_assign<value_type, value_type> > () (*this, vector<value_type> (*this - ae));
            return *this;
        }
        template<class AE>
        NUMERICS_INLINE
        vector_indirect &minus_assign (const vector_expression<AE> &ae) {
            vector_assign<scalar_minus_assign<value_type, NUMERICS_TYPENAME AE::value_type> > () (*this, ae);
            return *this;
        }
        template<class AT>
        NUMERICS_INLINE
        vector_indirect &operator *= (const AT &at) {
            vector_assign_scalar<scalar_multiplies_assign<value_type, AT> > () (*this, at);
            return *this;
        }
        template<class AT>
        NUMERICS_INLINE
        vector_indirect &operator /= (const AT &at) {
            vector_assign_scalar<scalar_divides_assign<value_type, AT> > () (*this, at);
            return *this;
        }

        // Swapping
        NUMERICS_INLINE
        void swap (vector_indirect &vi) {
            // Too unusual semantic.
            // check (this != &vi, external_logic ());
            if (this != &vi) {
                check (size () == vi.size (), bad_size ());
                std::swap_ranges (begin (), end (), vi.begin ());
            }
        }
#ifdef NUMERICS_FRIEND_FUNCTION
        NUMERICS_INLINE
        friend void swap (vector_indirect &vi1, vector_indirect &vi2) {
            vi1.swap (vi2);
        }
#endif

#ifdef NUMERICS_USE_INDEXED_ITERATOR
        typedef indexed_iterator<vector_indirect<vector_type>,
                                 NUMERICS_TYPENAME vector_type::iterator::iterator_category> iterator;
        typedef indexed_const_iterator<vector_indirect<vector_type>,
                                       NUMERICS_TYPENAME vector_type::const_iterator::iterator_category> const_iterator;
#else
        class const_iterator;
        class iterator;
#endif

        // Element lookup
        NUMERICS_INLINE
        const_iterator find_first (size_type i) const {
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return const_iterator (*this, i);
#else
            return const_iterator (data (), ia_.begin () + i);
#endif
        }
        NUMERICS_INLINE
        iterator find_first (size_type i) {
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return iterator (*this, i);
#else
            return iterator (data (), ia_.begin () + i);
#endif
        }
        NUMERICS_INLINE
        const_iterator find_last (size_type i) const {
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return const_iterator (*this, i);
#else
            return const_iterator (data (), ia_.begin () + i);
#endif
        }
        NUMERICS_INLINE
        iterator find_last (size_type i) {
#ifdef NUMERICS_USE_INDEXED_ITERATOR
            return iterator (*this, i);
#else
            return iterator (data (), ia_.begin () + i);
#endif
        }

        // Iterators simply are indices.

#ifndef NUMERICS_USE_INDEXED_ITERATOR
        class const_iterator:
            public container_const_reference<vector_type>,
#ifdef NUMERICS_USE_ITERATOR_BASE_TRAITS
            public iterator_base_traits<typename V::const_iterator::iterator_category>::template
                        iterator_base<const_iterator, value_type>::type {
#else
            public random_access_iterator_base<typename V::const_iterator::iterator_category,
                                               const_iterator, value_type> {
#endif
        public:
            typedef typename V::const_iterator::iterator_category iterator_category;
#ifndef BOOST_MSVC_STD_ITERATOR
            typedef typename V::const_iterator::difference_type difference_type;
            typedef typename V::const_iterator::value_type value_type;
            typedef typename V::const_iterator::value_type reference;
            typedef typename V::const_iterator::pointer pointer;
#endif

            // Construction and destruction
            NUMERICS_INLINE
            const_iterator ():
                container_const_reference<vector_type> (), it_ () {}
            NUMERICS_INLINE
            const_iterator (const vector_type &v, const const_iterator_type &it):
                container_const_reference<vector_type> (v), it_ (it) {}
#ifndef NUMERICS_QUALIFIED_TYPENAME
            NUMERICS_INLINE
            const_iterator (const iterator &it):
                container_const_reference<vector_type> (it ()), it_ (it.it_) {}
#else
            NUMERICS_INLINE
            const_iterator (const typename vector_indirect::iterator &it):
                container_const_reference<vector_type> (it ()), it_ (it.it_) {}
#endif

            // Arithmetic
            NUMERICS_INLINE
            const_iterator &operator ++ () {
                ++ it_;
                return *this;
            }
            NUMERICS_INLINE
            const_iterator &operator -- () {
                -- it_;
                return *this;
            }
            NUMERICS_INLINE
            const_iterator &operator += (difference_type n) {
                it_ += n;
                return *this;
            }
            NUMERICS_INLINE
            const_iterator &operator -= (difference_type n) {
                it_ -= n;
                return *this;
            }
            NUMERICS_INLINE
            difference_type operator - (const const_iterator &it) const {
                return it_ - it.it_;
            }

            // Dereference
            NUMERICS_INLINE
            value_type operator * () const {
                check (index () < (*this) ().size (), bad_index ());
                return (*this) () (*it_);
            }

            // Index
            NUMERICS_INLINE
            size_type index () const {
                return it_.index ();
            }

            // Assignment
            NUMERICS_INLINE
            const_iterator &operator = (const const_iterator &it) {
                container_const_reference<vector_type>::assign (&it ());
                it_ = it.it_;
                return *this;
            }

            // Comparison
            NUMERICS_INLINE
            bool operator == (const const_iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ == it.it_;
            }
            NUMERICS_INLINE
            bool operator < (const const_iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ < it.it_;
            }

        private:
            const_iterator_type it_;
        };
#endif

        NUMERICS_INLINE
        const_iterator begin () const {
            return find_first (0);
        }
        NUMERICS_INLINE
        const_iterator end () const {
            return find_last (size ());
        }

#ifndef NUMERICS_USE_INDEXED_ITERATOR
        class iterator:
            public container_reference<vector_type>,
#ifdef NUMERICS_USE_ITERATOR_BASE_TRAITS
            public iterator_base_traits<typename V::iterator::iterator_category>::template
                        iterator_base<iterator, value_type>::type {
#else
            public random_access_iterator_base<typename V::iterator::iterator_category,
                                               iterator, value_type> {
#endif
        public:
            typedef typename V::iterator::iterator_category iterator_category;
#ifndef BOOST_MSVC_STD_ITERATOR
            typedef typename V::iterator::difference_type difference_type;
            typedef typename V::iterator::value_type value_type;
            typedef typename V::iterator::reference reference;
            typedef typename V::iterator::pointer pointer;
#endif

            // Construction and destruction
            NUMERICS_INLINE
            iterator ():
                container_reference<vector_type> (), it_ () {}
            NUMERICS_INLINE
            iterator (vector_type &v, const iterator_type &it):
                container_reference<vector_type> (v), it_ (it) {}

            // Arithmetic
            NUMERICS_INLINE
            iterator &operator ++ () {
                ++ it_;
                return *this;
            }
            NUMERICS_INLINE
            iterator &operator -- () {
                -- it_;
                return *this;
            }
            NUMERICS_INLINE
            iterator &operator += (difference_type n) {
                it_ += n;
                return *this;
            }
            NUMERICS_INLINE
            iterator &operator -= (difference_type n) {
                it_ -= n;
                return *this;
            }
            NUMERICS_INLINE
            difference_type operator - (const iterator &it) const {
                return it_ - it.it_;
            }

            // Dereference
            NUMERICS_INLINE
            reference operator * () const {
                check (index () < (*this) ().size (), bad_index ());
                return (*this) () (*it_);
            }

            // Index
            NUMERICS_INLINE
            size_type index () const {
                return it_.index ();
            }

            // Assignment
            NUMERICS_INLINE
            iterator &operator = (const iterator &it) {
                container_reference<vector_type>::assign (&it ());
                it_ = it.it_;
                return *this;
            }

            // Comparison
            NUMERICS_INLINE
            bool operator == (const iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ == it.it_;
            }
            NUMERICS_INLINE
            bool operator < (const iterator &it) const {
                check (&(*this) () == &it (), external_logic ());
                return it_ < it.it_;
            }

        private:
            iterator_type it_;

            friend class const_iterator;
        };
#endif

        NUMERICS_INLINE
        iterator begin () {
            return find_first (0);
        }
        NUMERICS_INLINE
        iterator end () {
            return find_last (size ());
        }

        // Reverse iterator

#ifdef BOOST_MSVC_STD_ITERATOR
        typedef reverse_iterator<const_iterator, value_type, value_type> const_reverse_iterator;
#else
        typedef reverse_iterator<const_iterator> const_reverse_iterator;
#endif

        NUMERICS_INLINE
        const_reverse_iterator rbegin () const {
            return const_reverse_iterator (end ());
        }
        NUMERICS_INLINE
        const_reverse_iterator rend () const {
            return const_reverse_iterator (begin ());
        }

#ifdef BOOST_MSVC_STD_ITERATOR
        typedef reverse_iterator<iterator, value_type, reference> reverse_iterator;
#else
        typedef reverse_iterator<iterator> reverse_iterator;
#endif

        NUMERICS_INLINE
        reverse_iterator rbegin () {
            return reverse_iterator (end ());
        }
        NUMERICS_INLINE
        reverse_iterator rend () {
            return reverse_iterator (begin ());
        }

    private:
        vector_type &data_;
        indirect_array<> ia_;
        static vector_type nil_;
    };

    template<class V>
    typename vector_indirect<V>::vector_type vector_indirect<V>::nil_;

    // Projections
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template<class V>
    NUMERICS_INLINE
    vector_indirect<V> project (vector_indirect<V> &data, const range &r) {
        return data.project (r);
    }
    template<class V>
    NUMERICS_INLINE
    const vector_indirect<const V> project (const vector_indirect<const V> &data, const range &r) {
        return data.project (r);
    }
    template<class V>
    NUMERICS_INLINE
    vector_indirect<V> project (vector_indirect<V> &data, const slice &s) {
        return data.project (s);
    }
    template<class V>
    NUMERICS_INLINE
    const vector_indirect<const V> project (const vector_indirect<const V> &data, const slice &s) {
        return data.project (s);
    }
#endif
    template<class V>
    NUMERICS_INLINE
    vector_indirect<V> project (V &data, const indirect_array<> &ia) {
        return vector_indirect<V> (data, ia);
    }
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template<class V>
    NUMERICS_INLINE
    const vector_indirect<const V> project (const V &data, const indirect_array<> &ia) {
        return vector_indirect<const V> (data, ia);
    }
    template<class V>
    NUMERICS_INLINE
    vector_indirect<V> project (vector_indirect<V> &data, const indirect_array<> &ia) {
        return data.project (ia);
    }
    template<class V>
    NUMERICS_INLINE
    const vector_indirect<const V> project (const vector_indirect<const V> &data, const indirect_array<> &ia) {
        return data.project (ia);
    }
#endif

}}

#endif




