/*
 Copyright (C) 2016 Quaternion Risk Management Ltd
 All rights reserved.

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

#include <boost/make_shared.hpp>

#include <ored/portfolio/builders/bond.hpp>
#include <ored/portfolio/builders/cachingenginebuilder.hpp>
#include <ored/portfolio/enginefactory.hpp>
#include <ored/utilities/log.hpp>

#include <qle/pricingengines/discountingriskybondengine.hpp>
#include <ql/termstructures/yield/zerospreadedtermstructure.hpp>

using std::map;
using std::set;
using std::string;
using std::pair;
using std::tuple;
using ore::data::Market;
using QuantLib::PricingEngine;
using QuantLib::Disposable;

namespace ore {
namespace data {

DiscountingBondEngineBuilder::DiscountingBondEngineBuilder(const std::string& model, const std::string& engine)
    : CachingEngineBuilder(model, engine, {"Bond"}) {}

string DiscountingBondEngineBuilder::keyImpl(const Currency& ccy, const string& creditCurveId, const string& securityId,
                                             const string& referenceCurveId) {
    return ccy.code() + "_" + creditCurveId + "_" + securityId + "_" + referenceCurveId;
}

DiscountingBondEngineBuilder::DiscountingBondEngineBuilder()
    : DiscountingBondEngineBuilder("DiscountedCashflows", "DiscountingRiskyBondEngine") {}

boost::shared_ptr<PricingEngine> DiscountingBondEngineBuilder::engineImpl(const Currency& ccy,
                                                                          const string& creditCurveId,
                                                                          const string& securityId,
                                                                          const string& referenceCurveId) {
    Handle<YieldTermStructure> yts = market_->yieldCurve(referenceCurveId, configuration(MarketContext::pricing));
    Handle<DefaultProbabilityTermStructure> dpts;
    // credit curve may not always be used. If credit curve ID is empty proceed without it
    if (!creditCurveId.empty())
        dpts = market_->defaultCurve(creditCurveId, configuration(MarketContext::pricing));
    Handle<Quote> recovery;
    try {
        // try security recovery first
        recovery = market_->recoveryRate(securityId, configuration(MarketContext::pricing));
    } catch (...) {
        // otherwise fall back on curve recovery
        ALOG("security specific recovery rate not found for security ID "
                     << securityId << ", falling back on the recovery rate for credit curve Id " << creditCurveId);
        if (!creditCurveId.empty())
            recovery = market_->recoveryRate(creditCurveId, configuration(MarketContext::pricing));
    }
    Handle<Quote> spread;
    try {
        // spread is optional, pass empty handle to engine if not given (will be treated as 0 spread there)
        spread = market_->securitySpread(securityId, configuration(MarketContext::pricing));
        DLOG("Using spread of [" << spread->value() << "] for " << securityId)
    } catch (...) {
    }

    Handle<Quote> price;
    try {
        // price is optional, pass empty handle to engine if not given
        price = market_->securityPrice(securityId, configuration(MarketContext::pricing));
        DLOG("Using price of [" << price->value() << "] for " << securityId)
    } catch (...) {
    }

    string tsperiodStr = engineParameter("TimestepPeriod");
    Period tsperiod = parsePeriod(tsperiodStr);

    if (price.empty())
        return boost::make_shared<QuantExt::DiscountingRiskyBondEngine>(yts, dpts, recovery, spread, tsperiod);
    else
        return QuantExt::DiscountingRiskyBondEngine::pricedBased(yts, price, tsperiod);
}

} // namespace data
} // namespace ore

