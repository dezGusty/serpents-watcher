#ifndef SERPENTS_WATCH_VERSION_HPP
#define SERPENTS_WATCH_VERSION_HPP

//   This file is part of the "Serpents Watcher Utility", licensed under 
//   the terms of the MIT License (seen below).
//
//   The MIT License
//   Copyright (C) 2015  SERPENTS WATCHER TEAM (thegusty999@gmail.com)
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//

//
// This file provides version information for the Serpents Watcher utility.
// The information must be updated manually.
//

// --------------------------------------------- utility macros ----------------------------------------------------

//
// Utility macro:
// Concatenate preprocessor tokens A and B without expanding macro definitions
// (however, if invoked from a macro, macro arguments are expanded).
//
#define PPCAT_NX(A, B) A ## B

//
// Utility macro:
// Concatenate preprocessor tokens A and B after macro-expanding them.
//
#define PPCAT(A, B) PPCAT_NX(A, B)

//
// Utility macro:
// Turn A into a string literal without expanding macro definitions
// (however, if invoked from a macro, macro arguments are expanded).
//
#define STRINGIZE_NX(A) #A

//
// Utility macro:
// Turn A into a string literal after macro-expanding it.
//
#define STRINGIZE(A) STRINGIZE_NX(A)

// ------------------------------------------- version specifications --------------------------------------------------

//
// Define the major, minor and patch version numbers for separate use.
// These should be used in the generation of the resource file version information.
//
// Note: Semantic Versioning 2.0.0 is recommended for use (http://semver.org/).
// Given a version number MAJOR.MINOR.PATCH, increment the:
//  MAJOR version when you make incompatible API changes,
//  MINOR version when you add functionality in a backwards - compatible manner, and
//  PATCH version when you make backwards - compatible bug fixes.
//

// Major versiom
#define SERPENTS_WATCH_NUM_MAJOR_VER_SIMPLE 1

// Minor versions. Please keep in sync!
#define SERPENTS_WATCH_NUM_MINOR_VER_SIMPLE 2

// Patch versions. Please keep in sync!
#define SERPENTS_WATCH_NUM_PATCH_VER_SIMPLE 0

//
// Also define string adaptations
//

//  SERPENTS_WATCH_STRING_VERSION should be defined as the "product" version, as a string, so you can print it out in the application, in the form
//  "x.y.z", where "x" is the major version number, "y" is the minor version number, and "z" is the patch level. 

#define SERPENTS_WATCH_STRING_VERSION STRINGIZE(SERPENTS_WATCH_NUM_MAJOR_VER_SIMPLE) "." STRINGIZE(SERPENTS_WATCH_NUM_MINOR_VER_SIMPLE) "." STRINGIZE (SERPENTS_WATCH_NUM_PATCH_VER_SIMPLE)

//
// File version
//
#define SERPENTS_WATCH_STR_FILE_VERSION STRINGIZE(SERPENTS_WATCH_NUM_MAJOR_VER_SIMPLE) "." STRINGIZE(SERPENTS_WATCH_NUM_MINOR_VER_SIMPLE) "." STRINGIZE (SERPENTS_WATCH_NUM_PATCH_VER_SIMPLE)

//
// Product version
//
#define SERPENTS_WATCH_STR_PROD_VERSION STRINGIZE(SERPENTS_WATCH_NUM_MAJOR_VER_SIMPLE) "." STRINGIZE(SERPENTS_WATCH_NUM_MINOR_VER_SIMPLE) "." STRINGIZE (SERPENTS_WATCH_NUM_PATCH_VER_SIMPLE)



#endif // SERPENTS_WATCH_VERSION_HPP
