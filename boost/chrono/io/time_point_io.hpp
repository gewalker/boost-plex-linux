//  (C) Copyright Howard Hinnant
//  (C) Copyright 2010-2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
// This code was adapted by Vicente from Howard Hinnant's experimental work
// on chrono i/o to Boost

#ifndef BOOST_CHRONO_IO_TIME_POINT_IO_HPP
#define BOOST_CHRONO_IO_TIME_POINT_IO_HPP

#include <boost/chrono/io/time_point_put.hpp>
#include <boost/chrono/io/time_point_get.hpp>
#include <boost/chrono/io/duration_io.hpp>
#include <boost/chrono/io/ios_base_state.hpp>
#include <boost/chrono/io/utility/manip_base.hpp>
#include <boost/chrono/chrono.hpp>
#include <boost/chrono/clock_string.hpp>
#include <boost/chrono/round.hpp>
#include <boost/chrono/detail/scan_keyword.hpp>
#include <cstring>
#include <locale>
#include <string.h>

namespace boost
{
  namespace chrono
  {
    namespace detail
    {

      template <class CharT>
      class time_manip: public manip<time_manip<CharT> >
      {
        std::basic_string<CharT> fmt_;
        timezone_type tz_;
      public:

        time_manip(timezone_type tz, std::basic_string<CharT> fmt)
        // todo move semantics
        :
          fmt_(fmt), tz_(tz)
        {
        }

        /**
         * Change the timezone_type and time format ios state;
         */
        void operator()(std::ios_base &ios) const
        {
          set_time_fmt<CharT> (ios, fmt_);
          set_timezone(ios, tz_);
        }
      };

      class time_man: public manip<time_man>
      {
        timezone_type tz_;
      public:

        time_man(timezone_type tz)
        // todo move semantics
        :
          tz_(tz)
        {
        }

        /**
         * Change the timezone_type and time format ios state;
         */
        void operator()(std::ios_base &ios) const
        {
          //set_time_fmt<typename out_stream::char_type>(ios, "");
          set_timezone(ios, tz_);
        }
      };

    }

    template <class CharT>
    inline detail::time_manip<CharT> time_fmt(timezone_type tz, const CharT* fmt)
    {
      return detail::time_manip<CharT>(tz, fmt);
    }

    template <class CharT>
    inline detail::time_manip<CharT> time_fmt(timezone_type tz, std::basic_string<CharT> fmt)
    {
      // todo move semantics
      return detail::time_manip<CharT>(tz, fmt);
    }

    inline detail::time_man time_fmt(timezone_type f)
    {
      return detail::time_man(f);
    }

    /**
     * time_fmt_io_saver i/o saver.
     *
     * See Boost.IO i/o state savers for a motivating compression.
     */
    template <typename CharT = char, typename Traits = std::char_traits<CharT> >
    struct time_fmt_io_saver
    {

      //! the type of the state to restore
      typedef std::basic_ostream<CharT, Traits> state_type;
      //! the type of aspect to save
      typedef std::basic_string<CharT, Traits> aspect_type;

      /**
       * Explicit construction from an i/o stream.
       *
       * Store a reference to the i/o stream and the value of the associated @c time format .
       */
      explicit time_fmt_io_saver(state_type &s) :
        s_save_(s), a_save_(get_time_fmt(s_save_))
      {
      }

      /**
       * Construction from an i/o stream and a @c time format  to restore.
       *
       * Stores a reference to the i/o stream and the value @c new_value to restore given as parameter.
       */
      time_fmt_io_saver(state_type &s, aspect_type new_value) :
        s_save_(s), a_save_(new_value)
      {
      }

      /**
       * Destructor.
       *
       * Restores the i/o stream with the format to be restored.
       */
      ~time_fmt_io_saver()
      {
        this->restore();
      }

      /**
       * Restores the i/o stream with the time format to be restored.
       */
      void restore()
      {
        set_time_fmt(a_save_, a_save_);
      }
    private:
      state_type& s_save_;
      aspect_type a_save_;
    };

    /**
     * timezone_io_saver i/o saver.
     *
     * See Boost.IO i/o state savers for a motivating compression.
     */
    struct timezone_io_saver
    {

      //! the type of the state to restore
      typedef std::ios_base state_type;
      //! the type of aspect to save
      typedef timezone_type aspect_type;

      /**
       * Explicit construction from an i/o stream.
       *
       * Store a reference to the i/o stream and the value of the associated @c timezone.
       */
      explicit timezone_io_saver(state_type &s) :
        s_save_(s), a_save_(get_timezone(s_save_))
      {
      }

