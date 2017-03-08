zkanji v0.731 Copyright (c) 2007-2013 Sólyom Zoltán
---------------------------------------------------
zkanji is a feature rich Japanese language study suite and
dictionary for Windows. It is based on data from JMdict,
KANJIDIC and RADKFILE.


License:
===================================================
Please read the licence.txt about the licence of zkanji.


Compiling the zkanji source code:
===================================================
zkanji is developed with Rad Studio C++Builder XE. I've not
tried compiling it with another version but it might be
possible to compile with C++Builder 2009 and above.

* To compile zkanji you first need to build and install the
VCL components found in the "\source\components" folder.
If you don't do this zkanji won't compile. You will probably only
have to do this once unless the component was modified for a new
version.

* Open zkanji.cbproj and compile. (You might need to change the
"Paths and Defines" in the project options in case the compiler
doesn't find the custom VCL components, and set the path to some
libraries the program uses from within the C++Builder's lib
folder.)

* There are files needed to run the compiled program as they
contain the dictionary data and other crucial information for
zkanji. These files can be obtained with the full package of the
binary, but you can also make your own files if you need. Contact
me in case you need help with the data file formats.

---------------------------------------------------
home page: http://zkanji.sourceforge.net
e-mail: z-one@freemail.hu