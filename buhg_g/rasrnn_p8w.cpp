/*$Id: rasrnn_p.c,v 5.96 2014/08/31 06:18:18 sasa Exp $*/
/*16.02.2018	30.08.2005	Белых А.И.	rasrnn_p8w.c
Распечатка реестра полученных налоговых накладных 
*/
#include <math.h>
#include <ctype.h>
#include "buhg_g.h"
#include "xrnn_poiw.h"
#include "rnn_d5w.h"
#define RAZMER_MAS_STR 14


void rasrnn_p_str_xml8(int nomer_str,short d,short m,short g,short dv,short mv,short gv,const char *nomer_dok,const char *naim_kontr,const char *inn,double *suma,const char *viddok,FILE *ff_xml);
void rasrnn_p_xml_itog8(double *itog,double *itog_u,FILE *ff_xml);
int xml_nn_ukr(int *pnd,short dd,short md,short gd,const char *nomdok,const char *nom_nal_nak,short mn,short gn,short mk,int,class iceb_u_str *imaf_xml);
void rasrnn_p_svod8(const char *nomdok,const char *naim_kontr,double *mas_sum,const char *viddok,FILE *ff);
void rasrnn_p_vvrozkor8(int *nomer_str,short dnn,short mnn,short gnn,short dpn,short mpn,short gpn,const char *nomer_dok,const char *naim_kontr,const char *inn,double *stroka_xml_kor,const char *koment,FILE *ff_xml_rozkor);
void rpvnn_pi8(char *vsego,double *itogo,double *itogo_u,FILE *ff);

extern int kol_strok_na_liste;
extern SQL_baza bd;

int rasrnn_p8w(class xrnn_poi *data,
int period_type,
FILE *ff,FILE *ff_prot,FILE *ff_xml,
FILE *ff_dot,FILE *ff_xml_dot,
FILE *ff_xml_rozkor,
FILE *ff_lst_rozkor,
class rnn_d5 *dod5,
GtkWidget *bar,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
char strsql[2048];

short dn,mn,gn;
short dk,mk,gk;
iceb_u_rsdat(&dn,&mn,&gn,data->datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->datak.ravno(),1);

sprintf(strsql,"select datd,nnn,sumd1,snds1,sumd2,snds2,sumd3,snds3,sumd4,snds4,\
inn,nkontr,mi,datdi,nomdi,gr,koment,ko,dvd,vd,nz from Reenn1 order by datd asc,nnn asc");

if(data->datan.getdlinna() > 1 && data->datak.getdlinna() <= 1)
   sprintf(strsql,"select datd,nnn,sumd1,snds1,sumd2,snds2,sumd3,snds3,sumd4,snds4,\
inn,nkontr,mi,datdi,nomdi,gr,koment,ko,dvd,vd,nz from Reenn1 where \
datd >= '%s'  order by datd asc,nnn asc",data->datan.ravno_sqldata());

if(data->datan.getdlinna() > 1 && data->datak.getdlinna() > 1)
   sprintf(strsql,"select datd,nnn,sumd1,snds1,sumd2,snds2,sumd3,snds3,sumd4,snds4,\
inn,nkontr,mi,datdi,nomdi,gr,koment,ko,dvd,vd,nz from Reenn1 where \
datd >= '%s' and datd <= '%s' order by datd asc,nnn asc",data->datan.ravno_sqldata(),data->datak.ravno_sqldata());

SQLCURSOR cur,cur1;
SQL_str row,row1;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("Не найдено ни одной записи полученных налоговых накладных!"));
  iceb_printw(strsql,buffer,view);
  return(1);
 }

double proc_dot=data->proc_dot.ravno_atof();
class iceb_u_str imafalx("rasrnn_p8.alx");
//Вставляем шапку 

iceb_insfil(imafalx.ravno(),ff,wpredok);
if(ff_dot != NULL)
   iceb_insfil(imafalx.ravno(),ff_dot,wpredok);
 
iceb_zagolov(gettext("Реестр полученных налоговых накладных"),data->datan.ravno(),data->datak.ravno(),ff_prot,wpredok);
if(data->kodgr.ravno()[0] != '\0')
 fprintf(ff_prot,"%s:%s\n",gettext("Группа"),data->kodgr.ravno());

