/*$Id: zvb_pibw.c,v 1.19 2013/08/13 06:10:05 sasa Exp $*/
/*13.07.2015	19.07.2005	Белых А.И.	zvb_pibw.c
*/
#include        <errno.h>
#include    "buhg_g.h"
#include "l_nahud.h"

extern int      kol_strok_na_liste;
extern SQL_baza bd;

void	zvb_pibw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);
SQL_str		row,row1;
class iceb_u_str fio("");
double		sumai=0.,sum=0.;;
char            imafkartr[64];
char		imafkartf[64];
FILE            *ffkartr=NULL; //Распечатка выгрузки на картсчета
FILE		*ffkartf=NULL; //Файл выгрузки на дискету
int		kolstrkartr=0;
double          sumalist=0.;
char		strsql[512];
short		d,m,g;
int		kolstr=0;
class iceb_u_str bros("");
int		kolz=0;
class iceb_u_str bankshet("");
double          itogo=0.,itogoo=0.;
int		nomer=0;
class iceb_u_str inn("");
int		kolstrok=0;
class iceb_u_str rukov("");
class iceb_u_str glavbuh("");

SQLCURSOR curr;

iceb_poldan("Табельный номер руководителя",&bros,"zarnas.alx",wpredok);
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    rukov.new_plus(row1[0]);
 } 

iceb_poldan("Табельный номер бухгалтера",&bros,"zarnas.alx",wpredok);

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    glavbuh.new_plus(row1[0]);
 }


//printw("%s\n",strsql);
//OSTANOV();

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }


sprintf(imafkartr,"kartr%d.lst",getpid());
sprintf(imafkartf,"kartf%d.txt",getpid());



if((ffkartr = fopen(imafkartr,"w")) == NULL)
 {
  iceb_er_op_fil(imafkartr,"",errno,wpredok);
  return;
 }

hrvnks(ffkartr,&kolstrkartr);

if((ffkartf = fopen(imafkartf,"w")) == NULL)
 {
  iceb_er_op_fil(imafkartf,"",errno,wpredok);
  return;
 }



sumai=0.;
float kolstr1=0.;
int kol_na_liste=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  fio.new_plus(row[15]);  

   
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  sum=atof(row[4]);
  if(sum == 0.)
    continue;
  if(prn == 2)
    sum*=-1;
  
  


  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),row[1]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }
  inn.new_plus(row1[2]);
  bankshet.new_plus(row1[1]);
  if(bankshet.ravno()[0] == '\0')
   {
    class iceb_u_str repl;
    repl.plus(gettext("Не введён карт-счёт !"));

    sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),row[1],fio.ravno());
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    continue;
   }



  kolz++;
 
  pib_itlw(&kolstrkartr,&sumalist,&kol_na_liste,rukov.ravno(),glavbuh.ravno(),ffkartr);

  sumalist+=sum;
  pib_strw(kolz,bankshet.ravno(),sum,fio.ravno(),inn.ravno(),ffkartr);
  
  kol_na_liste++;    
  fprintf(ffkartf,"%-16s %8.2f %s\n",bankshet.ravno(),sum,fio.ravno());

  kolstrok+=2;
  sumai+=sum;
  itogo+=sum; 
  itogoo+=sum; 
  nomer++;
 }

pib_end_dokw(kol_na_liste,sumalist,itogoo,nomer,rukov.ravno(),glavbuh.ravno(),ffkartr);

fclose(ffkartr);

fclose(ffkartf);

//Записываем шапку и концовку
hdisk(nomer,itogoo,imafkartf);
  
class iceb_u_spisok imafr;
class iceb_u_spisok naimf;
imafr.plus(imafkartr);
imafr.plus(imafkartf);
naimf.plus(gettext("Ведомость для зачисления на карт-счёта."));
naimf.plus(gettext("Файл для передачи в банк."));

gdite.close();

iceb_ustpeh(imafr.ravno(0),1,wpredok);

iceb_rabfil(&imafr,&naimf,wpredok);

}
