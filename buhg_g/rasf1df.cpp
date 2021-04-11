/* $Id: rasf1df.c,v 1.25 2013/08/13 06:09:50 sasa Exp $ */
/*01.01.2018	13.07.2000	Белых А.И.	rasf1df.c
Вигрузка в *.dbf или *.lst файл формы 8ДР
*/
#include        <errno.h>
#include        "buhg_g.h"
#include        "dbfhead.h"

int  rasf1df_m(class iceb_u_str *kolih_v_htate,class iceb_u_str *kolih_po_sovm, GtkWidget *wpredok);


#define         KSNL 63 /*Количество строк на листе*/
#define		SH1L 34 /*Шапка первого листа*/
#define		SHPL 6 /*Шапка последующих листов*/

extern SQL_baza bd;


int rasf1df(const char *nomd,
int metka_oth, //0-звітний 1-новий звітний 2-уточнюючий
class iceb_u_spisok *imafils,
class iceb_u_spisok *naimfils,
GtkWidget *wpredok)
{

int		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_u_str inn("");
double		sumad,suman,isumad,isuman;
double		sumadn,sumann,isumadn,isumann;
class iceb_u_str priz("");
short		lgota=0;
class iceb_u_str lgotach("");
char		data1[32];
char		data2[32];
char		data11[32]; /*Дата для dbf файла*/
char		data21[32]; /*Дата для dbf файла*/
char		data12[32]; /*дата для xml файла*/
char		data22[32]; /*дата для xml файла*/
int		npp1;
char		imaf[64],imafdbf[64],imaffam[64];
FILE		*ff1=NULL,*ff4=NULL,*fffam=NULL;
short		d,m,g;
int		kolr=0; /*Количество физических лиц*/
int		klst=0; /*Количество строк*/
int		klls=1; /*Количество листов*/
short		god;
int		kvrt;
int		vidd;
class iceb_u_str bros("");
class iceb_u_str kod("");
short		TYP; /*Метка 0-юридическая особа 1-физическая особа*/
class iceb_u_str priznak("");
SQLCURSOR curr;
class iceb_u_str kolih_v_htate("");
class iceb_u_str kolih_po_sovm("");


/*Читаем шапку документа*/

sprintf(strsql,"select * from F8dr where nomd='%s'",nomd);
if(sql_readkey(&bd,strsql,&row,&curr) < 1)
 {
  iceb_menu_soob(gettext("Не найден документ !"),wpredok);
  return(1);
 } 
god=atoi(row[0]);
kvrt=atoi(row[1]);
vidd=atoi(row[3]);
iceb_refresh();
kolih_v_htate.new_plus(row[10]);
kolih_po_sovm.new_plus(row[11]);

sprintf(imafdbf,"da0000%02d.%d",1,kvrt);
if(iceb_poldan("Имя DBF файла",&bros,"zarsdf1df.alx",wpredok) == 0)
  sprintf(imafdbf,"%s%02d.%d",bros.ravno(),1,kvrt);



sprintf(strsql,"select inn,sumad,suman,priz,datap,datau,lgota,fio,sumadn,sumann,pr \
from F8dr1 where nomd='%s' order by inn asc,pr desc",nomd);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
  return(1);
 }
iceb_refresh();
class iceb_u_str naim_kontr("");
class iceb_u_str telef_kontr("");

TYP=0;
sprintf(strsql,"select naimkod,kod,telef from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  naim_kontr.new_plus(row[0]);
  kod.new_plus(row[1]);
  telef_kontr.new_plus(row[2]);
 }

if(iceb_poldan("Физическое лицо",&bros,"zarsdf1df.alx",wpredok) == 0)
 if(iceb_u_SRAV(bros.ravno(),"Вкл",1) == 0)
  {
   TYP=1;
  }


if(rasf1df_m(&kolih_v_htate,&kolih_po_sovm,wpredok) != 0)
 return(1);

sprintf(imaf,"dov%d_%d.lst",kvrt,getpid());
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }

sprintf(imaffam,"dovf%d_%d.lst",kvrt,getpid());
if((fffam = fopen(imaffam,"w")) == NULL)
 {
  iceb_er_op_fil(imaffam,"",errno,wpredok);
  return(1);
 }
//Заголовок распечаток

zagf1dfw(kolih_v_htate.ravno_atoi(),kolih_po_sovm.ravno_atoi(),kvrt,kod.ravno(),TYP,&klst,vidd,0,0,god,0,0,0,metka_oth,ff1,fffam,wpredok);

spkvdw(&klst,ff1);



creatheaddbfw(imafdbf,kolstr+3,wpredok);
if((ff4 = fopen(imafdbf,"a")) == NULL)
 {
  iceb_er_op_fil(imafdbf,"",errno,wpredok);
  return(1);
 }


char imaf_xml[50];
memset(imaf_xml,'\0',sizeof(imaf_xml));
FILE *ff_xml=NULL;
f1df_xml_zagw(kvrt,god,kod.ravno(),TYP,metka_oth,kolih_v_htate.ravno_atoi(),kolih_po_sovm.ravno_atoi(),naim_kontr.ravno(),imaf_xml,&ff_xml,wpredok);

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);