fprintf(ff_prot,"\
-------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_prot,"\
  N  |Дата п/в  |Номер н.н.|Сумма по д.|Сумма НДС|M|Груп.|Инд.нал.ном.|Подс.|Дата док. |Номер док.|Наимeнование контрагента|Коментарий\n");
/*
12345 1234567890 1234567890 12345678901 123456789 1 12345 123456789012 12345 1234567890 1234567890 123456789012345678901234
*/
fprintf(ff_prot,"\
-------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_lst_rozkor,"II Коригування податкового кредиту\n\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_lst_rozkor,"\
   |                                 |Податкова накладна,  |Розрахунок коригуван.|Підстава  |       Обсяг         |       Сума          |Обсяг пос-|Сумма податку на до- |\n\
   |    Отримувач (покупець)         |за якою пров.коригув.|кількісних і вартісн.|для кориг.|      поставки       |        ПДВ          |тачання,  |дану вартість з кол. |\n\
   |                                 |                     |показників до под.нак|податкових|      (без ПДВ)      |       (+чи-)        |за яким не|11 або 12, яка була  |\n\
   |-----------------------------------------------------------------------------|зобов'яза.|       (+чи-)        |                     |нарахову- |включена до податков-|\n\
 N |   Найменування     |Індивідуал  |  Дата    |  Номер   | Номер    |Дата отрим|з податку |                     |                     |  вався   |ого кредиту          |\n\
   |(П.І.Б. для фізичних|  ьний      |          |          |          |мання отри|на додану |-------------------------------------------|податок на|---------------------|\n\
   |      осіб)         |подаковий   |          |          |          |мувачем   |вартість  |   за     |   за     |   за     |    за    |  додану  |до рядка  |до рядка  |\n\
   |                    | номер      |          |          |          |(покупцем)|          |основною  |ставкою   |основною  | ставкою  | вартість |10.1.1    |10.1.2    |\n\
   |                    |            |          |          |          |розрахунку|          |ставкою   |   7%%     |  ставкою |    7%%    |          |          |          |\n\
------------------------------------------------------------------------------------------------------------------|--------------------------------|---------------------|\n\
 1 |         2          |     3      |     4    |    5     |    6     |     7    |    8     |     9    |    10    |    11    |    12    |    13    |    14    |    15    |\n\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------|\n");


double sumd[4];
double snds[4];
double sumdok=0.;
double sumnds=0.;

double itogo_ou[RAZMER_MAS_STR];
memset(itogo_ou,'\0',sizeof(itogo_ou));
int nomer_str=0;

int npz=0;

if(npz != 0)
 nomer_str=npz-1;
 
short mz=0;
short d,m,g;
short dd,md,gd;
float kolstr1=0;
int metka_pods;
class iceb_u_str podsis("");
double stroka_xml[RAZMER_MAS_STR];
double stroka_xml_dot[RAZMER_MAS_STR];
memset(stroka_xml,'\0',sizeof(stroka_xml));
memset(stroka_xml_dot,'\0',sizeof(stroka_xml_dot));

class iceb_u_str nomdok("");
class iceb_u_str naim_kontr("");
int nomer_xml_str=0;
int por_nom_dok=0;
int nomer_innn=0;
short dv,mv,gv;
double sumdok1=0.;

double itogo_mes1[RAZMER_MAS_STR];
double itogo_mes2[RAZMER_MAS_STR];
memset(itogo_mes1,'\0',sizeof(itogo_mes1));
memset(itogo_mes2,'\0',sizeof(itogo_mes2));

double itogo_ob1[RAZMER_MAS_STR]; /*общий итог */
double itogo_ob2[RAZMER_MAS_STR]; /*общий итог*/
memset(itogo_ob1,'\0',sizeof(itogo_ob1));
memset(itogo_ob2,'\0',sizeof(itogo_ob2));
#define RAZMER_MAS_KOR 7
double stroka_xml_kor[RAZMER_MAS_KOR];

int korrekt=0;
int nomer_str_rozkor=0;
double itog_rozkor[RAZMER_MAS_KOR];
memset(itog_rozkor,'\0',sizeof(itog_rozkor));
int metka_7=0;
while(cur.read_cursor(&row) != 0)
 {

  iceb_pbar(bar,kolstr,++kolstr1);    

  if(iceb_u_rsdat(&dv,&mv,&gv,row[18],2) != 0)
   {
    iceb_beep();
    sprintf(strsql,"%s! %s\n%s %d",gettext("Не правильно введена дата выписки налоговой накладной"),row[18],gettext("Номер строки"),nomer_str+1);
    iceb_menu_soob(strsql,wpredok);
   }

  if(data->metka_ins == 1) //*только отмеченные
   if(data->nomzap_v.find(atoi(row[20])) < 0)
    continue;
  if(data->metka_ins == 2) //*только не отмеченные
    if(data->nomzap_v.find(atoi(row[20])) >= 0)
     continue;
     
  if(data->datavnn.getdlinna() > 1)
   if(iceb_u_sravmydat(dv,mv,gv,data->datavnn.ravno(),1)  < 0)
    continue;

  if(data->datavnk.getdlinna() > 1)
   if(iceb_u_sravmydat(dv,mv,gv,data->datavnk.ravno(),1)  > 0)
    continue;
      

  if(iceb_u_proverka(data->kodgr.ravno(),row[15],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->viddok.ravno(),row[19],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->kodop.ravno(),row[17],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->naim_kontr.ravno(),row[11],4,0) != 0)
   continue;

   
  if(data->podsistema.ravno()[0] != '\0')
   {
    class iceb_u_str metka_pod("");

    if(row[12][0] == '1')
     metka_pod.new_plus(ICEB_MP_MATU);
    if(row[12][0] == '2')
     metka_pod.new_plus(ICEB_MP_USLUGI);
    if(row[12][0] == '3')
     metka_pod.new_plus(ICEB_MP_UOS);
    if(row[12][0] == '4')
     metka_pod.new_plus(ICEB_MP_UKR);
    if(row[12][0] == '5')
     metka_pod.new_plus(ICEB_MP_GK);

    if(iceb_u_proverka(data->podsistema.ravno(),metka_pod.ravno(),0,0) != 0)
     continue; 
   }
   
  if(iceb_u_proverka(data->nnn.ravno(),row[1],0,0) != 0)
     continue; 

  if(iceb_u_proverka(data->nomdok.ravno(),row[14],0,0) != 0)
     continue; 
  if(iceb_u_proverka(data->inn.ravno(),row[10],4,0) != 0)
   continue;
  if(iceb_u_proverka(data->koment.ravno(),row[16],4,0) != 0)
    continue;

  if(iceb_u_rsdat(&d,&m,&g,row[0],2) != 0)
   {
    iceb_beep();
    sprintf(strsql,"%s! %s\n%s %d",gettext("Не правильно введена дата получения налоговой накладной"),row[0],gettext("Номер строки"),nomer_str+1);
    iceb_menu_soob(strsql,wpredok);
   }
  dd=md=gd=0;
  if(iceb_u_SRAV("0000-00-00",row[13],0) != 0)
  if(iceb_u_rsdat(&dd,&md,&gd,row[13],2) != 0)
   {
    iceb_beep();
    sprintf(strsql,"%s! %s\n%s %d",gettext("Не правильно введена дата документа"),row[13],gettext("Номер строки"),nomer_str+1);
    iceb_menu_soob(strsql,wpredok);
   }

  korrekt=0;  
  if(iceb_u_strstrm(row[19],"У") == 1 || iceb_u_SRAV(row[19],"РКП",0) == 0 || iceb_u_SRAV(row[19],"РКЕ",0) == 0)/*Если в виде документа присутствует буква У - то это коректирующий документ*/
    korrekt=1;

  if(mz != m)
   {
    if(mz != 0)
     {

      rpvnn_pi8(gettext("Всего за месяц"),itogo_mes1,itogo_ou,ff);
      if(ff_dot != NULL)
        rpvnn_pi8(gettext("Всего за месяц"),itogo_mes2,itogo_ou,ff_dot);

      memset(itogo_mes1,'\0',sizeof(itogo_mes1));
      memset(itogo_mes2,'\0',sizeof(itogo_mes2));
      
     }

  
    mz=m;
   }
  sumd[0]=atof(row[2]);
  sumd[0]=iceb_u_okrug(sumd[0],0.01);
  snds[0]=atof(row[3]);
  snds[0]=iceb_u_okrug(snds[0],0.01);

  sumd[1]=atof(row[4]);
  sumd[1]=iceb_u_okrug(sumd[1],0.01);
  snds[1]=atof(row[5]);
  snds[1]=iceb_u_okrug(snds[1],0.01);

  sumd[2]=atof(row[6]);
  sumd[2]=iceb_u_okrug(sumd[2],0.01);
  snds[2]=atof(row[7]);
  snds[2]=iceb_u_okrug(snds[2],0.01);

  
  sumd[3]=atof(row[8]);
  sumd[3]=iceb_u_okrug(sumd[3],0.01);
  snds[3]=atof(row[9]);
  snds[3]=iceb_u_okrug(snds[3],0.01);

  double psnds=sumd[0]*7./100.;
  if(fabs(psnds-snds[0]) < 0.001 and snds[0] != 0.)
   {
    metka_7=1;
   // printw("%s-%.2f-%.2f=%.2f\n",__FUNCTION__,psnds,snds[0],psnds-snds[0]);
//    OSTANOV();
   }
  else
   metka_7=0;

  if(korrekt == 1)
   {
    itogo_ou[0]+=sumd[0];
    itogo_ou[1]=0; /*пока не знаю что это*/
    if(metka_7 == 0)
     {
      itogo_ou[2]+=sumd[0]-snds[0];
      itogo_ou[3]+=snds[0];
     }
    else
     {
      itogo_ou[4]+=sumd[0]-snds[0];
      itogo_ou[5]+=snds[0];
     }

    itogo_ou[6]=0.; /*пока не знаю что это*/
    itogo_ou[7]=0.;

    itogo_ou[8]+=sumd[1]-snds[1];
    itogo_ou[9]+=snds[1];
    itogo_ou[10]+=sumd[2]-snds[2];
    itogo_ou[11]+=snds[2];
    itogo_ou[12]+=sumd[3]-snds[3];
    itogo_ou[13]+=snds[3];
   }  

  metka_pods=atoi(row[12]);

  nomdok.new_plus(row[1]);
  naim_kontr.new_plus(row[11]);

  sumdok1=sumdok=sumd[0]+sumd[1]+sumd[2]+sumd[3];
  sumnds=snds[0]+snds[1]+snds[2]+snds[3];
  
  if(sumnds != 0.)
   if(row[10][0] == '\0' || isdigit(row[10][0]) == 0) /*значит символ не число*/
    {
     iceb_beep();
     class iceb_u_str repl(gettext("Реестр полученных налоговых накладных"));
     
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     repl.ps_plus(strsql);

     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[1]);
     repl.ps_plus(strsql);

     sprintf(strsql,"%s:%s",gettext("Контрагент"),row[11]);
     repl.ps_plus(strsql);

     repl.ps_plus(gettext("Не ввели индивидуальный налоговый номер!"));
     iceb_menu_soob(repl.ravno(),wpredok);
    }  

  if(row[18][0] != '0')
   {
    sprintf(strsql,"select datd from Reenn1 where dvd='%s' and inn='%s' and nnn='%s' and nz != %s",
    row[18],row[10],row[1],row[20]);

    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) > 0)
     {
      iceb_beep();
      sprintf(strsql,"Номер прибуткової податкової накладної %s дата виписки %s\nповторно введён в документ %s IНН %s",
      row[1],iceb_u_datzap(row[18]),iceb_u_datzap(row1[0]),row[10]);
      iceb_menu_soob(strsql,wpredok);
     }
   }

  if(proc_dot != 0.)
   {
    sumdok1=sumdok1*(100.-proc_dot)/100.;
    sumdok1=iceb_u_okrug(sumdok1,0.01);
   }
  
      
  fprintf(ff,"|%-5d|%02d.%02d.%04d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*.*s|%-*s|%10.2f|",
  ++nomer_str,
  d,m,g,
  dv,mv,gv,
  iceb_u_kolbait(9,nomdok.ravno()),iceb_u_kolbait(9,nomdok.ravno()),nomdok.ravno(),
  iceb_u_kolbait(5,row[19]),iceb_u_kolbait(5,row[19]),row[19],
  iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
  iceb_u_kolbait(14,row[10]),row[10],
  sumdok1);

  if(ff_dot != NULL)
   fprintf(ff_dot,"|%-5d|%02d.%02d.%04d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*.*s|%-*s|%10.2f|",
   nomer_str,
   d,m,g,
   dv,mv,gv,
   iceb_u_kolbait(9,nomdok.ravno()),iceb_u_kolbait(9,nomdok.ravno()),nomdok.ravno(),
   iceb_u_kolbait(5,row[19]),iceb_u_kolbait(5,row[19]),row[19],
   iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
   iceb_u_kolbait(14,row[10]),row[10],
   sumdok-sumdok1);


  memset(stroka_xml,'\0',sizeof(stroka_xml));
  memset(stroka_xml_dot,'\0',sizeof(stroka_xml_dot));

  stroka_xml[0]=sumdok;
  stroka_xml[1]=0.; /*пока не знаю что это*/

  stroka_xml[2]=sumd[0]-snds[0];
  stroka_xml[3]=snds[0];
  stroka_xml[4]=sumd[1]-snds[1];
  stroka_xml[5]=snds[1];
  stroka_xml[6]=sumd[2]-snds[2];
  stroka_xml[7]=snds[2];
  stroka_xml[8]=sumd[3]-snds[3];
  stroka_xml[9]=snds[3];

