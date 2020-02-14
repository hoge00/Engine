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
#include <ored/portfolio/bond.hpp>
#include <ored/utilities/log.hpp>
#include <ored/portfolio/builders/bond.hpp>

namespace ore {
namespace data {

using namespace std;
using namespace QuantLib;

MBS::MBS() : Bond("MBS") {}

MBS::~MBS() = default;

void MBS::fromXML(XMLNode* node) {
    Bond::fromXML(node);
    DLOG("Created underlying bond [" << securityId() << "]");
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
    return Bond::toXML(doc);
}

} // namespace data
} // namespace ore
