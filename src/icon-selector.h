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
	
	/**
	*Initializes the icon names 
	*
	*@param fileName - the file that holds the file name
	*@return void
	*/
	void initializeIconsFromFile(std::string fileName);
	
	/*
	*@return the name of the icon that will apear when the service is running
	*/
	std::string running() const;

	/*
	*@return the name of the icon that will apear when the service is changing state
	*/
	std::string intermidiate() const;

	/*
	*@return the name of the icon that will apear when the service is changing state
	*/
	std::string stopped() const;

	/*
	*@return the name of the icon that will apear when can't access the service
	*/
	std::string somthingWentWrong() const;
};