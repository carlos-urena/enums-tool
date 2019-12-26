#include <enums_tool.h>

int main( )
{
   enum EnumT  { cero0, second1, the_third123, almost_done, last_the_fourth_4 } ;

   using namespace enums_tool ;
   print_compiler_info();

   using namespace std ;

   cout << "name of enum type    = " << type_name_string<EnumT>() << endl
        << "num of values        = " << num_of_values<EnumT>() << endl
        << "list of values names = " << endl ;

   for ( EnumT v = first<EnumT>() ; v <= last<EnumT>() ; v++  )
      cout << "   " << name_string(v) << endl ;

}
