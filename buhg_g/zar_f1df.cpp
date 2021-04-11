/*$Id: zar_f1df.c,v 1.8 2013/08/13 06:10:00 sasa Exp $*/
/*19.11.2009	20.12.2006	Белых А.И.	zar_f1df.c
Расчёт формы 1ДФ
*/
#include "buhg_g.h"
#include "zar_f1df.h"

int zar_f1df_m(class zar_f1df_rek *rek,GtkWidget *wpredok);
int zar_f1df_r(class zar_f1df_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_f1df(GtkWidget *wpredok)
{


static class zar_f1df_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_f1df_m(&data,wpredok) != 0)
 return;

if(zar_f1df_r(&data,wpredok) != 0)
 return;
printf("%s\n",__FUNCTION__);

iceb_rabfil(&data.imaf,&data.naimf,wpredok);

}