      /**
       * Construction from an i/o stream and a @c timezone to restore.
       *
       * Stores a reference to the i/o stream and the value @c new_value to restore given as parameter.
       */
      timezone_io_saver(state_type &s, aspect_type new_value) :
        s_save_(s), a_save_(new_value)
      {
      }

      /**
       * Destructor.
       *
       * Restores the i/o stream with the format to be restored.
       */
      ~timezone_io_saver()
      {
        this->restore();
      }

      /**
       * Restores the i/o stream with the timezone to be restored.
       */
      void restore()
      {
        set_timezone(s_save_, a_save_);
      }
    private:
      state_type& s_save_;
      aspect_type a_save_;
    };

    /**
     *
     * @param os
     * @param tp
     * @Effects Behaves as a formatted output function. After constructing a @c sentry object, if the @ sentry
     * converts to true, calls to @c facet.put(os,os,os.fill(),tp) where @c facet is the @c time_point_put<CharT>
     * facet associated to @c os or a new created instance of the default @c time_point_put<CharT> facet.
     * @return @c os.
     */
    template <class CharT, class Traits, class Clock, class Duration>
    std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits>& os, const time_point<Clock, Duration>& tp)
    {

      typedef std::basic_string<CharT, Traits> string_type;
      bool failed = false;
      try
      {
        std::ios_base::iostate err = std::ios_base::goodbit;
        try
        {
          typename std::basic_ostream<CharT, Traits>::sentry opfx(os);
          if (opfx)
          {
            if (!std::has_facet<time_point_put<CharT> >(os.getloc()))
            {
              if (time_point_put<CharT> ().put(os, os, os.fill(), tp) .failed())
              {
                err = std::ios_base::badbit;
              }
            }
            else
            {
              if (std::use_facet<time_point_put<CharT> >(os.getloc()) .put(os, os, os.fill(), tp).failed())
              {
                err = std::ios_base::badbit;
              }
            }
            os.width(0);
          }
        }
        catch (...)
        {
          bool flag = false;
          try
          {
            os.setstate(std::ios_base::failbit);
          }
          catch (std::ios_base::failure )
          {
            flag = true;
          }
          if (flag) throw;
        }
        if (err) os.setstate(err);
        return os;
      }
      catch (...)
      {
        failed = true;
      }
      if (failed) os.setstate(std::ios_base::failbit | std::ios_base::badbit);
      return os;
    }

    template <class CharT, class Traits, class Clock, class Duration>
    std::basic_istream<CharT, Traits>&
    operator>>(std::basic_istream<CharT, Traits>& is, time_point<Clock, Duration>& tp)
    {
      std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
      std::ios_base::iostate err = std::ios_base::goodbit;

      try
      {
        std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
        typename std::basic_istream<CharT, Traits>::sentry ipfx(is);
        if (ipfx)
        {
          std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
          if (!std::has_facet<time_point_get<CharT> >(is.getloc()))
          {
            std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
            time_point_get<CharT> ().get(is, std::istreambuf_iterator<CharT, Traits>(), is, err, tp);
          }
          else
          {
            std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
            std::use_facet<time_point_get<CharT> >(is.getloc()).get(is, std::istreambuf_iterator<CharT, Traits>(), is,
                err, tp);
          }
        }
      }
      catch (...)
      {
        std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
        bool flag = false;
        try
        {
          std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
          is.setstate(std::ios_base::failbit);
        }
        catch (std::ios_base::failure )
        {
          std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
          flag = true;
        }
        std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
        if (flag) throw;
        std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
      }
      if (err) is.setstate(err);
      std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
      return is;
    }

#ifndef BOOST_CHRONO_NO_UTC_TIMEPOINT

    namespace detail
    {
#if defined BOOST_WINDOWS && ! defined(__CYGWIN__)
    int is_leap(int year)
    {
      if(year % 400 == 0)
      return 1;
      if(year % 100 == 0)
      return 0;
      if(year % 4 == 0)
      return 1;
      return 0;
    }
    inline int days_from_0(int year)
    {
      year--;
      return 365 * year + (year / 400) - (year/100) + (year / 4);
    }
    int days_from_1970(int year)
    {
      static const int days_from_0_to_1970 = days_from_0(1970);
      return days_from_0(year) - days_from_0_to_1970;
    }
    int days_from_1jan(int year,int month,int day)
    {
      static const int days[2][12] =
      {
        { 0,31,59,90,120,151,181,212,243,273,304,334},
        { 0,31,60,91,121,152,182,213,244,274,305,335}
      };
      return days[is_leap(year)][month-1] + day - 1;
    }

