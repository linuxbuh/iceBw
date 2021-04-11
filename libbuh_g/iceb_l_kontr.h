/*$Id: iceb_l_kontr.h,v 1.12 2013/08/13 06:10:10 sasa Exp $*/
/*21.06.2013	03.12.2003	Белых А.И.	iceb_l_kontr.h
*/


class  kontr_rek
 {
  public:
  
  iceb_u_str kodkontr;
  iceb_u_str naimkon;
  iceb_u_str naimp;  //Полное наименование
  iceb_u_str naimban;
  iceb_u_str adres;
  iceb_u_str adresb;
  iceb_u_str kod;
  iceb_u_str mfo;
  iceb_u_str nomsh;
  iceb_u_str innn;
  iceb_u_str nspnds;
  iceb_u_str telef;
  iceb_u_str grup;
  iceb_u_str na;
  iceb_u_str regnom;
  class iceb_u_str gk; /*Город контрагента*/
  int metka_nal; /*0-общая система 1-единый налог*/
  int metka_poi;  
  //Конструктор
  kontr_rek()
   {
    clear_zero();
    metka_poi=0;
   }  


  void clear_zero() //Записать нулевой байт во все переменные
   {
    kodkontr.new_plus("");
    naimkon.new_plus("");
    naimp.new_plus("");
    naimban.new_plus("");
    adres.new_plus("");
    adresb.new_plus("");
    kod.new_plus("");
    mfo.new_plus("");
    nomsh.new_plus("");
    innn.new_plus("");
    nspnds.new_plus("");
    telef.new_plus("");
    grup.new_plus("");
    na.new_plus("");
    regnom.new_plus("");
    gk.new_plus("");
    metka_nal=0;
   }

 };

void kontr_add_columns (GtkTreeView *treeview,const char*,int metka_rk);
int  iceb_l_kontrp(class  kontr_rek *data,GtkWidget*);
int  iceb_l_kontrv(class iceb_u_str *kod_kontr,const char *shetsk,GtkWidget *wpredok);
int  kontr_prov_row(SQL_str row,class kontr_rek *data);

