/*$Id: iceb_getkue.h,v 1.1 2013/09/26 09:47:04 sasa Exp $*/
/*05.09.2013	03.09.2013	Белых А.И.	iceb_t_getkue.h
получение cуммы в валюте
*/

class iceb_getkue_dat
 {
  public:
   GtkWidget *wpredok;
   class iceb_u_str dataz;
   class iceb_u_str kod_val;
   double kurs_ue;

   iceb_getkue_dat(const char *kodval,GtkWidget *predok)
    {
     kod_val.plus(kodval);
     kurs_ue=1.;
     dataz.plus("");
     wpredok=predok;
    }
   double fromue(const char *suma,const char *data);
   double toue(const char *suma,const char *data);
   double get(const char *suma,const char *data,int metka_r);
 };