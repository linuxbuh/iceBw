/*$Id: zar_sprav.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*16.01.2019	26.05.2016	Белых А.И.	sprtabotpw.h
Реквизиты для расчёта отпускных
*/
class sprtabotp_poi
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str tabnom;
   short mn,gn;
   short dk,mk,gk;
   class iceb_u_str vid_otp;
   class iceb_u_str prikaz;
   class iceb_u_str period;
   class iceb_u_str kol_dnei;
   class iceb_u_str kod_podr;
   int metka_ri;  /*0-расчёт с использванием дожностных оклатдов 1-расчёт по коэффициентам введённым в файл*/

   class iceb_u_str kodnvr; //Коды не входящие в расчёт отпускных
   class iceb_u_str kodnvrind; //Коды не входящие в расчёт индексации отпускных
   class iceb_u_str kodt_nvr; //Коды табеля не входящие в расчёт
   class iceb_u_str kodnvr_d; //Дополнительные коды не входящие в расчёт
   class iceb_u_str kodn_d_dk; //Коды начислений для дополнительных кодов
   int kolmes; /*количесво месяцев в заданном периоде*/




   class iceb_u_str fio;
   
   sprtabotp_poi()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    tabnom.new_plus("");
    vid_otp.new_plus("");
    prikaz.new_plus("");
    period.new_plus("");
    kol_dnei.new_plus("");
    kod_podr.new_plus("");
    dk=mn=gn=mk=gk=0;
    metka_ri=0;
    fio.new_plus("");

    kodnvr.new_plus(""); //Коды не входящие в расчёт отпускных
    kodnvrind.new_plus(""); //Коды не входящие в расчёт индексации отпускных
    kodt_nvr.new_plus(""); //Коды табеля не входящие в расчёт
    kodnvr_d.new_plus(""); //Дополнительные коды не входящие в расчёт
    kodn_d_dk.new_plus(""); //Коды начислений для дополнительных кодов
    kolmes=0;
   }
 };
