/*$Id: uosinvedw.h,v 1.2 2011-01-13 13:49:54 sasa Exp $*/
/*04.10.2017	13.01.2008	Белых А.И.	uosinvedw.h
Реквизиты для расчёта инвентаризационной ведомости основных средств
*/

class uosinvedw_data
 {
  public:
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str mat_ot;
   class iceb_u_str shetu;
   class iceb_u_str innom;
   int metka_bal_st; /*0-балансовая стоимость бух.учёта 1-бал.стоимость налогового учёта*/
   int metka_fn; /*0-заполнять колонки "Фактическое наличие" 1-не заполнять*/      
   
   uosinvedw_data()
    {
     clear();
    }   

   void clear()
    {
     metka_fn=0;
     metka_bal_st=0;
     podr.new_plus("");
     shetu.new_plus("");
     datak.new_plus("");
     mat_ot.new_plus("");
     innom.new_plus("");
    }

 };
