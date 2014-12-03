Cinema Scope
=============

This scope queries [TMDb](http://themoviedb.org) for movies and tv shows and allows people to have fast access to various information, including trailers.
It was written for the [Ubuntu Scope Showdown](http://developer.ubuntu.com/showdown/)

Compile and run
=============

In order to have a functional scope you need to insert a developer key into [config.h](include/api/config.h); just go to [TMDb login page](https://www.themoviedb.org/login) and register a new account.
For compiling and running you can use the Ubuntu SDK.

Main features
=============

- Movies and TV series sections
- Trailers
- Ability to find nearby cinemas playing a particular film (it uses the google.com/movies page and phone location)
- Links to IMDb, homepage and search on Google
- Support for translations (text language for queries is set to "en" by default, because information on other languages tend to be incomplete on TMDb sometimes)
- Find the shows airing today!
- Find actors and directors using the search function!
