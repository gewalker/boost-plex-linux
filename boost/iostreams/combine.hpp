// (C) Copyright Jonathan Turkanis 2003.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// To do: add support for random-access.

#ifndef BOOST_IOSTREAMS_COMBINE_HPP_INCLUDED
#define BOOST_IOSTREAMS_COMBINE_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#include <boost/config.hpp> // NO_STD_LOCALE, DEDUCED_TYPENAME.
#ifndef BOOST_NO_STD_LOCALE
# include <locale>
#endif
//#include <boost/detail/workaround.hpp>       
#include <boost/iostreams/detail/wrap_unwrap.hpp>       
#include <boost/iostreams/traits.hpp>         
#include <boost/iostreams/is_device.hpp>     
#include <boost/iostreams/operations.hpp>        
#include <boost/mpl/if.hpp>    
#include <boost/static_assert.hpp>  
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_same.hpp> 

namespace boost { namespace iostreams {

namespace detail {

//
// Template name: combined_device.
// Description: Model of Device defined in terms of a Source/Sink pair.
// Template paramters:
//      Source - A model of Source, with the same char_type and traits_type
//          as Sink.
//      Sink - A model of Sink, with the same char_type and traits_type
//          as Source.
//
template<typename Source, typename Sink>
class combined_device {
public:
    typedef BOOST_IOSTREAMS_CHAR_TYPE(Source) char_type;
    struct io_category
        : bidirectional, 
          device_tag, 
          closable_tag, 
          localizable_tag
        { };
    combined_device(const Source& src, const Sink& snk);
    std::streamsize read(char_type* s, std::streamsize n);
    void write(const char_type* s, std::streamsize n);
    void close(std::ios::openmode);
    #ifndef BOOST_NO_STD_LOCALE
        void imbue(const std::locale& loc);
    #endif
private:
    typedef BOOST_IOSTREAMS_CHAR_TYPE(Sink) sink_char_type;
    BOOST_STATIC_ASSERT((is_same<char_type, sink_char_type>::value));
    Source  src_;
    Sink    sink_;
};

//
// Template name: combined_filter.
// Description: Model of Device defined in terms of a Source/Sink pair.
// Template paramters:
//      InputFilter - A model of InputFilter, with the same char_type as 
//          OutputFilter.
//      OutputFilter - A model of OutputFilter, with the same char_type as 
//          InputFilter.
//
template<typename InputFilter, typename OutputFilter>
class combined_filter {
private:
    typedef BOOST_IOSTREAMS_CATEGORY(InputFilter)   in_category;
    typedef BOOST_IOSTREAMS_CATEGORY(OutputFilter)  out_category;
public:
    typedef BOOST_IOSTREAMS_CHAR_TYPE(InputFilter)  char_type;
    struct io_category 
        : multichar_bidirectional_filter_tag,
          closable_tag, 
          localizable_tag
        { };
    combined_filter(const InputFilter& in, const OutputFilter& out);

    template<typename Source>
    std::streamsize read(Source& src, char_type* s, std::streamsize n)
    { return boost::iostreams::read(in_, src, s, n); }

    template<typename Sink>
    void write(Sink& snk, const char_type* s, std::streamsize n)
    { boost::iostreams::write(out_, snk, s, n); }

    template<typename Sink>
    void close(Sink& snk, std::ios::openmode which)
        {
            if (which & std::ios::in)
                iostreams::close(in_, snk, which);
            if (which & std::ios::out)
                iostreams::close(out_, snk, which);
        }
    #ifndef BOOST_NO_STD_LOCALE
        void imbue(const std::locale& loc);
    #endif
private:
    typedef BOOST_IOSTREAMS_CHAR_TYPE(OutputFilter) output_char_type;
    BOOST_STATIC_ASSERT((is_same<char_type, output_char_type>::value));
    InputFilter   in_;
    OutputFilter  out_;
};

template<typename In, typename Out>
struct combined_traits 
    : mpl::if_<
          is_device<In>,
          combined_device<
              typename wrapped_type<In>::type,
              typename wrapped_type<Out>::type
          >,
          combined_filter<
              typename wrapped_type<In>::type,
              typename wrapped_type<Out>::type
          >
      >
    { };

} // End namespace detail.

template<typename In, typename Out>
struct combined_view : detail::combined_traits<In, Out>::type {
    typedef typename detail::combined_traits<In, Out>::type  base_type;
    typedef typename detail::wrapped_type<In>::type          in_type;
    typedef typename detail::wrapped_type<Out>::type         out_type;
    combined_view(const in_type& in, const out_type& out)
        : base_type(in, out) { }
};

//#if BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
//template<> struct combined_view<int, int> { };
//#endif

namespace detail {

// Workaround for VC6 ETI bug.
template<typename In, typename Out>
struct combine_traits {
    typedef combined_view<
                BOOST_DEDUCED_TYPENAME detail::unwrapped_type<In>::type, 
                BOOST_DEDUCED_TYPENAME detail::unwrapped_type<Out>::type
            > type;
};

} // End namespace detail.

//
// Template name: combine.
// Description: Takes a Source/Sink pair or InputFilter/OutputFilter pair and
//      returns a Reource or Filter which performs input using the first member
//      of the pair and output using the second member of the pair.
// Template paramters:
//      In - A model of Source or InputFilter, with the same char_type as Out.
//      Out - A model of Sink or OutputFilter, with the same char_type as In.
//
template<typename In, typename Out>
typename detail::combine_traits<In, Out>::type
combine(const In& in, const Out& out) 
{ 
    typedef typename detail::combine_traits<In, Out>::type return_type;
    return return_type(in, out); 
}

//----------------------------------------------------------------------------//

namespace detail {

//--------------Implementation of combined_device-----------------------------//

template<typename Source, typename Sink>
inline combined_device<Source, Sink>::combined_device
    (const Source& src, const Sink& snk)
    : src_(src), sink_(snk) { }

template<typename Source, typename Sink>
inline std::streamsize
combined_device<Source, Sink>::read(char_type* s, std::streamsize n)
{ return iostreams::read(src_, s, n); }

template<typename Source, typename Sink>
inline void
combined_device<Source, Sink>::write(const char_type* s, std::streamsize n)
{ iostreams::write(sink_, s, n); }

template<typename Source, typename Sink>
inline void
combined_device<Source, Sink>::close(std::ios::openmode which)
{ 
    if (which & std::ios::in)
        iostreams::close(src_, which); 
    if (which & std::ios::out)
        iostreams::close(sink_, which); 
}

#ifndef BOOST_NO_STD_LOCALE
    template<typename Source, typename Sink>
    void combined_device<Source, Sink>::imbue(const std::locale& loc)
    {
        iostreams::imbue(src_, loc);
        iostreams::imbue(sink_, loc);
    }
#endif

//--------------Implementation of filter_pair---------------------------------//

template<typename InputFilter, typename OutputFilter>
inline combined_filter<InputFilter, OutputFilter>::combined_filter
    (const InputFilter& in, const OutputFilter& out) : in_(in), out_(out)
    { }

#ifndef BOOST_NO_STD_LOCALE
    template<typename InputFilter, typename OutputFilter>
    void combined_filter<InputFilter, OutputFilter>::imbue
        (const std::locale& loc)
    {
        iostreams::imbue(in_, loc);
        iostreams::imbue(out_, loc);
    }
#endif


} // End namespace detail.

} } // End namespaces iostreams, boost.

#endif // #ifndef BOOST_IOSTREAMS_COMBINE_HPP_INCLUDED
