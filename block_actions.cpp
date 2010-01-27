/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "./block.hpp"
#include "./detail/actions_class.hpp"
#include "./detail/markups.hpp"
#include "./detail/quickbook.hpp"
#include <boost/assert.hpp>

namespace quickbook
{
    namespace {
        std::string fully_qualified_id(std::string const& library_id,
            std::string const& qualified_section_id,
            std::string const& section_id)
        {
            std::string id = library_id;
            if(!id.empty() && !qualified_section_id.empty()) id += '.';
            id += qualified_section_id;
            if(!id.empty() && !section_id.empty()) id += '.';
            id += section_id;
            return id;
        }
    }

    void process(quickbook::actions& actions, hr)
    {
        actions.phrase << hr_;
    }

    void process(quickbook::actions& actions, paragraph const& x)
    {
        actions.phrase << paragraph_pre << x.content << paragraph_post;
    }

    void process(quickbook::actions& actions, begin_section const& x)
    {
        // TODO: This uses the generated title.
        actions.section_id = x.id ? *x.id :
            detail::make_identifier(
                x.content.raw_markup.begin(),
                x.content.raw_markup.end());

        if (actions.section_level != 0) {
            actions.qualified_section_id += '.';
        }
        else {
            BOOST_ASSERT(actions.qualified_section_id.empty());
        }

        actions.qualified_section_id += actions.section_id;
        ++actions.section_level;

        if (qbk_version_n < 103) // version 1.2 and below
        {
            actions.phrase << "\n<section id=\""
                << actions.doc_id << "."
                << actions.section_id << "\">\n";
        }
        else // version 1.3 and above
        {
            actions.phrase << "\n<section id=\""
                << actions.doc_id
                << "."
                << actions.qualified_section_id << "\">\n";
        }

        if (qbk_version_n < 103) // version 1.2 and below
        {
            actions.phrase << "<title>" << x.content.content << "</title>\n";
        }
        else // version 1.3 and above
        {
            actions.phrase
                << "<title>"
                << "<link linkend=\""
                << actions.doc_id
                << "."
                << actions.qualified_section_id << "\">"
                << x.content.content
                << "</link>"
                << "</title>\n"
                ;
        }

    }

    void process(quickbook::actions& actions, end_section const& x)
    {
        actions.phrase << "</section>";

        --actions.section_level;
        if (actions.section_level < 0)
        {
            detail::outerr(x.position.file,x.position.line)
                << "Mismatched [endsect] near column " << x.position.column << ".\n";
            ++actions.error_count;
            
            // $$$ TODO: somehow fail parse else BOOST_ASSERT(std::string::npos != n)
            // $$$ below will assert.
        }
        if (actions.section_level == 0)
        {
            actions.qualified_section_id.clear();
        }
        else
        {
            std::string::size_type const n =
                actions.qualified_section_id.find_last_of('.');
            BOOST_ASSERT(std::string::npos != n);
            actions.qualified_section_id.erase(n, std::string::npos);
        }
    }

    void process(quickbook::actions& actions, heading const& x)
    {
        // TODO: Is this right?
        bool new_style = qbk_version_n >= 103 || x.level > 0;
        
        int level = x.level;
        if(level < 0) {
            level = actions.section_level + 2;  // section_level is zero-based. We need to use a
                                                // one-based heading which is one greater
                                                // than the current. Thus: section_level + 2.
            if (level > 6)                      // The max is h6, clip it if it goes
                level = 6;                      // further than that
        }

        if (!new_style) // version 1.2 and below
        {
            actions.phrase
                << "<anchor id=\""
                << actions.section_id << '.'
                << detail::make_identifier(
                    x.content.raw_markup.begin(),
                    x.content.raw_markup.end())
                << "\" />"
                << "<bridgehead renderas=\"sect" << level << "\">"
                << x.content.content
                << "</bridgehead>"
                ;
        }
        else // version 1.3 and above
        {
            std::string anchor = fully_qualified_id(
                actions.doc_id, actions.qualified_section_id,
                detail::make_identifier(
                    x.content.raw_markup.begin(),
                    x.content.raw_markup.end()));

            actions.phrase
                << "<anchor id=\"" << anchor << "\"/>"
                << "<bridgehead renderas=\"sect" << level << "\">"
                << "<link linkend=\"" << anchor << "\">"
                << x.content.content
                << "</link>"
                << "</bridgehead>"
                ;
        }
    }

    void process(quickbook::actions& actions, variablelist const& x)
    {
        actions.phrase << "<variablelist>\n";

        actions.phrase << "<title>";
        detail::print_string(x.title, actions.phrase.get());
        actions.phrase << "</title>\n";

        for(std::vector<varlistentry>::const_iterator
            it = x.entries.begin(); it != x.entries.end(); ++it)
        {
            actions.phrase << start_varlistentry_;
            std::for_each(it->begin(), it->end(), actions.process);
            actions.phrase << end_varlistentry_;
        }

        actions.phrase << "</variablelist>\n";
    }


    void process(quickbook::actions& actions, table const& x)
    {
        bool has_title = !x.title.empty();
        
        std::string table_id;
        if(qbk_version_n >= 105) {
            if(x.id) {
                table_id = fully_qualified_id(actions.doc_id,
                    actions.qualified_section_id, *x.id);
            }
            else if(has_title) {
                table_id = fully_qualified_id(actions.doc_id,
                    actions.qualified_section_id,
                    detail::make_identifier(x.title.begin(), x.title.end()));
            }
        }

        if (has_title)
        {
            actions.phrase << "<table frame=\"all\"";
            if(!table_id.empty())
                actions.phrase << " id=\"" << table_id << "\"";
            actions.phrase << ">\n";
            actions.phrase << "<title>";
            detail::print_string(x.title, actions.phrase.get());
            actions.phrase << "</title>";
        }
        else
        {
            actions.phrase << "<informaltable frame=\"all\"";
            if(!table_id.empty())
                actions.phrase << " id=\"" << table_id << "\"";
            actions.phrase << ">\n";
        }

        // This is a bit odd for backwards compatability: the old version just
        // used the last count that was calculated.
        actions.phrase << "<tgroup cols=\""
            << (x.rows.empty() ? 0 : x.rows.back().size())
            << "\">\n";

        std::vector<table_row>::const_iterator row = x.rows.begin();

        // Backwards compatability again: the old version turned the first row
        // into a header at the start of the second row. So it only happened
        // when there was more than one row.
        if (x.rows.size() > 1)
        {
            actions.phrase << "<thead>";
            actions.phrase << start_row_;
            std::for_each(row->begin(), row->end(), actions.process);
            actions.phrase << end_row_;
            actions.phrase << "</thead>\n";
            ++row;
        }

        actions.phrase << "<tbody>\n";

        while(row != x.rows.end()) {
            actions.phrase << start_row_;
            std::for_each(row->begin(), row->end(), actions.process);
            actions.phrase << end_row_;
            ++row;
        }

        actions.phrase << "</tbody>\n"
                     << "</tgroup>\n";

        if (has_title)
        {
            actions.phrase << "</table>\n";
        }
        else
        {
            actions.phrase << "</informaltable>\n";
        }
    }
}