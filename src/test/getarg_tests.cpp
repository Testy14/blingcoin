#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-blc");
    BOOST_CHECK(GetBoolArg("-blc"));
    BOOST_CHECK(GetBoolArg("-blc", false));
    BOOST_CHECK(GetBoolArg("-blc", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-blco"));
    BOOST_CHECK(!GetBoolArg("-blco", false));
    BOOST_CHECK(GetBoolArg("-blco", true));

    ResetArgs("-blc=0");
    BOOST_CHECK(!GetBoolArg("-blc"));
    BOOST_CHECK(!GetBoolArg("-blc", false));
    BOOST_CHECK(!GetBoolArg("-blc", true));

    ResetArgs("-blc=1");
    BOOST_CHECK(GetBoolArg("-blc"));
    BOOST_CHECK(GetBoolArg("-blc", false));
    BOOST_CHECK(GetBoolArg("-blc", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noblc");
    BOOST_CHECK(!GetBoolArg("-blc"));
    BOOST_CHECK(!GetBoolArg("-blc", false));
    BOOST_CHECK(!GetBoolArg("-blc", true));

    ResetArgs("-noblc=1");
    BOOST_CHECK(!GetBoolArg("-blc"));
    BOOST_CHECK(!GetBoolArg("-blc", false));
    BOOST_CHECK(!GetBoolArg("-blc", true));

    ResetArgs("-blc -noblc");  // -blc should win
    BOOST_CHECK(GetBoolArg("-blc"));
    BOOST_CHECK(GetBoolArg("-blc", false));
    BOOST_CHECK(GetBoolArg("-blc", true));

    ResetArgs("-blc=1 -noblc=1");  // -blc should win
    BOOST_CHECK(GetBoolArg("-blc"));
    BOOST_CHECK(GetBoolArg("-blc", false));
    BOOST_CHECK(GetBoolArg("-blc", true));

    ResetArgs("-blc=0 -noblc=0");  // -blc should win
    BOOST_CHECK(!GetBoolArg("-blc"));
    BOOST_CHECK(!GetBoolArg("-blc", false));
    BOOST_CHECK(!GetBoolArg("-blc", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--blc=1");
    BOOST_CHECK(GetBoolArg("-blc"));
    BOOST_CHECK(GetBoolArg("-blc", false));
    BOOST_CHECK(GetBoolArg("-blc", true));

    ResetArgs("--noblc=1");
    BOOST_CHECK(!GetBoolArg("-blc"));
    BOOST_CHECK(!GetBoolArg("-blc", false));
    BOOST_CHECK(!GetBoolArg("-blc", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-blc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-blc", "eleven"), "eleven");

    ResetArgs("-blc -bar");
    BOOST_CHECK_EQUAL(GetArg("-blc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-blc", "eleven"), "");

    ResetArgs("-blc=");
    BOOST_CHECK_EQUAL(GetArg("-blc", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-blc", "eleven"), "");

    ResetArgs("-blc=11");
    BOOST_CHECK_EQUAL(GetArg("-blc", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-blc", "eleven"), "11");

    ResetArgs("-blc=eleven");
    BOOST_CHECK_EQUAL(GetArg("-blc", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-blc", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-blc", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-blc", 0), 0);

    ResetArgs("-blc -bar");
    BOOST_CHECK_EQUAL(GetArg("-blc", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-blc=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-blc", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-blc=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-blc", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--blc");
    BOOST_CHECK_EQUAL(GetBoolArg("-blc"), true);

    ResetArgs("--blc=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-blc", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noblc");
    BOOST_CHECK(!GetBoolArg("-blc"));
    BOOST_CHECK(!GetBoolArg("-blc", true));
    BOOST_CHECK(!GetBoolArg("-blc", false));

    ResetArgs("-noblc=1");
    BOOST_CHECK(!GetBoolArg("-blc"));
    BOOST_CHECK(!GetBoolArg("-blc", true));
    BOOST_CHECK(!GetBoolArg("-blc", false));

    ResetArgs("-noblc=0");
    BOOST_CHECK(GetBoolArg("-blc"));
    BOOST_CHECK(GetBoolArg("-blc", true));
    BOOST_CHECK(GetBoolArg("-blc", false));

    ResetArgs("-blc --noblc");
    BOOST_CHECK(GetBoolArg("-blc"));

    ResetArgs("-noblc -blc"); // blc always wins:
    BOOST_CHECK(GetBoolArg("-blc"));
}

BOOST_AUTO_TEST_SUITE_END()
