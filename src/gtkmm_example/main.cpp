#include <gtkmm.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
//#include <X11>


#include"../interfacer.h"
#include"clock.h"


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
Gtk::Button * newKeyButton = nullptr;

Gtk::ComboBoxText * sourceListComboBoxText = nullptr;
Gtk::ComboBoxText * keyListComboBoxText = nullptr;

Gtk::Viewport * drawVP = nullptr;


//AnimationPlayer but like not really
Gtk::Button * restartButton = nullptr;

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

//Popup Key Window
Gtk::Window* popAddKeyWindow = nullptr;
Gtk::Button * popAddKeyAddButton = nullptr;
Gtk::Button * popAddKeyCancelButton = nullptr;
Gtk::Entry * popAddKeyTimeEntry = nullptr;
Gtk::Entry * popAddKeyAzimuthEntry = nullptr;
Gtk::Entry * popAddKeyElevationEntry = nullptr;
Gtk::Entry *  popAddKeyRadiusEntry = nullptr;
Gtk::Entry *  popAddKeyScaleEntry = nullptr;
Gtk::Switch *  popAddKeyLoopingSwitch = nullptr;
Gtk::Switch *  popAddKeyVisibleSwitch = nullptr;



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

static void close_pop_key(){
	//reset values/makesure resetting values doesn't change source file
	popAddKeyWindow->hide();
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


static void on_restart_clicked(){
	std::string cmd = "restart";
	interfacer->handle_input(cmd);
}

static void on_new_source_clicked(){
	//int * p;
	//*p =0;
	//char ** cv = {{}};
	//auto app = Gtk::Application::create(*p, cv, "org.gtkmm.example2");
	//app->run(*popNewSourceWindow);
	popNewSourceWindow->show();
}

static void on_add_key_clicked(){
	//int * p;
	//*p =0;
	//char ** cv = {{}};
	//auto app = Gtk::Application::create(*p, cv, "org.gtkmm.example2");
	//app->run(*popNewSourceWindow);
	popAddKeyWindow->show();
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

static void on_pop_key_add_clicked(){
	//set properties to default if no value
	std::string cmd;

	//add new keyframe
	cmd = "select -s " + sourceListComboBoxText->get_active_text();
	interfacer->handle_input(cmd);

	//add keyframe
	cmd = "add -k " + popAddKeyTimeEntry->get_text().raw();
	interfacer->handle_input(cmd);

	//select keyframe n
	cmd = "select -k " + std::to_string(interfacer->anime->getSource(interfacer->selectedSource)->keyFrameList.size() -1);
	interfacer->handle_input(cmd);

	//set positional properties
	cmd = "set -k -p theta " + popAddKeyAzimuthEntry->get_text().raw();
	interfacer->handle_input(cmd);

	cmd = "set -k -p phi " + popAddKeyElevationEntry->get_text().raw();
	interfacer->handle_input(cmd);

	//cmd = "set -k -p radius " + popAddKeyRaduesEntry->get_text().raw();
	//interfacer->handle_input(cmd);

	//set looping and visibility
	std::string state;
	//sourceMuteSwitch->set_state(bstate	);
	if(popAddKeyLoopingSwitch->get_state() == true)
		state = "true";
	else
		state = "false";
	cmd = "set -k -p looping " + state;
	//std::cout << cmd << std::endl;
	interfacer->handle_input(cmd);

	if(popAddKeyVisibleSwitch->get_state() == true)
		state = "true";
	else
		state = "false";
	cmd = "set -k -p visible " + state;
	//std::cout << cmd << std::endl;
	interfacer->handle_input(cmd);

	close_pop_key();

	//reset_pop_window_values();
	Glib::ustring id = std::to_string(interfacer->anime->getSource(interfacer->selectedSource)->keyFrameList.size() -1);

	keyListComboBoxText->append(id, id);
}
static void on_keylist_changed(){
	std::string cmd;
	cmd = "select -k " + keyListComboBoxText->get_active_text().raw();
	interfacer->handle_input(cmd);
	//update source and keyframe properties panels
	keyAzimuthEntry->set_text(std::to_string(interfacer->anime->getSource(sourceListComboBoxText->get_active_text().raw())->keyFrameList[(int)std::stod( keyListComboBoxText->get_active_text().raw())]->properties->position->theta));
	keyElevationEntry->set_text(std::to_string(interfacer->anime->getSource(sourceListComboBoxText->get_active_text().raw())->keyFrameList[(int)std::stod( keyListComboBoxText->get_active_text().raw())]->properties->position->phi));
	keyRadiusEntry->set_text(std::to_string(interfacer->anime->getSource(sourceListComboBoxText->get_active_text().raw())->keyFrameList[(int)std::stod( keyListComboBoxText->get_active_text().raw())]->properties->position->radius));

	keyTimeEntry->set_text(std::to_string(interfacer->anime->getSource(sourceListComboBoxText->get_active_text().raw())->keyFrameList[(int)std::stod( keyListComboBoxText->get_active_text().raw())]->time_s));
}

static void on_sourcelist_changed(){
	std::string cmd;
	cmd = "select -s " + sourceListComboBoxText->get_active_text().raw();
	interfacer->handle_input(cmd);
	//update source and keyframe properties panels
	sourceNameEntry->set_text(sourceListComboBoxText->get_active_text());
	sourceStartTimeEntry->set_text(std::to_string(interfacer->anime->getSource(sourceListComboBoxText->get_active_text().raw())->timeStart_s));
	sourceMuteSwitch->set_state(!interfacer->anime->getSource(sourceListComboBoxText->get_active_text().raw())->isVisible);
	fcb->set_filename(interfacer->anime->getSource(sourceListComboBoxText->get_active_text().raw())->source->getTrackFilepath());

	cmd = "select -k 0";
	interfacer->handle_input(cmd);

	Glib::ustring temp = std::to_string(0);

	keyListComboBoxText->set_active_id(temp);

	keyListComboBoxText->remove_all();
	int size = interfacer->anime->getSource(sourceListComboBoxText->get_active_text().raw())->keyFrameList.size();
	for(int i = 0; i < size; i++){
		Glib::ustring temp = std::to_string(i);
		keyListComboBoxText->append(temp, temp);
	}

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
	cmd = "set -s -p name " + sourceNameEntry->get_text().raw();
	interfacer->handle_input(cmd);
	//sourceListComboBoxText->set_active_text(sourceNameEntry->get_text());
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
	//XInitThreads();
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
	refBuilder->get_widget("PopAddKeyWindow", popAddKeyWindow);

	refBuilder->get_widget("NewKeyButton", newKeyButton);
	newKeyButton->signal_pressed().connect(sigc::ptr_fun(on_add_key_clicked));
	Clock c;
	c.interfacer = interfacer;

	refBuilder->get_widget("DrawViewport", drawVP);
	drawVP->add(c);
	c.show();


	refBuilder->get_widget("SourceListComboBoxText", sourceListComboBoxText);
	sourceListComboBoxText->signal_changed().connect(sigc::ptr_fun(on_sourcelist_changed));

	refBuilder->get_widget("KeyListComboBoxText", keyListComboBoxText);
	keyListComboBoxText->signal_changed().connect(sigc::ptr_fun(on_keylist_changed));

	//animation player but not really
	Gtk::ToggleButton * tb = nullptr;
	refBuilder->get_widget("ToggleButton", tb);
	tb->signal_toggled().connect(sigc::ptr_fun(on_play_toggled));

	refBuilder->get_widget("RestartButton", restartButton);
	restartButton->signal_clicked().connect(sigc::ptr_fun(on_restart_clicked));


	/*------------Source Properties------------------*/
	refBuilder->get_widget("filechooserbutton", fcb);
	fcb->signal_file_set().connect(sigc::ptr_fun(on_file_selected));

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

	/*--------------PopUp KeyFrame Window-----------------------*/
	refBuilder->get_widget("PopAddKeyAddButton", popAddKeyAddButton);
	popAddKeyAddButton->signal_pressed().connect(sigc::ptr_fun(on_pop_key_add_clicked));

	refBuilder->get_widget("PopAddKeyCancelButton", popAddKeyCancelButton);
	popAddKeyCancelButton->signal_pressed().connect(sigc::ptr_fun(close_pop_key));

	refBuilder->get_widget("PopAddKeyTimeEntry", popAddKeyTimeEntry);
	refBuilder->get_widget("PopAddKeyAzimuthEntry", popAddKeyAzimuthEntry);
	refBuilder->get_widget("PopAddKeyElevationEntry", popAddKeyElevationEntry);
	refBuilder->get_widget("PopAddKeyRadiusEntry", popAddKeyRadiusEntry);
	refBuilder->get_widget("PopAddKeyLoopingSwitch", popAddKeyLoopingSwitch);
	refBuilder->get_widget("PopAddKeyVisibleSwitch", popAddKeyVisibleSwitch);


	std::thread(&Interfacer::myMain, interfacer).detach();

	app->run(*window);


	delete window;

	return 0;
}
