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


/*
Usage:

#include "coordinateparser.hpp"

std::string error;
double latitude, longitude = 0;

// parse coordinate string
std::string str = "40.4183318° N 74.6411133° W"

bool valid = CoordinateParser::parse(str, latitude, longitude, error);

if (valid)
	std::cout << str << ": " << latitude <<  ",  " << longitude << std::endl;   // 40.123, -74.123 ✓
else
	std::cout << str << ": parsing failed, " << error << std::endl;
	
*/	

#pragma once

#include <string>
#include <vector>
#include <regex>
#include <cmath>

class CoordinateParser {

    class CoordinateNumber {
        int _sign;
        double _degrees;
        double _minutes;
        double _seconds;
        double _milliseconds;

        bool _degreescanbespecial;

        int normalizedSignOf(float number) const {
            if (number >= 0) {
                return 1;
            } else {
                return -1;
            }
        };

        bool degreesCanBeMilliseconds() const {
            return (_degrees > 909090);
        }

        bool degreesCanBeDegreesMinutesAndSeconds() const {
            return _degrees > 9090;
        }

        bool degreesCanBeDegreesAndMinutes() const {
            return (_degrees > 360);
        }

        void degreesAsMilliseconds() {
            _milliseconds = _degrees;
            _degrees = 0;
        }

        void degreesAsDegreesMinutesAndSeconds() {
            double newDegrees = std::floor(_degrees / 10000);
            _minutes = std::floor((_degrees - newDegrees * 10000) / 100);
            _seconds = std::floor(_degrees - newDegrees * 10000 - _minutes * 100);
            _degrees = newDegrees;
        }

        void degreesAsDegreesAndMinutes() {
            double newDegrees = std::floor(_degrees / 100);
            _minutes = _degrees - newDegrees * 100;
            _degrees = newDegrees;
        }

    public:

        CoordinateNumber(const std::vector<std::string> &coordinateNumbers) {
            if (coordinateNumbers.size() < 1) {
                throw std::runtime_error("coordinate number is too small");
            }

            _sign = normalizedSignOf(std::stod(coordinateNumbers[0]));

            _degrees = std::abs(std::stod(coordinateNumbers[0]));
            _minutes = coordinateNumbers.size() > 1 ? std::abs(std::stod(coordinateNumbers[1])) : 0;
            _seconds = coordinateNumbers.size() > 2 ? std::abs(std::stod(coordinateNumbers[2])) : 0;
            _milliseconds = coordinateNumbers.size() > 3 ? std::abs(std::stod(coordinateNumbers[3])) : 0;

            _degreescanbespecial = coordinateNumbers.size() < 2;
        }

        static std::vector<std::string> extractCoordinateNumbers(const std::string &coordinates) {

            std::regex str_expr("-?\\d+(\\.\\d+)?");

            std::smatch sm;
            std::string::const_iterator text_iter = coordinates.cbegin();

            std::vector<std::string> result;

            // loop through all occurences
            while (std::regex_search(text_iter, coordinates.end(), sm, str_expr)) {
                result.push_back(sm[0]);
                text_iter = sm[0].second;
            }
            return result;
        }

        void detectSpecialFormats() {
            if (_degreescanbespecial) {
                if (degreesCanBeMilliseconds()) {
                    degreesAsMilliseconds();
                } else if (degreesCanBeDegreesMinutesAndSeconds()) {
                    degreesAsDegreesMinutesAndSeconds();
                } else if (degreesCanBeDegreesAndMinutes()) {
                    degreesAsDegreesAndMinutes();
                }
            }
        }

        double toDecimal() const {
            return _sign * (_degrees + _minutes / 60 + _seconds / 3600 + _milliseconds / 3600000);
        }
    };

    class CoordinateValidator {
    private:
        static bool checkContainsNoLetters(const std::string &coordinates, std::string &message) {
            std::regex str_expr("(?![neswd])[a-z]", std::regex_constants::icase);

            if (std::regex_search(coordinates, str_expr)) {
                message = "Coordinate contains invalid alphanumeric characters.";
                return false;
            }
            return true;
        }

