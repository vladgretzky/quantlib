
/*
 Copyright (C) 2002, 2003, 2004 Ferdinando Ametrano
 Copyright (C) 2003 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/Volatilities/blackvariancecurve.hpp>
#include <ql/Math/interpolationtraits.hpp>

namespace QuantLib {

    #ifndef QL_DISABLE_DEPRECATED
    BlackVarianceCurve::BlackVarianceCurve(
                                 const Date& referenceDate,
                                 const std::vector<Date>& dates,
                                 const std::vector<Volatility>& blackVolCurve,
                                 const DayCounter& dayCounter)
    : BlackVarianceTermStructure(referenceDate), dayCounter_(dayCounter),
      maxDate_(dates.back()) {

        QL_REQUIRE(dates.size()==blackVolCurve.size(),
                   "mismatch between date vector and black vol vector");

        // cannot have dates[0]==referenceDate, since the
        // value of the vol at dates[0] would be lost
        // (variance at referenceDate must be zero)
        QL_REQUIRE(dates[0]>referenceDate,
                   "cannot have dates[0] <= referenceDate");

        variances_ = std::vector<Real>(dates.size()+1);
        times_ = std::vector<Time>(dates.size()+1);
        variances_[0] = 0.0;
        times_[0] = 0.0;
        Size j;
        for (j=1; j<=blackVolCurve.size(); j++) {
            times_[j] = timeFromReference(dates[j-1]);
            QL_REQUIRE(times_[j]>times_[j-1],
                       "dates must be sorted unique!");
            variances_[j] = times_[j] *
                blackVolCurve[j-1]*blackVolCurve[j-1];
            QL_REQUIRE(variances_[j]>=variances_[j-1],
                       "variance must be non-decreasing");
        }

        // default: linear interpolation
        #if defined(QL_PATCH_MSVC6)
        setInterpolation(Linear());
        #else
        setInterpolation<Linear>();
        #endif
    }
    #endif

    BlackVarianceCurve::BlackVarianceCurve(
                                 const Date& referenceDate,
                                 const std::vector<Date>& dates,
                                 const std::vector<Volatility>& blackVolCurve)
    : BlackVarianceTermStructure(referenceDate),
      #ifndef QL_DISABLE_DEPRECATED
      dayCounter_(Settings::instance().dayCounter()),
      #endif
      maxDate_(dates.back()) {

        QL_REQUIRE(dates.size()==blackVolCurve.size(),
                   "mismatch between date vector and black vol vector");

        // cannot have dates[0]==referenceDate, since the
        // value of the vol at dates[0] would be lost
        // (variance at referenceDate must be zero)
        QL_REQUIRE(dates[0]>referenceDate,
                   "cannot have dates[0] <= referenceDate");

        variances_ = std::vector<Real>(dates.size()+1);
        times_ = std::vector<Time>(dates.size()+1);
        variances_[0] = 0.0;
        times_[0] = 0.0;
        Size j;
        for (j=1; j<=blackVolCurve.size(); j++) {
            times_[j] = timeFromReference(dates[j-1]);
            QL_REQUIRE(times_[j]>times_[j-1],
                       "dates must be sorted unique!");
            variances_[j] = times_[j] *
                blackVolCurve[j-1]*blackVolCurve[j-1];
            QL_REQUIRE(variances_[j]>=variances_[j-1],
                       "variance must be non-decreasing");
        }

        // default: linear interpolation
        #if defined(QL_PATCH_MSVC6)
        setInterpolation(Linear());
        #else
        setInterpolation<Linear>();
        #endif
    }

    Real BlackVarianceCurve::blackVarianceImpl(Time t, Real) const {
        if (t<=times_.back()) {
            return varianceCurve_(t, true);
        } else {
            // extrapolate with flat vol
            return varianceCurve_(times_.back(), true)*t/times_.back();
        }
    }

}

