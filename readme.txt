Unity scope template

Building
--------

To build this scope outside of QtCreator, please install phablet-tools and run click-buddy.

Localization
------------

intltool is used to perform localization of the scope. Ensure whenever you add / rename files
that contain localizable strings you update "po/POTFILES.in".

When you want to add a new language to the translation catalogue:

 * Copy the .pot file from the "po" directory and rename it to e.g. "zh_CN.po", 
   where "zh_CN" is the language code for China.
 * Enter the language code in "Language:".
 * Enter the "UTF-8" as the "Content-Type:".
 * Enter the translations in the msgstr tags.

For the new translation to show up, cmake needs to be re-run to enable the recusive search
macro to delect the new translation. In QtCreator this can be done with the "Build->Run Cmake"
menu entry.

Remember! When you copy the .pot file, make sure you change the file extension to ".po".
