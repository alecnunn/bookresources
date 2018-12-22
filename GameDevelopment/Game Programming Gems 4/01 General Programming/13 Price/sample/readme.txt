Powerup sample

This project shows:
* a simple example of reading XDS data
* a simple example of writing XDS data
* an example of using the XDS-lite API to define a data stream

The project cannot be compiled until the powerups_dsd.h file is generated.  To
generate this file, run this command.

    xdsMakeSchema -s Powerups -r Powerup -o powerups_dsd.h powerups.h


To convert the XML file to XDS format, run these commands (or just run the
project after compiling with XDS_SUPPORT_WRITING #defined.

    xdsMakeSchema -s Powerups -r Powerup -o powerups.dsd powerups.h
    xdsConvert -o powerups.xds -d powerups.dsd powerups.xml


To create an XML Schema for use in an XML editor to create more data files,
run this command.

    xdsMakeSchema -s Powerups -r Powerup -o powerups.xsd powerups.h