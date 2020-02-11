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

/*! \file ored/portfolio/mbs.hpp
    \brief Mortgage Backed Security
    \ingroup tradedata
*/

#pragma once

#include <ored/portfolio/bond.hpp>

namespace ore {
namespace data {

class Bond;
class MBSData;

//! Serializable MBS
/*!
\ingroup tradedata
*/
class MBS : public Bond {
public:
    MBS();
    ~MBS() override;

    // Build QuantLib/QuantExt instrument, link pricing engine
    void build(const boost::shared_ptr<EngineFactory>&) override;

    std::map<std::string, std::set<QuantLib::Date>> fixings(const QuantLib::Date&) const override;

    void fromXML(XMLNode* node) override;
    XMLNode* toXML(XMLDocument& doc) override;

private:
    std::unique_ptr<MBSData> data_;
};

} // namespace data
} // namespace ore