        static bool checkValidOrientation(const std::string &coordinates, std::string &message) {
            std::regex str_expr("^[^nsew]*[ns]?[^nsew]*[ew]?[^nsew]*$", std::regex_constants::icase);

            if (!std::regex_match(coordinates, str_expr)) {
                message = "Invalid cardinal direction.";
                return false;
            }
            return true;
        }

        static bool checkNumbers(const std::string &coordinates, std::string &message) {
            std::vector<std::string> coordinateNumbers = CoordinateNumber::extractCoordinateNumbers(coordinates);

            if (!checkAnyCoordinateNumbers(coordinateNumbers, message))
                return false;
            if (!checkEvenCoordinateNumbers(coordinateNumbers, message))
                return false;
            if (!checkMaximumCoordinateNumbers(coordinateNumbers, message))
                return false;
            return true;
        }

        static bool checkAnyCoordinateNumbers(const std::vector<std::string> &coordinateNumbers, std::string &message) {
            if (coordinateNumbers.size() == 0) {
                message = "Could not find any coordinate number";
                return false;
            }
            return true;
        }

        static bool checkEvenCoordinateNumbers(const std::vector<std::string> &coordinateNumbers, std::string &message) {
            if (coordinateNumbers.size() % 2) {
                message = "Uneven count of latitude/longitude numbers";
                return false;
            }
            return true;
        }

        static bool checkMaximumCoordinateNumbers(const std::vector<std::string> &coordinateNumbers, std::string &message) {
            if (coordinateNumbers.size() > 6) {
                message = "Too many coordinate numbers";
                return false;
            }
            return true;
        }

    public:
        static bool isValid(const std::string &coordinates, std::string &message) {
            message = "";

            if (!checkContainsNoLetters(coordinates, message))
                return false;
            if (!checkValidOrientation(coordinates, message))
                return false;
            if (!checkNumbers(coordinates, message))
                return false;

            return true;
        }

    };


private:
    static double coordinateNumbersToDecimal(const std::vector<std::string> &coordinateNumbers) {
        CoordinateNumber coordinate(coordinateNumbers);
        coordinate.detectSpecialFormats();
        return coordinate.toDecimal();
    }

    static bool latitudeIsNegative(const std::string &coordinateString) {
        return (coordinateString.find("s") != std::string::npos) || (coordinateString.find("S") != std::string::npos);
    }

    static bool longitudeIsNegative(const std::string &coordinateString) {
        return (coordinateString.find("w") != std::string::npos) || (coordinateString.find("W") != std::string::npos);
    }

public:

    static bool isValid(const std::string &coordinateString, std::string &message) {
        return CoordinateValidator::isValid(coordinateString, message);
    }

    static bool parse(const std::string &coordinateString, double &latitude, double &longitude, std::string &message) {

        latitude = 0.0;
        longitude = 0.0;

        if (!CoordinateValidator::isValid(coordinateString, message))
            return false;

        std::vector<std::string> coordinateNumbers = CoordinateNumber::extractCoordinateNumbers(coordinateString);
        auto numberCountEachCoordinate = coordinateNumbers.size() / 2;

        // split into two vectory
        std::vector<std::string> latitudeNumbers(coordinateNumbers.begin(), coordinateNumbers.begin() + numberCountEachCoordinate);
        std::vector<std::string> longitudeNumbers(coordinateNumbers.begin() + numberCountEachCoordinate, coordinateNumbers.end());

        latitude = coordinateNumbersToDecimal(latitudeNumbers);
        if (latitudeIsNegative(coordinateString)) {
            latitude = latitude * -1;
        }

        longitude = coordinateNumbersToDecimal(longitudeNumbers);
        if (longitudeIsNegative(coordinateString)) {
            longitude = longitude * -1;
        }
        return true;
    }
};



