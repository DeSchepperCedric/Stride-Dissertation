
#include "geopop/io/HouseholdJSONReader.h"
#include "geopop/GeoGridConfig.h"

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;
using namespace geopop;
using namespace stride;

namespace {

TEST(HouseholdJSONReader, test1)
{
        string jsonString =
            R"({
	"households_name": "test_household",
	"households_list": [
		[42,38,15],
		[70,68],
		[40,39,9,6],
		[43,42],
		[55,54],
		[40,40,3,3],
		[35,32,6,3],
		[78,75]
	]
})";

        GeoGridConfig      geoConfig{};
        auto               instream = make_unique<istringstream>(jsonString);
        HouseholdJSONReader reader(move(instream));

        reader.SetReferenceHouseholds(geoConfig.refHH.person_count, geoConfig.refHH.ages);

        EXPECT_EQ(geoConfig.refHH.person_count, 23U);

        const vector<vector<unsigned int>>& HHages = geoConfig.refHH.ages;

        EXPECT_EQ(HHages.size(), 8U);
        EXPECT_EQ(HHages[0].size(), 3U);
        EXPECT_EQ(HHages[1].size(), 2U);
        EXPECT_EQ(HHages[2].size(), 4U);
        EXPECT_EQ(HHages[3].size(), 2U);
        EXPECT_EQ(HHages[4].size(), 2U);
        EXPECT_EQ(HHages[5].size(), 4U);
        EXPECT_EQ(HHages[6].size(), 4U);
        EXPECT_EQ(HHages[7].size(), 2U);

        EXPECT_EQ(HHages[0][0], 42U);
        EXPECT_EQ(HHages[0][1], 38U);
        EXPECT_EQ(HHages[0][2], 15U);

        EXPECT_EQ(HHages[1][0], 70U);
        EXPECT_EQ(HHages[1][1], 68U);

        EXPECT_EQ(HHages[6][0], 35U);
        EXPECT_EQ(HHages[6][1], 32U);
        EXPECT_EQ(HHages[6][2], 6U);
        EXPECT_EQ(HHages[6][3], 3U);

        EXPECT_EQ(HHages[7][0], 78U);
        EXPECT_EQ(HHages[7][1], 75U);
}

} // namespace
