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

/*! \file portfolio/bond.hpp
 \brief Bond trade data model and serialization
 \ingroup tradedata
 */
#pragma once

#include <ql/shared_ptr.hpp>
#include <ored/portfolio/legdata.hpp>
#include <ored/portfolio/trade.hpp>

namespace ore {
namespace data {

//! Serializable Bond
/*!
\ingroup tradedata
*/
class Bond : public Trade {
public:
    //! Default constructor
    Bond() : Trade("Bond"), faceAmount_(0), zeroBond_(false) {}

    //! Constructor specifying type
    Bond(const string& type) : Trade(type), faceAmount_(0), zeroBond_(false) {}

    //! Constructor for coupon bonds
    Bond(Envelope env, string issuerId, string creditCurveId, string securityId, string referenceCurveId,
         string settlementDays, string calendar, string issueDate, LegData& coupons, const string& instrumentStyle = "")
        : Trade("Bond", env), issuerId_(issuerId), creditCurveId_(creditCurveId), securityId_(securityId),
          referenceCurveId_(referenceCurveId), settlementDays_(settlementDays), calendar_(calendar),
          issueDate_(issueDate), coupons_(std::vector<LegData>{coupons}), faceAmount_(0), maturityDate_(), currency_(),
          zeroBond_(false), instrumentStyle_(instrumentStyle) {}

    //! Constructor for coupon bonds with multiple phases (represented as legs)
    Bond(Envelope env, string issuerId, string creditCurveId, string securityId, string referenceCurveId,
         string settlementDays, string calendar, string issueDate, const std::vector<LegData>& coupons,
         const string& instrumentStyle = "")
        : Trade("Bond", env), issuerId_(issuerId), creditCurveId_(creditCurveId), securityId_(securityId),
          referenceCurveId_(referenceCurveId), settlementDays_(settlementDays), calendar_(calendar),
          issueDate_(issueDate), coupons_(coupons), faceAmount_(0), maturityDate_(), currency_(), zeroBond_(coupons.empty()), instrumentStyle_(instrumentStyle) {}

    //! Constructor for zero bonds
    Bond(Envelope env, string issuerId, string creditCurveId, string securityId, string referenceCurveId,
         string settlementDays, string calendar, Real faceAmount, string maturityDate, string currency,
         string issueDate, const string& instrumentStyle = "")
        : Trade("Bond", env), issuerId_(issuerId), creditCurveId_(creditCurveId), securityId_(securityId),
          referenceCurveId_(referenceCurveId), settlementDays_(settlementDays), calendar_(calendar),
          issueDate_(issueDate), coupons_(), faceAmount_(faceAmount), maturityDate_(maturityDate), currency_(currency),
          zeroBond_(true), instrumentStyle_(instrumentStyle) {}

    // Build QuantLib/QuantExt instrument, link pricing engine
    void build(const boost::shared_ptr<EngineFactory>&) override;

    //! Return the fixings that will be requested to price the Bond given the \p settlementDate.
    std::map<std::string, std::set<QuantLib::Date>> fixings(const QuantLib::Date&) const override;

    void fromXML(XMLNode* node) override;
    XMLNode* toXML(XMLDocument& doc) override;

    ext::shared_ptr<const StatisticsData> statistics(ext::shared_ptr<Market>) const override;

    const string& issuerId() const { return issuerId_; }
    const string& creditCurveId() const { return creditCurveId_; }
    const string& securityId() const { return securityId_; }
    const string& referenceCurveId() const { return referenceCurveId_; }
    const string& settlementDays() const { return settlementDays_; }
    const string& calendar() const { return calendar_; }
    const string& issueDate() const { return issueDate_; }
    const std::vector<LegData>& coupons() const { return coupons_; }
    const Real& faceAmount() const { return faceAmount_; }
    const string& maturityDate() const { return maturityDate_; }
    const string& currency() const { return currency_; }
    const string& instrumentStyle() const { return instrumentStyle_; }

    bool isZeroBond() const;

protected:
    virtual boost::shared_ptr<LegData> createLegData() const;

private:
    string issuerId_;
    string creditCurveId_;
    string securityId_;
    string referenceCurveId_;
    string settlementDays_;
    string calendar_;
    string issueDate_;
    std::vector<LegData> coupons_;
    Real faceAmount_;
    string maturityDate_;
    string currency_;
    bool zeroBond_;
    string instrumentStyle_;

    /*! A bond may consist of multiple legs joined together to create a single leg. This member stores the 
        separate legs so that fixings can be retrieved later for legs that have fixings.
    */
    std::vector<QuantLib::Leg> separateLegs_;

    /*! Set of pairs where first element of pair is the ORE index name and the second element of the pair is 
        the index of the leg, in separateLegs_, that contains that ORE index.
    */
    std::set<std::pair<std::string, QuantLib::Size>> nameIndexPairs_;
};
} // namespace data
} // namespace ore
