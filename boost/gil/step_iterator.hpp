/*
    Copyright 2005-2007 Adobe Systems Incorporated
   
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).

    See http://opensource.adobe.com/gil for most recent version including documentation.
*/

/*************************************************************************************************/

#ifndef GIL_STEP_ITERATOR_H
#define GIL_STEP_ITERATOR_H

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief pixel step iterator
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date   2005-2007 \n Last updated on February 12, 2007
///
////////////////////////////////////////////////////////////////////////////////////////

#include <cstddef>
#include <iterator>
#include <boost/iterator/iterator_facade.hpp>
#include "gil_config.hpp"
#include "utilities.hpp"
#include "pixel_iterator.hpp"
#include "pixel_iterator_adaptor.hpp"

namespace boost { namespace gil {

/// \defgroup PixelIteratorModelStepPtr step iterators
/// \ingroup PixelIteratorModel
/// \brief Iterators that allow for specifying the step between two adjacent values


namespace detail {

/// \ingroup PixelIteratorModelStepPtr
/// \brief An adaptor over an existing iterator that changes the step unit
///
/// (i.e. distance(it,it+1)) by a given predicate. Instead of calling base's 
/// operators ++, --, +=, -=, etc. the adaptor is using the passed policy object SFn
/// for advancing and for computing the distance between iterators.

template <typename Derived,  // type of the derived class
          typename Iterator, // Models Iterator
          typename SFn>      // A policy object that can compute the distance between two iterators of type Iterator
                             // and can advance an iterator of type Iterator a given number of Iterator's units  
class step_iterator_adaptor : public iterator_adaptor<Derived, Iterator, use_default, use_default, use_default, typename SFn::difference_type> {
public:
    typedef iterator_adaptor<Derived, Iterator, use_default, use_default, use_default, typename SFn::difference_type> parent_t;
    typedef typename std::iterator_traits<Iterator>::difference_type base_difference_type;
    typedef typename SFn::difference_type                           difference_type;
    typedef typename std::iterator_traits<Iterator>::reference       reference;

    step_iterator_adaptor() {}
    step_iterator_adaptor(const Iterator& it, SFn step_fn=SFn()) : parent_t(it), _step_fn(step_fn) {}

    difference_type step() const { return _step_fn.step(); }

protected:
    SFn _step_fn;
private:
    friend class boost::iterator_core_access;

    void increment() { _step_fn.advance(this->base_reference(),1); }
    void decrement() { _step_fn.advance(this->base_reference(),-1); }
    void advance(base_difference_type d) { _step_fn.advance(this->base_reference(),d); }
    difference_type distance_to(const step_iterator_adaptor& it) const { return _step_fn.difference(this->base_reference(),it.base_reference()); }
};

// although iterator_adaptor defines these, the default implementation computes distance and compares for zero.
// it is often faster to just apply the relation operator to the base
template <typename D,typename Iterator,typename SFn> inline
bool operator>(const step_iterator_adaptor<D,Iterator,SFn>& p1, const step_iterator_adaptor<D,Iterator,SFn>& p2) { 
    return p1.step()>0 ? p1.base()> p2.base() : p1.base()< p2.base(); 
}

template <typename D,typename Iterator,typename SFn> inline
bool operator<(const step_iterator_adaptor<D,Iterator,SFn>& p1, const step_iterator_adaptor<D,Iterator,SFn>& p2) { 
    return p1.step()>0 ? p1.base()< p2.base() : p1.base()> p2.base(); 
}

template <typename D,typename Iterator,typename SFn> inline
bool operator>=(const step_iterator_adaptor<D,Iterator,SFn>& p1, const step_iterator_adaptor<D,Iterator,SFn>& p2) { 
    return p1.step()>0 ? p1.base()>=p2.base() : p1.base()<=p2.base(); 
}

template <typename D,typename Iterator,typename SFn> inline
bool operator<=(const step_iterator_adaptor<D,Iterator,SFn>& p1, const step_iterator_adaptor<D,Iterator,SFn>& p2) { 
    return p1.step()>0 ? p1.base()<=p2.base() : p1.base()>=p2.base(); 
}

template <typename D,typename Iterator,typename SFn> inline
bool operator==(const step_iterator_adaptor<D,Iterator,SFn>& p1, const step_iterator_adaptor<D,Iterator,SFn>& p2) { 
    return p1.base()==p2.base(); 
}

template <typename D,typename Iterator,typename SFn> inline
bool operator!=(const step_iterator_adaptor<D,Iterator,SFn>& p1, const step_iterator_adaptor<D,Iterator,SFn>& p2) { 
    return p1.base()!=p2.base(); 
}

} // namespace detail

////////////////////////////////////////////////////////////////////////////////////////
///                 BYTE-ADDRESSABLE STEP ITERATOR
////////////////////////////////////////////////////////////////////////////////////////

/// \class byte_addressable_step_iterator
/// \ingroup PixelIteratorModelStepPtr PixelBasedModel
/// \brief Iterator with dynamically specified step in bytes. Models StepIteratorConcept, IteratorAdaptorConcept, ByteAdvanceableIteratorConcept, PixelIteratorConcept, HasDynamicXStepTypeConcept
///
/// A refinement of step_iterator_adaptor that uses a dynamic parameter for the step 
/// which is specified in bytes
///
/// Pixel step iterators are used to provide iteration over non-adjacent pixels. 
/// Common use is a vertical traversal, where the step is the row stride.
///
/// Another application is as a sub-channel view. For example, a red intensity image over 
/// interleaved RGB data would use a step iterator adaptor with step sizeof(channel_t)*3
/// In the latter example the step size could be fixed at compile time for efficiency.
/// Compile-time fixed step can be implemented by providing a step function object that takes the step as a template
////////////////////////////////////////////////////////////////////////////////////////

/// \ingroup PixelIteratorModelStepPtr
/// \brief function object that returns the byte distance between two iterators and advances a given iterator a given number of bytes
template <typename Iterator>
struct byte_step_fn {
    typedef std::ptrdiff_t difference_type;

