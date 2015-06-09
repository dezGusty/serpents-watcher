# serpents-watcher
==================
Serpents Watcher is a small utility for Microsoft Windows operating systems used to watch the status of system services.

## Author(s)
------------
Petru Barko
Augustin Preda

## License
----------
Serpents Watcher is licensed under the terms of the MIT software license (https://github.com/dezGusty/serpents-watcher/blob/master/LICENSE)

## Setting-up the project
-------------------------
Please consult this link: https://github.com/dezGusty/serpents-watcher/blob/master/SETUP.md


## TODO List
------------
- [ ] The service start and stop are currently implemented as blocking operations. A state machine should take care of these operations and what can be triggered from the UI.
- [ ] Closing the main window using the close (X) button is not handled properly when the assigned log file is a file which does not exist. (Log file should be optional)
- [ ] The optional log file is read only once, but should monitor the file for changes.
- [ ] The optional log file cannot be written to once opened for display.