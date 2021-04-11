/*$Id: uosvamotbuw.h,v 1.3 2011-11-05 09:49:46 sasa Exp $*/
/*05.10.2011	31.12.2007	Белых А.И.	uosvamotbuw.h
Реквизиты для расчёта ведомости движения основных средств
*/

class uosvamotbuw_data
 {
  public:
   class iceb_u_str mesn;
   class iceb_u_str mesk;
   class iceb_u_str god;
   class iceb_u_str grupa;
   class iceb_u_str podr;
   class iceb_u_str mat_ot;
   class iceb_u_str shetu;
   class iceb_u_str hzt;
   class iceb_u_str innom;
   class iceb_u_str hau;
   int sost_ob;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   uosvamotbuw_data()
    {
     clear();
    }   

   void clear()
    {
     sost_ob=0;
     mesn.new_plus("");
     mesk.new_plus("");
     god.new_plus("");
     grupa.new_plus("");
     podr.new_plus("");
     mat_ot.new_plus("");
     shetu.new_plus("");
     hzt.new_plus("");
     innom.new_plus("");
     hau.new_plus("");
    }

 };
