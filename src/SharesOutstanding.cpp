// =====================================================================================
//
//       Filename:  SharesOutstanding.cpp
//
//    Description: class to find the number of shares ourstanding from a SEC 10K or 10Q report 
//
//        Version:  2.0
//        Created:  10/11/2019 01:11:43 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (), driedel@cox.net
//        License:  GNU General Public License -v3
//
// =====================================================================================

#include "SharesOutstanding.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <set>

//#include <range/v3/action/remove_if.hpp>
#include <range/v3/action/sort.hpp>
//#include <range/v3/action/transform.hpp>
//#include <range/v3/action/unique.hpp>
//#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/for_each.hpp>
//#include <range/v3/iterator/insert_iterators.hpp>
//#include <range/v3/view/cache1.hpp>
//#include <range/v3/view/filter.hpp>
//#include <range/v3/view/map.hpp>
//#include <range/v3/view/split.hpp>
//#include <range/v3/view/transform.hpp>
//#include <range/v3/view/trim.hpp>


#include "spdlog/spdlog.h"

#include "HTML_FromFile.h"

const int32_t MAX_HTML_TO_PARSE = 1'000'000;
const int32_t MAX_TEXT_TO_CLEAN = 20'000;

int64_t SharesOutstanding::operator() (EM::sv html) const
{
//    const std::string se_8 = R"***(\byes\b.{1,10}?no.{1,1000}?(\b[1-9](?:[0-9]{0,2})(?:,[0-9]{3})+\b))***";
    const std::string se_8 = R"***((\b[1-9](?:[0-9]{0,2})(?:,[0-9]{3})+\b))***";

    const boost::regex::flag_type my_flags = {boost::regex_constants::normal | boost::regex_constants::icase};

//    std::map<int, std::unique_ptr<boost::regex>> shares_extractors_;
//
//    shares_extractors_.emplace(1, std::make_unique<boost::regex>(se_8, my_flags));

    const boost::regex regex_share_extractor{se_8, my_flags};

    std::string the_text = ParseHTML(html, MAX_HTML_TO_PARSE, MAX_TEXT_TO_CLEAN);
    std::vector<EM::sv> possibilites = FindCandidates(the_text);

    if (possibilites.empty())
    {
        spdlog::info("No possibles found");
        spdlog::info(the_text);
        return -1;
    }

    spdlog::debug("\npossibilities-----------------------------");
    ranges::for_each(possibilites, [](const auto& x) { spdlog::debug(catenate("Possible: ", x)); });

    std::string shares = "-1";

    for (auto possible : possibilites)
    {
        boost::cmatch the_shares;

        if (bool found_it = boost::regex_search(std::begin(possible), std::end(possible), the_shares, regex_share_extractor))
        {
           shares = the_shares.str(1); 
           break;
        }
    }

    int64_t shares_outstanding;

    if (shares != "-1")
    {

        // need to replace any commas we might have.

        const std::string delete_this{""};
        const boost::regex regex_comma{R"***(,)***"};
        shares = boost::regex_replace(shares, regex_comma, delete_this);

        if (auto [p, ec] = std::from_chars(shares.data(), shares.data() + shares.size(), shares_outstanding); ec != std::errc())
        {
            throw ExtractorException(catenate("Problem converting shares outstanding: ", std::make_error_code(ec).message(), '\n'));
        }
    }
    else
    {
        shares_outstanding = -1;
    }
    
    spdlog::debug(catenate("Shares outstanding: ", shares_outstanding));
    return shares_outstanding;
}		// -----  end of method SharesOutstanding::operator()  ----- 

void CleanText(GumboNode* node, size_t max_length_to_clean, std::string& cleaned_text)
{
    //    this code is based on example code in Gumbo Parser project
    //    I've added the ability to break out of the recursive
    //    loop if a maximum length to parse is given.
    //    I use a Pythonic approach...throw an exception (think 'stop iteration').

    static const boost::regex regex_nbr{R"***(\b[1-9](?:[0-9]{0,2})(?:,[0-9]{3})+\b)***"};

    if (node->type == GUMBO_NODE_TEXT)
    {
        // the gumbo parse library seems to find all the actual text content in nodes
        // here.

        // sometimes, the number of shares runs together with some text so try
        // to separate them.

        EM::sv the_text{node->v.text.text};
        cleaned_text += the_text;
        cleaned_text += ' ';
    }
    if (node->type == GUMBO_NODE_ELEMENT && node->v.element.tag != GUMBO_TAG_SCRIPT && node->v.element.tag != GUMBO_TAG_STYLE)
    {
        std::string contents;
        GumboVector* children = &node->v.element.children;

        for (unsigned int i = 0; i < children->length; ++i)
        {
            CleanText((GumboNode*) children->data[i], max_length_to_clean, cleaned_text);
        }
        if (max_length_to_clean > 0 && cleaned_text.size() >= max_length_to_clean)
        {
            throw std::length_error("'stop iteration'");
        }
    }
}		// -----  end of method SharesOutstanding::CleanText  ----- 

