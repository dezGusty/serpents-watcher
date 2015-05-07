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
#include "guslib\util\filehelper.h"

//
// Constructor (default).
//
IconSelector::IconSelector(const std::string& resource_root_path)
  : running_icon_name_(""),
    intermediate_icon_name_(""),
    stopped_icon_name_(""),
    error_icon_name_(""),
    resource_root_path_(resource_root_path)
{
}

//
// Initializes the icon names 
// @param fileName The name of the file that holds the configuration. 
// (The icon names should be contained within).
//
// @authors Petru Barko, Augustin Preda.
//
void IconSelector::InitializeIconsFromFile(const std::string& file_name)
{
  // The config object can easily parse config files.
  guslib::config::Configuration config_object;

  if (guslib::filehelper::IsFileAccessible(file_name))
  {
    // Load the configuration.
    config_object.load(file_name);

    this->InitializeIconsFromConfig(config_object);
  }
}

//
// Initialize the icons from a configuration object.
// @param config_object The configuration object.
//
// @author Augustin Preda.
//
void IconSelector::InitializeIconsFromConfig(guslib::config::Configuration& config_object)
{
  //stores the icon file names in these class members
  this->running_icon_name_ = config_object["running icon"]["name"].getAsStringOrDefaultVal("");
  this->intermediate_icon_name_ = config_object["intermidiate icon"]["name"].getAsStringOrDefaultVal("");
  this->stopped_icon_name_ = config_object["stopped icon"]["name"].getAsStringOrDefaultVal("");
  this->error_icon_name_ = config_object["error icon"]["name"].getAsStringOrDefaultVal("");
}

//
// Accessors.
//

//
// @return The name of the icon that is used when the service can't be accessed.
//
// @author Petru Barko.
//
const std::string& IconSelector::error_icon_name() const
{
  return this->error_icon_name_;
}

//
// @return The name of the icon that apears when the service is changing state (starting).
//
// @author Petru Barko.
//
const std::string& IconSelector::intermediate_icon_name() const
{
  return this->intermediate_icon_name_;
}

//
// @return The root path location.
//
// @author Augustin Preda.
//
const std::string& IconSelector::resource_root_path() const
{
  return this->resource_root_path_;
}

//
// @return The name of the icon that apear when the service is running.
//
// @author Petru Barko.
//
const std::string& IconSelector::running_icon_name() const
{
  return this->running_icon_name_;
}

//
// @return The name of the icon that apears when the service is stopped.
//
// @author Petru Barko.
//
const std::string& IconSelector::stopped_icon_name() const
{
  return this->stopped_icon_name_;
}