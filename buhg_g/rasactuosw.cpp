/*$Id: rasactuosw.c,v 1.14 2013/09/26 09:46:53 sasa Exp $*/
/*19.06.2019	20.07.2003	Белых А.И.	rasactuosw.c
Распечатка аста приема-передачи основных средств
*/
#include        <errno.h>
#include	"buhg_g.h"



extern double	okrg1;
extern SQL_baza bd;

void rasactuosw(const char *data_dok, //дата документа
const char *nomdok, //Номер документа
GtkWidget *wpredok)
{
short dd,md,gd;
char		strsql[1024];
SQL_str		row,row1;
SQLCURSOR curr;
int 		kolstr=0;
char		imaf[64];
FILE		*ff=NULL;
class iceb_u_str shetu("");
class iceb_u_str shetz("");
class iceb_u_str naim("");
int		kolstrok=0;
int		kollist=0;

iceb_u_rsdat(&dd,&md,&gd,data_dok,1);

sprintf(strsql,"select innom,bs,bsby from Uosdok1 where datd='%04d-%02d-%02d' and \
nomd='%s'",gd,md,dd,nomdok);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 { 
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),wpredok);
  return;
 } 

class iceb_tmptab tabtmp;
const char *imatmptab={"rasactuosw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(24) not null,\
naim char(112) not null,\
innom char(24) not null,\
bs double(16,6) not null,\
bsby double(16,6) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,wpredok) != 0)
 return;


while(cur.read_cursor(&row) != 0)
 {
  //Читаем запись в таблице переменных данных
  sprintf(strsql,"select shetu from Uosinp where innom=%s and (god < %d or (god=%d  and mes <= %d)) order by god desc,mes desc limit 1",row[0],gd,gd,md);

  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) <= 0)
   {
    char soob[2048];
    sprintf(soob,"%s !\n%s",gettext("Не найдена запись"),strsql);
    iceb_menu_soob(soob,wpredok);
    continue;
   }
  shetu.new_plus(row1[0]);  

  //Читаем наименование основного средства
  
  naim.new_plus("");
  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   naim.new_plus(row1[0]);


  sprintf(strsql,"insert into %s values ('%s','%s',%s,%s,%s)",
  imatmptab,
  shetu.ravno(),naim.ravno_filtr(),row[0],row[1],row[2]);

  iceb_sql_zapis(strsql,1,0,wpredok);    
 }



sprintf(strsql,"select * from %s order by sh asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

sprintf(imaf,"actm%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

hrasactw(&kolstrok,&kollist,ff);


double bsby=0.;
double bs=0.;
double balst=0.;
int  nomerstr=0;
double kolihi=0.;
double sumai=0.;
class iceb_u_str naimshet("");
double kolihshet=0.;
double sumashet=0.;
class iceb_u_str innom("");


while(cur.read_cursor(&row) != 0)
 {
  shetu.new_plus(row[0]);
  naim.new_plus(row[1]);
  innom.new_plus(row[2]);
  bs=atof(row[3]);
  bsby=atof(row[4]);  

  bs=iceb_u_okrug(bs,0.01);
  bsby=iceb_u_okrug(bsby,0.01);
  balst=bsby;

  if(balst == 0.)
   balst=bs;
     
  if(iceb_u_SRAV(shetz.ravno(),shetu.ravno(),0) != 0)
   {

    if(shetz.ravno()[0] != '\0')
     {
      sapkk1ivw(&kolstrok,&kollist,ff);
      sprintf(strsql,"%s %s %s",gettext("Итого по счёту"),shetz.ravno(),naimshet.ravno());
      fprintf(ff,"%*s %10.10g %10.2f\n",iceb_u_kolbait(62,strsql),strsql,kolihshet,sumashet);
     }

    sprintf(strsql,"select nais from Plansh where ns=%s",shetu.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     naimshet.new_plus(row1[0]);
    else
     naimshet.new_plus("");

    sapkk1ivw(&kolstrok,&kollist,ff);
    fprintf(ff,"%20s ******%s %s %s*******\n"," ",gettext("Счёт учета"),shetu.ravno(),naimshet.ravno());

    shetz.new_plus(shetu.ravno());
    kolihshet=0.;
    sumashet=0.;
   }  

  sumai+=balst;
  kolihi+=1;

  sumashet+=balst;
  kolihshet+=1;  

  sapkk1ivw(&kolstrok,&kollist,ff);
  
  fprintf(ff,"%3d|%-*.*s|%-*s|%10.10g|%10.10g|%10.2f|%10s|%10s|%10s|%10s|\n",
  ++nomerstr,
  iceb_u_kolbait(40,naim.ravno()),iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(6,"Шт"),"Шт",
  balst,1.0,balst," "," "," "," ");

  sapkk1ivw(&kolstrok,&kollist,ff);
  fprintf(ff,"%3s %s:%s\n"," ",gettext("Инвентарный номер"),innom.ravno());

 }

krasactw(shetz.ravno(),naimshet.ravno(),kolihshet,sumashet,kolihi,sumai,ff);

iceb_podpis(ff,wpredok);
fclose(ff);


class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf);
naimf.plus(gettext("Акт приёма-передачи основных средств"));
iceb_ustpeh(imaf,3,wpredok);
iceb_rabfil(&imafil,&naimf,wpredok);

}