#if 0
#####################################################33  
  if(metka_7 == 0)
   {
    stroka_xml[2]=sumd[0]-snds[0];
    stroka_xml[3]=snds[0];    
   }
  else
   {
    stroka_xml[4]=sumd[0]-snds[0];
    stroka_xml[5]=snds[0];    
   }

  stroka_xml[8]=0.;
  stroka_xml[9]=0.;    

  stroka_xml[8]=sumd[1]-snds[1];
  stroka_xml[9]=snds[1];    
  stroka_xml[10]=sumd[2]-snds[2];
  stroka_xml[11]=snds[2];    
  stroka_xml[12]=sumd[3]-snds[3];
  stroka_xml[13]=snds[3];    
  
#########################################################
#endif
  if(proc_dot != 0.)
   {
    for(int nom=0; nom < RAZMER_MAS_STR; nom++)
     {
      stroka_xml[nom]=stroka_xml[nom]*(100.0-proc_dot)/100.;
      stroka_xml[nom]=iceb_u_okrug(stroka_xml[nom],0.01);
     }
   }

  for(int nom=0; nom < RAZMER_MAS_STR; nom++)
   {
    itogo_mes1[nom]+=stroka_xml[nom];
    itogo_ob1[nom]+=stroka_xml[nom];
   }

  if(korrekt == 1)/*Если в виде документа присутствует буква У - то это коректирующий документ*/
   {
    class iceb_u_str komentout(row[16]);
    if(komentout.getdlinna() <= 1)
      komentout.new_plus("Несвоєчасне отримання податкової накладної");
    
    memset(stroka_xml_kor,'\0',sizeof(stroka_xml_kor));
    
    stroka_xml_kor[0]=stroka_xml[2];
    stroka_xml_kor[1]=stroka_xml[4];
    stroka_xml_kor[3]=stroka_xml[3];
    stroka_xml_kor[3]=stroka_xml[5];
    stroka_xml_kor[4]=stroka_xml[6]+stroka_xml[8];

    for(int nom=0; nom < RAZMER_MAS_KOR;nom++)
      itog_rozkor[nom]+=stroka_xml_kor[nom];  

    rasrnn_p_vvrozkor8(&nomer_str_rozkor,d,m,g,dv,mv,gv,nomdok.ravno(),naim_kontr.ravno(),row[10],stroka_xml_kor,komentout.ravno(),ff_xml_rozkor);
    
    fprintf(ff_lst_rozkor,"%3d %-*.*s %-*s %02d.%02d.%04d %-*s %-10d %02d.%02d.%04d %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    nomer_str_rozkor,
    iceb_u_kolbait(20,naim_kontr.ravno()),
    iceb_u_kolbait(20,naim_kontr.ravno()),
    naim_kontr.ravno(),
    iceb_u_kolbait(10,row[10]),row[10],
    dv,mv,gv,
    iceb_u_kolbait(10,nomdok.ravno()),nomdok.ravno(),
    1,
    d,m,g,
    iceb_u_kolbait(10,komentout.ravno()),
    iceb_u_kolbait(10,komentout.ravno()),
    komentout.ravno(),
    stroka_xml_kor[0],    
    stroka_xml_kor[1],    
    stroka_xml_kor[2],    
    stroka_xml_kor[3],    
    stroka_xml_kor[4],    
    stroka_xml_kor[5],    
    stroka_xml_kor[6]);
   }  
  
  rasrnn_p_str_xml8(++nomer_xml_str,d,m,g,dv,mv,gv,nomdok.ravno(),naim_kontr.ravno(),row[10],stroka_xml,row[19],ff_xml);
  

  if(ff_xml_dot != NULL)
   {
    stroka_xml_dot[0]=sumdok-stroka_xml[0];

    stroka_xml_dot[1]=sumd[0]-snds[0]-stroka_xml[1];
    stroka_xml_dot[2]=snds[0]-stroka_xml[2];    
    stroka_xml_dot[3]=sumd[1]-snds[1]-stroka_xml[3];
    stroka_xml_dot[4]=snds[1]-stroka_xml[4];    

    if(stroka_xml_dot[1] != 0.)
     {
      stroka_xml_dot[1]+=stroka_xml_dot[3];
      stroka_xml_dot[2]+=stroka_xml_dot[4];
      stroka_xml_dot[3]=0.;
      stroka_xml_dot[4]=0.;
     }



    stroka_xml_dot[5]=sumd[2]-snds[2]-stroka_xml[5];
    stroka_xml_dot[6]=snds[2]-stroka_xml[6];    
    stroka_xml_dot[7]=sumd[3]-snds[3]-stroka_xml[7];
    stroka_xml_dot[8]=snds[3]-stroka_xml[8];    
    
    rasrnn_p_str_xml8(nomer_xml_str,d,m,g,dv,mv,gv,nomdok.ravno(),naim_kontr.ravno(),row[10],stroka_xml_dot,row[19],ff_xml_dot);

    for(int nom=0; nom < RAZMER_MAS_STR; nom++)
     {
      itogo_mes2[nom]+=stroka_xml_dot[nom];
      itogo_ob2[nom]+=stroka_xml_dot[nom];
      
     }

   }  
  
  rasrnn_p_svod8(nomdok.ravno(),naim_kontr.ravno(),stroka_xml,row[19],ff);
  if(ff_dot != NULL)
    rasrnn_p_svod8(nomdok.ravno(),naim_kontr.ravno(),stroka_xml_dot,row[19],ff_dot);
   

  podsis.new_plus("");
  if(metka_pods == 1)
   {
    class iceb_u_str imafil_xml("");
    podsis.new_plus(ICEB_MP_MATU);
    //Запись накладной в xml формате
    xml_nn_matw(&por_nom_dok,1,dd,md,gd,row[14],row[1],mn,gn,mk,period_type,row[19],&imafil_xml,wpredok);
   }
  if(metka_pods == 2)
   {
    class iceb_u_str imafil_xml("");
    podsis.new_plus(gettext("УCЛ"));
    //Запись накладной в xml формате
    xml_nn_uslw(&por_nom_dok,1,dd,md,gd,row[14],row[1],mn,gn,mk,period_type,row[19],&imafil_xml,wpredok);
   }

  if(metka_pods == 3)
   {
    class iceb_u_str imafil_xml("");
    podsis.new_plus(gettext("УОC"));
    //Запись накладной в xml формате
    xml_nn_uosw(&por_nom_dok,1,dd,md,gd,row[14],row[1],mn,gn,mk,period_type,row[19],&imafil_xml,wpredok);
   }

  if(metka_pods == 4)
   {
    class iceb_u_str imafil_xml("");
    podsis.new_plus(ICEB_MP_UKR);
    //Запись накладной в xml формате
    xml_nn_ukrw(&por_nom_dok,dd,md,gd,row[14],row[1],mn,gn,mk,period_type,row[19],&imafil_xml,wpredok);
   }
   

  fprintf(ff_prot,"%5d|%02d.%02d.%04d|%-*s|%11s|%9s|%d|%-5s|%-12s|%-*s|%02d.%02d.%04d|%-10s|%-*.*s|%s\n",
  nomer_str,d,m,g,
  iceb_u_kolbait(10,row[1]),
  row[1],
  row[2],row[3],0,row[15],row[10],
  iceb_u_kolbait(5,podsis.ravno()),
  podsis.ravno(),
  dd,md,gd,row[14],
  iceb_u_kolbait(24,naim_kontr.ravno()),
  iceb_u_kolbait(24,naim_kontr.ravno()),
  naim_kontr.ravno(),
  row[16]);

  fprintf(ff_prot,"%27s|%11s|%9s|\n","",row[4],row[5]);
  fprintf(ff_prot,"%27s|%11s|%9s|\n","",row[6],row[7]);
  fprintf(ff_prot,"%27s|%11s|%9s|\n","",row[8],row[9]);

  if(korrekt == 0)
   {
    if(snds[0] != 0.)
     {
      if(dod5->innn1.find(row[10]) < 0)
       dod5->innn1.plus(row[10]);    
      
      sprintf(strsql,"%s|%02d.%04d",row[10],mv,gv);
      
      if((nomer_innn=dod5->innn_per1.find(strsql)) < 0)
       {
        dod5->innn_per1.plus(strsql);    
        dod5->naim_kontr1.plus(row[11]);
       }  

      dod5->suma_bez_nds1.plus(sumd[0]-snds[0],nomer_innn);
      if(metka_7 == 0)
       {
        dod5->suma_nds1.plus(snds[0],nomer_innn);
        dod5->suma_nds1_7.plus(0.,nomer_innn);
       }
      else
       {
        dod5->suma_nds1_7.plus(snds[0],nomer_innn);
        dod5->suma_nds1.plus(0.,nomer_innn);
       }
      
      if(metka_pods == 3) /*Учёт основных средств*/
       {
        dod5->os_sr1[0]+=sumd[0]-snds[0];
        dod5->os_sr1[1]+=snds[0];
       }
     }

    if(snds[1] != 0. || snds[2] != 0. || snds[3] != 0. )
     {
      if(dod5->innn1d.find(row[10]) < 0)
       {
        dod5->innn1d.plus(row[10]);    
        dod5->naim_kontr1d.plus(row[11]);
       }

      sprintf(strsql,"%s|%02d.%04d",row[10],mv,gv);
      
      if((nomer_innn=dod5->innn_per1d.find(strsql)) < 0)
       {
        dod5->innn_per1d.plus(strsql);    
        
        dod5->suma_bez_nds1d.plus(0.,-1);
        dod5->suma_nds1d.plus(0.,-1);
        nomer_innn=dod5->innn_per1d.find(strsql);
       }  
      dod5->suma_bez_nds1d.plus(sumd[2]-snds[2],nomer_innn);
      dod5->suma_nds1d.plus(snds[2],nomer_innn);
     }
    
    if( (sumd[0] != 0. &&  snds[0] == 0.) || (sumd[0] == 0. && sumd[1] != 0. && snds[1] == 0.))
     {
      if(dod5->innn1d1.find(row[10]) < 0)
       {
        dod5->innn1d1.plus(row[10]);    
        if(iceb_u_SRAV(row[10],"100000000000",0) == 0
        || iceb_u_SRAV(row[10],"200000000000",0) == 0
        || iceb_u_SRAV(row[10],"300000000000",0) == 0
        || iceb_u_SRAV(row[10],"400000000000",0) == 0)
         dod5->naim_kontr1d1.plus(gettext("Освобождены от НДС"));
        else
         dod5->naim_kontr1d1.plus(row[11]);
       }
      sprintf(strsql,"%s|%02d.%04d",row[10],mv,gv);
      
      if((nomer_innn=dod5->innn_per1d1.find(strsql)) < 0)
       {
        dod5->innn_per1d1.plus(strsql);    
        
        dod5->suma_bez_nds1d1.plus(0.,-1);
        dod5->suma_nds1d1.plus(0.,-1);
        nomer_innn=dod5->innn_per1d1.find(strsql);
       }  
      if(sumd[0] != 0. &&  snds[0] == 0.)
       {
        dod5->suma_bez_nds1d1.plus(sumd[0],nomer_innn);
        dod5->suma_nds1d1.plus(snds[0],nomer_innn);
       }
      if(sumd[0] == 0. && sumd[1] != 0. && snds[1] == 0.)
       {
        dod5->suma_bez_nds1d1.plus(sumd[1],nomer_innn);
        dod5->suma_nds1d1.plus(snds[1],nomer_innn);
       }
     }
   }
 }


