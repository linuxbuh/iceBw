/*$Id: l_saldo_sh.h,v 1.10 2011-01-13 13:49:50 sasa Exp $*/
/*04.02.2016	30.12.2003	Белых А.И.	l_saldo_sh.h
Класс для поистк стартового сальдо по счетам со свётрнутым сальдо
*/

class  saldo_sh_poi
 {
  public:
  class iceb_u_str god;
  class iceb_u_str shet;
  class iceb_u_str debet;
  class iceb_u_str kredit;
  short metka_poi;

  void clear_zero()      
   {
    god.new_plus("");  
    shet.new_plus("");  
    debet.new_plus("");  
    kredit.new_plus("");  
   }
  saldo_sh_poi()
   {
    clear_zero();
    metka_poi=0;
   }
 };
 
