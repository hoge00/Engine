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

/*! \file ored/mesaures/statisticsdata.hpp
    \brief Statistics data
    \ingroup measures
*/

#pragma once

#include <boost/noncopyable.hpp>
#include <ql/types.hpp>

namespace ore {
namespace data {

using namespace QuantLib;

class StatisticsData : boost::noncopyable {

public:
    StatisticsData();

    void duration(const Time& duration) { duration_ = duration; }
    const Time& duration() const { return duration_; }

    void yieldToMaturity(const Rate& yieldToMaturity) { yieldToMaturity_ = yieldToMaturity; }
    const Rate& yieldToMaturity() const { return yieldToMaturity_; }

private:
    Time duration_;
    Rate yieldToMaturity_;
};

} // namespace data
} // namespace ore