    time_t internal_timegm(std::tm const *t)
    {
      int year = t->tm_year + 1900;
      int month = t->tm_mon;
      if(month > 11)
      {
        year += month/12;
        month %= 12;
      }
      else if(month < 0)
      {
        int years_diff = (-month + 11)/12;
        year -= years_diff;
        month+=12 * years_diff;
      }
      month++;
      int day = t->tm_mday;
      int day_of_year = days_from_1jan(year,month,day);
      int days_since_epoch = days_from_1970(year) + day_of_year;

      time_t seconds_in_day = 3600 * 24;
      time_t result = seconds_in_day * days_since_epoch + 3600 * t->tm_hour + 60 * t->tm_min + t->tm_sec;

      return result;
    }
#endif
    } // detail

    template <class CharT, class Traits, class Duration>
    std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits>& os, const time_point<system_clock, Duration>& tp)
    {
      std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
      typename std::basic_ostream<CharT, Traits>::sentry ok(os);
      if (ok)
      {
        std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
        bool failed = false;
        try
        {
          std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
          const CharT* pb = 0; //nullptr;
          const CharT* pe = pb;
          std::basic_string<CharT> fmt = get_time_fmt<CharT> (os);
          pb = fmt.data();
          pe = pb + fmt.size();

          timezone_type tz = get_timezone(os);
          std::locale loc = os.getloc();
          time_t t = system_clock::to_time_t(tp);
          std::tm tm;
          if (tz == timezone::local)
          {
      std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
#if defined BOOST_WINDOWS && ! defined(__CYGWIN__)
            std::tm *tmp = 0;
            if ((tmp=localtime(&t)) == 0)
            failed = true;
            tm =*tmp;
#else
            if (localtime_r(&t, &tm) == 0) failed = true;
#endif
          }
          else
          {
            std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
#if defined BOOST_WINDOWS && ! defined(__CYGWIN__)
            std::tm *tmp = 0;
            if((tmp = gmtime(&t)) == 0)
            failed = true;
            tm = *tmp;
#else
            if (gmtime_r(&t, &tm) == 0) failed = true;
#endif

          }
          std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
          if (!failed)
          {
            std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
            const std::time_put<CharT>& tpf = std::use_facet<std::time_put<CharT> >(loc);
            if (pb == pe)
            {
              std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
              CharT pattern[] =
              { '%', 'Y', '-', '%', 'm', '-', '%', 'd', ' ', '%', 'H', ':', '%', 'M', ':' };
              pb = pattern;
              pe = pb + sizeof (pattern) / sizeof(CharT);
              failed = tpf.put(os, os, os.fill(), &tm, pb, pe).failed();
              if (!failed)
              {
                std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
                duration<double> d = tp - system_clock::from_time_t(t) + seconds(tm.tm_sec);
                std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
                if (d.count() < 10) os << CharT('0');
                if (! os.good()) {
                  std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
                  throw "exception";
                }
                std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
                std::ios::fmtflags flgs = os.flags();
                std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
                os.setf(std::ios::fixed, std::ios::floatfield);
                if (! os.good()) {
                  std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
                  throw "exception";
                }
                std::cerr << __FILE__ << "[" << __LINE__ << "]"<< " " << d.count()  << std::endl;
                os << d.count();
                if (! os.good()) {
                  std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
                  throw "exception";
                }
                std::cerr << __FILE__ << "[" << __LINE__ << "]"<< " " << d.count() << std::endl;
                os.flags(flgs);
                std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
                if (tz == timezone::local)
                {
                  std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
                  CharT sub_pattern[] =
                  { ' ', '%', 'z' };
                  pb = sub_pattern;
                  pe = pb + +sizeof (sub_pattern) / sizeof(CharT);
                  failed = tpf.put(os, os, os.fill(), &tm, pb, pe).failed();
                }
                else
                {
                  std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
                  CharT sub_pattern[] =
                  { ' ', '+', '0', '0', '0', '0', 0 };
                  os << sub_pattern;
                }
                std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
              }
              std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
            }
            else
            {
              std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
              failed = tpf.put(os, os, os.fill(), &tm, pb, pe).failed();
            }
          }
        }
        catch (...)
        {
          std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
          failed = true;
        }
        if (failed)
        {
          std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
          os.setstate(std::ios_base::failbit | std::ios_base::badbit);
        }
      }
      std::cerr << __FILE__ << "[" << __LINE__ << "]"<< std::endl;
      return os;
    }

    namespace detail
    {

      template <class CharT, class InputIterator>
      minutes extract_z(InputIterator& b, InputIterator e, std::ios_base::iostate& err, const std::ctype<CharT>& ct)
      {
        int min = 0;
        if (b != e)
        {
          char cn = ct.narrow(*b, 0);
          if (cn != '+' && cn != '-')
          {
            err |= std::ios_base::failbit;
            return minutes(0);
          }
          int sn = cn == '-' ? -1 : 1;
          int hr = 0;
          for (int i = 0; i < 2; ++i)
          {
            if (++b == e)
            {
              err |= std::ios_base::eofbit | std::ios_base::failbit;
              return minutes(0);
            }
            cn = ct.narrow(*b, 0);
            if (! ('0' <= cn && cn <= '9'))
            {
              err |= std::ios_base::failbit;
              return minutes(0);
            }
            hr = hr * 10 + cn - '0';
          }
          for (int i = 0; i < 2; ++i)
          {
            if (++b == e)
            {
              err |= std::ios_base::eofbit | std::ios_base::failbit;
              return minutes(0);
            }
            cn = ct.narrow(*b, 0);
            if (! ('0' <= cn && cn <= '9'))
            {
              err |= std::ios_base::failbit;
              return minutes(0);
            }
            min = min * 10 + cn - '0';
          }
          if (++b == e) err |= std::ios_base::eofbit;
          min += hr * 60;
          min *= sn;
        }
        else
          err |= std::ios_base::eofbit | std::ios_base::failbit;
        return minutes(min);
      }

    } // detail

    template <class CharT, class Traits, class Duration>
    std::basic_istream<CharT, Traits>&
    operator>>(std::basic_istream<CharT, Traits>& is, time_point<system_clock, Duration>& tp)
    {
      typename std::basic_istream<CharT, Traits>::sentry ok(is);
      if (ok)
      {
        std::ios_base::iostate err = std::ios_base::goodbit;
        try
        {
          const CharT* pb = 0; //nullptr;
          const CharT* pe = pb;
          std::basic_string<CharT> fmt = get_time_fmt<CharT> (is);
          pb = fmt.data();
          pe = pb + fmt.size();

          timezone_type tz = get_timezone(is);
          std::locale loc = is.getloc();
          const std::time_get<CharT>& tg = std::use_facet<std::time_get<CharT> >(loc);
          const std::ctype<CharT>& ct = std::use_facet<std::ctype<CharT> >(loc);
          tm tm; // {0}
          typedef std::istreambuf_iterator<CharT, Traits> It;
          if (pb == pe)
          {
            CharT pattern[] =
            { '%', 'Y', '-', '%', 'm', '-', '%', 'd', ' ', '%', 'H', ':', '%', 'M', ':' };
            pb = pattern;
            pe = pb + sizeof (pattern) / sizeof(CharT);
            tg.get(is, 0, is, err, &tm, pb, pe);
            if (err & std::ios_base::failbit) goto exit;
            double sec;
            CharT c = CharT();
            is >> sec;
            if (is.fail())
            {
              err |= std::ios_base::failbit;
              goto exit;
            }
            It i(is);
            It eof;
            c = *i;
            if (++i == eof || c != ' ')
            {
              err |= std::ios_base::failbit;
              goto exit;
            }
            minutes min = detail::extract_z(i, eof, err, ct);
            if (err & std::ios_base::failbit) goto exit;
            time_t t;
#if defined BOOST_WINDOWS && ! defined(__CYGWIN__)
            t = detail::internal_timegm(&tm);
#else
            t = timegm(&tm);
#endif
            tp = system_clock::from_time_t(t) - min + round<microseconds> (duration<double> (sec));
          }
          else
          {
            const CharT z[2] =
            { '%', 'z' };
            const CharT* fz = std::search(pb, pe, z, z + 2);
            tg.get(is, 0, is, err, &tm, pb, fz);
            minutes minu(0);
            if (fz != pe)
            {
              if (err != std::ios_base::goodbit)
              {
                err |= std::ios_base::failbit;
                goto exit;
              }
              It i(is);
              It eof;
              minu = extract_z(i, eof, err, ct);
              if (err & std::ios_base::failbit) goto exit;
              if (fz + 2 != pe)
              {
                if (err != std::ios_base::goodbit)
                {
                  err |= std::ios_base::failbit;
                  goto exit;
                }
                tg.get(is, 0, is, err, &tm, fz + 2, pe);
                if (err & std::ios_base::failbit) goto exit;
              }
            }
            tm.tm_isdst = -1;
            time_t t;
            if (tz == timezone::utc || fz != pe)
#if defined BOOST_WINDOWS && ! defined(__CYGWIN__)
              t = detail::internal_timegm(&tm);
#else
              t = timegm(&tm);
#endif
            else
              t = mktime(&tm);
            tp = system_clock::from_time_t(t) - minu;
          }
        }
        catch (...)
        {
          err |= std::ios_base::badbit | std::ios_base::failbit;
        }
        exit: is.setstate(err);
      }
      return is;
    }
#endif //UTC
  } // chrono

}

#endif  // header