isumad=isuman=0.;
isumadn=isumann=0.;
npp1=0;
float kolstr1=0.;
class iceb_u_spisok fiz_lico;
int nomer_xml_str=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  if(fiz_lico.find(row[1]) < 0)
   fiz_lico.plus(row[1]);
  
  inn.new_plus(row[0]);

  sumad=atof(row[1]);
  sumad=iceb_u_okrug(sumad,0.01);

  suman=atof(row[2]);
  suman=iceb_u_okrug(suman,0.01);

  sumadn=atof(row[8]);
  sumadn=iceb_u_okrug(sumadn,0.01);

  sumann=atof(row[9]);
  sumann=iceb_u_okrug(sumann,0.01);

  isumad+=sumad;
  isuman+=suman;

  isumadn+=sumadn;
  isumann+=sumann;

  priz.new_plus(row[3]);

  d=m=g=0;
  iceb_u_rsdat(&d,&m,&g,row[4],2);
  memset(data1,'\0',sizeof(data1));
  memset(data11,'\0',sizeof(data11));
  memset(data12,'\0',sizeof(data12));
  if(d != 0)
   {
    sprintf(data1,"%04d%02d%02d",g,m,d);
    sprintf(data11,"%02d.%02d.%4d",d,m,g);
    sprintf(data12,"%02d%02d%4d",d,m,g);
   }
  d=m=g=0;
  iceb_u_rsdat(&d,&m,&g,row[5],2);
  memset(data2,'\0',sizeof(data2));
  memset(data21,'\0',sizeof(data21));
  memset(data22,'\0',sizeof(data22));
  if(d != 0)
   {
    sprintf(data2,"%04d%02d%02d",g,m,d);
    sprintf(data21,"%02d.%02d.%04d",d,m,g);
    sprintf(data22,"%02d%02d%04d",d,m,g);
   }

  lgota=atoi(row[6]);
  if(lgota != 0)
   sprintf(strsql,"%02d",lgota);  
  else
   memset(strsql,'\0',sizeof(strsql));
  lgotach.new_plus(strsql);

  if(vidd != 0)
    priznak.new_plus(row[10]);
  else
    priznak.new_plus("");
  //Счетчик строк
  ssf8drw(&klst,&klls,ff1);
  npp1++;    

  
  fprintf(ff1,"%-5d|%-10s|%10.2f|%10.2f|%10.2f|%10.2f|  %-4s|%-10s|%-10s|%-10s|%6s|\n",
  npp1,inn.ravno(),sumadn,sumad,sumann,suman,priz.ravno(),data11,data21,lgotach.ravno(),priznak.ravno());

  class iceb_u_str fam("");    
  class iceb_u_str tabn("");

  if(row[7][0] == '\0')
   {
    sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     {
      tabn.new_plus(row1[0]);
      fam.new_plus(row1[1]);
     }
   }
  else
   {
    fam.new_plus(row[7]);      
   }

  fprintf(fffam,"%-5d %-10s %10.2f %10.2f %10.2f %10.2f %-6s %10s %10s %-10s %6s %-5s %s\n",
  npp1,inn.ravno(),sumadn,sumad,sumann,suman,priz.ravno(),data11,data21,lgotach.ravno(),priznak.ravno(),tabn.ravno(),fam.ravno());   

  zapf8drw(npp1,kvrt,god,kod.ravno(),TYP,inn.ravno(),sumadn,sumad,sumann,suman,priz.ravno(),data1,data2,lgota,atoi(row[10]),ff4);

  f1df_xml_strw(&nomer_xml_str,inn.ravno(),sumadn,sumad,sumann,suman,priz.ravno(),data12,data22,lgotach.ravno(),row[10],ff_xml);
  
 }
kolr=fiz_lico.kolih();

/*узнаём военный сбор*/
double nah_dohod_vs=0.;
double vip_dohod_vs=0.;
double nah_vs=0.;
double vip_vs=0.;

sprintf(strsql,"select sfr,svs,sfrv,svsv from F1dfvs where nomd='%s'",nomd);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }


while(cur.read_cursor(&row) != 0)
 {
  nah_dohod_vs+=atof(row[0]);
  nah_vs+=atof(row[1]);
  vip_dohod_vs+=atof(row[2]);
  vip_vs+=atof(row[3]);
    
 }

gdite.close(); //Закрываем окно Ждите
iceb_refresh();


koncf8drw(kvrt,god,kod.ravno(),TYP,isumad,isumadn,isuman,isumann,isumad,isumadn,isuman,isumann,kolr,klls,&npp1,2,nah_dohod_vs,vip_dohod_vs,nah_vs,vip_vs,ff1,fffam,ff4,wpredok);
fclose(ff1);
fclose(fffam);

fputc(26, ff4);
fclose(ff4);


f1df_xml_konw(isumadn,isumad,isumann,isuman,nomer_xml_str,kolr,klls,naim_kontr.ravno(),telef_kontr.ravno(),TYP,nah_dohod_vs,vip_dohod_vs,nah_vs,vip_vs,ff_xml,wpredok);

imafils->plus(imaf);
naimfils->plus(gettext("Распечатка формы 1ДФ (для сдачи в налоговую)"));

imafils->plus(imaffam);
naimfils->plus(gettext("Распечатка формы 1ДФ (c фамилиями)"));

for(int nom=0; nom < imafils->kolih(); nom++)
 iceb_ustpeh(imafils->ravno(nom),3,wpredok);

imafils->plus(imafdbf);
naimfils->plus(gettext("Форма 1ДФ в dbf формате"));


imafils->plus(imaf_xml);
naimfils->plus(gettext("Форма 1ДФ в XML формате"));


return(0);
}
