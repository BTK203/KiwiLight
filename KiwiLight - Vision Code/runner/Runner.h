#ifndef KiwiLight_RUNNER_H
#define KiwiLight_RUNNER_H

#include "Settings.h"
#include "../util/Util.h"
#include "opencv2/opencv.hpp"

/**
 * The main header file for the KiwiLight Runner
 * Written By: Brach Knutson
 */

namespace KiwiLight {

    /**
     * Descriptor enumeration for the type of preprocessor being used
     */
    enum PreProcessorType {
        FULL,
        PARTIAL
    };

    enum RunnerProperty {
        IMAGE_WIDTH,
        IMAGE_HEIGHT,
        TRUE_WIDTH,
        PERCEIVED_WIDTH,
        CALIBRATED_DISTANCE,
        ERROR_CORRECTION
    };

    
    enum PreProcessorProperty {
        THRESHOLD,
        THRESH_VALUE,
        DILATION,
        COLOR_HUE,
        COLOR_SATURATION,
        COLOR_VALUE
    };

    /**
     * Describes the names of certain properties of a target.
     */
    enum TargetProperty {
        DIST_X,
        DIST_Y,
        ANGLE,
        ASPECT_RATIO,
        SOLIDITY,
        MINIMUM_AREA
    };


    /**
     * Represents a single setting that is used by either the PostProcessor or Preprocessor.
     */
    class ConfigurationSetting {
        public:
        ConfigurationSetting(std::string name, std::string value);
        std::string Name() { return this->name; };
        void SetValue(std::string value);
        std::string GetValue() { return this->value; };

        private:
        std::string name;
        std::string value;
    };

    /**
     * Represents a list of ConfigurationSetting's
     */
    class ConfigurationSettingsList {
        public:
        ConfigurationSettingsList();
        void AddSetting(std::string settingName, std::string value);
        void SetSetting(std::string settingName, std::string value);
        std::string GetSetting(std::string settingName);

        private:
        std::vector<ConfigurationSetting> settings;
    };


    class Contour {
        public:
        Contour(std::vector<cv::Point> points);
        int X()      { return this->x;      };
        int Y()      { return this->y;      };
        int Width()  { return this->width;  };
        int Height() { return this->height; };
        int Angle()  { return this->angle;  };
        int Area()   { return this->width * this->height;   };
        double AspectRatio() { return this->width / (double) this->height; };
        double Solidity()    { return this->solidity;    };

        private:
        std::vector<cv::Point> points;
        int x,
            y,
            width,
            height,
            angle;
        
        double solidity;
    };

    
    class ExampleContour {
        public:
        ExampleContour(int id,
                       SettingPair distX,
                       SettingPair distY,
                       SettingPair angle,
                       SettingPair aspectRatio,
                       SettingPair solidity,
                       int minimumArea);

        bool IsContour(Contour contour);
        int ID() { return this->id; };
        SettingPair DistX()       { return this->distX; };
        SettingPair DistY()       { return this->distY; };
        SettingPair Angle()       { return this->angle; };
        SettingPair AspectRatio() { return this->aspectRatio; };
        SettingPair Solidity()    { return this->solidity; };
        int MinimumArea()         { return this->minimumArea; };

        void SetDistX(SettingPair distX);
        void SetDistY(SettingPair distY);
        void SetAngle(SettingPair angle);
        void SetAspectRatio(SettingPair aspectRatio);
        void SetSolidity(SettingPair solidity);
        void SetMinimumArea(int minimumArea);

        private:
        int id;
        SettingPair distX,
                    distY,
                    angle,
                    aspectRatio,
                    solidity;

        int minimumArea;
    };

    /**
     * Represents a target found in an image
     */
    class Target {
        public:
        Target() {};
        Target(int id, std::vector<Contour> contours, double knownHeight, double focalHeight, double distErrorCorrect, double calibratedDistance);
        int ID() { return this->id; };
        double Distance();
        int Angle(int imageCenterX);
        int Angle(double distanceToTarget, int imageCenterX);
        cv::Point Center() { return cv::Point(this->x, this->y); };
        cv::Rect Bounds();

        private:
        std::vector<Contour> contours;

        int id,
            x,
            y,
            width,
            height;

        double knownHeight,
               focalHeight,
               distErrorCorrect,
               calibratedDistance;
    };


