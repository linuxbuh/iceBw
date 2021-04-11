/*$Id: usldupkw.h,v 1.3 2011-01-13 13:49:53 sasa Exp $*/
/*08.02.2015	08.02.2015	Белых А.И.	usldupkw.h
Реквизиты для расчёта списания/получения материалов по контрагентам
*/

class usldupkw_rr
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str kontr;
   class iceb_u_str grup_usl;
   class iceb_u_str kodop;
   class iceb_u_str kodusl;
   class iceb_u_str shetu;
   class iceb_u_str nomdok;
   int prih_rash; /*0-всё 1-приходы 2-расходы*/
   

   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   usldupkw_rr()
    {
     clear_data();
    }
  
   void clear_data()
    {
     datan.new_plus("");
     datak.new_plus("");
     podr.new_plus("");
     kontr.new_plus("");
     grup_usl.new_plus("");
     kodop.new_plus("");
     kodusl.new_plus("");
     shetu.new_plus("");
     nomdok.new_plus("");
     prih_rash=0;
     
    }

 }; 
