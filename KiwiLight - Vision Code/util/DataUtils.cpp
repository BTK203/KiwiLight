#include "Util.h"

/**
 * Source file for the DataUtils class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Sorts "data" in order from least to greatest. Sorts using insertion sort.
 */
std::vector<double> DataUtils::SortLeastGreatestDouble(std::vector<double> data) {
    std::vector<double> result;

    for(int i=0; i<data.size(); i++) {
        double valueToInsert = data[i];

        // if there is nothing in result or valueToInsert is less than the smallest value...
        if(result.size() == 0 || result[0] >= valueToInsert) {
            result.emplace(result.begin(), valueToInsert);
        } else {
            //look for a position to insert the value in
            bool placeToInsertFound = false;
            for(int k=1; k<result.size(); k++) {
                double valueBeforeI = result[k - 1];
                double valueAtI = result[k];

                if(valueToInsert > valueBeforeI && valueToInsert <= valueAtI) {
                    result.emplace(result.begin() + k, valueToInsert);
                    placeToInsertFound = true;
                    break;
                }
            }

            //value is greater than everything in the list
            if(!placeToInsertFound) {
                result.emplace(result.end(), valueToInsert);
            }
        }
    }

    return result;
}

/**
 * Returns the greatest value of the set after outliers are removed.
 */
double DataUtils::MaxWithoutOutliers(std::vector<double> data, double allowableDeviation) {
    std::vector<double> noOutliers = DataUtils::RemoveOutliers(data, allowableDeviation);
    return DataUtils::Greatest(noOutliers);
}

/**
 * Returns the sum of all numbers in the set.
 */
double DataUtils::Total(std::vector<double> data) {
    double total = 0.0;

    for(int i=0; i<data.size(); i++) {
        total += data[i];
    }

    return total;
}

/**
 * Returns the average of all numbers in the set.
 */
double DataUtils::Average(std::vector<double> data) {
    double avg = 0.0;

    for(int i=0; i<data.size(); i++) {
        avg += data[i];
    }

    avg /= data.size();
    return avg;
}

/**
 * Returns the median of all numbers in the set.
 */
double DataUtils::Median(std::vector<double> data) {
    int middle = data.size() / 2;
    return data[middle];
}

/**
 * Returns the greatest value in the set.
 */
double DataUtils::Greatest(std::vector<double> data) {
    double greatest = -1000000000.0;

    for(int i=0; i<data.size(); i++) {
        double value = data[i];
        if(value > greatest) {
            greatest = value;
        }
    }

    return greatest;
}

/**
 * Returns the smallest value in the set.
 */
double DataUtils::Least(std::vector<double> data) {
    double least = 1000000000.0;

    for(int i=0; i<data.size(); i++) {
        double value = data[i];
        if(value < least) {
            least = value;
        }
    }

    return least;
}

/**
 * Returns the average difference of all numbers in the set.
 */
double DataUtils::AverageDifference(std::vector<double> data) {
    double avgDiff = 0.0;
    int totalDifferences = 0;

    for(int i=0; i<data.size() - 1; i++) {
        double val1 = data[i];
        double val2 = data[i + 1];
        double difference = abs(val1 - val2);
        avgDiff += difference;

        totalDifferences++;
    }

    avgDiff /= (double) totalDifferences;
    return avgDiff;
}

/**
 * Returns the number of times "value" occurs in the set.
 */
int DataUtils::NumberOfOccurrances(std::vector<double> data, double value) {
    int occurrances = 0;

    for(int i=0; i<data.size(); i++) {
        if(data[i] == value) {
            occurrances++;
        }
    }

    return occurrances;
}

/**
 * Returns the value with the most occurances in the set.
 */
double DataUtils::MostCommonValue(std::vector<double> data) {
    int indexWithMostOccurrances = 0;
    int numberOfOccurrances = -1;

    for(int i=0; i<data.size(); i++) {
        double value = data[i];
        int occurrances = DataUtils::NumberOfOccurrances(data, value);
        if(occurrances > numberOfOccurrances) {
            numberOfOccurrances = occurrances;
            indexWithMostOccurrances = i;
        }
    }

    return data[indexWithMostOccurrances];
}

/**
 * Returns whether or not the value at "indexOfValue" is an outlier in the set "data"
 */
bool DataUtils::IsOutlier(std::vector<double> data, int indexOfValue, double allowableError) {
    double average = DataUtils::Average(data);
    double upperBound = average + allowableError;
    double lowerBound = average - allowableError;

    return !(data[indexOfValue] > lowerBound && data[indexOfValue] < upperBound);
}

/**
 * Removes all outliers from the set.
 * @param data The data set.
 * @param allowableError The allowable devation from the median to not be considered an outlier.
 */
std::vector<double> DataUtils::RemoveOutliers(std::vector<double> data, double allowableError) {
    std::vector<double> newSet = std::vector<double>();
    std::vector<double> sorted = DataUtils::SortLeastGreatestDouble(data);
    double dataAvg = DataUtils::Median(data);

    for(int i=0; i<data.size(); i++) {
        if(abs(data[i] - dataAvg) <= allowableError) {
            newSet.push_back(data[i]);
        }
    }

    return newSet;
}

/**
 * Removes all occurances of "occurances" from data.
 */
std::vector<double> DataUtils::RemoveOccurances(std::vector<double> data, double occurance) {
    std::vector<double> newSet = std::vector<double>();

    for(int i=0; i<data.size(); i++) {
        if(data[i] != occurance) {
            newSet.push_back(data[i]);
        }
    }

    return newSet;
}

/**
 * Casts all doubles in "data" to int and returns the vector.
 */
std::vector<int> DataUtils::VectorDoubleToInt(std::vector<double> data) {
    std::vector<int> result;
    for(int i=0; i<data.size(); i++) {
        result.push_back((int) data[i]);
    }

    return result;
}

/**
 * Cast all ints in "data" to double.
 */
std::vector<double> DataUtils::VectorIntToDouble(std::vector<int> data) {
    std::vector<double> result;
    for(int i=0; i<data.size(); i++) {
        result.push_back((double) data[i]);
    }

    return result;
}

/**
 * Encodes data into a string.
 */
std::string DataUtils::VectorToString(std::vector<double> data) {
    std::string vectorString = "[";

    for(int i=0; i<data.size(); i++) {
        vectorString += " " + std::to_string(data[i]);

        if(i < data.size() - 1) {
            vectorString += ", ";
        }
    }

    vectorString += " ]";
    return vectorString;
}