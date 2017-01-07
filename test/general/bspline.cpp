/*
 * This file is part of the SPLINTER library.
 * Copyright (C) 2012 Bjarne Grimstad (bjarne.grimstad@gmail.com).
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <Catch.h>
#include <bspline_test_utils.h>
#include <utilities.h>

#include <iostream>

using namespace SPLINTER;

#define COMMON_TAGS "[general][bspline]"
#define COMMON_TEXT " subdivision test"

TEST_CASE("BSpline domain reduction" COMMON_TEXT, COMMON_TAGS "[subdivision]")
{
    REQUIRE(domainReductionTest1());
}

TEST_CASE("BSpline recursive subdivision" COMMON_TEXT, COMMON_TAGS "[subdivision]")
{
    // TODO: The current code for comparing BSplines require identical bounds which fails in this test.
    //REQUIRE(runRecursiveDomainReductionTest());
}

TEST_CASE("BSpline knot insertion" COMMON_TEXT, COMMON_TAGS "[knotinsertion]")
{
    REQUIRE(testKnotInsertion());
}

TEST_CASE("BSpline knot averages" COMMON_TEXT, COMMON_TAGS "[knotaverages]")
{
    // Build a tensor product B-spline with three input variables
    std::vector<std::vector<double>> kv1 = {linspace(-10, 10, 21),
                                            linspace(-10, 10, 21),
                                            linspace(-10, 10, 21)};

    std::vector<unsigned int> deg1 = {1, 2, 3};

    // Building a B-spline without specifying control points (just to get the number of basis functions)
    BSpline bs1 = BSpline(kv1, deg1);

    // Create vector of 1-D control points
    auto cp1_vec = linspace(0, 100, bs1.getNumBasisFunctions());
    std::vector<std::vector<double>> cp1;
    for (auto cpi : cp1_vec)
        cp1.push_back({cpi});

    // Building B-spline
    bs1 = BSpline(cp1, kv1, deg1);

    // Get control points and knot averages
    auto cp1_mat = bs1.getControlPoints();
    auto mu1_mat = bs1.getKnotAverages();

    // Build a B-spline with multidimensional control points P_i = (mu_i, cp_i), where {mu_i} are the knot averages
    DenseMatrix cp1_new = DenseMatrix::Zero(cp1_mat.rows(), mu1_mat.cols() + 1);
    cp1_new.block(0, 0, mu1_mat.rows(), mu1_mat.cols()) = mu1_mat;
    cp1_new.block(0, mu1_mat.cols(), cp1_mat.rows(), cp1_mat.cols()) = cp1_mat;

    BSpline bs1_new = BSpline(eigMatToStdVecVec(cp1_new), kv1, deg1);

    /*
     * The new B-spline should evaluate to (x, f(x)) for any x in the B-spline support
     * E.g. for x = (0, 0, 0) we expect y = (0, 0, 0, 50), where 50 is the old B-spline value at x = (0, 0, 0)
     */
    auto x = std::vector<double>({0, 0, 0});
    auto y = bs1_new.eval(x);

    for (size_t i = 0; i < x.size(); ++i)
        REQUIRE(assertNear(x.at(i), y.at(i)));

    REQUIRE(assertNear(50., y.back()));

    /*
     * Testing for many points in support
     */
    bool test = true;
    for (auto x0 : linspace(-6, 6, 100))
    for (auto x1 : linspace(-6, 6, 100))
    for (auto x2 : linspace(-6, 6, 100))
    {
        auto x = std::vector<double>({x0, x1, x2});
        auto y = bs1_new.eval(x);

        if (!assertNear(x0, y.at(0)))
        {
            test = false;
            break;
        }

        if (!assertNear(x1, y.at(1)))
        {
            test = false;
            break;
        }

        if (!assertNear(x2, y.at(2)))
        {
            test = false;
            break;
        }
    }

    REQUIRE(test);
}