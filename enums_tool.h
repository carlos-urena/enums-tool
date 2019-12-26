// C++11 enum tools
// Carlos Ureña
//
// A C++11 header-only library for getting strings and count from a enumerated type and its values
// hosted in github: https://github.com/carlos-urena/enums-tool/blob/master/enums_tool.h
//
// Ideas taken from 'magic_enum', see :
//      https://github.com/Neargye/magic_enum/blob/master/include/magic_enum.hpp
// (but simplified so its works in C++11)
// use 'print_enum_info<E>()' to test the features, for any enum type 'E'
//
// Features, limitations:
//
// * works with -std=c++11
// * works for both classic enums and enum class.
// * max number of values in the enum cannot be larger than 'max_enums' constant (defined as 256)
// * doesn't work if you explicitly assign integral values for representing the enums.
//   (it only works fine for consecutive integers starting at 0)
// * only works for compilers with suitable __PRETTY_FUNCTION__ macro (MSVC has an equivalent one)
//   (that is, clang++ and g++)
//
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef ENUMS_TOOL_H
#define ENUMS_TOOL_H


#include <vector>
#include <string>
#include <iostream>
#include <cassert>

#   if defined(__clang__)
#      define COMPILER_STR "LLVM clang++"
#   elif defined(__GNUC__)
#      define COMPILER_STR "GNU g++"
#   elif defined(_MSC_VER)
#      define COMPILER_STR "Microsoft Visual C"
#   endif

// *************************************************************************************************
// definitions

