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

#include <ql/experimental/amortizingbonds/amortizingfixedratebond.hpp>
#include <ored/portfolio/mbs.hpp>
#include <ored/portfolio/mbsdata.hpp>
#include <ored/portfolio/bond.hpp>
#include <ored/utilities/log.hpp>
#include <ored/portfolio/builders/bond.hpp>

namespace {
    std::unique_ptr<ore::data::MBSData> getData(ore::data::XMLNode* node) {
        auto data = std::unique_ptr<ore::data::MBSData>(new ore::data::MBSData());
        auto dataNode = ore::data::XMLUtils::getChildNode(node, "MBSData");
        QL_REQUIRE(dataNode, "No <MBSData> node in trade XML");
        data->fromXML(dataNode);
        return data;
    }
}

namespace ore {
namespace data {

using namespace std;
using namespace QuantLib;

MBS::MBS() : Bond("MBS") {}

MBS::~MBS() = default;

void MBS::fromXML(XMLNode* node) {
    Bond::fromXML(node);
    DLOG("Created underlying bond [" << securityId() << "]");
    data_ = getData(node);
}

void MBS::build(const boost::shared_ptr<EngineFactory> & engineFactory) {
    DLOG("MBS::build() called for trade " << id());
    QL_REQUIRE(coupons().size() == 1, tradeType_ << " only supports single leg");

    Bond::build(engineFactory);
}

map<string, set<Date>> MBS::fixings(const Date &) const {
    return map<string, set<Date>>();
}

XMLNode *MBS::toXML(XMLDocument &doc) {
    // TODO
    QL_FAIL("Not implemented");
}

} // namespace data
} // namespace ore
