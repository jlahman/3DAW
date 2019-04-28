#include <gtkmm.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>


#include"../interfacer.h"


std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
};

Glib::RefPtr<Gtk::Builder> refBuilder;// = Gtk::Builder::create();
Gtk::Window* window = nullptr;
Gtk::Button * newSourceButton = nullptr;
Gtk::ComboBoxText * sourceListComboBoxText = nullptr;

//Popup Source Window
Gtk::Window* popNewSourceWindow = nullptr;
Gtk::Button * popNewSourceAddButton = nullptr;
Gtk::Button * popNewSourceCancelButton = nullptr;
Gtk::FileChooserButton* popfcb = nullptr;
Gtk::Entry * popSourceNameEntry = nullptr;
Gtk::Entry * popSourceStartTimeEntry = nullptr;
Gtk::Entry * popKeyAzimuthEntry = nullptr;
Gtk::Entry * popKeyElevationEntry = nullptr;
Gtk::Entry *  popKeyRadiusEntry = nullptr;
Gtk::Entry *  popKeyScaleEntry = nullptr;
Gtk::Switch *  popKeyLoopingSwitch = nullptr;
Gtk::Switch *  popKeyVisibleSwitch = nullptr;


//SoundSourceProperties
Gtk::FileChooserButton* fcb = nullptr;
Gtk::Entry * sourceNameEntry = nullptr;
Gtk::Entry * sourceStartTimeEntry = nullptr;
Gtk::Switch * sourceMuteSwitch = nullptr;

//key frame properties
Gtk::Entry * keyAzimuthEntry = nullptr;
Gtk::Entry * keyElevationEntry = nullptr;
Gtk::Entry * keyRadiusEntry = nullptr;
Gtk::Entry * keyScaleEntry = nullptr;
Gtk::Entry * keyTimeEntry = nullptr;
Gtk::Switch * keyLoopingSwitch = nullptr;
Gtk::Switch * keyVisibleSwitch = nullptr;



Interfacer *interfacer = new Interfacer();

static void close_pop_source(){
	//reset values/makesure resetting values doesn't change source file
	popNewSourceWindow->hide();
}

static void on_file_selected(){

	std::string filepath =  fcb->get_filename();
	std::string filename = split((const std::string)filepath, '/').back();
	std::string cmd;

	int index = 0;
	if(interfacer->hasTrack(filepath)){
		index = interfacer->getTrackIndex(filepath);
	} else {
		index = interfacer->getTrackSize();
		cmd = "import -f ";
		cmd += filepath;
		interfacer->handle_input(cmd);
	}

	cmd = "add -s " + filename + " " + std::to_string(index);
	interfacer->handle_input(cmd);

	cmd = "select -s " + filename;
	interfacer->handle_input(cmd);

	cmd = "select -k 0";
	interfacer->handle_input(cmd);


	printf("Filename: %s\n", filename.c_str());

}

static void on_new_source_clicked(){
	//int * p;
	//*p =0;
	//char ** cv = {{}};
	//auto app = Gtk::Application::create(*p, cv, "org.gtkmm.example2");
	//app->run(*popNewSourceWindow);
	popNewSourceWindow->show();
}

static void on_pop_source_add_clicked(){
	//set properties to default if no value
	std::string cmd;

	//import file
	std::string filepath =  popfcb->get_filename();
	std::string filename = split((const std::string)filepath, '/').back();

	int index = 0;
	if(interfacer->hasTrack(filepath)){
		index = interfacer->getTrackIndex(filepath);
	} else {
		index = interfacer->getTrackSize();
		cmd = "import -f ";
		cmd += filepath;
		interfacer->handle_input(cmd);
	}

	//add source
	cmd = "add -s " + filename + " " + std::to_string(index);
	interfacer->handle_input(cmd);

	cmd = "select -s " + filename;
	interfacer->handle_input(cmd);

	//set name
	cmd = "set -s -p name " + popSourceNameEntry->get_text().raw();
	interfacer->handle_input(cmd);

	//set start time
	cmd = "set -s -p start_time " + popSourceStartTimeEntry->get_text().raw();
	interfacer->handle_input(cmd);

	//select keyframe 0
	cmd = "select -k 0";
	interfacer->handle_input(cmd);

	//set positional properties
	cmd = "set -k -p theta " + popKeyAzimuthEntry->get_text().raw();
	interfacer->handle_input(cmd);

	cmd = "set -k -p phi " + popKeyElevationEntry->get_text().raw();
	interfacer->handle_input(cmd);

	//cmd = "set -k -p radius " + popKeyRaduesEntry->get_text().raw();
	//interfacer->handle_input(cmd);

	//set looping and visibility
	std::string state;
	//sourceMuteSwitch->set_state(bstate	);
	if(popKeyLoopingSwitch->get_state() == true)
		state = "true";
	else
		state = "false";
	cmd = "set -k -p looping " + state;
	//std::cout << cmd << std::endl;
	interfacer->handle_input(cmd);

	if(popKeyVisibleSwitch->get_state() == true)
		state = "true";
	else
		state = "false";
	cmd = "set -k -p visible " + state;
	//std::cout << cmd << std::endl;
	interfacer->handle_input(cmd);

	close_pop_source();

	//reset_pop_window_values();

	sourceListComboBoxText->append(popSourceNameEntry->get_text(), popSourceNameEntry->get_text());
}

