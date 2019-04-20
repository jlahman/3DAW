/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/
#include "../src/interfacer.h"
#include <iostream>
#include <fstream>

#pragma once

//==============================================================================
class MainContentComponent   : public Component,
                               public Slider::Listener
{
public:
    //==============================================================================
    MainContentComponent()
    {
        addAndMakeVisible (azimuthSlider);
        azimuthSlider.setRange (-180, 180);
        azimuthSlider.setSliderStyle (Slider::Rotary);
        azimuthSlider.setTextValueSuffix (" Degrees");
        azimuthSlider.addListener (this);
    		azimuthSlider.setNumDecimalPlacesToDisplay(2);
        azimuthSlider.setRotaryParameters(0, MathConstants<float>::twoPi, false);

        addAndMakeVisible (azimuthLabel);
        azimuthLabel.setText ("Azimuth", dontSendNotification);
        azimuthLabel.attachToComponent (&azimuthSlider, false);

        addAndMakeVisible (elevationSlider);
		    elevationSlider.setRange(-45, 90);
		    elevationSlider.setSliderStyle (Slider::LinearVertical);
        elevationSlider.setTextValueSuffix (" Degrees");
        elevationSlider.addListener (this);
        elevationSlider.setNumDecimalPlacesToDisplay(2);

        addAndMakeVisible (elevationLabel);
        elevationLabel.setText ("Elevation", dontSendNotification);
        elevationLabel.attachToComponent (&elevationSlider, false);
        
        azimuthSlider.setValue (0);
        elevationSlider.setValue(0);

        addAndMakeVisible (openButton);
        openButton.setButtonText ("Open...");
        openButton.onClick = [this] { openButtonClicked(); };

        addAndMakeVisible (playButton);
        playButton.setButtonText ("Play");
        playButton.onClick = [this] { playButtonClicked(); };

        addAndMakeVisible (exportButton);
        exportButton.setButtonText ("Export");
        exportButton.onClick = [this] { playButtonClicked(); };
        exportButton.setColour (TextButton::buttonColourId, Colours::green);
        exportButton.setEnabled (false);

        std::thread(&Interfacer::myMain, interfacer).detach();

        setSize (600, 300);
    }

    ~MainContentComponent() {
        //delete interfacer;
    }

    Interfacer * interfacer = new Interfacer();

    void resized() override
    {
        auto sliderLeft = 120;
        azimuthSlider.setBounds (sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
        elevationSlider .setBounds (sliderLeft, 50, getWidth() - sliderLeft - 10, 20);
        openButton.setBounds (sliderLeft, 80, getWidth() - sliderLeft - 10, 20);
        exportButton.setBounds (sliderLeft, 110, getWidth() - sliderLeft - 10, 20);
        playButton.setBounds (sliderLeft, 140, getWidth() - sliderLeft - 10, 20);

        auto border = 4;
        auto area = getLocalBounds();
        auto dialArea = area.removeFromTop (area.getHeight() / 2);
        azimuthSlider.setBounds(dialArea.removeFromLeft (dialArea.getWidth() / 1.5).reduced (border));
        elevationSlider.setBounds (dialArea.reduced (border));

        auto buttonHeight = 30;

        auto outputArea = area.removeFromRight ( area.getWidth()/2);
        playButton.setBounds (outputArea.removeFromTop (outputArea.getHeight() / 2).reduced (border));
        exportButton.setBounds (outputArea.reduced (border));
        openButton.setBounds (area.reduced (border));

    }

    void sliderValueChanged (Slider* slider) override
    {
		if (slider == &azimuthSlider) {
			std::string azi, ele, cmds;
			 azi = std::to_string(azimuthSlider.getValue());
		      ele =  std::to_string(elevationSlider.getValue());
			  cmds = "select -k 0";
			  interfacer->handle_input(cmds);

			  cmds = "set -k -p theta " + azi;
			  interfacer->handle_input(cmds);

			  cmds = "list -k ";
			interfacer->handle_input(cmds);


			  cmds = "set -k -p phi " + ele ;
			  interfacer->handle_input(cmds);

}
		else if (slider == &elevationSlider) {
			std::string azi, ele, cmds;
			 azi = std::to_string(azimuthSlider.getValue());
		      ele =  std::to_string(elevationSlider.getValue());
			  cmds = "select -k 0";
			  interfacer->handle_input(cmds);

			  cmds = "set -k -p theta " + azi;
			  interfacer->handle_input(cmds);

			  cmds = "list -k ";
			interfacer->handle_input(cmds);


			  cmds = "set -k -p phi " + ele ;
			  interfacer->handle_input(cmds);
		}

    }

    void openButtonClicked()
    {
        FileChooser chooser ("Select a Wave file to spatialize...",{},"*.wav");
        if (chooser.browseForFileToOpen())
        {
            targetFile = chooser.getResult();
			std::string cmds, fp, azi, ele;
			fp = targetFile.getFullPathName().toStdString();
			azi = std::to_string(azimuthSlider.getValue());
			ele =  std::to_string(elevationSlider.getValue());
			std::cout << azi << std::endl;
			cmds = "import -file " + fp ;
			interfacer->handle_input(cmds);
			index++;
			cmds = "add -s " + fp + " " + std::to_string(index);
			interfacer->handle_input(cmds);

			cmds = "select -s " + fp ;
			interfacer->handle_input(cmds);

			cmds = "select -k 0";
			interfacer->handle_input(cmds);

			cmds = "set -k -p theta " + azi;
			interfacer->handle_input(cmds);

			cmds = "set -k -p phi " + ele ;
			interfacer->handle_input(cmds);

        }
    }

    void playButtonClicked()
    {
        // Call Justin's code using slider parameters and file
        // callAudioBackend( File file, int azimuth, int elevation )
			play = !play;
			if(play)
				interfacer->handle_input("play");
			else
				interfacer->handle_input("pause");

       /* std::filebuf fb;

        if (fb.open ("t",std::ios::in))
        {
            std::istream is(&fb);
            interfacer->setInStream(&is);
            std::thread(&Interfacer::myMain, interfacer).detach();
        }*/

        /*

        callAudioBackend( targetFile, (int)azimuthSlider.getValue(), (int)azimuthElevation.getValue() )

        */
    }
    	juce::File targetFile;


private:
    Slider azimuthSlider;
    Label  azimuthLabel;
    Slider elevationSlider;
    Label  elevationLabel;
	int index = -1;
	bool play = false;

    TextButton openButton;
    TextButton exportButton;

    TextButton playButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