rpvnn_pi8(gettext("Всего за месяц"),itogo_mes1,itogo_ou,ff);
if(ff_dot != NULL)
  rpvnn_pi8(gettext("Всего за месяц"),itogo_mes2,itogo_ou,ff_dot);


if(mn != mk)
 {
  rpvnn_pi8(gettext("Общий итог"),itogo_ob1,itogo_ou,ff);
  if(ff_dot != NULL)
    rpvnn_pi8(gettext("Общий итог"),itogo_ob2,itogo_ou,ff_dot);
 }

rasrnn_p_xml_itog8(itogo_mes1,itogo_ou,ff_xml);
if(ff_xml_dot != NULL)
  rasrnn_p_xml_itog8(itogo_mes2,itogo_ou,ff_xml_dot);
if(nomer_str_rozkor > 0)
 {
  fprintf(ff_xml_rozkor,"   <R021G9>%.2f</R021G9>\n",itog_rozkor[0]);
  fprintf(ff_xml_rozkor,"   <R021G10>%.2f</R021G10>\n",itog_rozkor[1]);
  fprintf(ff_xml_rozkor,"   <R021G11>%.2f</R021G11>\n",itog_rozkor[2]);
  fprintf(ff_xml_rozkor,"   <R021G12>%.2f</R021G12>\n",itog_rozkor[3]);
  fprintf(ff_xml_rozkor,"   <R021G13>%.2f</R021G13>\n",itog_rozkor[4]);
  fprintf(ff_xml_rozkor,"   <R021G14>%.2f</R021G14>\n",itog_rozkor[5]);
  fprintf(ff_xml_rozkor,"   <R021G15>%.2f</R021G15>\n",itog_rozkor[6]);

  fprintf(ff_lst_rozkor,"%*s:%10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  iceb_u_kolbait(92,gettext("Итого")),gettext("Итого"),
  itog_rozkor[0],
  itog_rozkor[1],
  itog_rozkor[2],
  itog_rozkor[3],
  itog_rozkor[4],
  itog_rozkor[5],
  itog_rozkor[6]);
 }
