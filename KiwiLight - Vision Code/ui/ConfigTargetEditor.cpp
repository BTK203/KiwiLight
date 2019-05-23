#include "UI.h"

/**
 * Source file for the ConfigTargetEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

static void learnTargetPressed() {
    Flags::RaiseFlag("StartLearner");

    ConfirmationDialog confirmLearn = ConfirmationDialog("Ensure that the target is focused in the camera's view so that a blue box is drawn around it.\nWhen this is done, press OK.");
    bool userWantsLearn = confirmLearn.ShowAndGetResponse();

    if(userWantsLearn) {
        std::cout << "learn" << std::endl;
        Flags::RaiseFlag("StopLearner");
    } else {
        std::cout << "no learn" << std::endl;
        Flags::RaiseFlag("StopLearner");
    }
}

/**
 * Creates a new ConfigTargetEditor editing the targets in the file at fileName
 */
ConfigTargetEditor::ConfigTargetEditor(std::string fileName, Runner runner) {
    XMLDocument document = XMLDocument(fileName);
    this->currentPreProcessor = PreProcessorType::FULL; //because the checkbox defaults to full
    this->runner = runner;
    this->panel = Panel(false, 0);

        
        XMLTag configurationTag = document.GetTagsByName("configuration")[0];

        //create editor for the preprocessor
        Panel preprocessorPanel = Panel(false, 0);

            XMLTag preprocessorTag = configurationTag.GetTagsByName("preprocessor")[0];
            
            Label preprocessorPanelHeader = Label("Preprocessor");
                preprocessorPanelHeader.SetName("subHeader");
                preprocessorPanel.Pack_start(preprocessorPanelHeader.GetWidget(), false, false, 0);

            //chooser for whether or not the preprocessor is full or partial
            Panel preprocessorTypeChooser = Panel(true, 5);
                Label preprocessorTypeHeader = Label("Type:");
                    preprocessorTypeChooser.Pack_start(preprocessorTypeHeader.GetWidget(), false, false, 0);

                if(preprocessorTag.GetAttributesByName("type")[0].Value() == "full") {
                    this->currentPreProcessor = PreProcessorType::FULL;
                } else {
                    this->currentPreProcessor = PreProcessorType::PARTIAL;
                }

                this->fullPreProcessor = CheckBox("Full", false);
                    preprocessorTypeChooser.Pack_start(this->fullPreProcessor.GetWidget(), false, false, 0);

                this->partialPreProcessor = CheckBox("Partial", false);
                    preprocessorTypeChooser.Pack_start(this->partialPreProcessor.GetWidget(), false, false, 0);

                preprocessorPanel.Pack_start(preprocessorTypeChooser.GetWidget(), false, false, 0);
            
            Label colorHeader = Label("Target Color(HSV):");
                preprocessorPanel.Pack_start(colorHeader.GetWidget(), false, false, 0);

            Panel preprocessorColorChooser = Panel(true, 3);

                //target color tag where the hsv is stored
                XMLTag targetColor = preprocessorTag.GetTagsByName("targetColor")[0];

                //create the H, S, and V and asisgn them the values from the file.
                this->colorH = NumberBox(0.0, 100.0, 0.0);
                    int realColorH = std::stoi(targetColor.GetTagsByName("h")[0].Content());
                    this->colorH.SetValue((double) realColorH);
                    preprocessorColorChooser.Pack_start(this->colorH.GetWidget(), false, false, 0);

                this->colorS = NumberBox(0.0, 255.0, 0.0);
                    int realColorS = std::stoi(targetColor.GetTagsByName("s")[0].Content());
                    this->colorS.SetValue((double) realColorS);
                    preprocessorColorChooser.Pack_start(this->colorS.GetWidget(), false, false, 0);

                this->colorV = NumberBox(0.0, 255.0, 0.0);
                    int realColorV = std::stoi(targetColor.GetTagsByName("v")[0].Content());
                    this->colorV.SetValue((double) realColorV);
                    preprocessorColorChooser.Pack_start(this->colorV.GetWidget(), false, false, 0);

                preprocessorPanel.Pack_start(preprocessorColorChooser.GetWidget(), false, false, 0);

            //now create threshold and dilation sliders and set the values to the ones from the file
            this->thresholdValue = LabeledSlider("Threshold Value:", 0.0, 255.0, 1.0, 0.0);
                int realThreshold = std::stoi(preprocessorTag.GetTagsByName("targetThreshold")[0].GetTagsByName("threshold")[0].Content());
                this->thresholdValue.SetValue((double) realThreshold);
                preprocessorPanel.Pack_start(this->thresholdValue.GetWidget(), false, false, 0);

            this->dilationFactor = LabeledSlider("Dilation:", 1.0, 50.0, 0.5, 0.0);
                double realDilation = std::stod(preprocessorTag.GetTagsByName("dilation")[0].Content());
                this->dilationFactor.SetValue(realDilation);
                preprocessorPanel.Pack_start(this->dilationFactor.GetWidget(), false, false, 0);

            this->panel.Pack_start(preprocessorPanel.GetWidget(), false, false, 0);

            Separator sep = Separator(true);
                this->panel.Pack_start(sep.GetWidget(), false, false, 5);

            Panel postprocessorPanel = Panel(false, 3);
                Label postprocessorPanelHeader = Label("Postprocessor");
                    postprocessorPanel.Pack_start(postprocessorPanelHeader.GetWidget(), false, false, 0);

                Label contourHeader = Label("Contours");
                    contourHeader.SetName("subHeader");
                    postprocessorPanel.Pack_start(contourHeader.GetWidget(), false, false, 0);

                //the panel where the user can chooser which contour they are editing
                Panel postprocessorContourChooser = Panel(true, 3);
                    Label postprocessorChooserHeader = Label("Contour:");
                        postprocessorChooserHeader.SetName("gray");
                        postprocessorContourChooser.Pack_start(postprocessorChooserHeader.GetWidget(), false ,false, 0);

                    this->ContourID = NumberBox(0.0, 8.0, 0.0);
                        postprocessorContourChooser.Pack_start(this->ContourID.GetWidget(), false ,false, 0);

                    postprocessorPanel.Pack_start(postprocessorContourChooser.GetWidget(), false, false, 0);

                this->panel.Pack_start(postprocessorPanel.GetWidget(), true, true, 0);

                XMLTag postprocessorTag = configurationTag.GetTagsByName("postprocessor")[0];
                XMLTag firstTarget;
                XMLTag firstContour;
                std::vector<XMLTag> potentialTargets = postprocessorTag.GetTagsByName("target");

                for(int i=0; i<potentialTargets.size(); i++) {
                    if(potentialTargets[i].GetAttributesByName("id")[0].Value() == "0") {
                        //the target at i is the default target
                        firstTarget = potentialTargets[i];
                        break;
                    }
                }

                std::vector<XMLTag> potentialContours = firstTarget.GetTagsByName("contour");
                for(int i=0; i<potentialContours.size(); i++) {
                    if(potentialContours[i].GetAttributesByName("id")[0].Value() == "0") {
                        firstContour = potentialContours[i];
                        break;
                    }
                }
                
                Panel distXPanel = Panel(true, 3);
                    Label xLabel = Label("X Distance: ");
                        distXPanel.Pack_start(xLabel.GetWidget(), false, false, 0);

                    this->ContourDistX = NumberBox(0.0, 100.0, 0.0);
                        distXPanel.Pack_start(this->ContourDistX.GetWidget(), false, false, 0);

                    this->ContourDistXErr = LabeledSlider("Error", 0.0, 100.0, 0.05, 5);
                        distXPanel.Pack_start(this->ContourDistXErr.GetWidget(), true, true, 0);

                    this->panel.Pack_start(distXPanel.GetWidget(), true, true, 0);

                Panel distYPanel = Panel(true, 3);
                    Label yLabel = Label("Y Distance: ");
                        distYPanel.Pack_start(yLabel.GetWidget(), false, false, 0);

                    this->ContourDistY = NumberBox(0.0, 100.0, 0.0);
                        distYPanel.Pack_start(this->ContourDistY.GetWidget(), false, false, 0);

                    this->ContourDistYErr = LabeledSlider("Error", 0.0, 100.0, 0.05, 5);
                        distYPanel.Pack_start(this->ContourDistYErr.GetWidget(), true ,true, 0);

                    this->panel.Pack_start(distYPanel.GetWidget(), true, true, 0);

                //angle editor
                Panel anglePanel = Panel(true, 0);
                    this->ContourAngle = LabeledSlider("Angle", -360.0, 360.0, 0.05, 0);
                        anglePanel.Pack_start(this->ContourAngle.GetWidget(), true, true, 0);

                    this->ContourAngleErr = LabeledSlider("Angle Error", 0.0, 50.0, 0.05, 5.0);
                        anglePanel.Pack_start(this->ContourAngleErr.GetWidget(), true, true, 0);

                    this->panel.Pack_start(anglePanel.GetWidget(), true, true, 0);
                
                //solidity editor
                Panel solidPanel = Panel(true, 0);
                    this->ContourSolidity = LabeledSlider("Solidity", 0.0, 1.0, 0.01, 0.9);
                        solidPanel.Pack_start(this->ContourSolidity.GetWidget(), true, true, 0);

                    this->ContourSolidityErr = LabeledSlider("Solidity Error", 0.0, 1.0, 0.01, 0.3);
                        double realSolidErr = std::stod(firstContour.GetTagsByName("solidity")[0].GetAttributesByName("error")[0].Value());
                        this->ContourSolidityErr.SetValue(realSolidErr);
                        solidPanel.Pack_start(this->ContourSolidityErr.GetWidget(), true, true, 0);

                    this->panel.Pack_start(solidPanel.GetWidget(), true, true, 0);

                //AR editor
                Panel ARPanel = Panel(true, 0);
                    this->ContourAR = LabeledSlider("Aspect Ratio", 0.1, 10.0, 0.01, 1.0);
                        ARPanel.Pack_start(this->ContourAR.GetWidget(), true, true, 0);
                    
                    this->ContourARErr = LabeledSlider("Aspect Ratio Error", 0.0, 10.0, 0.01, 0.2);
                        ARPanel.Pack_start(this->ContourARErr.GetWidget(), true, true, 0);

                    this->panel.Pack_start(ARPanel.GetWidget(), true, true, 0);

                //minimum area editor
                this->ContourMinArea = LabeledSlider("Minimum Area:", 5, 10000, 5, 350);
                    this->panel.Pack_start(this->ContourMinArea.GetWidget(), false, false, 0);

                this->learnTarget = Button("Learn Target", learnTargetPressed);
                    this->panel.Pack_start(this->learnTarget.GetWidget(), false, false, 0);

    //set the sliders to the values for contour 0
    this->SetValues(0);
    this->configtargeteditor = this->panel.GetWidget();
}

