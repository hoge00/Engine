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

#include <ored/portfolio/mbs.hpp>
#include <ored/portfolio/mbsdata.hpp>
#include <ored/portfolio/bond.hpp>
#include <ored/utilities/log.hpp>

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

MBS::MBS() : Trade("MBS") {}

MBS::~MBS() = default;

void MBS::fromXML(XMLNode* node) {
    auto bond = unique_ptr<Bond>(new Bond());
    bond->fromXML(node);
    bond_ = move(bond);
    DLOG("Created underlying bond [" << bond_->securityId() << "]");
    data_ = getData(node);
}

void MBS::build(const boost::shared_ptr<EngineFactory> &) {

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
