You should have python installed. Tested and working with Python 3.3.2.

To add an external tool to Visual Studio, go to the [Tools] menu, and select [External Tools...].
Click the [Add] button, then fill in the fields as follows:
- add "[gus] cpplint" as the title.
- set the path to the python executable as the command. If you have it in the path, you can just use the exe name (E.g. "python.exe") (without quotes).  
- set the arguments to (with quotes): 
  "d:\work\GustysSerpents\Serpents\tools\cpplint\cpplint.py" --output=vs7 "$(ItemPath)"
- adjust it to your location of cpplint.py
- set the initial directory to "$(ProjectDir)" (without quotes). TODO: or is it "$(ItemDir)" ? Or does it not matter?
- enable "Use Output window"
- hit OK/Apply

Optional step: Add a button linking to the tool.
- right click the toolbar, hit "Customize"
- click the "New..." button and create a toolbar named "external_tools"
- switch to the second tab ("Commands"), select the 2nd radio button entry ("Toolbar").
- from the corresponding drop-down list, select the "external_tools" entry.
- click the "Add command..." button.
- select the "Tools" entry from the categories section.
- select the new entry from the list of entries in the form "External Command N".
    The N number should correspond to the index of the tool specified in the 