static void on_sourcelist_changed(){
	std::string cmd;
	cmd = "select -s " + sourceListComboBoxText->get_active_text().raw();
	interfacer->handle_input(cmd);
	//update source and keyframe properties panels
}

static void on_play_toggled(){
	std::string cmd;
	if(interfacer->pause){
		cmd = "play";
		interfacer->handle_input(cmd);
	} else {
		cmd = "pause";
		interfacer->handle_input(cmd);
	}
}

static void on_source_name_changed(){
	std::string cmd;
	sourceListComboBoxText->set_active_text( sourceNameEntry->get_text());
	cmd = "set -s -p name " + sourceNameEntry->get_text().raw();
	interfacer->handle_input(cmd);
}

static void on_source_time_changed(){
	std::string cmd;
	cmd = "set -s -p start_time " + sourceStartTimeEntry->get_text().raw();
	interfacer->handle_input(cmd);
}

static void on_source_mute_set(){
	std::string cmd;
	std::string state;
	//sourceMuteSwitch->set_state(bstate	);
	if(sourceMuteSwitch->get_state() != true)
		state = "true";
	else
		state = "false";
	cmd = "set -s -p visible " + state;
	std::cout << cmd << std::endl;
	interfacer->handle_input(cmd);
}

//key frame properties
static void on_key_azimuth_changed(){
	std::string cmd;
	cmd = "set -k -p theta " + keyAzimuthEntry->get_text().raw();
	interfacer->handle_input(cmd);
}

static void on_key_elevation_changed(){
	std::string cmd;
	cmd = "set -k -p phi " + keyElevationEntry->get_text().raw();
	interfacer->handle_input(cmd);
}

static void on_key_radius_changed(){
	//std::string cmd;
	//cmd = "set -k -p radius " + keyRaduesEntry->get_text().raw();
	//interfacer->handle_input(cmd);
}

static void on_key_scale_changed(){
	std::string cmd;
	cmd = "set -k -p scale " + keyScaleEntry->get_text().raw();
	interfacer->handle_input(cmd);
}

static void on_key_time_changed(){
	std::string cmd;
	cmd = "set -k -p time " + keyTimeEntry->get_text().raw();
	interfacer->handle_input(cmd);
}

static void on_key_looping_set(){
	std::string cmd;
	std::string state;
	//sourceMuteSwitch->set_state(bstate	);
	if(keyLoopingSwitch->get_state() == true)
		state = "true";
	else
		state = "false";
	cmd = "set -k -p looping " + state;
	std::cout << cmd << std::endl;
	interfacer->handle_input(cmd);
}

static void on_key_visible_set(){
	std::string cmd;
	std::string state;
	//sourceMuteSwitch->set_state(bstate	);
	if(keyVisibleSwitch->get_state() == true)
		state = "true";
	else
		state = "false";
	cmd = "set -k -p visible " + state;
	std::cout << cmd << std::endl;
	interfacer->handle_input(cmd);
}