    byte_step_fn(difference_type step=byte_step(Iterator())) : _step(step) {}

    difference_type difference(const Iterator& it1, const Iterator& it2) const { return byte_distance(it1,it2)/_step; }
    void            advance(Iterator& it, difference_type d)       const { byte_advance(it,d*_step); } 
    difference_type step()                                   const { return _step; }

    void            set_step(std::ptrdiff_t step) { _step=step; }
private:
    GIL_CLASS_REQUIRE(Iterator, boost::gil, ByteAdvanceableIteratorConcept);
    difference_type _step;
};

template <typename Iterator>
class byte_addressable_step_iterator : public detail::step_iterator_adaptor<byte_addressable_step_iterator<Iterator>, 
                                                                            Iterator, 
                                                                            byte_step_fn<Iterator> > {
    GIL_CLASS_REQUIRE(Iterator, boost::gil, ByteAdvanceableIteratorConcept);
public:
    typedef detail::step_iterator_adaptor<byte_addressable_step_iterator<Iterator>, 
                                          Iterator, 
                                          byte_step_fn<Iterator> > parent_t;
    typedef typename parent_t::reference                            reference;
    typedef typename parent_t::difference_type                      difference_type;
    typedef Iterator                                                x_iterator;

    byte_addressable_step_iterator() : parent_t(Iterator()) {}
    byte_addressable_step_iterator(Iterator it, std::ptrdiff_t byte_step) : parent_t(it, byte_step_fn<Iterator>(byte_step)) {}
    template <typename I2> 
    byte_addressable_step_iterator(const byte_addressable_step_iterator<I2>& it)
        : parent_t(it.base(), byte_step_fn<Iterator>(it.step())) {}

    /// For some reason operator[] provided by iterator_adaptor returns a custom class that is convertible to reference
    /// We require our own reference because it is registered in iterator_traits
    reference operator[](difference_type d) const { return *(*this+d); }

    void set_step(std::ptrdiff_t byte_step) { this->_step_fn.set_step(byte_step); }

    x_iterator& base()              { return parent_t::base_reference(); }
    x_iterator const& base() const  { return parent_t::base_reference(); }
};

template <typename Iterator>
struct const_iterator_type<byte_addressable_step_iterator<Iterator> > {
    typedef byte_addressable_step_iterator<typename const_iterator_type<Iterator>::type>  type;
};

template <typename Iterator>
struct iterator_is_mutable<byte_addressable_step_iterator<Iterator> > : public iterator_is_mutable<Iterator> {};


/////////////////////////////
//  IteratorAdaptorConcept
/////////////////////////////

template <typename Iterator>
struct is_iterator_adaptor<byte_addressable_step_iterator<Iterator> > : public mpl::true_{};

template <typename Iterator>
struct iterator_adaptor_get_base<byte_addressable_step_iterator<Iterator> > {
    typedef Iterator type;
};

template <typename Iterator, typename NewBaseIterator>
struct iterator_adaptor_rebind<byte_addressable_step_iterator<Iterator>,NewBaseIterator> {
    typedef byte_addressable_step_iterator<NewBaseIterator> type;
};

/////////////////////////////
//  PixelBasedConcept
/////////////////////////////

template <typename Iterator>
struct color_space_type<byte_addressable_step_iterator<Iterator> > : public color_space_type<Iterator> {};

template <typename Iterator>
struct channel_mapping_type<byte_addressable_step_iterator<Iterator> > : public channel_mapping_type<Iterator> {};

template <typename Iterator>
struct is_planar<byte_addressable_step_iterator<Iterator> > : public is_planar<Iterator> {};

template <typename Iterator>
struct channel_type<byte_addressable_step_iterator<Iterator> > : public channel_type<Iterator> {};

/////////////////////////////
//  ByteAdvanceableIteratorConcept
/////////////////////////////

template <typename Iterator>
inline std::ptrdiff_t byte_step(const byte_addressable_step_iterator<Iterator>& p) { return p.step(); }

template <typename Iterator>
inline std::ptrdiff_t byte_distance(const byte_addressable_step_iterator<Iterator>& p1, 
                                    const byte_addressable_step_iterator<Iterator>& p2) { 
    return byte_distance(p1.base(),p2.base()); 
}

template <typename Iterator>
inline void byte_advance(byte_addressable_step_iterator<Iterator>& p, 
                         std::ptrdiff_t byteDiff) { 
    byte_advance(p.base(), byteDiff);
}

template <typename Iterator>
inline byte_addressable_step_iterator<Iterator> 
byte_advanced(const byte_addressable_step_iterator<Iterator>& p, 
              std::ptrdiff_t byteDiff) {
    return byte_addressable_step_iterator<Iterator>(byte_advanced(p.base(), byteDiff),p.step());
}

template <typename Iterator>
inline typename std::iterator_traits<Iterator>::reference 
byte_advanced_ref(const byte_addressable_step_iterator<Iterator>& p, 
                  std::ptrdiff_t byteDiff) { 
    return byte_advanced_ref(p.base(), byteDiff);
}

/////////////////////////////
//  HasDynamicXStepTypeConcept
/////////////////////////////

template <typename Iterator>
struct dynamic_x_step_type<byte_addressable_step_iterator<Iterator> > {
    typedef byte_addressable_step_iterator<Iterator> type;
};

// For step iterators, pass the function object to the base
template <typename Iterator, typename Deref>
struct iterator_add_deref<byte_addressable_step_iterator<Iterator>,Deref> {
    GIL_CLASS_REQUIRE(Deref, boost::gil, PixelDereferenceAdaptorConcept);

