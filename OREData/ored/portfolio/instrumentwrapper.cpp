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

#include <ored/measures/statisticsdata.hpp>
#include <ored/portfolio/instrumentwrapper.hpp>
#include <ored/portfolio/bond.hpp>
#include <ored/utilities/parsers.hpp>
#include <ql/pricingengines/bond/bondfunctions.hpp>

using namespace QuantLib;
using namespace std;

namespace ore {
namespace data {

boost::shared_ptr<const StatisticsData> InstrumentWrapper::statistics(boost::shared_ptr<Market> market, const Bond &bond) const {
    return boost::shared_ptr<const StatisticsData>{new StatisticsData{}};
}

// TODO: Move this to a suitable instrument specific wrapper class
boost::shared_ptr<const StatisticsData> VanillaInstrument::statistics(boost::shared_ptr<Market> market, const Bond &bond) const {
    // Only calculate for coupon bonds
    for (const auto& coupon : bond.coupons()) {
        DayCounter dc = parseDayCounter(coupon.dayCounter());
        Frequency freq = Frequency::OtherFrequency;
        Compounding compounding = Compounding::Compounded;
        for (const auto& schedRule : coupon.schedule().rules()) {
            auto period = parsePeriod(schedRule.tenor());
            auto schedFreq = period.frequency();
            if (freq == Frequency::OtherFrequency) {
                freq = schedFreq;
            } else {
                QL_REQUIRE(schedFreq == freq, "Bond schedule with varying frequencies not allowed");
            }
        }
        // calculate yield-to-maturity
        auto qlBond = boost::static_pointer_cast<QuantLib::Bond>(bond.instrument()->qlInstrument());
        auto ytm = qlBond->yield(dc, compounding, freq);
        auto macDuration = BondFunctions::duration(*qlBond,
                InterestRate{ytm, dc, compounding, freq},
                Duration::Macaulay,
                market->asofDate());

        // use the first leg as definition for the coupon parameters, the whole cashflow is collapsed to one leg
        // in the QuantLib instrument anyway
        auto stats = boost::shared_ptr<StatisticsData>{new StatisticsData{} };
        stats->duration(macDuration);
        stats->yieldToMaturity(ytm);
        return boost::shared_ptr<const StatisticsData> { stats };
    }
    return boost::shared_ptr<const StatisticsData> { new StatisticsData{} };
}

}
}