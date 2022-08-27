
/*
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Contributors:  Fabio Soldati, peakfinder.org
Flexible algorithm to parse strings containing various latitude/longitude formats.
This is a C++ implementation of the javascript version [coordinate-parser](https://github.com/otto-dev/coordinate-parser).
Credit: Originally developed for javascript by [otto-dev](https://github.com/otto-dev).
*/


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.h"

#include "../include/coordinateparser.hpp"


#define CHECK_PREC( aexpr, bexpr, prec ) INTERNAL_CATCH_TEST( std::abs(aexpr - bexpr) < prec, Catch::ResultDisposition::ContinueOnFailure, "CHECK" )


TEST_CASE("Testing CoordinateParser") {

    SECTION("test formats") {

        std::string formats[] = {
                "40.4183318, -74.6411133",
                "40.4183318° N 74.6411133° W",
                "40° 25´ 5.994\" N 74° 38´ 28.008\" W",
                "40° 25.0999’ , -74° 38.4668’",
                "N40°25’5.994, W74°38’28.008\"",
                "40°25’5.994\"N, 74°38’28.008\"W",
                "40 25 5.994, -74 38 28.008",
                "40.4183318 -74.6411133",
                "40.4183318°,-74.6411133°",
                "40-25.0999N 74-38.4668W",
                "145505994.48, -268708007.88",
                "40.4183318N74.6411133W",
                "4025.0999N7438.4668W",
                "40°25’5.994\"N, 74°38’28.008\"W",
                "402505.994N743828.008W",
                "N 40 25.0999    W 74 38.4668",
                "40:25:6N,74:38:28W",
                "40:25:5.994N 74:38:28.008W",
                "40°25’6\"N 74°38’28\"W",
                "40°25’6\" -74°38’28\"",
                "40d 25’ 6\" N 74d 38’ 28\" W",
                "40.4183318N 74.6411133W",
                "40° 25.0999, -74° 38.4668"
        };

        for (const auto& str: formats) {
            std::string error;
            double latitude, longitude = 0.0;
            CHECK(CoordinateParser::parse(str, latitude, longitude, error) == true);

            CHECK_PREC(latitude, 40.4183318, 0.001);
            CHECK_PREC(longitude, -74.6411133, 0.001);
        }
    }

    SECTION("test reversed") {

        std::string formats[] = {
                "-40.4183318, 74.6411133",
                "40.4183318° S 74.6411133° E",
                "40° 25´ 5.994\" S 74° 38´ 28.008\" E",
                "-40° 25.0999’ , 74° 38.4668’",
                "S40°25’5.994, E74°38’28.008\"",
                "40°25’5.994\"S, 74°38’28.008\"E",
                "-40 25 5.994, 74 38 28.008",
                "-40.4183318 74.6411133",
                "-40.4183318°,74.6411133°",
                "40-25.0999S 74-38.4668E",
                "-145505994.48, 268708007.88",
                "40.4183318S74.6411133E",
                "4025.0999S7438.4668E",
                "40°25’5.994\"S, 74°38’28.008\"E",
                "402505.994S743828.008E",
                "S 40 25.0999    E 74 38.4668",
                "40:25:6S,74:38:28E",
                "40:25:5.994S 74:38:28.008E",
                "40°25’6\"S 74°38’28\"E",
                "-40°25’6\" 74°38’28\"",
                "40d 25’ 6\" S 74d 38’ 28\" E",
                "40.4183318S 74.6411133E",
                "40.4183318S 74.6411133",
                "-40° 25.0999, 74° 38.4668"
        };

        for (const auto& str: formats) {

            // std::cout << str << std::endl;

            std::string error;
            double latitude, longitude = 0.0;
            CHECK(CoordinateParser::parse(str, latitude, longitude, error) == true);

            CHECK_PREC(latitude, -40.4183318, 0.001);
            CHECK_PREC(longitude, 74.6411133, 0.001);
        }
    }

    SECTION("test invalid") {

        std::string formats[] = {
                "blablabla",
                "5 Fantasy street 12",
                "-40.1X, 74",
                "-40.1 X, 74",
                "-40.1, 74X",
                "-40.1, 74 X",
                "1 2 3 4 5 6 7 8",
                "1 2 3 4 5 6 7",
                "1 2 3 4 5",
                "1 2 3 ",
                "1",
                "40.1° SS 60.1° EE",
                "40.1° E 60.1° S",
                "40.1° W 60.1° N",
                "40.1° W 60.1° W",
                "40.1° N 60.1° N",
                "-40.4183318, 12.345, 74.6411133",
        };

        std::string error;

        for (const auto& str: formats) {
            if (CoordinateParser::isValid(str, error) != false)
                std::cout << str << " -> " << error <<  std::endl;

            CHECK(CoordinateParser::isValid(str, error) == false);
        }
    }
}

