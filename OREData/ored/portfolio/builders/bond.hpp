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
#include <utility>

namespace QuantLib {
class Bond;
}

namespace ore {
namespace data {

//! Engine Builder base class for Bonds
/*! Pricing engines are cached by security id
\ingroup builders
*/

class BondEngineBuilderArgs  {
public:
    BondEngineBuilderArgs(Currency ccy, string creditCurveId, string securityId, string referenceCurveId, ext::shared_ptr<QuantLib::Bond> bond)
    : ccy_(std::move(ccy)),
    creditCurveId_(std::move(creditCurveId)),
    securityId_(std::move(securityId)),
    referenceCurveId_(std::move(referenceCurveId)),
    bond_(bond) {}

    const Currency& ccy() const { return ccy_; }
    const string& creditCurveId() const { return creditCurveId_; }
    const string& securityId() const { return securityId_; }
    const string& referenceCurveId() const { return referenceCurveId_; }
    ext::shared_ptr<QuantLib::Bond> bond() const { return bond_; }

private:
    const Currency ccy_;
    const string creditCurveId_;
    const string securityId_;
    const string referenceCurveId_;
    const ext::shared_ptr<QuantLib::Bond> bond_;
};

template <typename... Args>
using BondEngineBuilder = CachingPricingEngineBuilder<string, const BondEngineBuilderArgs&, Args...>;

class AbstractDiscountingBondEngineBuilder : public BondEngineBuilder<> {
public:
    AbstractDiscountingBondEngineBuilder() = delete;

protected:
    AbstractDiscountingBondEngineBuilder(const std::string& model,
                                         const std::string& engine,
                                         const std::set<std::string>& types);

    AbstractDiscountingBondEngineBuilder(const std::string& model,
                                         const std::string& engine,
                                         const std::string& type);

    AbstractDiscountingBondEngineBuilder(const std::string& model,
                                         const std::string& engine,
                                         const std::string& type,
                                         const std::string& style);

};

class DiscountingBondEngineBuilder : public AbstractDiscountingBondEngineBuilder {
public:
    DiscountingBondEngineBuilder();

protected:
    DiscountingBondEngineBuilder(const std::string& model, const std::string& engine);

    string keyImpl(const BondEngineBuilderArgs&) override;
    boost::shared_ptr<PricingEngine> engineImpl(const BondEngineBuilderArgs&) override;
};

class MtmImpliedBondEngineBuilder : public AbstractDiscountingBondEngineBuilder {
public:
    MtmImpliedBondEngineBuilder();

protected:
    MtmImpliedBondEngineBuilder(const std::string& model, const std::string& engine);

    string keyImpl(const BondEngineBuilderArgs&) override;
    boost::shared_ptr<PricingEngine> engineImpl(const BondEngineBuilderArgs&) override;
};

} // namespace data
} // namespace ore
