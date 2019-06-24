Project Sun - A C++11 utils library.
===========

ubuntu&macOS: [![Build Status](https://travis-ci.org/francissuen/Sun.svg?branch=master)](https://travis-ci.org/francissuen/Sun)  
windows: [![Build status](https://ci.appveyor.com/api/projects/status/b6nvibpg09er9hod/branch/master?svg=true)](https://ci.appveyor.com/project/francissuen/sun/branch/master)

## Features ##
1. All features are compatiable with **C++11**.
2. Cross-platform supported(including linux, macOS and windows).
3. **Async**, unlike std::async(*C++11*), it wraps the function into a coresponding async  
verison and has guarantee of running in a thread-pool.
4. **cout**, an async output utility with powerful output formats supported.
5. **utility.h**, a rich utility set. (e.g. `Apply`, `IndexOf::In`, `Invoke<>::for<>::With()`... ).
6. **RangeViewer**, almost a *std::span*, but with a movable _begin_.  
...
