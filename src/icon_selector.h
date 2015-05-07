#ifndef SERPENTS_ICON_SELECTOR_H
#define SERPENTS_ICON_SELECTOR_H

//
// Includes
//

//
// C system headers
//

// none

//
// C++ system headers
//
#include <string>

//
// 3rd party libraries
//
#include "guslib\util\config\configuration.h"

//
// Class designed to access icon (names) to be used by the application.
//
class IconSelector
{
private:
  std::string running_icon_name_;
  std::string intermediate_icon_name_;
  std::string stopped_icon_name_;
  std::string error_icon_name_;
  std::string resource_root_path_;

public:
  //
  // Constructor (default).
  //
  IconSelector(const std::string& resource_root_path);

  //
  // Initializes the icon names 
  // @param fileName The name of the file that holds the configuration. 
  // (The icon names should be contained within).
  //
  // @authors Petru Barko, Augustin Preda.
  //
  void InitializeIconsFromFile(const std::string& file_name);

  //
  // Initialize the icons from a configuration object.
  // @param config_object The configuration object.
  //
  // @author Augustin Preda.
  //
  void InitializeIconsFromConfig(guslib::config::Configuration& config_object);
  
  //
  // @return The name of the icon that is used when the service can't be accessed.
  //
  // @author Petru Barko.
  //
  const std::string& error_icon_name() const;

  //
  // @return The name of the icon that apears when the service is changing state (starting).
  //
  // @author Petru Barko.
  //
  const std::string& intermediate_icon_name() const;
  
  //
  // @return The root path location.
  //
  // @author Augustin Preda.
  //
  const std::string& resource_root_path() const;

  //
  // @return The name of the icon that apear when the service is running.
  //
  // @author Petru Barko.
  //
  const std::string& running_icon_name() const;

  //
  // @return The name of the icon that apears when the service is stopped.
  //
  // @author Petru Barko.
  //
  const std::string& stopped_icon_name() const;

};

#endif  // SERPENTS_ICON_SELECTOR_H
