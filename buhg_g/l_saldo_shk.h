/*$Id: l_saldo_shk.h,v 1.6 2014/08/31 06:19:21 sasa Exp $*/
/*17.08.2014	30.12.2003	Белых А.И.	l_saldo_shk.h
Класс для поиска записей в списке сальдо по счетам
*/
class  saldo_shk_poi
 {
  public:
   class iceb_u_str god;
   class iceb_u_str shet;
   class iceb_u_str kodkontr;
   class iceb_u_str debet;
   class iceb_u_str kredit;
   class iceb_u_str naim_kontr;

   short metka_poi;

   void clear_zero()      
    {
     god.new_plus("");  
     shet.new_plus("");  
     kodkontr.new_plus("");  
     debet.new_plus("");  
     kredit.new_plus("");  
     naim_kontr.new_plus("");
    }

   saldo_shk_poi()
    {
     metka_poi=0;
    }
 };
