#include "Runner.h"

/**
 * Source file for the PreProcessor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new PreProcessor with the given settings. 
 * PreProcessor will pick out the settings it will use, so just give it all of them.
 */
PreProcessor::PreProcessor(ConfigurationSettingsList settings, bool fullPreprocessor, bool debug) {
    this->isFullPreprocessor = fullPreprocessor;
    this->debugging = debug;

    try {
        this->threshold = std::stod(settings.GetSetting("thresholdValue"));
        this->threshValue = std::stod(settings.GetSetting("threshMaxValue"));
        this->threshtype = std::stoi(settings.GetSetting("threshType"));

        this->dilate = std::stoi(settings.GetSetting("dilation"));

        int colorH = std::stoi(settings.GetSetting("colorH"));
        int colorS = std::stoi(settings.GetSetting("colorS"));
        int colorV = std::stoi(settings.GetSetting("colorV"));
        int colorHError = std::stoi(settings.GetSetting("colorH_error"));
        int colorSError = std::stoi(settings.GetSetting("colorS_error"));
        int colorVError = std::stoi(settings.GetSetting("colorV_error"));

        this->targetColor = Color(colorH, colorS, colorV, colorHError, colorSError, colorVError);
    } catch(std::invalid_argument ex) {
        std::cout << "WARNING: FORMATTING ERROR FOR PREPROCESSOR CONFIGURATION; REVERTING TO PARTIAL PREPROCESSOR" << std::endl;
        this->isFullPreprocessor = false;
    }
}


PreProcessor::PreProcessor(bool fullPreprocessor, Color targetColor, int threshold, int dilation, bool debug) {
    this->isFullPreprocessor = fullPreprocessor;
    this->debugging = debug;
    this->targetColor = targetColor;
    this->threshold = threshold;
    this->threshValue = 255;
    this->threshtype = 0;
    this->dilate = dilation;
}


/**
 * Takes the given generic image and makes it usable for the PostProcessor.
 */
cv::Mat PreProcessor::ProcessImage(cv::Mat img) {
    cv::Mat out;

    if(this->isFullPreprocessor) {
        cv::threshold(img, img, this->threshold, this->threshValue, this->threshtype);
        cv::Mat dilateKernel = cv::getStructuringElement(cv::MORPH_RECT, 
                                                cv::Size(this->dilate,this->dilate));
                                                
        cv::Mat erodeKernel = cv::getStructuringElement(cv::MORPH_RECT,
                                                        cv::Size(3,3));
        
        cv::erode(img, img, erodeKernel);
        cv::dilate(img, img, dilateKernel);
        cv::cvtColor(img, img, cv::COLOR_BGR2HSV);
        cv::inRange(img, this->targetColor.GetLowerBound(), this->targetColor.GetUpperBound(), out);
    } else {
        cv::cvtColor(img, out, cv::COLOR_BGR2GRAY);
    }

    return out;
}


void PreProcessor::SetProperty(PreProcessorProperty prop, double value) {
    switch(prop) {
        case PreProcessorProperty::IS_FULL:
            this->isFullPreprocessor = (value == 0 ? true : false);
            break;
        case PreProcessorProperty::THRESHOLD:
            this->threshold = value;
            break;
        case PreProcessorProperty::THRESH_VALUE:
            this->threshValue = value;
            break;
        case PreProcessorProperty::DILATION:
            this->dilate = value;
            break;
        case PreProcessorProperty::COLOR_HUE:
            {
            Color newColor = Color((int) value, this->targetColor.GetS(), this->targetColor.GetV(), this->targetColor.GetHError(), this->targetColor.GetSError(), this->targetColor.GetVError());
            this->targetColor = newColor;
            break;
            }
        case PreProcessorProperty::COLOR_SATURATION:
            {
            Color newColor = Color(this->targetColor.GetH(), (int) value, this->targetColor.GetV(), this->targetColor.GetHError(), this->targetColor. GetSError(), this->targetColor.GetVError());
            this->targetColor = newColor;
            break;
            }
        case PreProcessorProperty::COLOR_VALUE:
            {
            Color newColor = Color(this->targetColor.GetH(), this->targetColor.GetS(), (int) value, this->targetColor.GetHError(), this->targetColor.GetSError(), this->targetColor.GetVError());
            this->targetColor = newColor;
            break;
            }

        case PreProcessorProperty::COLOR_ERROR:
            {
            Color newColor = Color(this->targetColor.GetH(), this->targetColor.GetS(), this->targetColor.GetV(), value, value, value);
            this->targetColor = newColor;
            break;
            }
    }
}


double PreProcessor::GetProperty(PreProcessorProperty prop) {
    double finalValue = -1.0;

    switch(prop) {
        case PreProcessorProperty::IS_FULL:
            finalValue = (this->isFullPreprocessor ? 0 : 1);
            break;
        case PreProcessorProperty::THRESHOLD:
            finalValue = this->threshold;
            break;
        case PreProcessorProperty::THRESH_VALUE:
            finalValue = this->threshValue;
            break;
        case PreProcessorProperty::DILATION:
            finalValue = this->dilate;
            break;
        case PreProcessorProperty::COLOR_HUE:
            finalValue = this->targetColor.GetH();
            break;
        case PreProcessorProperty::COLOR_SATURATION:
            finalValue = this->targetColor.GetS();
            break;
        case PreProcessorProperty::COLOR_VALUE:
            finalValue = this->targetColor.GetV();
            break;
        case PreProcessorProperty::COLOR_ERROR:
            finalValue = this->targetColor.GetHError();
            break;
        default:
            std::cout << "PROPERTY NOT FOUND" << std::endl;
            break;
    }

    return finalValue;
}


std::string PreProcessor::toString() {
    std::string endline = "\n";
    std::string thisString = "";
    thisString += std::string("Debugging: ") + std::string((this->debugging ? "YES" : "NO")) + endline;

    if(this->isFullPreprocessor) {
                      std::string("Type: FULL\n") +
                      std::string("Threshold Value: ") + std::to_string(this->threshold) + endline +
                      std::string("Dilation: ") + std::to_string(this->dilate) + endline + 
                      std::string("Color H: ") + std::to_string(this->targetColor.GetH()) + endline + 
                      std::string("Color S: ") + std::to_string(this->targetColor.GetS()) + endline + 
                      std::string("Color V: ") + std::to_string(this->targetColor.GetV()) + endline +
                      std::string("Color Error: ") + std::to_string(this->targetColor.GetHError()) + endline;
    } else {
        thisString += std::string("Type: PARTIAL") + endline;
    }

    return thisString;
}
