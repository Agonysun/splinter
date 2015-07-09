/*
 * This file is part of the SPLINTER library.
 * Copyright (C) 2012 Bjarne Grimstad (bjarne.grimstad@gmail.com).
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "testingutilities.h"
#include <iostream>

using namespace std;

namespace SPLINTER
{

// Checks if a is within margin of b
bool equalsWithinRange(double a, double b, double margin)
{
    return b - margin <= a && a <= b + margin;
}

bool compareFunctions(const Function &f1, const Function &f2)
{
    if(f1.getNumVariables() != f2.getNumVariables()) {
        return false;
    }

    /* TODO:
     * Generate permutations of x and eval
     */
    auto x0_vec = linspace(0, 2, 20);
    auto x1_vec = linspace(0, 2, 20);
    DenseVector x(2);
    double y;
    for (auto x0 : x0_vec)
    {
        for (auto x1 : x1_vec)
        {
            // Sample function at x
            x(0) = x0;
            x(1) = x1;

            if(f1.eval(x) != f2.eval(x)) {
                return false;
            }
        }
    }

    return true;
}

bool compareBSplines(BSpline &bs, const BSpline &bs_orig)
{
    auto lb = bs.getDomainLowerBound();
    auto ub = bs.getDomainUpperBound();

    auto x0_vec = linspace(lb.at(0), ub.at(0), 10);
    auto x1_vec = linspace(lb.at(1), ub.at(1), 10);

    DenseVector x(2);
    for (auto x0 : x0_vec)
    {
        for (auto x1 : x1_vec)
        {
            x(0) = x0;
            x(1) = x1;

            double yb = bs.eval(x);
            double yb_orig = bs_orig.eval(x);
            if (std::abs(yb-yb_orig) > 1e-8)
            {
                cout << yb << endl;
                cout << yb_orig << endl;
                return false;
            }
        }
    }

    return true;
}

bool compareDataTables(DataTable &a, DataTable &b)
{
    if (a.getNumVariables() != b.getNumVariables())
        return false;

    auto ait = a.cbegin(), bit = b.cbegin();
    for (; ait != a.cend() && bit != b.cend(); ait++, bit++)
    {
        for (unsigned int i = 0; i < a.getNumVariables(); i++)
        {
//            std::cout << std::setprecision(SAVE_DOUBLE_PRECISION) << ait->getX().at(i) << " == " << std::setprecision(SAVE_DOUBLE_PRECISION) << bit->getX().at(i) << " ";
            if (!equalsWithinRange(ait->getX().at(i), bit->getX().at(i)))
                return false;
        }

//            std::cout << std::setprecision(SAVE_DOUBLE_PRECISION) << ait->getY().at(j) << " == " << std::setprecision(SAVE_DOUBLE_PRECISION) << bit->getY().at(j) << " ";
        if (!equalsWithinRange(ait->getY(), bit->getY()))
            return false;
//        std::cout << std::endl;
    }

//    std::cout << "Finished comparing samples..." << std::endl;

    return ait == a.cend() && bit == b.cend();
}

std::vector<double> linspace(double start, double stop, unsigned int num)
{
    std::vector<double> ret;
    double dx = 0;
    if (num > 1)
        dx = (stop - start)/(num-1);
    for (unsigned int i = 0; i < num; ++i)
        ret.push_back(start + i*dx);
    return ret;
}

double sixHumpCamelBack(DenseVector x)
{
    assert(x.rows() == 2);
    return (4 - 2.1*x(0)*x(0) + (1/3.)*x(0)*x(0)*x(0)*x(0))*x(0)*x(0) + x(0)*x(1) + (-4 + 4*x(1)*x(1))*x(1)*x(1);
}

double oneNorm(const DenseMatrix &m)
{
    double norm = 0.0;
    for(int i = 0; i < m.rows(); i++) {
        for(int j = 0; j < m.cols(); j++) {
            norm += std::abs(m(i, j));
        }
    }

    return norm;
}

double twoNorm(const DenseMatrix &m)
{
    double norm = 0.0;
    for(int i = 0; i < m.rows(); i++) {
        for(int j = 0; j < m.cols(); j++) {
            norm += std::pow(m(i, j), 2);
        }
    }

    norm = std::sqrt(norm);

    return norm;
}

double maxNorm(const DenseMatrix &m)
{
    double norm = 0.0;
    for(int i = 0; i < m.rows(); i++) {
        for(int j = 0; j < m.cols(); j++) {
            norm = std::max(std::abs(m(i, j)), norm);
        }
    }

    return norm;
}

double log(double base, double x)
{
    return std::log(x) / std::log(base);
}

} // namespace SPLINTER