/**
 * Updates the ConfigTargetEditor's values and the output image
 */
void ConfigTargetEditor::Update() {
    // //check for invalid selections in preprocessor area (i.e both are checked, none are checked)
    if(this->fullPreProcessor.GetState() && this->partialPreProcessor.GetState()) {
        //if both are checked, switch to the unselected option
        if(this->currentPreProcessor == PreProcessorType::FULL) {
            this->fullPreProcessor.SetState(false);
            this->currentPreProcessor = PreProcessorType::PARTIAL;
        } else {
            this->partialPreProcessor.SetState(false);
            this->currentPreProcessor = PreProcessorType::FULL;
        }
    }

    if(!this->fullPreProcessor.GetState() && !this->partialPreProcessor.GetState()) {
        if(this->currentPreProcessor == PreProcessorType::FULL) {
            this->fullPreProcessor.SetState(true);
        } else {
            this->partialPreProcessor.SetState(true);
        }
    }

    int currentRequestedContour = this->ContourID.GetValue();
    if(currentRequestedContour != this->lastRequestedContour) {
        //the requested contour has changed in last frame, update values
        SetValues(currentRequestedContour);
        this->lastRequestedContour = currentRequestedContour;
    }
}

/**
 * Returns an XMLTag describing the target being edited by this editor.
 */
