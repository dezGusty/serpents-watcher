#pragma once

#include "guslib\util\config\configuration.h"
#include "guslib\util\filehelper.h"


//this class takes the icon names from the configuration file
class IconSelector{


private:
	std::string runningIconName;
	std::string intermidiateIconName;
	std::string stoppedIconName;
	std::string errorIconName;

public:
	IconSelector();
	void initializeIconsFromFile(std::string fileName);
	std::string running() const;
	std::string intermidiate() const;
	std::string stopped() const;
	std::string somthingWentWrong() const;
};