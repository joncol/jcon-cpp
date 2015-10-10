#include <common/erase_helpers.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/test/unit_test.hpp>

#include <list>
#include <map>
#include <string>
#include <vector>

using namespace stl_helpers;

struct erase_fixture {
    erase_fixture()
        : v{1, 3, 10, 5, 2, 7, 4}
        , l{1, 3, 10, 5, 2, 7, 4}
        , m{{1, "one"}, {3, "three"}, {10, "ten"}, {5, "five"}, {2, "two"}}
    {}

    std::vector<int> v;
    std::list<int> l;
    std::map<int, std::string> m;
};

BOOST_FIXTURE_TEST_SUITE( erase_suite, erase_fixture )

BOOST_AUTO_TEST_CASE( erase_value_from_vector )
{
    erase(v, 5);
    std::vector<int> expected{1, 3, 10, 2, 7, 4};
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(),
                                  expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( erase_values_from_vector_using_predicate )
{
    erase_if(v, [](int i) { return i % 2 == 1; });
    std::vector<int> expected{10, 2, 4};
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(),
                                  expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( erase_value_from_list )
{
    erase(l, 5);
    std::list<int> expected{1, 3, 10, 2, 7, 4};
    BOOST_CHECK_EQUAL_COLLECTIONS(l.begin(), l.end(),
                                  expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( erase_values_from_list_using_predicate )
{
    erase_if(l, [](int i) { return i % 2 == 1; });
    std::list<int> expected{10, 2, 4};
    BOOST_CHECK_EQUAL_COLLECTIONS(l.begin(), l.end(),
                                  expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( erase_key_from_map )
{
    erase(m, 5);

    std::vector<int> keys;
    boost::push_back(keys, m | boost::adaptors::map_keys);
    std::vector<int> expected_keys{1, 2, 3, 10};
    BOOST_CHECK_EQUAL_COLLECTIONS(keys.begin(), keys.end(),
                                  expected_keys.begin(), expected_keys.end());

    std::vector<std::string> values;
    boost::push_back(values, m | boost::adaptors::map_values);
    std::vector<std::string> expected_values{"one", "two", "three", "ten"};
    BOOST_CHECK_EQUAL_COLLECTIONS(values.begin(), values.end(),
                                  expected_values.begin(), expected_values.end());
}

BOOST_AUTO_TEST_CASE( erase_elements_from_map_using_predicate )
{
    erase_if(m, [](std::pair<int, std::string> const& element) {
        return element.first % 2 == 1;
    });

    std::vector<int> keys;
    boost::push_back(keys, m | boost::adaptors::map_keys);
    std::vector<int> expected_keys{2, 10};
    BOOST_CHECK_EQUAL_COLLECTIONS(keys.begin(), keys.end(),
                                  expected_keys.begin(), expected_keys.end());

    std::vector<std::string> values;
    boost::push_back(values, m | boost::adaptors::map_values);
    std::vector<std::string> expected_values{"two", "ten"};
    BOOST_CHECK_EQUAL_COLLECTIONS(values.begin(), values.end(),
                                  expected_values.begin(), expected_values.end());
}

BOOST_AUTO_TEST_SUITE_END()