    class ExampleTarget {
        public:
        ExampleTarget(int id, std::vector<ExampleContour> contours, double knownHeight, double focalHeight, double distErrorCorrect, double calibratedDistance);
        std::vector<Target> GetTargets(std::vector<Contour> contours);
        bool isTarget(std::vector<Contour> contours);
        int ID() { return this->id; };
        std::vector<ExampleContour> Contours() { return this->contours; };
        ExampleContour GetExampleContourByID(int id);
        void SetContourProperty(int contour, TargetProperty prop, SettingPair values);
        SettingPair GetContourProperty(int contour, TargetProperty prop);

        private:
        bool ArrayMaxed(int arr[], int size, int max);
        bool ContainsDuplicates(int arr[], int size);
        int id;
        std::vector<ExampleContour> contours;

        double knownHeight,
               focalHeight,
               distErrorCorrect,
               calibratedDistance;
    };

    /**
     * A module which takes images and gets them ready for work by the PostProcessor.
     */
    class PreProcessor {
        public:
        PreProcessor() {};
        PreProcessor(ConfigurationSettingsList settings, bool FullPreprocessor, bool debug);
        void SetProperty(PreProcessorProperty prop, double value);
        double GetProperty(PreProcessorProperty prop);
        void SetIsFull(bool isFull);
        bool GetIsFull() { return this->isFullPreprocessor; };
        cv::Mat ProcessImage(cv::Mat img);

        private:
        bool isFullPreprocessor,
             debugging;

        //threshold values for contrasting image
        double threshold,
            threshValue;
        
        int threshtype;

        //dilation values for expanding, blurring, or dilating
        int dilate;
        
        //what the camera looks for
        Color targetColor;
    };

    /**
     * Takes preprocessed images and finds targets within them.
     */
    class PostProcessor {
        public:
        PostProcessor() {};
        PostProcessor(std::vector<ExampleTarget> targets);
        //x, y, angle, solidity, aspectratio, minarea
        void SetTargetContourProperty(int contour, TargetProperty prop, SettingPair values);
        SettingPair GetTargetContourProperty(int contour, TargetProperty prop);
        std::vector<Target> ProcessImage(cv::Mat img);

        private:
        std::vector<ExampleTarget> targets;
    };

    /**
     * utility that learns a seen target
     */
    class ConfigLearner {
        public:
        ConfigLearner() {};
        ConfigLearner(XMLTag preprocessor, VideoCapture stream);
        XMLTag LearnTarget();
        void Update(int minArea);
        void Stop();
        cv::Mat GetOutputImage() { return this->out; };

        private:
        VideoCapture stream;
        cv::Mat out;
        PreProcessor preprocessor;
        ConfigurationSettingsList configsettings;
    };

    /**
     * Handles everything vision from taking images to send coordinates to a RoboRIO(or other UDP destination)
     */
    class Runner {
        public:
        Runner() {};
        Runner(std::string filename, bool debugging);
        int GetCameraIndex() { return this->cameraIndex; };
        void SetImageResize(Size sz);
        void Loop();
        void Stop();
        void Start();
        std::string Iterate();
        std::string GetFileName() { return this->src; };
        cv::Mat GetOriginalImage() { return this->originalImage; };
        cv::Mat GetOutputImage() { return this->outputImage; };
        cv::VideoCapture GetVideoStream() { return this->cap; };
        ExampleTarget GetExampleTargetByID(int id);
        void SetPreprocessorProperty(PreProcessorProperty prop, double value);
        double GetPreprocessorProperty(PreProcessorProperty prop);
        void SetPostProcessorContourProperty(int contour, TargetProperty prop, SettingPair values);
        SettingPair GetPostProcessorContourProperty(int contour, TargetProperty prop);
        std::string GetSetting(std::string settingName);

        // void SetPreprocessorSetting();

        private:
        void parseDocument(XMLDocument doc);

        VideoCapture cap;
        PreProcessor preprocessor;
        PostProcessor postprocessor;
        Size constantResize;

        std::string src;
        int cameraIndex;

        cv::Mat outputImage,
                originalImage;
        ConfigurationSettingsList settings;
        std::vector<ExampleTarget> postProcessorTargets;
        bool stop,
             debug;

        int centerOffset;
    };
}

#endif