return(0);
}
/********************************/
/*Вывод строки в xml файл реестра налоговых накладных*/
/**************************/
void rasrnn_p_str_xml8(int nomer_str,
short d,short m,short g,
short dv,short mv,short gv,
const char *nomer_dok,
const char *naim_kontr,
const char *inn,
double *suma,
const char *viddok,
FILE *ff_xml)
{

fprintf(ff_xml,"   <T2RXXXXG1 ROWNUM=\"%d\">%d</T2RXXXXG1>\n",nomer_str,nomer_str); //1
fprintf(ff_xml,"   <T2RXXXXG2D ROWNUM=\"%d\">%02d%02d%d</T2RXXXXG2D>\n",nomer_str,d,m,g); //1
fprintf(ff_xml,"   <T2RXXXXG3D ROWNUM=\"%d\">%02d%02d%d</T2RXXXXG3D>\n",nomer_str,dv,mv,gv); //2

if(nomer_dok[0] != '\0')
 fprintf(ff_xml,"   <T2RXXXXG4S ROWNUM=\"%d\">%s</T2RXXXXG4S>\n",nomer_str,iceb_u_filtr_xml(nomer_dok)); //3
else
 fprintf(ff_xml,"   <T2RXXXXG4S xsi:nil=\"true\" ROWNUM=\"%d\"></T2RXXXXG4S>\n",nomer_str); //3
 

if(viddok[0] != '\0')
 {
  class iceb_u_str r1("");
  class iceb_u_str r2("");
  class iceb_u_str r3("");
  class iceb_u_str r4("");
  
  if(iceb_u_polen(viddok,&r1,1,',') != 0)
   r1.new_plus(viddok);
  else
   {
    iceb_u_polen(viddok,&r2,2,',');
    iceb_u_polen(viddok,&r3,3,',');
    iceb_u_polen(viddok,&r4,4,',');
   }   

  if(iceb_u_strlen(r1.ravno()) > 3)
   {
    char bros[1024];
    sprintf(bros,"%s %s %s 3 %s\n%d.%d.%d %s\n%.*s",
    gettext("Первое поле вида документа"),
    r1.ravno(),
    gettext("больше"),
    gettext("символов"),
    d,m,g,nomer_dok,
    iceb_u_kolbait(40,naim_kontr),
    naim_kontr);
   }

  if(iceb_u_strlen(r2.ravno()) > 2)
   {
    char bros[1024];
    sprintf(bros,"%s %s %s 2 %s\n%d.%d.%d %s\n%.*s",
    gettext("Второе поле вида документа"),
    r2.ravno(),
    gettext("больше"),
    gettext("символов"),
    d,m,g,nomer_dok,
    iceb_u_kolbait(40,naim_kontr),
    naim_kontr);
   }

  if(iceb_u_strlen(r3.ravno()) > 1)
   {
    char bros[1024];
    sprintf(bros,"%s %s %s 1 %s\n%d.%d.%d %s\n%.*s",
    gettext("Третье поле вида документа"),
    r2.ravno(),
    gettext("больше"),
    gettext("символов"),
    d,m,g,nomer_dok,
    iceb_u_kolbait(40,naim_kontr),
    naim_kontr);
   }

  if(r1.getdlinna() > 1)
   fprintf(ff_xml,"   <T2RXXXXG51S ROWNUM=\"%d\">%s</T2RXXXXG51S>\n",nomer_str,r1.ravno()); //3

    if(r2.getdlinna() > 1)
     fprintf(ff_xml,"   <T2RXXXXG54S ROWNUM=\"%d\">%s</T2RXXXXG54S>\n",nomer_str,r2.ravno()); //3
    if(r3.getdlinna() > 1)
     fprintf(ff_xml,"   <T2RXXXXG52S ROWNUM=\"%d\">%s</T2RXXXXG52S>\n",nomer_str,r3.ravno()); //3
    if(r4.getdlinna() > 1)
     fprintf(ff_xml,"   <T2RXXXXG53S ROWNUM=\"%d\">%s</T2RXXXXG53S>\n",nomer_str,r4.ravno()); //3
  
 }
 
if(naim_kontr[0] != '\0')
 fprintf(ff_xml,"   <T2RXXXXG6S ROWNUM=\"%d\">%s</T2RXXXXG6S>\n",nomer_str,iceb_u_filtr_xml(naim_kontr)); //4
else
 fprintf(ff_xml,"   <T2RXXXXG6S xsi:nil=\"true\" ROWNUM=\"%d\"></T2RXXXXG6S>\n",nomer_str); //4

if(inn[0] == '\0' || isdigit(inn[0]) == 0)  //Значит символ не число
  fprintf(ff_xml,"   <T2RXXXXG7 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer_str);
else
 {
  fprintf(ff_xml,"   <T2RXXXXG7 ROWNUM=\"%d\">%s</T2RXXXXG7>\n",nomer_str,inn); //5
 }
  fprintf(ff_xml,"   <T2RXXXXG8 ROWNUM=\"%d\">%.2f</T2RXXXXG8>\n",nomer_str,suma[0]); //8
  fprintf(ff_xml,"   <T2RXXXXG109 ROWNUM=\"%d\">%.2f</T2RXXXXG109>\n",nomer_str,suma[1]); //9

  fprintf(ff_xml,"   <T2RXXXXG110 ROWNUM=\"%d\">%.2f</T2RXXXXG110>\n",nomer_str,suma[2]); //10
  fprintf(ff_xml,"   <T2RXXXXG111 ROWNUM=\"%d\">%.2f</T2RXXXXG111>\n",nomer_str,suma[3]); //11
  fprintf(ff_xml,"   <T2RXXXXG112 ROWNUM=\"%d\">%.2f</T2RXXXXG112>\n",nomer_str,suma[4]); //12
  fprintf(ff_xml,"   <T2RXXXXG113 ROWNUM=\"%d\">%.2f</T2RXXXXG113>\n",nomer_str,suma[5]); //13
  fprintf(ff_xml,"   <T2RXXXXG9 ROWNUM=\"%d\">%.2f</T2RXXXXG9>\n",nomer_str,suma[6]); //14
  fprintf(ff_xml,"   <T2RXXXXG10 ROWNUM=\"%d\">%.2f</T2RXXXXG10>\n",nomer_str,suma[7]); //15
  fprintf(ff_xml,"   <T2RXXXXG11 ROWNUM=\"%d\">%.2f</T2RXXXXG11>\n",nomer_str,suma[8]); //16
  fprintf(ff_xml,"   <T2RXXXXG12 ROWNUM=\"%d\">%.2f</T2RXXXXG12>\n",nomer_str,suma[9]); //17
  fprintf(ff_xml,"   <T2RXXXXG13 ROWNUM=\"%d\">%.2f</T2RXXXXG13>\n",nomer_str,suma[10]); //18
  

  fprintf(ff_xml,"   <T2RXXXXG14 ROWNUM=\"%d\">%.2f</T2RXXXXG14>\n",nomer_str,suma[11]); //19
  fprintf(ff_xml,"   <T2RXXXXG15 ROWNUM=\"%d\">%.2f</T2RXXXXG15>\n",nomer_str,suma[12]); //20
  fprintf(ff_xml,"   <T2RXXXXG16 ROWNUM=\"%d\">%.2f</T2RXXXXG16>\n",nomer_str,suma[13]); //21

}
/*****************************/
/*Вывод в xml файл расчёта корректировки*/
/*******************************/
void rasrnn_p_vvrozkor8(int *nomer_str,
short dnn,short mnn,short gnn, //Дата выписки налоговой накладной
short dpn,short mpn,short gpn, //Дата получения налоговой накладной
const char *nomer_dok,
const char *naim_kontr,
const char *inn,
double *stroka_xml_kor,
const char *koment,
FILE *ff_xml_rozkor)
{
*nomer_str+=1;
fprintf(ff_xml_rozkor,"   <T21RXXXXG2S ROWNUM=\"%d\">%s</T21RXXXXG2S>\n",*nomer_str,iceb_u_filtr_xml(naim_kontr));
fprintf(ff_xml_rozkor,"   <T21RXXXXG3 ROWNUM=\"%d\">%s</T21RXXXXG3>\n",*nomer_str,inn);
fprintf(ff_xml_rozkor,"   <T21RXXXXG4D ROWNUM=\"%d\">%02d%02d%d</T21RXXXXG4D>\n",*nomer_str,dpn,mpn,gpn);
fprintf(ff_xml_rozkor,"   <T21RXXXXG5S ROWNUM=\"%d\">%s</T21RXXXXG5S>\n",*nomer_str,iceb_u_filtr_xml(nomer_dok));
fprintf(ff_xml_rozkor,"   <T21RXXXXG6S ROWNUM=\"%d\">1</T21RXXXXG6S>\n",*nomer_str); /*номер расчёта корректировки*/
fprintf(ff_xml_rozkor,"   <T21RXXXXG7D ROWNUM=\"%d\">%02d%02d%d</T21RXXXXG7D>\n",*nomer_str,dnn,mnn,gnn);
fprintf(ff_xml_rozkor,"   <T21RXXXXG8S ROWNUM=\"%d\">%s</T21RXXXXG8S>\n",*nomer_str,iceb_u_filtr_xml(koment));

fprintf(ff_xml_rozkor,"   <T21RXXXXG9 ROWNUM=\"%d\">%.2f</T21RXXXXG9>\n",*nomer_str,stroka_xml_kor[0]);
fprintf(ff_xml_rozkor,"   <T21RXXXXG10 ROWNUM=\"%d\">%.2f</T21RXXXXG10>\n",*nomer_str,stroka_xml_kor[1]);
fprintf(ff_xml_rozkor,"   <T21RXXXXG11 ROWNUM=\"%d\">%.2f</T21RXXXXG11>\n",*nomer_str,stroka_xml_kor[2]);
fprintf(ff_xml_rozkor,"   <T21RXXXXG12 ROWNUM=\"%d\">%.2f</T21RXXXXG12>\n",*nomer_str,stroka_xml_kor[3]);
fprintf(ff_xml_rozkor,"   <T21RXXXXG13 ROWNUM=\"%d\">%.2f</T21RXXXXG13>\n",*nomer_str,stroka_xml_kor[4]);
fprintf(ff_xml_rozkor,"   <T21RXXXXG14 ROWNUM=\"%d\">%.2f</T21RXXXXG14>\n",*nomer_str,stroka_xml_kor[5]);
fprintf(ff_xml_rozkor,"   <T21RXXXXG15 ROWNUM=\"%d\">%.2f</T21RXXXXG15>\n",*nomer_str,stroka_xml_kor[6]);

}

/********************************/
/*Распечатать итог*/
/*****************************/
void rasrnn_p_xml_itog8(double *itog,double *itog_u,FILE *ff_xml)
{
fprintf(ff_xml,"   <R021G8>%.2f</R021G8>\n",itog[0]);
fprintf(ff_xml,"   <R021G109>%.2f</R021G109>\n",itog[1]);
fprintf(ff_xml,"   <R021G110>%.2f</R021G110>\n",itog[2]);
fprintf(ff_xml,"   <R021G111>%.2f</R021G111>\n",itog[3]);
fprintf(ff_xml,"   <R021G112>%.2f</R021G112>\n",itog[4]);
fprintf(ff_xml,"   <R021G113>%.2f</R021G113>\n",itog[5]);
fprintf(ff_xml,"   <R021G9>%.2f</R021G9>\n",itog[6]);
fprintf(ff_xml,"   <R021G10>%.2f</R021G10>\n",itog[7]);
fprintf(ff_xml,"   <R021G11>%.2f</R021G11>\n",itog[8]);
fprintf(ff_xml,"   <R021G12>%.2f</R021G12>\n",itog[9]);
fprintf(ff_xml,"   <R021G13>%.2f</R021G13>\n",itog[10]);
fprintf(ff_xml,"   <R021G14>%.2f</R021G14>\n",itog[11]);
fprintf(ff_xml,"   <R021G15>%.2f</R021G15>\n",itog[12]);
fprintf(ff_xml,"   <R021G16>%.2f</R021G16>\n",itog[13]);

fprintf(ff_xml,"   <R022G8>%.2f</R022G8>\n",itog_u[0]);
fprintf(ff_xml,"   <R022G109>%.2f</R022G109>\n",itog_u[1]);
fprintf(ff_xml,"   <R022G110>%.2f</R022G110>\n",itog_u[2]);
fprintf(ff_xml,"   <R022G111>%.2f</R022G111>\n",itog_u[3]);
fprintf(ff_xml,"   <R022G112>%.2f</R022G112>\n",itog_u[4]);
fprintf(ff_xml,"   <R022G113>%.2f</R022G113>\n",itog_u[5]);
fprintf(ff_xml,"   <R022G9>%.2f</R022G9>\n",itog_u[6]);
fprintf(ff_xml,"   <R022G10>%.2f</R022G10>\n",itog_u[7]);
fprintf(ff_xml,"   <R022G11>%.2f</R022G11>\n",itog_u[8]);
fprintf(ff_xml,"   <R022G12>%.2f</R022G12>\n",itog_u[9]);
fprintf(ff_xml,"   <R022G13>%.2f</R022G13>\n",itog_u[10]);
fprintf(ff_xml,"   <R022G14>%.2f</R022G14>\n",itog_u[11]);
fprintf(ff_xml,"   <R022G15>%.2f</R022G15>\n",itog_u[12]);
fprintf(ff_xml,"   <R022G16>%.2f</R022G16>\n",itog_u[13]);

}
/****************************/
/*Вывод концовки строки в распечатку*/
/****************************/
void rasrnn_p_svod8(const char *nomdok,const char *naim_kontr,double *stroka_xml,const char *viddok,FILE *ff)
{
fprintf(ff,"%9s|","");//9

for(int nom=2; nom < RAZMER_MAS_STR; nom+=2)
 {
  if(stroka_xml[nom] != 0. || stroka_xml[nom+1] != 0.) //10,11
   {
    if(stroka_xml[nom+1] != 0.)
      fprintf(ff,"%9.2f|%8.2f|",stroka_xml[nom],stroka_xml[nom+1]);
    else
     {
      if(stroka_xml[nom] != 0.)
        fprintf(ff,"%9.2f|XXXXXXXX|",stroka_xml[nom]);
      else
        fprintf(ff,"%9s|%8s|","","");
      }
   }
  else
   fprintf(ff,"%9s|%8s|","","");
 }

fprintf(ff,"\n");

if(iceb_u_strlen(naim_kontr) > 20 || iceb_u_strlen(nomdok) > 9 || iceb_u_strlen(viddok) > 5)
 {
  fprintf(ff,"|%5s|%10s|%10s|%-*.*s|%-*.*s|%-*.*s|%14s|%10s|%9s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|\n",
  "","","",
  iceb_u_kolbait(9,iceb_u_adrsimv(9,nomdok)),iceb_u_kolbait(9,iceb_u_adrsimv(9,nomdok)),iceb_u_adrsimv(9,nomdok),
  iceb_u_kolbait(5,iceb_u_adrsimv(5,viddok)),iceb_u_kolbait(5,iceb_u_adrsimv(5,viddok)),iceb_u_adrsimv(5,viddok),
  iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),iceb_u_adrsimv(20,naim_kontr),
  "","","","","","","","","","","","","","","");
 }
if(iceb_u_strlen(naim_kontr) > 40 || iceb_u_strlen(nomdok) > 18)
 {
  fprintf(ff,"|%5s|%10s|%10s|%-*.*s|%5s|%-*.*s|%14s|%10s|%9s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|\n",
  "","","",
  iceb_u_kolbait(9,iceb_u_adrsimv(18,nomdok)),iceb_u_kolbait(9,iceb_u_adrsimv(18,nomdok)),iceb_u_adrsimv(18,nomdok),
  "",
  iceb_u_kolbait(20,iceb_u_adrsimv(40,naim_kontr)),iceb_u_kolbait(20,iceb_u_adrsimv(40,naim_kontr)),iceb_u_adrsimv(40,naim_kontr),
  "","","","","","","","","","","","","","","");
 }
for(int nom=60; nom < iceb_u_strlen(naim_kontr); nom+=20)
 {
  fprintf(ff,"|%5s|%10s|%10s|%9s|%5s|%-*.*s|%14s|%10s|%9s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|\n",
  "","","","","",
  iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),iceb_u_adrsimv(nom,naim_kontr),
  "","","","","","","","","","","","","","","");
 }

}
/******************************************************/
void rpvnn_pi8s(double *itogo,FILE *ff)
{

fprintf(ff,"%9s|",""); //9
 
if(itogo[2] != 0.)//10
 fprintf(ff,"%9.2f|",itogo[2]);
else
 fprintf(ff,"%9s|","");


if(itogo[3] != 0.)//11
 fprintf(ff,"%8.2f|",itogo[3]);
else
 fprintf(ff,"%8s|","");

if(itogo[4] != 0.)//12
 fprintf(ff,"%9.2f|",itogo[4]);
else
 fprintf(ff,"%9s|","");


if(itogo[5] != 0.)//13
 fprintf(ff,"%8.2f|",itogo[5]);
else
 fprintf(ff,"%8s|","");
//printw("%s-%.2f %.2f\n",__FUNCTION__,itogo[6],itogo[7]);
//OSTANOV();
if(itogo[6] != 0.)//14
 fprintf(ff,"%9.2f|",itogo[6]);
else
 fprintf(ff,"%9s|","");


if(itogo[7] != 0.)//15
 fprintf(ff,"%8.2f|",itogo[7]);
else
 fprintf(ff,"%8s|","");

if(itogo[8] != 0.)//16
 fprintf(ff,"%9.2f|",itogo[8]);
else
 fprintf(ff,"%9s|","");

if(itogo[9] != 0.)//17
 fprintf(ff,"%8.2f|",itogo[9]);
else
 fprintf(ff,"%8s|","");


if(itogo[10] != 0.)//18
 fprintf(ff,"%9.2f|",itogo[10]);
else
 fprintf(ff,"%9s|","");

if(itogo[11] != 0.)//19
 fprintf(ff,"%8.2f|",itogo[11]);
else
 fprintf(ff,"%8s|","");

if(itogo[12] != 0.)//20
 fprintf(ff,"%9.2f|",itogo[12]);
else
 fprintf(ff,"%9s|","");

if(itogo[13] != 0.)//21
 fprintf(ff,"%8.2f|",itogo[13]);
else
 fprintf(ff,"%8s|","");

fprintf(ff,"\n");

}
/**************************************************/
void rpvnn_pi8(char *vsego,double *itogo,double *itogo_u,FILE *ff)
{
const char *stroka_pod={"------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"};


fprintf(ff,"%s\n",stroka_pod);


fprintf(ff,"|%*s|%10.2f|",iceb_u_kolbait(79,vsego),vsego,itogo[0]); //8

rpvnn_pi8s(itogo,ff);


fprintf(ff,"%s\n",stroka_pod);

const char *repl={"З них включено до уточнюючих розрахунків за звітний період"};
fprintf(ff,"|%*s|%10.2f|",iceb_u_kolbait(79,repl),repl,itogo_u[0]);//8

rpvnn_pi8s(itogo_u,ff);

fprintf(ff,"%s\n",stroka_pod);

}