XMLTag ConfigTargetEditor::GetTarget() {

}


double ConfigTargetEditor::GetPropertyValue(int contour, TargetProperty property) {
    int currentContour = this->lastRequestedContour;
    SetValues(contour);

    double finalValue = -1.0;
    //find out which property is requested and then return its value
    switch(property) {
        case TargetProperty::DIST_X:
            finalValue = this->ContourDistX.GetValue();
            break;
        case TargetProperty::DIST_Y:
            finalValue = this->ContourDistY.GetValue();
            break;
        case TargetProperty::ANGLE:
            finalValue = this->ContourAngle.GetValue();
            break;
        case TargetProperty::SOLIDITY:
            finalValue = this->ContourSolidity.GetValue();
            break;
        case TargetProperty::MINIMUM_AREA:
            finalValue = this->ContourMinArea.GetValue();
            break;
    }

    SetValues(currentContour);
    return finalValue;
}


void ConfigTargetEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configtargeteditor, name.c_str());
}


/**
 * Sets the slider values for contour with the passed id.
 */
void ConfigTargetEditor::SetValues(int contourID) {
    ExampleContour contourToDisplay = this->runner.GetExampleTargetByID(0).GetExampleContourByID(contourID);

    //set contour dist X values
    this->ContourDistX.SetValue(contourToDisplay.DistX().Value());
    this->ContourDistXErr.SetValue(contourToDisplay.DistX().Error());
    
    //set contour dist Y values
    this->ContourDistY.SetValue(contourToDisplay.DistY().Value());
    this->ContourDistYErr.SetValue(contourToDisplay.DistY().Error());

    //set contour angle values
    this->ContourAngle.SetValue(contourToDisplay.Angle().Value());
    this->ContourAngleErr.SetValue(contourToDisplay.Angle().Error());

    //set contour solidity values
    this->ContourSolidity.SetValue(contourToDisplay.Solidity().Value());
    this->ContourSolidityErr.SetValue(contourToDisplay.Solidity().Error());

    //set contour aspect ratio values
    this->ContourAR.SetValue(contourToDisplay.AspectRatio().Value());
    this->ContourARErr.SetValue(contourToDisplay.AspectRatio().Error());

    //set contour minimum area values
    this->ContourMinArea.SetValue(contourToDisplay.MinimumArea());
}