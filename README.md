# enums-tool
A C++11 header-only library for getting strings and count from a enumerated type and its values

The main idea (use a macro in a function template) is taken from 'magic_enum', but I have adapted this so its works with C++11, see :

      https://github.com/Neargye/magic_enum/blob/master/include/magic_enum.hpp

Features, limitations:

* works with -std=c++11
* works for both classic enums and enum class.
* max number of values in the enum cannot be larger than 'max_enums' constant (currently defined as 255)
* doesn't work if you explicitly assign integral values for representing the enums (it only works fine for consecutive integers starting at 0).
* only works for compilers with suitable __PRETTY_FUNCTION__ macro, that is LLVM clang++ and GNU g++  (Microsoft VC has an equivalent macro but I have not adpated the code for this compiler yet) 