std::vector<EM::sv> FindCandidates(const std::string& the_text)
{
    // the beginning of the HTML content is actually a 'form' which almost always contains
    // the information we are looking for.

    // this regex looks for an identifiable part of the form followed by something which
    // looks like the number of outstanding shares.

    static const std::string shares = R"***((?:\bshares|outstanding|common\b))***";
    static const std::string a7 = R"***(\byes\b.{1,10}?no.{1,1000}?\b[1-9](?:[0-9]{0,2})(?:,[0-9]{3})+\b)***";
    static const std::string a8 = R"***((?:\binidcate\b|\bas of \b|\bnumber of\b).{1,200}?\b[1-9](?:[0-9]{0,2})(?:,[0-9]{3})+\b)***";

    static const boost::regex regex_shares{shares, boost::regex_constants::normal | boost::regex_constants::icase};
    static const boost::regex regex_shares_only7{a7, boost::regex_constants::normal | boost::regex_constants::icase};
    static const boost::regex regex_shares_only8{a8, boost::regex_constants::normal | boost::regex_constants::icase};

    std::vector<EM::sv> results;

    boost::sregex_iterator iter1(the_text.begin(), the_text.end(), regex_shares_only7);
    std::for_each(iter1, boost::sregex_iterator{}, [&the_text, &results] (const boost::smatch& m)
    {
        EM::sv xx(the_text.data() + m.position(), m.length());
        results.push_back(xx);
    });

    if (results.empty())
    {
        boost::sregex_iterator iter2(the_text.begin(), the_text.end(), regex_shares_only8);
        std::for_each(iter2, boost::sregex_iterator{}, [&the_text, &results] (const boost::smatch& m)
        {
            EM::sv xx(the_text.data() + m.position(), m.length() + 100);
            if (boost::regex_search(xx.begin(), xx.end(), regex_shares))
            {
                results.push_back(xx);
            }
        });
    }
//    boost::sregex_iterator iter3(the_text.begin(), the_text.end(), regex_shares_only6);
//    std::for_each(iter3, boost::sregex_iterator{}, [&the_text, &results] (const boost::smatch& m)
//    {
//        EM::sv xx(the_text.data() + m.position() -10, m.length() + 110);
//        results.push_back(xx);
//    });

    // prefer shortest matches

    if (results.size() > 1)
    {
        results |= ranges::actions::sort([](const auto& a, const auto& b) { return a.size() < b.size(); });
    }
    return results;
}		// -----  end of method SharesOutstanding::FindCandidates  ----- 

std::string ParseHTML (EM::sv html, size_t max_length_to_parse, size_t max_length_to_clean)
{
    static const boost::regex regex_hi_ascii{R"***([^\x00-\x7f])***"};
    static const boost::regex regex_multiple_spaces{R"***( {2,})***"};
    static const boost::regex regex_nl{R"***(\n{1,})***"};
    static const std::string one_space = " ";
    static const boost::regex regex_nbr{R"***(([1-9](?:[0-9]{0,2})(?:,[0-9]{3})+))***"};
    static const boost::regex regex_dollar_number{R"***(\$ +\b[1-9](?:[0-9]{0,2})(?:,[0-9]{3})+\b)***"};

    GumboOptions options = kGumboDefaultOptions;

    size_t length_HTML_to_parse = max_length_to_parse == 0 ? html.length() : std::min(html.length(), max_length_to_parse);

    std::unique_ptr<GumboOutput, std::function<void(GumboOutput*)>> output(gumbo_parse_with_options(&options, html.data(), length_HTML_to_parse),
            [&options](GumboOutput* output){ gumbo_destroy_output(&options, output); });

    std::string parsed_text;
    try
    {
        CleanText(output->root, max_length_to_clean, parsed_text);
    }
    catch (std::length_error& e)
    {
        //   nothing to do, should be 'stop iteration' 
    }
    gumbo_destroy_output(&options, output.release());
    //
    // do a little cleanup to make searching easier

    std::string the_text = boost::regex_replace(parsed_text, regex_hi_ascii, one_space);
    the_text = boost::regex_replace(the_text, regex_multiple_spaces, one_space);
    the_text = boost::regex_replace(the_text, regex_nl, one_space);
    the_text = boost::regex_replace(the_text, regex_nbr, " $1 ");
    the_text = boost::regex_replace(the_text, regex_dollar_number, one_space);

    return the_text;
}		// -----  end of method SharesOutstanding::ParseHTML  ----- 


