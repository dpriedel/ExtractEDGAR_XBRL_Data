/*
 * =====================================================================================
 *
 *       Filename:  ExtractEDGAR_Utils.h
 *
 *    Description:  Routines shared by XBRL and HTLM extracts.
 *
 *        Version:  1.0
 *        Created:  11/14/2018 11:13:04 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  David P. Riedel (), driedel@cox.net
 *   Organization:  
 *
 * =====================================================================================
 */


	/* This file is part of ExtractEDGARData. */

	/* ExtractEDGARData is free software: you can redistribute it and/or modify */
	/* it under the terms of the GNU General Public License as published by */
	/* the Free Software Foundation, either version 3 of the License, or */
	/* (at your option) any later version. */

	/* ExtractEDGARData is distributed in the hope that it will be useful, */
	/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
	/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
	/* GNU General Public License for more details. */

	/* You should have received a copy of the GNU General Public License */
	/* along with ExtractEDGARData.  If not, see <http://www.gnu.org/licenses/>. */

// =====================================================================================
//        Class:  EDGAR_FileFilter
//  Description:  class which EDGAR files to extract data from.
// =====================================================================================

#ifndef  ExtractEDGAR_Utils_INC
#define  ExtractEDGAR_Utils_INC

#include <exception>
#include <map>
#include <string_view>
#include <tuple>
#include <vector>

#include <boost/assert.hpp>

using sview = std::string_view;

// some code to help with putting together error messages,

// first, we need to be able to convert things to std::string.

template<typename T>
std::string dpr_to_string(T&& t)
{
    if constexpr(std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>)
    {
        return std::to_string(std::forward<T>(t));
    }
    else if constexpr(std::is_convertible_v<T, std::string>)
    {
        return std::string(std::forward<T>(t));
    }
    else if constexpr(std::is_same_v<T, std::string>)
    {
        return t;
    }
    else
    {
        return {};
    }
}

// now, a function to concatenate a bunch of string-like things.

template<typename... Ts>
std::string catenate(Ts&&... ts)
{
    // let's use fold expression

    return (dpr_to_string(std::forward<Ts>(ts)) += ...);
}

#include "ExtractEDGAR.h"

std::string LoadDataFileForUse(const char* file_name);

// so we can recognize our errors if we want to do something special

class ExtractException : public std::runtime_error
{
public:

    explicit ExtractException(const char* what);

    explicit ExtractException(const std::string& what);
};

// function to split a string on a delimiter and return a vector of string-views

inline std::vector<sview> split_string(sview string_data, char delim)
{
    std::vector<sview> results;
	for (auto it = 0; it != sview::npos; ++it)
	{
		auto pos = string_data.find(delim, it);
        if (pos != sview::npos)
        {
    		results.emplace_back(string_data.substr(it, pos - it));
        }
        else
        {
    		results.emplace_back(string_data.substr(it));
            break;
        }
		it = pos;
	}
    return results;
}

// utility function

template<typename ...Ts>
auto NotAllEmpty(Ts ...ts)
{
    return ((! ts.empty()) || ...);
}

template<typename ...Ts>
auto AllNotEmpty(Ts ...ts)
{
    return ((! ts.empty()) && ...);
}

// a little helper to run our filters.

template<typename... Ts>
auto ApplyFilters(const EE::SEC_Header_fields& header_fields, sview file_content, Ts ...ts)
{
    // unary left fold

	return (... && (ts(header_fields, file_content)));
}

bool FormIsInFileName(std::vector<sview>& form_types, const std::string& file_name);

std::vector<sview> LocateDocumentSections(sview file_content);

sview FindFileName(sview document);

sview FindFileType(sview document);

sview FindHTML(sview document);

#endif   /* ----- #ifndef ExtractEDGAR_Utils_INC  ----- */
