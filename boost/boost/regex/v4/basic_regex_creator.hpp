/*
 *
 * Copyright (c) 2004
 * Dr John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         basic_regex_creator.cpp
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: Declares template class basic_regex_creator which fills in
  *                the data members of a regex_data object.
  */

#ifndef BOOST_REGEX_V4_BASIC_REGEX_CREATOR_HPP
#define BOOST_REGEX_V4_BASIC_REGEX_CREATOR_HPP

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost{

namespace re_detail{

template <class charT>
struct digraph : public std::pair<charT, charT>
{
   digraph(charT c1 = 0, charT c2 = 0) : std::pair<charT, charT>(c1, c2){}
   digraph(const std::basic_string<charT>& s) : std::pair<charT, charT>()
   {
      BOOST_ASSERT(s.size() <= 2);
      BOOST_ASSERT(s.size());
      this->first = s[0];
      this->second = (s.size() > 1) ? s[1] : 0;
   }
};

template <class charT, class traits>
class basic_char_set
{
public:
   typedef digraph<charT>             digraph_type;
   typedef std::basic_string<charT>   string_type;
   typedef typename traits::char_class_type mask_type;

   basic_char_set()
   {
      m_negate = false;
      m_has_digraphs = false;
      m_classes = 0;
      m_empty = true;
   }

   void add_single(const digraph_type& s)
   {
      m_singles.push_back(s);
      if(s.second)
         m_has_digraphs = true;
      m_empty = false;
   }
   void add_range(const digraph_type& first, const digraph_type& end)
   {
      m_ranges.push_back(first);
      m_ranges.push_back(end);
      if(first.second || end.second)
         m_has_digraphs = true;
      m_empty = false;
   }
   void add_class(mask_type m)
   {
      m_classes |= m;
      m_empty = false;
   }
   void negate()
   { 
      m_negate = true;
      m_empty = false;
   }

   //
   // accessor functions:
   //
   bool has_digraphs()const
   {
      return m_has_digraphs;
   }
   bool is_negated()const
   {
      return m_negate;
   }
   typedef typename std::vector<digraph_type>::const_iterator  list_iterator;
   list_iterator singles_begin()const
   {
      return m_singles.begin();
   }
   list_iterator singles_end()const
   {
      return m_singles.end();
   }
   list_iterator ranges_begin()const
   {
      return m_ranges.begin();
   }
   list_iterator ranges_end()const
   {
      return m_ranges.end();
   }
   mask_type classes()const
   {
      return m_classes;
   }
   bool empty()const
   {
      return m_empty;
   }
private:
   std::vector<digraph_type> m_singles;        // a list of single characters to match
   std::vector<digraph_type> m_ranges;         // a list of end points of our ranges
   bool                      m_negate;         // true if the set is to be negated
   bool                      m_has_digraphs;   // true if we have digraphs present
   mask_type                 m_classes;        // character classes to match
   bool                      m_empty;          // whether we've added anything yet
};
   
template <class charT, class traits>
class basic_regex_creator
{
public:
   basic_regex_creator(regex_data<charT, traits>* data);
   std::ptrdiff_t getoffset(void* addr)
   {
      return getoffset(addr, m_pdata->m_data.data());
   }
   std::ptrdiff_t getoffset(void* addr, void* base)
   {
      return static_cast<const char*>(addr) - static_cast<const char*>(base);
   }
   re_syntax_base* getaddress(std::ptrdiff_t off)
   {
      return getaddress(off, m_pdata->m_data.data());
   }
   re_syntax_base* getaddress(std::ptrdiff_t off, void* base)
   {
      return static_cast<re_syntax_base*>(static_cast<void*>(static_cast<char*>(base) + off));
   }
   void init(unsigned flags)
   {
      m_pdata->m_flags = flags;
      m_icase = flags & regex_constants::icase;
   }
   regbase::flag_type flags()
   {
      return m_pdata->m_flags;
   }
   re_syntax_base* append_state(syntax_element_type t, std::size_t s = sizeof(re_syntax_base));
   re_syntax_base* insert_state(std::ptrdiff_t pos, syntax_element_type t, std::size_t s = sizeof(re_syntax_base));
   re_literal* append_literal(charT c);
   re_syntax_base* append_set(const basic_char_set<charT, traits>& char_set);
   re_syntax_base* append_set(const basic_char_set<charT, traits>& char_set, mpl::false_*);
   re_syntax_base* append_set(const basic_char_set<charT, traits>& char_set, mpl::true_*);
   void finalize(const charT* p1, const charT* p2);
protected:
   regex_data<charT, traits>*    m_pdata;              // pointer to the basic_regex_data struct we are filling in
   const traits&                 m_traits;             // convenience reference to traits class
   re_syntax_base*               m_last_state;         // the last state we added
   bool                          m_icase;              // true for case insensitive matches
   unsigned                      m_repeater_id;        // the id of the next repeater
   unsigned                      m_backrefs;           // bitmask of permitted backrefs
   boost::uintmax_t              m_bad_repeats;        // bitmask of repeats we can't deduce a startmap for;
   typename traits::char_class_type m_word_mask;       // mask used to determine if a character is a word character
   typename traits::char_class_type m_mask_space;      // mask used to determine if a character is a word character
   typename traits::char_class_type m_lower_mask;       // mask used to determine if a character is a lowercase character
   typename traits::char_class_type m_upper_mask;      // mask used to determine if a character is an uppercase character
   typename traits::char_class_type m_alpha_mask;      // mask used to determine if a character is an alphabetic character
private:
   basic_regex_creator& operator=(const basic_regex_creator&);
   basic_regex_creator(const basic_regex_creator&);

   void fixup_pointers(re_syntax_base* state);
   void create_startmaps(re_syntax_base* state);
   void create_startmap(re_syntax_base* state, unsigned char* map, unsigned int* pnull, unsigned char mask);
   unsigned get_restart_type(re_syntax_base* state);
   void set_all_masks(unsigned char* bits, unsigned char);
   bool is_bad_repeat(re_syntax_base* pt);
   void set_bad_repeat(re_syntax_base* pt);
};

template <class charT, class traits>
basic_regex_creator<charT, traits>::basic_regex_creator(regex_data<charT, traits>* data)
   : m_pdata(data), m_traits(data->m_traits), m_last_state(0), m_repeater_id(0), m_backrefs(0)
{
   m_pdata->m_data.clear();
   static const charT w = 'w';
   static const charT s = 's';
   static const charT l[] = { 'l', 'o', 'w', 'e', 'r', };
   static const charT u[] = { 'u', 'p', 'p', 'e', 'r', };
   static const charT a[] = { 'a', 'l', 'p', 'h', 'a', };
   m_word_mask = m_traits.lookup_classname(&w, &w +1);
   m_mask_space = m_traits.lookup_classname(&s, &s +1);
   m_lower_mask = m_traits.lookup_classname(l, l + 5);
   m_upper_mask = m_traits.lookup_classname(u, u + 5);
   m_alpha_mask = m_traits.lookup_classname(a, a + 5);
   BOOST_ASSERT(m_word_mask); 
   BOOST_ASSERT(m_mask_space); 
   BOOST_ASSERT(m_lower_mask); 
   BOOST_ASSERT(m_upper_mask); 
   BOOST_ASSERT(m_alpha_mask); 
}

template <class charT, class traits>
re_syntax_base* basic_regex_creator<charT, traits>::append_state(syntax_element_type t, std::size_t s)
{
   // append a new state, start by aligning our last one:
   m_pdata->m_data.align();
   // set the offset to the next state in our last one:
   if(m_last_state)
      m_last_state->next.i = m_pdata->m_data.size() - getoffset(m_last_state);
   // now actually extent our data:
   m_last_state = static_cast<re_syntax_base*>(m_pdata->m_data.extend(s));
   // fill in boilerplate options in the new state:
   m_last_state->next.i = 0;
   m_last_state->type = t;
   return m_last_state;
}

template <class charT, class traits>
re_syntax_base* basic_regex_creator<charT, traits>::insert_state(std::ptrdiff_t pos, syntax_element_type t, std::size_t s)
{
   // append a new state, start by aligning our last one:
   m_pdata->m_data.align();
   // set the offset to the next state in our last one:
   if(m_last_state)
      m_last_state->next.i = m_pdata->m_data.size() - getoffset(m_last_state);
   // remember the last state position:
   std::ptrdiff_t off = getoffset(m_last_state) + s;
   // now actually insert our data:
   re_syntax_base* new_state = static_cast<re_syntax_base*>(m_pdata->m_data.insert(pos, s));
   // fill in boilerplate options in the new state:
   new_state->next.i = s;
   new_state->type = t;
   m_last_state = getaddress(off);
   return new_state;
}

template <class charT, class traits>
re_literal* basic_regex_creator<charT, traits>::append_literal(charT c)
{
   re_literal* result;
   // start by seeing if we have an existing re_literal we can extend:
   if((0 == m_last_state) || (m_last_state->type != syntax_element_literal))
   {
      // no existing re_literal, create a new one:
      result = static_cast<re_literal*>(append_state(syntax_element_literal, sizeof(re_literal) + sizeof(charT)));
      result->length = 1;
      *static_cast<charT*>(static_cast<void*>(result+1)) = m_traits.translate(c, m_icase);
   }
   else
   {
      // we have an existing re_literal, extend it:
      std::ptrdiff_t off = getoffset(m_last_state);
      m_pdata->m_data.extend(sizeof(charT));
      m_last_state = result = static_cast<re_literal*>(getaddress(off));
      charT* characters = static_cast<charT*>(static_cast<void*>(result+1));
      characters[result->length] = m_traits.translate(c, m_icase);
      ++(result->length);
   }
   return result;
}

template <class charT, class traits>
inline re_syntax_base* basic_regex_creator<charT, traits>::append_set(
   const basic_char_set<charT, traits>& char_set)
{
   typedef mpl::bool_<sizeof(charT) == 1> truth_type;
   return char_set.has_digraphs() 
      ? append_set(char_set, static_cast<mpl::false_*>(0))
      : append_set(char_set, static_cast<truth_type*>(0));
}

template <class charT, class traits>
re_syntax_base* basic_regex_creator<charT, traits>::append_set(
   const basic_char_set<charT, traits>& char_set, mpl::false_*)
{
   typedef std::basic_string<charT> string_type;
   typedef typename basic_char_set<charT, traits>::list_iterator item_iterator;
   typedef typename traits::char_class_type mask_type;
   
   re_set_long<mask_type>* result = static_cast<re_set_long<mask_type>*>(append_state(syntax_element_long_set, sizeof(re_set_long<mask_type>)));
   //
   // fill in the basics:
   //
   result->csingles = static_cast<unsigned int>(std::distance(char_set.singles_begin(), char_set.singles_end()));
   result->cranges = static_cast<unsigned int>(std::distance(char_set.ranges_begin(), char_set.ranges_end())) / 2;
   result->cequivalents = 0;
   result->cclasses = char_set.classes();
   if(flags() & regbase::icase)
   {
      // adjust classes as needed:
      if(((result->cclasses & m_lower_mask) == m_lower_mask) || ((result->cclasses & m_upper_mask) == m_upper_mask))
         result->cclasses |= m_alpha_mask;
   }

   result->isnot = char_set.is_negated();
   result->singleton = !char_set.has_digraphs();
   //
   // remember where the state is for later:
   //
   std::ptrdiff_t offset = getoffset(result);
   //
   // now extend with all the singles:
   //
   item_iterator first, last;
   first = char_set.singles_begin();
   last = char_set.singles_end();
   while(first != last)
   {
      charT* p = static_cast<charT*>(this->m_pdata->m_data.extend(sizeof(charT) * (first->second ? 3 : 2)));
      p[0] = m_traits.translate(first->first, m_icase);
      if(first->second)
      {
         p[1] = m_traits.translate(first->second, m_icase);
         p[2] = 0;
      }
      else
         p[1] = 0;
      ++first;
   }
   //
   // now extend with all the ranges:
   //
   first = char_set.ranges_begin();
   last = char_set.ranges_end();
   while(first != last)
   {
      // first grab the endpoints of the range:
      digraph<charT> c1 = *first;
      c1.first = this->m_traits.translate(c1.first, this->m_icase);
      c1.second = this->m_traits.translate(c1.second, this->m_icase);
      ++first;
      digraph<charT> c2 = *first;
      c2.first = this->m_traits.translate(c2.first, this->m_icase);
      c2.second = this->m_traits.translate(c2.second, this->m_icase);
      ++first;
      string_type s1, s2;
      // different actions now depending upon whether collation is turned on:
      if(flags() & regex_constants::collate)
      {
         // we need to transform our range into sort keys:
         s1 = this->m_traits.transform(&c1.first, (c1.second ? &c1.second +1 : &c1.second));
         s2 = this->m_traits.transform(&c2.first, (c2.second ? &c2.second +1 : &c2.second));
      }
      else
      {
         if(c1.second)
            s1 = string_type(&c1.first, &c1.second+1);
         else
            s1 = string_type(1, c1.first);
         if(c2.second)
            s2 = string_type(&c2.first, &c2.second+1);
         else
            s2 = string_type(1, c2.first);
      }
      if(s1 > s2)
      {
         // Oops error:
         return 0;
      }
      charT* p = static_cast<charT*>(this->m_pdata->m_data.extend(sizeof(charT) * (s1.size() + s2.size() + 2) ) );
      std::memcpy(p, s1.c_str(), sizeof(charT) * (s1.size() + 1));
      p += s1.size() + 1;
      std::memcpy(p, s2.c_str(), sizeof(charT) * (s2.size() + 1));
   }
   //
   // finally reset the address of our last state:
   //
   m_last_state = result = static_cast<re_set_long<mask_type>*>(getaddress(offset));
   return result;
}

template <class charT, class traits>
re_syntax_base* basic_regex_creator<charT, traits>::append_set(
   const basic_char_set<charT, traits>& char_set, mpl::true_*)
{
   typedef std::basic_string<charT> string_type;
   typedef typename basic_char_set<charT, traits>::list_iterator item_iterator;
   
   re_set* result = static_cast<re_set*>(append_state(syntax_element_set, sizeof(re_set)));
   bool negate = char_set.is_negated();
   std::memset(result->_map, 0, sizeof(result->_map));
   //
   // handle singles first:
   //
   item_iterator first, last;
   first = char_set.singles_begin();
   last = char_set.singles_end();
   while(first != last)
   {
      for(unsigned int i = 0; i < (1 << CHAR_BIT); ++i)
      {
         if(this->m_traits.translate(static_cast<charT>(i), this->m_icase)
            == this->m_traits.translate(first->first, this->m_icase))
            result->_map[i] = true;
      }
      ++first;
   }
   //
   // OK now handle ranges:
   //
   first = char_set.ranges_begin();
   last = char_set.ranges_end();
   while(first != last)
   {
      // first grab the endpoints of the range:
      charT c1 = this->m_traits.translate(first->first, this->m_icase);
      ++first;
      charT c2 = this->m_traits.translate(first->first, this->m_icase);
      ++first;
      // different actions now depending upon whether collation is turned on:
      if(flags() & regex_constants::collate)
      {
         // we need to transform our range into sort keys:
         string_type s1 = this->m_traits.transform(&c1, &c1 +1);
         string_type s2 = this->m_traits.transform(&c2, &c2 +1);
         if(s1 > s2)
         {
            // Oops error:
            return 0;
         }
         for(unsigned i = 0; i < (1u << CHAR_BIT); ++i)
         {
            charT c3 = static_cast<charT>(i);
            string_type s3 = this->m_traits.transform(&c3, &c3 +1);
            if((s1 <= s3) && (s3 <= s2))
               result->_map[i] = true;
         }
      }
      else
      {
         if(c1 > c2)
         {
            // Oops error:
            return 0;
         }
         // everything in range matches:
         std::memset(result->_map + static_cast<unsigned char>(c1), true, 1 + static_cast<unsigned char>(c2) - static_cast<unsigned char>(c1));
      }
   }
   //
   // and now the classes:
   //
   typedef typename traits::char_class_type mask_type;
   mask_type m = char_set.classes();
   if(flags() & regbase::icase)
   {
      // adjust m as needed:
      if(((m & m_lower_mask) == m_lower_mask) || ((m & m_upper_mask) == m_upper_mask))
         m |= m_alpha_mask;
   }
   if(m != 0)
   {
      for(unsigned i = 0; i < (1u << CHAR_BIT); ++i)
      {
         if(this->m_traits.is_class(static_cast<charT>(i), m))
            result->_map[i] = true;
      }
   }
   if(negate)
   {
      for(unsigned i = 0; i < (1u << CHAR_BIT); ++i)
      {
         result->_map[i] = !(result->_map[i]);
      }
   }
   return result;
}

template <class charT, class traits>
void basic_regex_creator<charT, traits>::finalize(const charT* p1, const charT* p2)
{
   // we've added all the states we need, now finish things off.
   // start by adding a terminating state:
   append_state(syntax_element_match);
   // extend storage to store original expression:
   std::ptrdiff_t len = p2 - p1;
   m_pdata->m_expression_len = len;
   charT* ps = static_cast<charT*>(m_pdata->m_data.extend(sizeof(charT) * (1 + (p2 - p1))));
   m_pdata->m_expression = ps;
   std::copy(p1, p2, ps);
   ps[p2 - p1] = 0;
   // fill in our other data...
   // successful parsing implies a zero status:
   m_pdata->m_status = 0;
   // get the first state of the machine:
   m_pdata->m_first_state = static_cast<re_syntax_base*>(m_pdata->m_data.data());
   // fixup pointers in the machine:
   fixup_pointers(m_pdata->m_first_state);
   // create nested startmaps:
   create_startmaps(m_pdata->m_first_state);
   // create main startmap:
   std::memset(m_pdata->m_startmap, 0, sizeof(m_pdata->m_startmap));
   m_pdata->m_can_be_null = 0;

   m_bad_repeats = 0;
   create_startmap(m_pdata->m_first_state, m_pdata->m_startmap, &(m_pdata->m_can_be_null), mask_all);
   // get the restart type:
   m_pdata->m_restart_type = get_restart_type(m_pdata->m_first_state);
}

template <class charT, class traits>
void basic_regex_creator<charT, traits>::fixup_pointers(re_syntax_base* state)
{
   while(state)
   {
      switch(state->type)
      {
      case syntax_element_rep:
      case syntax_element_dot_rep:
      case syntax_element_char_rep:
      case syntax_element_short_set_rep:
      case syntax_element_long_set_rep:
         // set the id of this repeat:
         static_cast<re_repeat*>(state)->id = m_repeater_id++;
         // fall through:
      case syntax_element_alt:
         std::memset(static_cast<re_alt*>(state)->_map, 0, sizeof(static_cast<re_alt*>(state)->_map));
         static_cast<re_alt*>(state)->can_be_null = 0;
         // fall through:
      case syntax_element_jump:
         static_cast<re_jump*>(state)->alt.p = getaddress(static_cast<re_jump*>(state)->alt.i, state);
         // fall through again:
      default:
         if(state->next.i)
            state->next.p = getaddress(state->next.i, state);
         else
            state->next.p = 0;
      }
      state = state->next.p;
   }
}

template <class charT, class traits>
void basic_regex_creator<charT, traits>::create_startmaps(re_syntax_base* state)
{
   // recursive implementation:
   // create the last map in the machine first, so that earlier maps
   // can make use of the result...
   while(state)
   {
      switch(state->type)
      {
      case syntax_element_alt:
      case syntax_element_rep:
      case syntax_element_dot_rep:
      case syntax_element_char_rep:
      case syntax_element_short_set_rep:
      case syntax_element_long_set_rep:
         // create other startmaps *first*, since we can use the
         // results from these when creating out own:
         create_startmaps(state->next.p);
         m_bad_repeats = 0;
         create_startmap(state->next.p, static_cast<re_alt*>(state)->_map, &static_cast<re_alt*>(state)->can_be_null, mask_take);
         m_bad_repeats = 0;
         create_startmap(static_cast<re_alt*>(state)->alt.p, static_cast<re_alt*>(state)->_map, &static_cast<re_alt*>(state)->can_be_null, mask_skip);
         return;
      default:
         state = state->next.p;
      }
   }
}

template <class charT, class traits>
void basic_regex_creator<charT, traits>::create_startmap(re_syntax_base* state, unsigned char* map, unsigned int* pnull, unsigned char mask)
{
   int not_last_jump = 1;
   while(state)
   {
      switch(state->type)
      {
      case syntax_element_literal:
      {
         // don't set anything in *pnull, set each element in map
         // that could match the first character in the literal:
         if(map)
         {
            map[0] |= mask_init;
            charT first_char = *static_cast<charT*>(static_cast<void*>(static_cast<re_literal*>(state) + 1));
            for(unsigned int i = 0; i < (1u << CHAR_BIT); ++i)
            {
               if(m_traits.translate(static_cast<charT>(i), m_icase) == first_char)
                  map[i] |= mask;
            }
         }
         return;
      }
      case syntax_element_end_line:
      {
         // next character must be a line separator (if there is one):
         if(map)
         {
            map[0] |= mask_init;
            map['\n'] |= mask;
            map['\r'] |= mask;
         }
         // now figure out if we can match a NULL string at this point:
         if(pnull)
            create_startmap(state->next.p, 0, pnull, mask);
         return;
      }
      case syntax_element_backref:
      case syntax_element_wild:
      {
         // can't be null, any character can match:
         set_all_masks(map, mask);
         return;
      }
      case syntax_element_match:
      {
         // must be null, any character can match:
         set_all_masks(map, mask);
         if(pnull)
            *pnull |= mask;
         return;
      }
      case syntax_element_word_start:
      {
         // recurse, then AND with all the word characters:
         create_startmap(state->next.p, map, pnull, mask);
         if(map)
         {
            map[0] |= mask_init;
            for(unsigned int i = 0; i < (1u << CHAR_BIT); ++i)
            {
               if(!m_traits.is_class(static_cast<charT>(i), m_word_mask))
                  map[i] &= static_cast<unsigned char>(~mask);
            }
         }
         return;
      }
      case syntax_element_word_end:
      {
         // recurse, then AND with all the word characters:
         create_startmap(state->next.p, map, pnull, mask);
         if(map)
         {
            map[0] |= mask_init;
            for(unsigned int i = 0; i < (1u << CHAR_BIT); ++i)
            {
               if(m_traits.is_class(static_cast<charT>(i), m_word_mask))
                  map[i] &= static_cast<unsigned char>(~mask);
            }
         }
         return;
      }
      case syntax_element_buffer_end:
      {
         // we *must be null* :
         if(pnull)
            *pnull |= mask;
         return;
      }
      case syntax_element_long_set:
         if(map)
         {
            typedef typename traits::char_class_type mask_type;
            map[0] |= mask_init;
            for(unsigned int i = 0; i < (1u << CHAR_BIT); ++i)
            {
               charT c = static_cast<charT>(i);
               if(&c != re_is_set_member(&c, &c + 1, static_cast<re_set_long<mask_type>*>(state), *m_pdata))
                  map[i] |= mask;
            }
         }
         return;
      case syntax_element_set:
         if(map)
         {
            map[0] |= mask_init;
            for(unsigned int i = 0; i < (1u << CHAR_BIT); ++i)
            {
               if(static_cast<re_set*>(state)->_map[
                  static_cast<unsigned char>(m_traits.translate(static_cast<charT>(i), this->m_icase))])
                  map[i] |= mask;
            }
         }
         return;
      case syntax_element_jump:
         // take the jump:
         state = static_cast<re_alt*>(state)->alt.p;
         not_last_jump = -1;
         break;
      case syntax_element_alt:
      case syntax_element_rep:
      case syntax_element_dot_rep:
      case syntax_element_char_rep:
      case syntax_element_short_set_rep:
      case syntax_element_long_set_rep:
         {
            re_alt* rep = static_cast<re_alt*>(state);
            if(rep->_map[0] & mask_init)
            {
               if(map)
               {
                  // copy previous results:
                  map[0] |= mask_init;
                  for(unsigned int i = 0; i <= UCHAR_MAX; ++i)
                  {
                     if(rep->_map[i] & mask_any)
                        map[i] |= mask;
                  }
               }
               if(pnull)
               {
                  if(rep->can_be_null & mask_any)
                     *pnull |= mask;
               }
            }
            else
            {
               // we haven't created a startmap for this alternative yet
               // so take the union of the two options:
               if(is_bad_repeat(state))
               {
                  set_all_masks(map, mask);
                  return;
               }
               set_bad_repeat(state);
               create_startmap(state->next.p, map, pnull, mask);
               if((state->type == syntax_element_alt) 
                  || (static_cast<re_repeat*>(state)->min == 0)
                  || (not_last_jump == 0))
                  create_startmap(rep->alt.p, map, pnull, mask);
            }
         }
         return;
      case syntax_element_soft_buffer_end:
         // match newline or null:
         if(map)
         {
            map[0] |= mask_init;
            map['\n'] |= mask;
            map['\r'] |= mask;
         }
         if(pnull)
            *pnull |= mask;
         return;
      default:
         state = state->next.p;
      }
      ++not_last_jump;
   }
}

template <class charT, class traits>
unsigned basic_regex_creator<charT, traits>::get_restart_type(re_syntax_base* state)
{
   //
   // find out how the machine starts, so we can optimise the search:
   //
   while(state)
   {
      switch(state->type)
      {
      case syntax_element_startmark:
      case syntax_element_endmark:
         state = state->next.p;
         continue;
      case syntax_element_start_line:
         return regbase::restart_line;
      case syntax_element_word_boundary:
      case syntax_element_word_start:
         return regbase::restart_word;
      case syntax_element_buffer_start:
      case syntax_element_restart_continue:
         return regbase::restart_continue;
      default:
         state = 0;
         continue;
      }
   }
   return regbase::restart_any;
}

template <class charT, class traits>
void basic_regex_creator<charT, traits>::set_all_masks(unsigned char* bits, unsigned char mask)
{
   //
   // set mask in all of bits elements, 
   // if bits[0] has mask_init not set then we can 
   // optimise this to a call to memset:
   //
   if(bits)
   {
      if(bits[0] == 0)
         (std::memset)(bits, mask, 1u << CHAR_BIT);
      else
      {
         for(unsigned i = 0; i < (1u << CHAR_BIT); ++i)
            bits[i] |= mask;
      }
      bits[0] |= mask_init;
   }
}

template <class charT, class traits>
bool basic_regex_creator<charT, traits>::is_bad_repeat(re_syntax_base* pt)
{
   switch(pt->type)
   {
   case syntax_element_rep:
   case syntax_element_dot_rep:
   case syntax_element_char_rep:
   case syntax_element_short_set_rep:
   case syntax_element_long_set_rep:
      {
         unsigned id = static_cast<re_repeat*>(pt)->id;
         if(id > sizeof(m_bad_repeats) * CHAR_BIT)
            return true;  // run out of bits, assume we can't traverse this one.
         return m_bad_repeats & (1u << id);
      }
   default:
      return false;
   }
}

template <class charT, class traits>
void basic_regex_creator<charT, traits>::set_bad_repeat(re_syntax_base* pt)
{
   switch(pt->type)
   {
   case syntax_element_rep:
   case syntax_element_dot_rep:
   case syntax_element_char_rep:
   case syntax_element_short_set_rep:
   case syntax_element_long_set_rep:
      {
         unsigned id = static_cast<re_repeat*>(pt)->id;
         if(id <= sizeof(m_bad_repeats) * CHAR_BIT)
            m_bad_repeats |= (1u << id);
      }
   default:
      break;
   }
}

} // namespace re_detail

} // namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif
