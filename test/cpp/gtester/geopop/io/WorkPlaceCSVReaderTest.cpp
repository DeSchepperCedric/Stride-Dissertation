//
// Created by wannes on 31.03.19.
//

#include "geopop/io/WorkplaceCSVReader.h"
#include "geopop/GeoGridConfig.h"

#include "util/Exception.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::util;

using namespace std;

namespace {

TEST(workplaceCSVReaderTest, fileParsedCorrectly)
{
        string csvString =
            R"(ratio,size_min,size_max
        0.778532842256952,1,9
        0.171901116625764,10,49
        0.0410039025210945,50,199
        0.00856213859618965,200,400)";
        GeoGridConfig      geoConfig{};
        auto               instream = make_unique<istringstream>(csvString);
        WorkplaceCSVReader reader(move(instream));
        reader.SetWorkplaceSizeDistributions(geoConfig.refWP.ratios, geoConfig.refWP.min, geoConfig.refWP.max);

        std::vector<double> refRatios = {0.778532842256952, 0.171901116625764, 0.0410039025210945, 0.00856213859618965};
        std::vector<unsigned int> refMin = {1, 10, 50, 200};
        std::vector<unsigned int> refMax = {9, 49, 199, 400};

        for (unsigned int i = 0; i < 4; i++) {
                EXPECT_EQ(refRatios[i], geoConfig.refWP.ratios[i]);
                EXPECT_EQ(refMin[i], geoConfig.refWP.min[i]);
                EXPECT_EQ(refMax[i], geoConfig.refWP.max[i]);
        }
}

TEST(workplaceCSVReaderTest, invalidStreamCorrectExceptionTest)
{
        string csvString =
            R"(ratio,size_min,size_max
        0.778532842256952,1,
        0.171901116625764,10,49
        0.0410039025210945,50,199
        0.00856213859618965,200,400)";
        GeoGridConfig      geoConfig{};
        auto               instream = make_unique<istringstream>(csvString);
        WorkplaceCSVReader reader(move(instream));

        EXPECT_THROW(
            reader.SetWorkplaceSizeDistributions(geoConfig.refWP.ratios, geoConfig.refWP.min, geoConfig.refWP.max),
            Exception);
}

TEST(workplaceCSVReaderTest, invalidTypeCorrectExceptionTest)
{
        string csvString =
            R"(ratio,size_min,size_max
        0.778532842256952,1, 9.5
        0.171901116625764,10,49
        0.0410039025210945,50,199
        0.00856213859618965,200,400)";
        GeoGridConfig      geoConfig{};
        auto               instream = make_unique<istringstream>(csvString);
        WorkplaceCSVReader reader(move(instream));

        EXPECT_THROW(
            reader.SetWorkplaceSizeDistributions(geoConfig.refWP.ratios, geoConfig.refWP.min, geoConfig.refWP.max),
            Exception);
}

TEST(workplaceCSVReaderTest, emptyStreamHandledCorrectlyTest)
{

        GeoGridConfig      geoConfig{};
        auto               instream = make_unique<istringstream>("");
        WorkplaceCSVReader reader(move(instream));
        reader.SetWorkplaceSizeDistributions(geoConfig.refWP.ratios, geoConfig.refWP.min, geoConfig.refWP.max);
        EXPECT_TRUE(geoConfig.refWP.ratios.empty());
        EXPECT_TRUE(geoConfig.refWP.min.empty());
        EXPECT_TRUE(geoConfig.refWP.max.empty());
}

} // namespace
