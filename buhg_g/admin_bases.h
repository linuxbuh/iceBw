/*$Id: admin_bases.h,v 1.1 2012-11-29 09:53:01 sasa Exp $*/
/*20.11.2012	20.11.2012	Белых А.И.	admin_bases.h
поиск
*/
class admin_bases_poi
 {
  public:
   class iceb_u_str imabaz;
   class iceb_u_str naim00;
   int metka_poi;
   admin_bases_poi()
    {
     metka_poi=0;
     clear();
    }
   void clear()
    {
     imabaz.new_plus("");
     naim00.new_plus("");
    }
 };
 