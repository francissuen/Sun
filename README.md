Project Sun - A C++11 utils library.
===========

ubuntu&macOS: [![Build Status](https://travis-ci.org/francissuen/Sun.svg?branch=master)](https://travis-ci.org/francissuen/Sun)  
windows: [![Build status](https://ci.appveyor.com/api/projects/status/b6nvibpg09er9hod/branch/master?svg=true)](https://ci.appveyor.com/project/francissuen/sun/branch/master)

## Features ##
1. All features are compatiable with **C++11**.
2. Cross-platform supported(including linux, macOS and windows).
3. **variant**, a safe union type, and needs no RTTI. 
4. **async**, unlike std::async(*C++11*), it wraps the function into a coresponding async  
verison and has guarantee of running in a thread-pool.
5. **cout**, an async output utility with powerful output formats supported.
6. **utils.h**, a rich utils set. (e.g. `apply`, `index_of_seq`, `invoke::with::for_each`... ).
7. **range\_viewer**, almost a *std::span*, but with a movable _begin_.  
...
