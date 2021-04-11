/*$Id: zar_f1df.h,v 1.3 2012-11-29 09:53:03 sasa Exp $*/
/*17.05.2018	20.12.2006	Белых А.И.	zar_f1df.h
Реквизиты для расчёта формы 1ДФ
*/
class zar_f1df_rek
 {
  public:
//   class iceb_u_str kvart;
   class iceb_u_str kvrt;
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str tabnom;
   class iceb_u_str podr;
      
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_f1df_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    podr.new_plus("");
    kvrt.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    tabnom.new_plus("");
   }
 };
