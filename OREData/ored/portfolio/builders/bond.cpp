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

#include <vector>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string/join.hpp>

#include <ored/portfolio/builders/bond.hpp>
#include <ored/portfolio/builders/cachingenginebuilder.hpp>
#include <ored/portfolio/enginefactory.hpp>
#include <ored/utilities/log.hpp>

#include <qle/instruments/impliedbondspread.hpp>
#include <qle/pricingengines/discountingriskybondengine.hpp>
#include <ql/pricingengines/bond/bondfunctions.hpp>
#include <ql/pricingengines/bond/discountingbondengine.hpp>
#include <ql/instruments/bond.hpp>
#include <ql/time/daycounters/all.hpp>
#include <ql/termstructures/yield/zerospreadedtermstructure.hpp>

using std::map;
using std::set;
using std::string;
using std::pair;
using std::tuple;
using ore::data::Market;
using QuantLib::PricingEngine;
using QuantLib::Disposable;

namespace {
    std::map<std::string, std::set<std::string>> merge(const std::string& type, const std::string& style) {
        return std::map<std::string, std::set<std::string>> { std::make_pair(type, std::set<std::string> { style }) };
    }
}

namespace ore {
namespace data {

AbstractDiscountingBondEngineBuilder::AbstractDiscountingBondEngineBuilder(const std::string &model,
                                                                           const std::string &engine,
                                                                           const std::set<std::string>& types)
    : CachingEngineBuilder(model, engine, types) {}

AbstractDiscountingBondEngineBuilder::AbstractDiscountingBondEngineBuilder(const std::string &model,
                                                                           const std::string &engine,
                                                                           const std::string& type)
    : CachingEngineBuilder(model, engine, { type }) {}

AbstractDiscountingBondEngineBuilder::AbstractDiscountingBondEngineBuilder(const std::string &model,
                                                                           const std::string &engine,
                                                                           const std::string& type,
                                                                           const std::string& style)
    : CachingEngineBuilder(model, engine, merge(type, style)) {}

DiscountingBondEngineBuilder::DiscountingBondEngineBuilder()
    : DiscountingBondEngineBuilder("DiscountedCashflows", "DiscountingRiskyBondEngine") {}

DiscountingBondEngineBuilder::DiscountingBondEngineBuilder(const std::string& model, const std::string& engine)
    : AbstractDiscountingBondEngineBuilder(model, engine, std::set<string> {"Bond", "MBS"}) {}

string DiscountingBondEngineBuilder::keyImpl(const BondEngineBuilderArgs& args) {
    auto tokens = { args.ccy().code(), args.creditCurveId(), args.securityId(), args.referenceCurveId() };
    return boost::algorithm::join(tokens, "_");
}

boost::shared_ptr<PricingEngine> DiscountingBondEngineBuilder::engineImpl(const BondEngineBuilderArgs& args) {

    const auto& referenceCurveId = args.referenceCurveId();
    const auto& creditCurveId = args.creditCurveId();
    const auto& securityId = args.securityId();

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

    string tsperiodStr = engineParameter("TimestepPeriod");
    Period tsperiod = parsePeriod(tsperiodStr);

    return boost::make_shared<QuantExt::DiscountingRiskyBondEngine>(yts, dpts, recovery, spread, tsperiod);
}

MtmImpliedBondEngineBuilder::MtmImpliedBondEngineBuilder()
: MtmImpliedBondEngineBuilder("DiscountedCashflows", "DiscountingBondEngine") {}

MtmImpliedBondEngineBuilder::MtmImpliedBondEngineBuilder(const std::string &model, const std::string &engine)
: AbstractDiscountingBondEngineBuilder(model, engine, "Bond", "MarkedToMarketImpliedSpread") {}

boost::shared_ptr<PricingEngine> MtmImpliedBondEngineBuilder::engineImpl(const BondEngineBuilderArgs& args) {
    // Fetching price (TODO: Support dirty quotes)
    auto priceAndType = market_->securityPriceAndType(args.securityId(), configuration(MarketContext::pricing));
    auto price = priceAndType.first;
    auto priceType = priceAndType.second;
    QL_REQUIRE(Bond::Price::Type::Clean == priceType, "MtmImpliedBondEngineBuilder only supports clean price quotes");

    DLOG("Using price of [" << price->value() << "] for " << args.securityId())
    // Fetching benchmark discount curve
    auto yts = market_->yieldCurve(args.referenceCurveId(), configuration(MarketContext::pricing));
    // Fetching bond
    auto bond = args.bond();
    // Assuming annual continuous compounding
    auto zspread = BondFunctions::zSpread(*bond,
                                          price->value(),
                                          yts.currentLink(),
                                          yts->dayCounter(),
                                          Compounding::Continuous,
                                          Frequency::Annual,
		                                  yts->referenceDate());

    DLOG("Implied spread [" << zspread << "] for " << args.securityId())

    // Build a spreaded term structure with the implied spread
    auto spreadQuote = Handle<Quote>(boost::make_shared<SimpleQuote>(zspread));
    auto spreadCurve = Handle<YieldTermStructure>(
            boost::make_shared<ZeroSpreadedTermStructure>(yts, spreadQuote, Compounding::Continuous, Frequency::Annual));
    auto engine = ext::shared_ptr<PricingEngine>(new DiscountingBondEngine(spreadCurve, boost::none));
    return engine;
}

string MtmImpliedBondEngineBuilder::keyImpl(const BondEngineBuilderArgs& args) {
    auto tokens = { args.ccy().code(), args.creditCurveId(), args.securityId(), args.referenceCurveId() };
    return boost::algorithm::join(tokens, "_");
}

} // namespace data
} // namespace ore

