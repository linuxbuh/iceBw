/*$Id:$*/
/*22.09.2015	22.09.2015	Белых А.И.	iceb_vstavfil.h
Структрура с данными для вставки в строки
*/

class iceb_vstavfil_rk
 {
  public:
   class iceb_u_int nom_str;
   class iceb_u_int hag;
   class iceb_u_int poz_begin;
   class iceb_u_int poz_end;
   class iceb_u_spisok str;

   void clear()
    {
     nom_str.free_class();
     hag.free_class();
     poz_begin.free_class();
     poz_end.free_class();
     str.free_class();
    }
   void zap(int nom_strv,int hagv,int poz_beginv,int poz_endv,const char *strv)
    {
     nom_str.plus(nom_strv);
     hag.plus(hagv);
     poz_begin.plus(poz_beginv);
     poz_end.plus(poz_endv);
     str.plus(strv);     
    }
 };