/*$Id: iceb_poldan.c,v 1.11 2014/02/28 05:21:03 sasa Exp $*/
/*20.02.2014	27.09.2009	Белых А.И. 	iceb_poldan.c
Поиск настройки в файле и получение значения принадлежащее этой настройке
*/
#include "iceb_libbuh.h"






int iceb_poldan(const char *strpoi,class iceb_u_str *find_dat,const char *imaf,
int metka_ps, /*0-складывать продолжения без перевода строки 1- с переводом*/
GtkWidget *wpredok)
{
char strsql[strlen(strpoi)+1024];
SQL_str row;
class iceb_u_str prom("");
class SQLCURSOR cur;
find_dat->new_plus("");
int kol=0;
sprintf(strsql,"select ns,str from Alx where fil='%s' and str like '%s|%%'",imaf,strpoi);

if((kol=iceb_sql_readkey(strsql,&row,&cur,wpredok)) <= 0)
 {
/**************
  class iceb_u_str repl(strsql);  
  sprintf(strsql,"%s\n%s\n%s",gettext("Ненайдена настройка!"),strpoi,imaf);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
**************/
  return(1);  
 }

if(kol > 1)
 {
  sprintf(strsql,"%s-%s %s\n%s\n%s %d",__FUNCTION__,gettext("Имя файла"),imaf,strpoi,gettext("Количество настроек"),kol);
  iceb_menu_soob(strsql,wpredok);
 }


if(iceb_u_polen(row[1],&prom,2,'|') == 0)
 {
  if(iceb_u_polen(prom.ravno(),find_dat,1,'\\') != 0)
   {
    find_dat->new_plus(prom.ravno());
    return(0);
   }
  int otn=1;  
  int ns=atoi(row[0]);
  while(prom.ravno()[strlen(prom.ravno())-otn] == '\\')
   {
    sprintf(strsql,"select ns,str from Alx where fil='%s' and ns=%d",imaf,++ns);

    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
      return(0);    

    if(row[1][0] == '#')
     continue;

    prom.new_plus(row[1]);

    class iceb_u_str prom1("");

    if(iceb_u_polen(prom.ravno(),&prom1,1,'\\') != 0)
     {
      int dlin=strlen(prom.ravno())-1;
      if(prom.ravno()[dlin] == '\n')
       prom.udsimv("\n");
//       prom[dlin]='\0';
      find_dat->plus(prom.ravno());
      return(0);
     }
    find_dat->plus(prom1.ravno());
    otn=2;
    if(metka_ps == 1)
     find_dat->plus("\n");
   }
  return(0);
 }


return(1);


}
/*********************************/
int iceb_poldan(const char *strpoi,class iceb_u_str *find_dat,const char *imaf,GtkWidget *wpredok)
{
return(iceb_poldan(strpoi,find_dat,imaf,0,wpredok));
}

/********************************/

int iceb_poldan(const char *strpoi,char *find_dat,const char *imaf,GtkWidget *wpredok)
{
class iceb_u_str str("");
find_dat[0]='\0';

if(iceb_poldan(strpoi,&str,imaf,wpredok) != 0)
 return(1);

strcpy(find_dat,str.ravno());
return(0);
}
/********************************/

int iceb_poldan(const char *strpoi,int *find_dat,const char *imaf,GtkWidget *wpredok)
{
class iceb_u_str str("");
*find_dat=0;

if(iceb_poldan(strpoi,&str,imaf,wpredok) != 0)
 return(1);

*find_dat=str.ravno_atoi();
return(0);
}
/********************************/

int iceb_poldan(const char *strpoi,short *find_dat,const char *imaf,GtkWidget *wpredok)
{
class iceb_u_str str("");
*find_dat=0;

if(iceb_poldan(strpoi,&str,imaf,wpredok) != 0)
 return(1);

*find_dat=str.ravno_atoi();
return(0);
}
/****************************/
int iceb_poldan(const char *strpoi,double *find_dat,const char *imaf,GtkWidget *wpredok)
{
class iceb_u_str str("");
*find_dat=0.;

if(iceb_poldan(strpoi,&str,imaf,wpredok) != 0)
 return(1);

*find_dat=str.ravno_atof();
return(0);
}

