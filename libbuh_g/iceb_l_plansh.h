/*$Id: iceb_l_plansh.h,v 1.7 2011-11-18 06:27:50 sasa Exp $*/
/*15.11.2011	02.01.2004	Белых А.И.	iceb_l_plansh.h
*/




class  plansh_rek
 {
  public:
  
  iceb_u_str shet;
  iceb_u_str naim;
  int        tip;
  int        vid;
  int        saldo;
  int        status;
  int        val;  //Пока не используется

  class iceb_u_str kod_subbal;
  //Конструктор
  plansh_rek()
   {
    clear_zero();
   }  


  void clear_zero() //Записать нулевой байт во все переменные
   {
    shet.new_plus("");
    naim.new_plus("");
    val=tip=vid=saldo=status=0;
    kod_subbal.new_plus("");
   }

 };


extern SQL_baza	bd;