    typedef byte_addressable_step_iterator<typename iterator_add_deref<Iterator, Deref>::type> type;

    static type make(const byte_addressable_step_iterator<Iterator>& it, const Deref& d) { return type(iterator_add_deref<Iterator, Deref>::make(it.base(),d),it.step()); }
};

////////////////////////////////////////////////////////////////////////////////////////
/// make_step_iterator
////////////////////////////////////////////////////////////////////////////////////////

template <typename I> typename dynamic_x_step_type<I>::type make_step_iterator(const I& it, std::ptrdiff_t step);

namespace detail {

// if the iterator is a plain base iterator (non-adaptor), wraps it in byte_addressable_step_iterator
template <typename I> 
typename dynamic_x_step_type<I>::type make_step_iterator_impl(const I& it, std::ptrdiff_t step, mpl::false_) {
    return byte_addressable_step_iterator<I>(it, step);
}

// If the iterator is compound, put the step in its base
template <typename I> 
typename dynamic_x_step_type<I>::type make_step_iterator_impl(const I& it, std::ptrdiff_t step, mpl::true_) {
    return make_step_iterator(it.base(), step);
}

// If the iterator is byte_addressable_step_iterator, change the step
template <typename BaseIt> 
byte_addressable_step_iterator<BaseIt> make_step_iterator_impl(const byte_addressable_step_iterator<BaseIt>& it, std::ptrdiff_t step, mpl::true_) {
    return byte_addressable_step_iterator<BaseIt>(it.base(), step);
}
}

/// \brief Constructs a step iterator from a base iterator and a step.
///
/// To construct a step iterator from a given iterator Iterator and a given step, if Iterator does not
/// already have a dynamic step, we wrap it in a byte_addressable_step_iterator. Otherwise we 
/// do a compile-time traversal of the chain of iterator adaptors to locate the step iterator
/// and then set it step to the new one.
///
/// The step iterator of Iterator is not always byte_addressable_step_iterator<Iterator>. For example, Iterator may
/// already be a byte_addressable_step_iterator, in which case it will be inefficient to stack them;
/// we can obtain the same result by multiplying their steps. Note that for Iterator to be a
/// step iterator it does not necessarily have to have the form byte_addressable_step_iterator<J>.
/// The step iterator can be wrapped inside another iterator. Also, it may not have the
/// type byte_addressable_step_iterator, but it could be a user-provided type.
template <typename I>  // Models ByteAdvanceableIteratorConcept, HasDynamicXStepTypeConcept
typename dynamic_x_step_type<I>::type make_step_iterator(const I& it, std::ptrdiff_t step) {
    return detail::make_step_iterator_impl(it, step, typename is_iterator_adaptor<I>::type());
}

} }  // namespace boost::gil

#endif