int main (int argc, char **argv)
{
	auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

	//Load the GtkBuilder file and instantiate its widgets:
	refBuilder= Gtk::Builder::create();
	try
	{
		refBuilder->add_from_file("builder2.glade");
	}
	catch(const Glib::FileError& ex)
	{
		std::cerr << "FileError: " << ex.what() << std::endl;
		return 1;
	}
	catch(const Glib::MarkupError& ex)
	{
		std::cerr << "MarkupError: " << ex.what() << std::endl;
		return 1;
	}
	catch(const Gtk::BuilderError& ex)
	{
		std::cerr << "BuilderError: " << ex.what() << std::endl;
		return 1;
	}

	//Get the GtkBuilder-instantiated Dialog:
	refBuilder->get_widget("NewSourceButton", newSourceButton);
	newSourceButton->signal_pressed().connect(sigc::ptr_fun(on_new_source_clicked));

	refBuilder->get_widget("window", window);

	refBuilder->get_widget("PopNewSourceWindow", popNewSourceWindow);

	refBuilder->get_widget("SourceListComboBoxText", sourceListComboBoxText);
	sourceListComboBoxText->signal_changed().connect(sigc::ptr_fun(on_sourcelist_changed));


	/*------------Source Properties------------------*/
	refBuilder->get_widget("filechooserbutton", fcb);
	fcb->signal_file_set().connect(sigc::ptr_fun(on_file_selected));

	Gtk::ToggleButton * tb = nullptr;
	refBuilder->get_widget("ToggleButton", tb);
	tb->signal_toggled().connect(sigc::ptr_fun(on_play_toggled));

	refBuilder->get_widget("SourceNameEntry", sourceNameEntry);
	sourceNameEntry->signal_changed().connect(sigc::ptr_fun(on_source_name_changed));

	refBuilder->get_widget("SourceStartTimeEntry", sourceStartTimeEntry);
	sourceStartTimeEntry->signal_changed().connect(sigc::ptr_fun(on_source_time_changed));

	refBuilder->get_widget("SourceMuteSwitch", sourceMuteSwitch);
	sourceMuteSwitch->property_active().signal_changed().connect(sigc::ptr_fun(on_source_mute_set));


	/*------------Keyframe Properties------------------*/
	refBuilder->get_widget("KeyAzimuthEntry", keyAzimuthEntry);
	keyAzimuthEntry->signal_changed().connect(sigc::ptr_fun(on_key_azimuth_changed));

	refBuilder->get_widget("KeyElevationEntry", keyElevationEntry);
	keyElevationEntry->signal_changed().connect(sigc::ptr_fun(on_key_elevation_changed));

	refBuilder->get_widget("KeyRadiusEntry", keyRadiusEntry);
	keyRadiusEntry->signal_changed().connect(sigc::ptr_fun(on_key_radius_changed));

	refBuilder->get_widget("KeyScaleEntry", keyScaleEntry);
	keyScaleEntry->signal_changed().connect(sigc::ptr_fun(on_key_scale_changed));

	refBuilder->get_widget("KeyTimeEntry", keyTimeEntry);
	keyTimeEntry->signal_changed().connect(sigc::ptr_fun(on_key_time_changed));

	refBuilder->get_widget("KeyLoopingSwitch", keyLoopingSwitch);
	keyLoopingSwitch->property_active().signal_changed().connect(sigc::ptr_fun(on_key_looping_set));

	refBuilder->get_widget("KeyVisibleSwitch", keyVisibleSwitch);
	keyVisibleSwitch->property_active().signal_changed().connect(sigc::ptr_fun(on_key_visible_set));

	/*--------------PopUp Source Window-----------------------*/
	refBuilder->get_widget("PopNewSourceAddButton", popNewSourceAddButton);
	popNewSourceAddButton->signal_pressed().connect(sigc::ptr_fun(on_pop_source_add_clicked));

	refBuilder->get_widget("PopNewSourceCancelButton", popNewSourceCancelButton);
	popNewSourceCancelButton->signal_pressed().connect(sigc::ptr_fun(close_pop_source));

	refBuilder->get_widget("PopSourceNameEntry", popSourceNameEntry);
	refBuilder->get_widget("Popfilechooserbutton", popfcb);
	refBuilder->get_widget("PopSourceStartTimeEntry", popSourceStartTimeEntry);
	refBuilder->get_widget("PopKeyAzimuthEntry", popKeyAzimuthEntry);
	refBuilder->get_widget("PopKeyElevationEntry", popKeyElevationEntry);
	refBuilder->get_widget("PopKeyRadiusEntry", popKeyRadiusEntry);
	refBuilder->get_widget("PopKeyLoopingSwitch", popKeyLoopingSwitch);
	refBuilder->get_widget("PopKeyVisibleSwitch", popKeyVisibleSwitch);


	std::thread(&Interfacer::myMain, interfacer).detach();

	app->run(*window);


	delete window;

	return 0;
}
