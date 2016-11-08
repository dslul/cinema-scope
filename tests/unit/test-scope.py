#!/usr/bin/env python3

from scope_harness import (
    CategoryListMatcher, CategoryListMatcherMode, CategoryMatcher,
    Parameters, ResultMatcher, ScopeHarness
)
from scope_harness.testing import *
import unittest, sys, os
from subprocess import Popen, PIPE

class AppsTest (ScopeHarnessTestCase):
    @classmethod
    def setUpClass(cls):
        cls.process = Popen(["/usr/bin/python3", FAKE_SERVER], stdout=PIPE)
        port = cls.process.stdout.readline().decode("utf-8").rstrip('\n')
        os.environ["NETWORK_SCOPE_APIROOT"] = "http://127.0.0.1:" + port

    @classmethod
    def tearDownClass(cls):
        cls.process.terminate()


    def start_harness(self):
        self.harness = ScopeHarness.new_from_scope_list(Parameters([SCOPE_INI]))
        self.view = self.harness.results_view
        self.view.active_scope = SCOPE_NAME


    def test_surfacing_results(self):
        self.start_harness()
        self.view.search_query = ''

        match = CategoryListMatcher() \
            .has_exactly(2) \
            .mode(CategoryListMatcherMode.BY_ID) \
            .category(CategoryMatcher("current") \
                .title("London, GB") \
                .has_at_least(1) \
                .result(ResultMatcher("5091") \
                    .title("21.8°C") \
                    .art("http://openweathermap.org/img/w/02d.png") \
                    .subtitle("few clouds") ) ) \
            .category(CategoryMatcher("forecast") \
                .title("7 day forecast") \
                .has_at_least(7) \
                .result(ResultMatcher("500") \
                    .title("25.1°C to 18.8°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("light rain") ) \
                .result(ResultMatcher("501") \
                    .title("20.9°C to 15.5°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("moderate rain") ) \
                .result(ResultMatcher("802") \
                    .title("19.6°C to 13.2°C") \
                    .art("http://openweathermap.org/img/w/03d.png") \
                    .subtitle("scattered clouds") ) \
                .result(ResultMatcher("501") \
                    .title("18.1°C to 13.5°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("moderate rain") ) \
                .result(ResultMatcher("502") \
                    .title("17.4°C to 15.4°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("heavy intensity rain") ) \
                .result(ResultMatcher("501") \
                    .title("18.4°C to 16.2°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("moderate rain") ) \
                .result(ResultMatcher("500") \
                    .title("19°C to 16.7°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("light rain") ) ) \
            .match(self.view.categories)
        self.assertMatchResult(match)


    def test_search_results(self):
        self.start_harness()
        self.view.search_query = 'Manchester,uk'

        match = CategoryListMatcher() \
            .has_exactly(2) \
            .mode(CategoryListMatcherMode.BY_ID) \
            .category(CategoryMatcher("current") \
                .title("Manchester, GB") \
                .has_at_least(1) \
                .result(ResultMatcher("5060") \
                    .title("17.4°C") \
                    .art("http://openweathermap.org/img/w/03d.png") \
                    .subtitle("scattered clouds") ) ) \
            .category(CategoryMatcher("forecast") \
                .title("7 day forecast") \
                .has_at_least(7) \
                .result(ResultMatcher("800") \
                    .title("18.8°C to 12°C") \
                    .art("http://openweathermap.org/img/w/01d.png") \
                    .subtitle("sky is clear") ) \
                .result(ResultMatcher("501") \
                    .title("18.6°C to 12.3°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("moderate rain") ) \
                .result(ResultMatcher("803") \
                    .title("17.1°C to 10.8°C") \
                    .art("http://openweathermap.org/img/w/04d.png") \
                    .subtitle("broken clouds") ) \
                .result(ResultMatcher("501") \
                    .title("16°C to 12.2°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("moderate rain") ) \
                .result(ResultMatcher("500") \
                    .title("15.7°C to 13°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("light rain") ) \
                .result(ResultMatcher("501") \
                    .title("17.6°C to 14.1°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("moderate rain") ) \
                .result(ResultMatcher("501") \
                    .title("15.5°C to 13.4°C") \
                    .art("http://openweathermap.org/img/w/10d.png") \
                    .subtitle("moderate rain") ) ) \
            .match(self.view.categories)
        self.assertMatchResult(match)


if __name__ == '__main__':
    SCOPE_NAME = sys.argv[1]
    SCOPE_INI = sys.argv[2]
    FAKE_SERVER = sys.argv[3]

    unittest.main(argv = sys.argv[:1])

