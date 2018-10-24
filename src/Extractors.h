// =====================================================================================
//
//       Filename:  Extractors.h
//
//    Description:  module which scans the set of collected EDGAR files and extracts
//                  relevant data from the file.
//
//      Inputs:
//
//        Version:  1.0
//        Created:  03/20/2018
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (dpr), driedel@cox.net
//        License:  GNU General Public License v3
//        Company:
//
// =====================================================================================
//


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
#ifndef __XBRL_Extractors__
#define __XBRL_Extractors__

#include <experimental/filesystem>
#include <experimental/string_view>
#include <variant>
using sview = std::experimental::string_view;

//#include <boost/hana.hpp>

#include "gq/Node.h"

#include "ExtractEDGAR.h"

namespace fs = std::experimental::filesystem;
/* namespace hana = boost::hana; */

// for use with ranges.

std::vector<sview> FindDocumentSections(sview file_content);

sview FindFileNameInSection(sview document);

sview FindHTML(sview document);

sview FindTableOfContents(sview document);

std::string CollectAllAnchors (sview document);

std::string CollectTableContent(sview html);

std::string CollectFinancialStatementContent (sview document_content);

std::string ExtractTextDataFromTable (CNode& a_table);

std::string FilterFoundHTML(const std::string& new_row_data);

// list of filters that can be applied to the input document
// to select content.

struct XBRL_data
{
    void UseExtractor(sview document, const fs::path& output_directory, const EE::SEC_Header_fields& fields);
};

struct XBRL_Label_data
{
    void UseExtractor(sview document, const fs::path& output_directory, const EE::SEC_Header_fields& fields);
};

struct SS_data
{
    void UseExtractor(sview document, const fs::path& output_directory, const EE::SEC_Header_fields& fields);
};

struct Count_SS
{
    int SS_counter = 0;

    void UseExtractor(sview document, const fs::path&, const EE::SEC_Header_fields&);
};

struct DocumentCounter
{
    int document_counter = 0;

    void UseExtractor(sview, const fs::path&, const EE::SEC_Header_fields&);
};

struct HTM_data
{
    void UseExtractor(sview, const fs::path&, const EE::SEC_Header_fields&);
};

// this filter will export all document sections.

struct ALL_data
{
    void UseExtractor(sview, const fs::path&, const EE::SEC_Header_fields&);
};

// someday, the user can sellect filters.  We'll pretend we do that here.

// NOTE: this implementation uses code from a Stack Overflow example.
// https://stackoverflow.com/questions/28764085/how-to-create-an-element-for-each-type-in-a-typelist-and-add-it-to-a-vector-in-c

// BUT...I am beginning to think that this is not really doable since the point of static polymorphism is that things
// are known at compile time, not runtime.
// So, unless I come to a different understanding, I'm going to go with just isolating where the applicable filters are set
// and leave the rest of the code as generic.
// I use the Hana library since it makes the subsequent iteration over the generic list MUCH simpler...

// BUT...I can achieve this effect nicely using a hetereogenous list containing one or more extractors.

using FilterTypes = std::variant<XBRL_data, XBRL_Label_data, SS_data, Count_SS, DocumentCounter, HTM_data, ALL_data>;
using FilterList = std::vector<FilterTypes>;

FilterList SelectExtractors(int argc, const char* argv[]);
//{
//    // we imagine the user has somehow told us to use these three filter types.
//
//    auto L = hana::make_tuple(std::make_unique<XBRL_data>(),  std::make_unique<XBRL_Label_data>(), std::make_unique<SS_data>(),  std::make_unique<DocumentCounter>());
//    // auto L = hana::make_tuple(std::make_unique<XBRL_data>(), std::make_unique<SS_data>(), std::make_unique<DocumentCounter>(), std::make_unique<HTM_data>());
//    // auto L = hana::make_tuple(std::make_unique<ALL_data>(), std::make_unique<DocumentCounter>());
//    return L;
//}


#endif /* end of include guard:  __XBRL_Extractors__*/
