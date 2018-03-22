// =====================================================================================
//
//       Filename:  ExtractEDGAR_XBRL.cpp
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

#include <fstream>

#include "ExtractEDGAR_XBRL.h"


void WriteDataToFile(const fs::path& output_file_name, const std::string_view& document)
{
    std::ofstream output_file(output_file_name);
    if (not output_file)
        throw(std::runtime_error("Can't open output file: " + output_file_name.string()));

    output_file.write(document.data(), document.length());
    output_file.close();
}

fs::path FindFileName(const fs::path& output_directory, const std::string_view& document, const boost::regex& regex_fname)
{
    boost::cmatch matches;
    bool found_it = boost::regex_search(document.cbegin(), document.cend(), matches, regex_fname);
    if (found_it)
    {
        const std::string_view file_name(matches[1].first, matches[1].length());
        fs::path output_file_name{output_directory};
        output_file_name /= file_name;
        return output_file_name;
    }
    else
        throw std::runtime_error("Can't find file name in document.\n");
}
