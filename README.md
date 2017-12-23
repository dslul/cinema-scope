Cinema Scope
=============

This scope queries [TMDb](http://themoviedb.org) for movies and tv shows and allows people to have fast access to various information, including trailers.
It was written for the [Ubuntu Scope Showdown](http://developer.ubuntu.com/showdown/)

Main features
=============

- Movies and TV series sections
- Trailers
- Ability to find nearby cinemas playing a particular film (it uses the google.com/movies page and phone location)
- Links to IMDb, homepage and search on Google
- Support for translations (text language for queries is set to "en" by default, because information on other languages tend to be incomplete on TMDb sometimes)
- Find the shows airing today!
- Find actors and directors using the search function!


Building
=============

In order to have a functional scope you need to insert a developer key into [config.h](include/api/config.h); just go to [TMDb login page](https://www.themoviedb.org/login) and register a new account.
To build this scope outside of QtCreator, please install phablet-tools and run click-buddy.


Localization
=============

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
