//
// Includes
//

//
// Own header
//
#include "icon_selector.h"

//
// 3rd party libraries
//
#include "guslib\util\config\configuration.h"
#include "guslib\util\filehelper.h"

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
    //loads the file
    config_.load(cfgFileName);

    //stores the icon file names in these class members
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