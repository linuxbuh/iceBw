/*$Id: zarrnesvw.c,v 1.9 2014/01/31 12:13:00 sasa Exp $*/
/*13.07.2015	26.01.2011	Белых А.И.	zarrnesvw.c
Чтение настроек для расчёта единого сочиального взноса
*/
#include "buhg_g.h"

int kod_esv=0; /*Код удержания единого социального взноса*/
int kod_esv_bol=0; /*Код удержания единого социального взноса*/
int kod_esv_bol_inv=0; /*Код удержания единого социального взноса с больничных для инвалидов*/
int kod_esv_inv=0; /*Код удержания единого социального взноса*/
int kod_esv_dog=0; /*Код удержания единого социального взноса*/
int kod_esv_vs=0;  /*код удержания единого социального взноса с военнослужащих*/
class iceb_u_str kods_esv_all; /*Все коды единого социального взноса*/

int kodf_esv=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_bol=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_bol_inv=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_inv=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_dog=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_vs=0; /*Код фонда удержания единого социального взноса*/

class iceb_u_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
class iceb_u_str kod_zv_gr; /*коды званий гражданских*/
class iceb_u_str kod_dop_nah_bol; /*коды начилений дополнительно входящие в расчёт удержания ЕСВ с больничного*/

void zarrnesvw(GtkWidget *wpredok)
{

iceb_poldan("Код удержания единого социального взноса",&kod_esv,"zarnast.alx",wpredok);
iceb_poldan("Код удержания единого социального взноса для больничных",&kod_esv_bol,"zarnast.alx",wpredok);
iceb_poldan("Код удержания единого социального взноса с больничных для инвалидов",&kod_esv_bol_inv,"zarnast.alx",wpredok);
iceb_poldan("Код удержания единого социального взноса для инвалидов",&kod_esv_inv,"zarnast.alx",wpredok);
iceb_poldan("Код удержания единого социального взноса для договоров",&kod_esv_dog,"zarnast.alx",wpredok);
iceb_poldan("Код удержания единого социального взноса для военных",&kod_esv_vs,"zarnast.alx",wpredok);

kods_esv_all.plus(kod_esv);
kods_esv_all.z_plus(kod_esv_bol);
kods_esv_all.z_plus(kod_esv_inv);
kods_esv_all.z_plus(kod_esv_vs);
kods_esv_all.z_plus(kod_esv_bol_inv);

iceb_poldan("Коды начислений не входящие в расчёт удержания единого социального взноса",&knvr_esv_r,"zarnast.alx",wpredok);
iceb_poldan("Коды званий гражданских",&kod_zv_gr,"zarnast.alx",wpredok);
iceb_poldan("Коды начислений дополнительно входящие в расчёт удержания ЕСВ для больничных",&kod_dop_nah_bol,"zarnast.alx",wpredok);

iceb_poldan("Код фонда единого социального взноса",&kodf_esv,"zarnast.alx",wpredok);
iceb_poldan("Код фонда единого социального взноса для инвалидов",&kodf_esv_inv,"zarnast.alx",wpredok);
iceb_poldan("Код фонда единого социального взноса для больничных",&kodf_esv_bol,"zarnast.alx",wpredok);
iceb_poldan("Код фонда единого социального взноса с больничных для инвалидов",&kodf_esv_bol_inv,"zarnast.alx",wpredok);
iceb_poldan("Код фонда единого социального взноса для договоров",&kodf_esv_dog,"zarnast.alx",wpredok);
iceb_poldan("Код фонда единого социального взноса для военных",&kodf_esv_vs,"zarnast.alx",wpredok);

gchar *strsql;
strsql=g_strdup_printf("%s-Прочитали настройки\n\
Код удержания единого социального взноса:%d\n\
Код удержания единого социального взноса для больничных:%d\n\
Код удержания единого социального взноса c больничных для инвалидов:%d\n\
Код удержания единого социального взноса для инвалидов:%d\n\
Код удержания единого социального взноса для договоров:%d\n\
Коды начислений не входящие в расчёт удержания единого социального взноса:%s\n\
Код фонда единого социального взноса:%d\n\
Код фонда единого социального взноса для инвалидов:%d\n\
Код фонда единого социального взноса для больничных:%d\n\
Код фонда единого социального взноса c больничных для инвалидов:%d\n\
Код фонда единого социального взноса для договоров:%d\n\
Код фонда единого социального взноса для военных:%d\n\
Коды званий гражданских:%s\n\
Коды начислений дополнительно входящие в расчёт удержания ЕСВ для больничных:%s\n",
  __FUNCTION__,
  kod_esv,
  kod_esv_bol,
  kod_esv_bol_inv,
  kod_esv_inv,
  kod_esv_dog,
  knvr_esv_r.ravno(),
  kodf_esv,
  kodf_esv_bol,
  kodf_esv_bol_inv,
  kodf_esv_inv,
  kodf_esv_dog,
  kodf_esv_vs,
  kod_zv_gr.ravno(),
  kod_dop_nah_bol.ravno());


zar_pr_insw(strsql,wpredok);
g_free(strsql);

}