// =====================================================================================
//
//       Filename:  ExtractEDGAR_XBRL.h
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
#ifndef __EXTRACTEDGAR_XBRL__
#define __EXTRACTEDGAR_XBRL__


#include <experimental/filesystem>
#include <experimental/string_view>

#include <boost/regex.hpp>

#include "Filters.h"

namespace fs = std::experimental::filesystem;

void WriteDataToFile(const fs::path& output_file_name, const std::string_view& document);

fs::path FindFileName(const fs::path& output_directory, const std::string_view& document, const boost::regex& regex_fname);


#endif /* end of include guard: __EXTRACTEDGAR_XBRL__*/