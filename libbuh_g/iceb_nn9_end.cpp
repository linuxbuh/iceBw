/*$Id:$*/
/*22.03.2017	16.03.2016	Белых А.И.	iceb_nn9_end.c
Распечатка концовки налоговой накладной 
*/

#include "iceb_libbuh.h"

extern SQL_baza bd;

void iceb_nn9_end(int tipz, 
short lnds,
const char *imaf_nastr,
const char *kodop,
const char *kod_kontr,
FILE *ff,
GtkWidget *wpredok)
{
class iceb_u_str stroka("");
int nomer_st=0;
class SQLCURSOR cur_alx;
SQL_str row_alx;
int kolstr=0;
char strsql[512];
class iceb_u_str fio("");
class iceb_u_str inn_op("");

fio.new_plus(iceb_getfioop(wpredok));/*берём фамилию оператора на случай если не определим фамилию по табельному номеру*/

/*Узнаём индивидуальный налоговый номер оператора*/
sprintf(strsql,"select tn from icebuser where login='%s'",iceb_u_getlogin());
if(iceb_sql_readkey(strsql,&row_alx,&cur_alx,wpredok) == 1)
 {
  if(atoi(row_alx[0]) != 0)
   {
    class iceb_fioruk_rk rp;
    if(iceb_get_rtn(atoi(row_alx[0]),&rp,wpredok) == 0)
     {
      inn_op.new_plus(rp.inn.ravno());
      fio.new_plus(rp.fio.ravno());
     }
   }
 }


sprintf(strsql,"select str from Alx where fil='nn9_end.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nn9u_end.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
     continue;
  nomer_st++;
  stroka.new_plus(row_alx[0]);
  switch (nomer_st)
   {  

    case 5: /*Фамилия исполнителя*/
      if(tipz == 1)
        break;

      iceb_u_vstav(&stroka,fio.ravno(),33,86,1);
      iceb_u_vstav(&stroka,inn_op.ravno(),91,120,2);
          
      break;

    case 13: /*пункти налогового кодекса которыми пердусмотрено освобождение от НДС*/
      if(lnds == 3) //Освобождение от НДС
       {
        class iceb_u_str punkt_zak(""); //Пункт закона на основании которого есть освобождение от НДС
        class iceb_u_str naim_nast("Пункт закона освобождения от НДС для операции");
        naim_nast.plus(" ",kodop);

        iceb_poldan(naim_nast.ravno(),&punkt_zak,imaf_nastr,wpredok);

        if(punkt_zak.ravno()[0] != '\0')
         {        
          iceb_u_vstav(&stroka,punkt_zak.ravno(),3,154,1);
         }      
       } 
       break;
   }
 fprintf(ff,"%s",stroka.ravno());
}
fprintf(ff,"%s\n",kod_kontr);

if(iceb_poldan_vkl("Системная подпись в налоговой накладной","matnast.alx",wpredok) == 0)
 {
  iceb_podpis(ff,wpredok);
 }

fclose(ff);

}