namespace enums_tool
{

// *************************************************************************************************
// implementation definitions (not to be used directly)

namespace implementation
{
constexpr int max_enums = 255 ; // for some reason, cannot be larger than 255 in 'godbolt' ?

// -------------------------------------------------------------------------------------------------
template <typename E> constexpr const char * ce_raw_pf_str() noexcept
{
#  if defined(__clang__)
     return __PRETTY_FUNCTION__ ;
#  elif defined(__GNUC__)
     return __PRETTY_FUNCTION__ ;
#  elif defined(_MSC_VER)
     return __FUNCSIG__ ;
#  endif
}
// -------------------------------------------------------------------------------------------------

template <typename E, E value> constexpr const char * ce_raw_pf_str() noexcept
{
#  if defined(__clang__)
      return __PRETTY_FUNCTION__ ;
#  elif defined(__GNUC__)
      return __PRETTY_FUNCTION__ ;
#  elif defined(_MSC_VER)
      return __FUNCSIG__ ;
#  endif
}
// -------------------------------------------------------------------------------------------------

constexpr int ce_length_rec( const char * str, int cur_len )
{
   return *str == 0  ? cur_len : ce_length_rec( str+1, cur_len+1 ) ;
}
// -------------------------------------------------------------------------------------------------

constexpr int ce_length( const char *str )
{
   return str == nullptr ? 0 : ce_length_rec( str, 0 ) ;
}
// -------------------------------------------------------------------------------------------------

constexpr int ce_rfind_rec( const char *str_cur, const char *sub_cur, const char * sub_beg,
                            const int str_cur_idx, const int str_match_idx )
{
   return
      *str_cur == 0 || *sub_cur == 0 ?
         str_match_idx
      : *str_cur == *sub_cur ?
         ce_rfind_rec(  str_cur+1, sub_cur+1, sub_beg, str_cur_idx+1,
                        str_match_idx >= 0 ? str_match_idx : str_cur_idx )
      :
         ce_rfind_rec( str_cur+1, sub_beg, sub_beg, str_cur_idx+1,  -1 ) ;
}
// -------------------------------------------------------------------------------------------------

constexpr int ce_rfind( const char *str, const char *sub )
{
   return str == nullptr || sub == nullptr ? -1 : ce_rfind_rec( str, sub, sub, 0, 0 );
}
// -------------------------------------------------------------------------------------------------

template <typename E>  constexpr const char * ce_string_after( const char * sub )
{
#  define PTR0  (ce_raw_pf_str<E>())
   return PTR0 + ce_rfind( PTR0, sub ) + ce_length( sub );
}
// -------------------------------------------------------------------------------------------------

template <typename E, E value>  constexpr const char * ce_string_after( const char * sub )
{
   return ce_raw_pf_str<E,value>() + ce_rfind( ce_raw_pf_str<E,value>(), sub ) + ce_length( sub );
}
// -------------------------------------------------------------------------------------------------

template <typename E>  constexpr const char * ce_type_name_str()
{
   return ce_string_after<E>( "[E = ") ;
}
// -------------------------------------------------------------------------------------------------

template <typename E, E value>  constexpr const char * ce_value_name_from_enum()
{
#  if defined(__GNUC__)
     return ce_string_after<E,value>( "value = ") ;
#  elif defined(__clang__)
     return ce_string_after<E,value>( ", value = ") ;
#  elif defined(_MSC_VER)
     return ce_string_after<E,value>( ", value = ") ;
#  endif
}
// -------------------------------------------------------------------------------------------------
// returns true iif 'k' is in the range of the enum E

template <typename E, int k> constexpr bool ce_is_in_range(  )
{
#  define PTR ( ce_value_name_from_enum< E, static_cast<E>(k) >() )

#  if defined(__clang__)
         return ! ( '0' <= *PTR &&  *PTR <= '9' ) ;
#  elif defined(__GNUC__)
         return ! ( '(' == *PTR ) ;
#  elif defined(_MSC_VER)
         return false ;  // TODO: check this compiler.....
#  endif
}
// -------------------------------------------------------------------------------------------------

template <typename E, int k> constexpr const char * ce_value_name_from_int(  )
{
    return ce_is_in_range<E,k>()
        ? ce_value_name_from_enum<E, static_cast<E>(k)>()
        : "** out of range ** " ;
}
// -------------------------------------------------------------------------------------------------

template <typename E, int k> struct num_values_rec_str
{
   static constexpr int num_values = ce_is_in_range<E,k>() ? k : num_values_rec_str<E,k-1>::num_values ;
} ;
// -------------------------------------------------------------------------------------------------

template <typename E> struct num_values_rec_str<E,1>
{
   static constexpr int num_values = 1 ;
} ;
// -------------------------------------------------------------------------------------------------

template <typename E> constexpr int ce_num_values()
{
   return 1 + num_values_rec_str<E,max_enums>::num_values ;
}
// -------------------------------------------------------------------------------------------------
// struct with k+1 pointers  to zero-terminated strings

template< typename E, int k > struct ptrs_array_struct
{
   ptrs_array_struct<E,k-1> rem_array ;
   const char * ptr = ce_value_name_from_int<E,k>();
} ;
// -------------------------------------------------------------------------------------------------
// struct with a single pointer to a zero-terminated string
// (specialization of the above struct, for k=0)

template< typename E> struct ptrs_array_struct<E,0>
{
   const char * ptr = ce_value_name_from_int<E,0>();
} ;

} // end of 'implementation' namespace

// *************************************************************************************************
// "public" api:

// -------------------------------------------------------------------------------------------------
// returns the name of the enum type

template<typename E> std::string  type_name_string(  )
{
   using namespace implementation ;

   static constexpr const char * str_ptr     = ce_type_name_str<E>(); // get raw string
   static constexpr int          len         = ce_length( str_ptr ) ;  // get raw string length
   static std::string            name_string = std::string( str_ptr, len-1 ) ; // build string just once

   return name_string ;
}
// -------------------------------------------------------------------------------------------------
// returns the name of a enum value, from an integer.
// (if the integer is out of range, it returns the string '** out of range **')

template<typename E> std::string name_string( const int k )
{
   using namespace implementation ;

   // define 'N', check that 'k' is in range,
   constexpr int N = ce_num_values<E>() ;
   assert( N > 0 );
   if ( N <= k )
      return std::string { "** out of range **" } ;

   // static array with all the processed strings
   static std::vector<std::string> strings_vector { N } ;

   // compute 'strings_vector', but just in the first call
   static bool first_call = true ;

   if ( first_call )
   {
      for( unsigned i = 0 ; i < N ; i++ )
      {
         // declare a compiler-populated static array of pointers to strings
         static ptrs_array_struct<E,N-1> ptrs_array ;

         // get pointer to i-th cero-terminated c-string, by using the i-th pointer in in ptrs_array...
         const char * str_ptr = static_cast<const char *>( *(((void **) &(ptrs_array.rem_array)) +i ));

         // create a 'std::string' from the pointer (without the last character, which is ']')
         std::string value_string =  std::string( str_ptr, ce_length( str_ptr )-1 ) ;

         // remove everything before the last instance of '::', if any
         const auto pos = value_string.rfind( "::" ) ;
         if (  pos != std::string::npos )
            value_string = value_string.substr( pos+2 );

         // add to string_vector
         strings_vector[i] = value_string ;

      }
      // no longer the first call..
      first_call = false ;
   }

   // done:
   return strings_vector[k] ;
}
// -------------------------------------------------------------------------------------------------
// returns the name of an enum value

template<typename E> std::string  name_string( const E v )
{
   return name_string<E>( static_cast<int>(v) );
}
// -------------------------------------------------------------------------------------------------
// returns num of values in the enumerated type

template<typename E> constexpr int num_of_values()
{
   using namespace implementation ;
   return ce_num_values<E>() ;
}
// -------------------------------------------------------------------------------------------------
// returns first value

template<typename E> constexpr E first()
{
   return static_cast<E>(0) ;
}
// -------------------------------------------------------------------------------------------------
// returns last value

template<typename E> constexpr E last()
{
   using namespace implementation ;
   return static_cast<E>( ce_num_values<E>()-1 );
}
// -------------------------------------------------------------------------------------------------
// returns next value, possibly yielding an out-of-range value

template<typename E> constexpr E next( const E v )
{
   return static_cast<E>( (static_cast<int>(v)) + 1 ) ;
}
// -------------------------------------------------------------------------------------------------
// returns next value (if given the last one, it returns the first)

template<typename E> constexpr E next_mod( const E v )
{
   return static_cast<E>( ((static_cast<int>(v)) + 1) % num_of_values<E>() ) ;
}
// -------------------------------------------------------------------------------------------------
// increases and returns next value (if given the last one, it returns an out of range enum)
// (postfix version)

template<typename E> E operator ++ ( E & v, int )
{
   const E org = v ;
   v = next(v) ;
   return org ;
}
// -------------------------------------------------------------------------------------------------
// increases and returns next value (if given the last one, it returns an out of range enum)
// prefix version

template<typename E> E& operator ++ ( E & v  )
{
   v = next(v) ;
   return v ;
}
// -------------------------------------------------------------------------------------------------
// shows typename, num of values, and each value name, for a given enum type (just for testing)
// (just for debug)

template<typename E> void print_enum_info()
{
   using namespace std ;

   cout << "name of enum type    = " << type_name_string<E>() << endl
        << "num of values        = " << num_of_values<E>() << endl
        << "list of values names = " << endl ;

   for ( E v = first<E>() ; v <= last<E>() ; v++  )
   {
      cout << "   " << name_string(v) << endl ;
   }

}
// -------------------------------------------------------------------------------------------------


void print_compiler_info()
{
   using namespace std ;
   cout  << "--------------------------------" << endl
         << "Compiler: " << COMPILER_STR << endl
         << "-------------------------------" << endl ;
}

} // end namespace enum_tools

#endif // ENUM_TOOLS_H
