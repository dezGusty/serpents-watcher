#include "IconSelector.h"

IconSelector::IconSelector(){

	this->runningIconName = "";
	this->intermidiateIconName = "";
	this->stoppedIconName = "";
}

void IconSelector::initializeIconsFromFile(std::string fileName){

	guslib::config::Configuration config_;
	std::string cfgFileName(fileName);

	if (guslib::filehelper::IsFileAccessible(cfgFileName))
	{
		config_.load(cfgFileName);

		this->runningIconName = config_["running icon"]["name"].getAsStringOrDefaultVal("");
		this->intermidiateIconName = config_["intermidiate icon"]["name"].getAsStringOrDefaultVal("");
		this->stoppedIconName = config_["stopped icon"]["name"].getAsStringOrDefaultVal("");
		this->stoppedIconName = config_["stopped icon"]["name"].getAsStringOrDefaultVal("");
		this->errorIconName = config_["error icon"]["name"].getAsStringOrDefaultVal("");
	}
}

std::string IconSelector::running() const{ return this->runningIconName; }
std::string IconSelector::intermidiate() const{ return this->intermidiateIconName; }
std::string IconSelector::stopped() const{ return this->stoppedIconName; }
std::string IconSelector::somthingWentWrong() const{ return this->errorIconName; }