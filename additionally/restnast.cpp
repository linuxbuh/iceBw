/*$Id: restnast.c,v 1.18 2013/08/13 06:10:25 sasa Exp $*/
/*29.10.2010	08.12.2004	Белых А.И.	restnast.c
Чтерие настроечного файла
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "i_rest.h"

short metka_crk=0; //0-расчёт клиентов по подразделениям 1-централизованный
short start_god_rest=0; //Стартовый год в выписке счетов
short startgod; /*Стартовый год просмотров в материальном учёте*/

extern SQL_baza bd;

int restnast()
{
class iceb_u_str stroka1("");
char strsql[512];
int kolstr=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;

start_god_rest=metka_crk=0;

/***************************

iceb_u_str imaf("restnast.alx");
if(iceb_alxout(imaf.ravno(),NULL) != 0)
 return(1);
FILE *ff;
if((ff = fopen(imaf.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imaf.ravno(),gettext("Аварийное завершение работы программы !"),errno,NULL);
  return(1);
 }    


while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
*********************/
sprintf(strsql,"select str from Alx where fil='restnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"restnast.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;


  iceb_u_polen(row_alx[0],&stroka1,1,'|');
  if(iceb_u_SRAV(stroka1.ravno(),"Стартовый год",0) == 0)
   {
    iceb_u_polen(row_alx[0],&stroka1,2,'|');
    start_god_rest=stroka1.ravno_atoi();     
    continue;
   }

  if(iceb_u_SRAV(stroka1.ravno(),"Централизованный расчёт клиентов",0) == 0)
    {
     iceb_u_polen(row_alx[0],&stroka1,2,'|');
     if(iceb_u_SRAV(stroka1.ravno(),"Вкл",1) == 0)
        metka_crk=1;
     else
        metka_crk=0;
     
     continue;
    }

 }
//fclose(ff);
//unlink(imaf.ravno()); 

iceb_poldan("Стартовый год",&startgod,"matnast.alx",NULL);

/*********************
imaf.new_plus("matnast.alx");
if(iceb_alxout(imaf.ravno(),NULL) != 0)
 return(1);
 
if((ff = fopen(imaf.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imaf.ravno(),"",errno,NULL);
  return(0);
 }    

while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  iceb_u_polen(stroka,stroka1,sizeof(stroka1),1,'|');
  if(iceb_u_SRAV(stroka1,"Стартовый год",0) == 0)
   {
//    printf("Стартовый год\n");
    iceb_u_polen(stroka,stroka1,sizeof(stroka1),2,'|');
    startgod=atoi(stroka1);     
    continue;
   }

 }
fclose(ff);
unlink(imaf.ravno()); 
*************************/
return(0);
}

