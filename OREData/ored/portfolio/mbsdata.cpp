/*
 This file is part of ORE, a free-software/open-source library
 for transparent pricing and risk analysis - http://opensourcerisk.org

 ORE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.
 The license is also available online at <http://opensourcerisk.org>

 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

#include <ql/errors.hpp>
#include <ored/portfolio/mbsdata.hpp>
#include <ored/utilities/log.hpp>

namespace ore {
namespace data {

using namespace std;
using namespace QuantLib;

void MBSData::fromXML(XMLNode* node) {
}

XMLNode *MBSData::toXML(XMLDocument &doc) {
    // TODO
    QL_FAIL("Not implemented");
}

} // namespace data
} // namespace ore
