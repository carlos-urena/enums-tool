# enums-tool

A C++11 header-only library for getting strings and values count from an enumerated type and its values

The main idea is to use the __PRETTY_FUNCTION__ macro in a function template (whose name includes enumerated values identifiers). I got this idea from 'magic_enum' equivalent header, but I have adapted that so its works with C++11, see :

      https://github.com/Neargye/magic_enum/blob/master/include/magic_enum.hpp

## Sample usage:

See this code from main.cpp :

```cpp
   enum EnumT  { cero0, second1, the_third123, almost_done, last_the_fourth_4 } ;

   using namespace enums_tool ;
   print_compiler_info();

   using namespace std ;

   cout << "name of enum type    = " << type_name_string<EnumT>() << endl
        << "num of values        = " << num_of_values<EnumT>() << endl
        << "list of values names = " << endl ;

   for ( EnumT v = first<EnumT>() ; v <= last<EnumT>() ; v++  )
      cout << "   " << name_string(v) << endl ;
```
  
## Features, limitations:

* works with -std=c++11
* works for both classic enums and enum class.
* max number of values in the enum cannot be larger than 'max_enums' constant (currently defined as 255)
* doesn't work if you explicitly assign integral values for representing the enums (it only works fine for consecutive integers starting at 0).
* only works for compilers with suitable __PRETTY_FUNCTION__ macro, that is LLVM clang++ and GNU g++  (Microsoft VC has an equivalent macro but I have not adapted the code for this compiler yet) 


