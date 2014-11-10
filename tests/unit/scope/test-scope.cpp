#include <scope/scope.h>

#include <core/posix/exec.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <unity/scopes/SearchReply.h>
#include <unity/scopes/SearchReplyProxyFwd.h>
#include <unity/scopes/Variant.h>
#include <unity/scopes/testing/Category.h>
#include <unity/scopes/testing/MockSearchReply.h>
#include <unity/scopes/testing/TypedScopeFixture.h>

using namespace std;
using namespace testing;
using namespace scope;

namespace posix = core::posix;
namespace sc = unity::scopes;
namespace sct = unity::scopes::testing;

/**
 * Keep the tests in an anonymous namespace
 */
namespace {

/**
 * Custom matcher to check the properties of search results
 */
MATCHER_P2(ResultProp, prop, value, "") {
    if (arg.contains(prop)) {
        *result_listener << "result[" << prop << "] is " << arg[prop].serialize_json();
    } else {
        *result_listener << "result[" << prop << "] is not set";
    }
    return arg.contains(prop) && arg[prop] == sc::Variant(value);
}

/**
 * Custom matcher to check the presence of departments
 */
MATCHER_P(IsDepartment, department, "") {
    return arg->serialize() == department->serialize();
}

typedef sct::TypedScopeFixture<Scope> TypedScopeFixtureScope;

class TestScope: public TypedScopeFixtureScope {
protected:
    void SetUp() override
    {
        // Start up Python-based fake OpenWeatherMap server
        fake_server_ = posix::exec("/usr/bin/python3", { FAKE_SERVER }, { },
                                   posix::StandardStream::stdout);

        // Check it's running
        ASSERT_GT(fake_server_.pid(), 0);
        string port;
        // The server will print out the random port it is using
        fake_server_.cout() >> port;
        // Check we have a port
        ASSERT_FALSE(port.empty());

        // Build up the API root
        string apiroot = "http://127.0.0.1:" + port;
        // Override the API root that the scope will use
        setenv("NETWORK_SCOPE_APIROOT", apiroot.c_str(), true);

        // Do the parent SetUp
        TypedScopeFixture::set_scope_directory(TEST_SCOPE_DIRECTORY);
        TypedScopeFixtureScope::SetUp();
    }

    /**
     * Start by assuming the server is invalid
     */
    posix::ChildProcess fake_server_ = posix::ChildProcess::invalid();
};

TEST_F(TestScope, empty_search_string) {
    const sc::CategoryRenderer renderer;
    NiceMock<sct::MockSearchReply> reply;

    // Build a query with an empty search string
    sc::CannedQuery query(SCOPE_NAME, "", "");

    // Expect the current weather category
    EXPECT_CALL(reply, register_category("current", "London, GB", "", _)).Times(1)
            .WillOnce(Return(make_shared<sct::Category>("current", "London, GB", "", renderer)));

    // With one result
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "21.8°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/02d.png"),
                                                                      ResultProp("subtitle", "few clouds")
                                                                      )))).WillOnce(
                Return(true));

    // Expect the forecast category
    EXPECT_CALL(reply, register_category("forecast", "7 day forecast", "", _)).Times(1)
            .WillOnce(Return(make_shared<sct::Category>("forecast", "7 day forecast", "", renderer)));

    // With seven results
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "25.1°C to 18.8°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "light rain")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "20.9°C to 15.5°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "moderate rain")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "19.6°C to 13.2°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/03d.png"),
                                                                      ResultProp("subtitle", "scattered clouds")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "18.1°C to 13.5°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "moderate rain")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "17.4°C to 15.4°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "heavy intensity rain")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "18.4°C to 16.2°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "moderate rain")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "19°C to 16.7°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "light rain")
                                                                      )))).WillOnce(Return(true));

    sc::SearchReplyProxy reply_proxy(&reply, [](sc::SearchReply*) {}); // note: this is a std::shared_ptr with empty deleter
    sc::SearchMetadata meta_data("en_EN", "phone");

    // Create a query object
    auto search_query = scope->search(query, meta_data);
    ASSERT_NE(nullptr, search_query);

    // Run the search
    search_query->run(reply_proxy);

    // Google Mock will make assertions when the mocks are destructed.
}

TEST_F(TestScope, search) {
    const sc::CategoryRenderer renderer;
    NiceMock<sct::MockSearchReply> reply;

    // Build a query with a non-empty search string
    sc::CannedQuery query(SCOPE_NAME, "Manchester,uk", "");

    // Expect the current weather category
    EXPECT_CALL(reply, register_category("current", "Manchester, GB", "", _)).Times(1)
            .WillOnce(Return(make_shared<sct::Category>("current", "Manchester, GB", "", renderer)));

    // With one result
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "17.4°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/03d.png"),
                                                                      ResultProp("subtitle", "scattered clouds")
                                                                      )))).WillOnce(
                Return(true));

    // Expect the forecast category
    EXPECT_CALL(reply, register_category("forecast", "7 day forecast", "", _)).Times(1)
            .WillOnce(Return(make_shared<sct::Category>("forecast", "7 day forecast", "", renderer)));

    // With seven results
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "18.8°C to 12°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/01d.png"),
                                                                      ResultProp("subtitle", "sky is clear")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "18.6°C to 12.3°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "moderate rain")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "17.1°C to 10.8°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/04d.png"),
                                                                      ResultProp("subtitle", "broken clouds")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "16°C to 12.2°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "moderate rain")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "15.7°C to 13°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "light rain")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "17.6°C to 14.1°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "moderate rain")
                                                                      )))).WillOnce(Return(true));
    EXPECT_CALL(reply, push(Matcher<sc::CategorisedResult const&>(AllOf(
                                                                      ResultProp("title", "15.5°C to 13.4°C"),
                                                                      ResultProp("art", "http://openweathermap.org/img/w/10d.png"),
                                                                      ResultProp("subtitle", "moderate rain")
                                                                      )))).WillOnce(Return(true));

    sc::SearchReplyProxy reply_proxy(&reply, [](sc::SearchReply*) {}); // note: this is a std::shared_ptr with empty deleter
    sc::SearchMetadata meta_data("en_EN", "phone");

    // Create a query object
    auto search_query = scope->search(query, meta_data);
    ASSERT_NE(nullptr, search_query);

    // Run the search
    search_query->run(reply_proxy);

    // Google Mock will make assertions when the mocks are destructed.
}

} // namespace

