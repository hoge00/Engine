/*
 Copyright (C) 2017 Quaternion Risk Management Ltd
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

/*! \file portfolio/builders/bond.hpp
\brief
\ingroup builders
*/

#pragma once

#include <ored/portfolio/builders/cachingenginebuilder.hpp>

namespace ore {
namespace data {

//! Engine Builder base class for Bonds
/*! Pricing engines are cached by security id
\ingroup builders
*/

class BondEngineBuilder
    : public CachingPricingEngineBuilder<string, const Currency&, const string&, const string&, const string&> {
protected:
    BondEngineBuilder(const std::string& model, const std::string& engine);

    string keyImpl(const Currency& ccy,
                   const string& creditCurveId,
                   const string& securityId,
                   const string& referenceCurveId) override;
};

//! Discounting Engine Builder class for Bonds
/*! This class creates a DiscountingRiskyBondEngine
\ingroup builders
*/

class BondDiscountingEngineBuilder : public BondEngineBuilder {
public:
    BondDiscountingEngineBuilder();

protected:
    boost::shared_ptr<PricingEngine> engineImpl(const Currency& ccy,
                                                const string& creditCurveId,
                                                const string& securityId,
                                                const string& referenceCurveId) override;
};

} // namespace data
} // namespace ore
