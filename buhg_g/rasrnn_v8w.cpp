/*$Id: rasrnn_v.c,v 5.83 2014/07/31 07:09:17 sasa Exp $*/
/*31.08.2016	29.11.2014	Белых А.И.	rasrnn_v8w.c
Распечатка реестра выданных налоговых накладных 
*/
#include <ctype.h>
#include <errno.h>
#include "buhg_g.h"
#include "xrnn_poiw.h"
#include "rnn_d5w.h"

#define RAZMER_MAS 10

void rasrnn_v_xml_itog8(double *itog,double *itogo_u,FILE *ff_xml);
void rasrnn_v_str_xml8(int nomer_str,short d,short m,short g,const char *nomer_dok,const char *naim_kontr,const char *inn,double *suma,const char *viddok,FILE *ff_xml);
void rasrnn_v_rm8(double proc_dot,double *itogo_o,double *itogo_o1,double *itogo_o2);
void rasrnn_v_str8(const char *nomdok,const char *naim_kontr,int lnds,double sumd,double snds,double *stroka_xml,const char *viddok,FILE *ff);

void rasrnn_v_vvrozkor8(int *nomer_str,short dnn,short mnn,short gnn,short dpn,short mpn,short gpn,const char *nomer_dok,const char *naim_kontr,const char *inn,double *stroka_xml,const char *koment,int lnds,FILE *ff_xml_rozkor);
void rpvnn_vi8(char *vsego,double *itogo,double *itogo_u,FILE *ff);

extern int kol_strok_na_liste;
extern SQL_baza bd;

int rasrnn_v8w(class xrnn_poi *data,
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
int metka_sort=0;
class iceb_u_str sort("");
short dn,mn,gn;
short dk,mk,gk;

if((metka_sort=rnn_vsw()) < 0)
 return(1);

iceb_u_rsdat(&dn,&mn,&gn,data->datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->datak.ravno(),1);

if(metka_sort == 0)
 sort.new_plus(" order by datd asc,nnni asc");
else
 sort.new_plus(" order by datd asc,dnz asc");
//printw("Реестр выданных накладных\n");

sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv,nz from Reenn");

if(data->datan.getdlinna() > 1 && data->datak.getdlinna() <= 1)
  sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv,nz from Reenn where \
datd >= '%s'",data->datan.ravno_sqldata());

if(data->datan.getdlinna() > 1 && data->datak.getdlinna() > 1)
  sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv,nz from Reenn where \
datd >= '%s' and datd <= '%s'",data->datan.ravno_sqldata(),data->datak.ravno_sqldata());

strcat(strsql,sort.ravno());


SQLCURSOR cur;
SQL_str row;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("Не найдено ни одной записи выданных налоговых накладных!"));
  iceb_printw(strsql,buffer,view);
  return(1);
 }
double proc_dot=data->proc_dot.ravno_atof();
class iceb_u_str imafalx("rasrnn_v8.alx");


iceb_zagolov("Реєстро виданих та отриманих податкових накладних",data->datan.ravno(),data->datak.ravno(),ff,wpredok);
//Вставляем шапку 
iceb_insfil(imafalx.ravno(),ff,wpredok);
if(ff_dot != NULL)
   iceb_insfil(imafalx.ravno(),ff_dot,wpredok);

iceb_zagolov(gettext("Реестр выданных налоговых накладных"),data->datan.ravno(),data->datak.ravno(),ff_prot,wpredok);
if(data->kodgr1.ravno()[0] != '\0')
 fprintf(ff_prot,"%s:%s\n",gettext("Группа"),data->kodgr1.ravno());

fprintf(ff_prot,"\
-------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_prot,"\
  N  |Дата п/в  |Номер н.н.|Сумма по д.|Сумма НДС|M|Груп.|Инд.нал.ном.|Подс.|Дата док. |Номер док.|Наимeнование контрагента|Коментарий\n");
/*
12345 1234567890 1234567890 12345678901 123456789 1 12345 123456789012 12345 1234567890 1234567890 123456789012345678901234
*/
fprintf(ff_prot,"\
-------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_lst_rozkor,"I Коригування податкових зобов'язань\n\
----------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_lst_rozkor,"\
   |                                 |Податкова накладна,  |Розрахунок коригуван.|Підстава  |       Обсяг         |       Сума          |Обсяг пос-|\n\
   |    Отримувач (покупець)         |за якою пров.коригув.|кількісних і вартісн.|для кориг.|      поставки       |        ПДВ          |тачання,  |\n\
   |                                 |                     |показників до под.нак|податкових|      (без ПДВ)      |       (+чи-)        |за яким не|\n\
   |-----------------------------------------------------------------------------|зобов'яза.|       (+чи-)        |                     |нарахову- |\n\
 N |   Найменування     |Індивідуал  |  Дата    |  Номер   | Номер    |Дата отрим|з податку |                     |                     |  вався   |\n\
   |(П.І.Б. для фізичних|  ьний      |          |          |          |мання отри|на додану |-------------------------------------------|податок на|\n\
   |      осіб)         |подаковий   |          |          |          |мувачем   |вартість  |   за     |   за     |   за     |    за    |  додану  |\n\
   |                    | номер      |          |          |          |(покупцем)|          |основною  |ставкою   |основною  | ставкою  | вартість |\n\
   |                    |            |          |          |          |розрахунку|          |ставкою   |   7%%     |  ставкою |    7%%    |          |\n\
------------------------------------------------------------------------------------------------------------------|--------------------------------|\n\
 1 |         2          |     3      |     4    |    5     |    6     |     7    |    8     |    9     |    10    |    11    |    12    |    13    |\n\
---------------------------------------------------------------------------------------------------------------------------------------------------|\n");

double sumd=0.;
double sumd1=0.;
double snds=0.;
double snds1=0.;

double itogo_mes[RAZMER_MAS];
memset(itogo_mes,'\0',sizeof(itogo_mes));

double itogo_o[RAZMER_MAS];
memset(itogo_o,'\0',sizeof(itogo_o));
double itogo_ou[RAZMER_MAS];
memset(itogo_ou,'\0',sizeof(itogo_ou));
int nomer_str=0;
int npz=0;
if(npz != 0)
 nomer_str=npz-1;

short mz=0;
short d,m,g;
short dd,md,gd;
float kolstr1=0;
int lnds=0;
int metka_pods;
class iceb_u_str podsis("");
double sumd_pr=0.;
double sumnds_pr=0.;
class iceb_u_str nomdok("");
class iceb_u_str naim_kontr("");
int nomer_str_xml=0;
double stroka_xml[RAZMER_MAS];
double stroka_xml_dot[RAZMER_MAS];
#define RAZMER_MAS_KOR 5
double stroka_xml_kor[RAZMER_MAS_KOR];
double itogo_o1[RAZMER_MAS];
double itogo_o2[RAZMER_MAS];
int por_nom_dok=0;
int nomer_innn=0;
int korrekt=0;
int nomer_str_rozkor=0;
double itogo_rozkor[RAZMER_MAS_KOR];
memset(itogo_rozkor,'\0',sizeof(itogo_rozkor));
short dv,mv,gv;

sprintf(strsql,"%s-kolstr=%d\n",__FUNCTION__,kolstr);
iceb_printw(strsql,buffer,view);

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(bar,kolstr,++kolstr1);    

  if(data->metka_ins == 1) //*только отмеченные
   if(data->nomzap_v.find(atoi(row[15])) < 0)
    continue;

  if(data->metka_ins == 2) //*только не отмеченные
    if(data->nomzap_v.find(atoi(row[15])) >= 0)
     continue;
     
  if(iceb_u_proverka(data->kodgr1.ravno(),row[10],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->kodop.ravno(),row[12],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->viddok.ravno(),row[13],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->naim_kontr.ravno(),row[6],4,0) != 0)
   continue;
   
  if(data->podsistema.ravno()[0] != '\0')
   {
    class iceb_u_str metka_pod("");

    if(row[7][0] == '1')
     metka_pod.new_plus(ICEB_MP_MATU);
    if(row[7][0] == '2')
     metka_pod.new_plus(ICEB_MP_USLUGI);
    if(row[7][0] == '3')
     metka_pod.new_plus(ICEB_MP_UOS);
    if(row[7][0] == '4')
     metka_pod.new_plus(ICEB_MP_UKR);
    if(row[7][0] == '5')
     metka_pod.new_plus(gettext("ГК"));
    if(iceb_u_proverka(data->podsistema.ravno(),metka_pod.ravno(),0,0) != 0)
    continue;
   }
   
  if(iceb_u_proverka(data->nnn.ravno(),row[1],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->nomdok.ravno(),row[9],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->inn.ravno(),row[5],4,0) != 0)
   continue;
  if(iceb_u_proverka(data->koment.ravno(),row[11],4,0) != 0)
   continue;

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  iceb_u_rsdat(&dd,&md,&gd,row[8],2);
  iceb_u_rsdat(&dv,&mv,&gv,row[14],2);

  korrekt=0;  
  if(iceb_u_strstrm(row[13],"У") == 1 || iceb_u_SRAV(row[13],"РКП",0) == 0 || iceb_u_SRAV(row[13],"РКЕ",0) == 0) /*Если в виде документа присутствует буква У - то это коректирующий документ*/
    korrekt=1;

  if(mz != m)
   {
    if(mz != 0)
     {
      rasrnn_v_rm8(proc_dot,itogo_mes,itogo_o1,itogo_o2);
      rpvnn_vi8(gettext("Всего за месяц"),itogo_o1,itogo_ou,ff);
      if(ff_dot != NULL)
        rpvnn_vi8(gettext("Всего за месяц"),itogo_o2,itogo_ou,ff_dot);
     }

    for(int nom=0; nom < RAZMER_MAS; nom++)
      itogo_o[nom]+=itogo_mes[nom];
    
    memset(itogo_mes,'\0',sizeof(itogo_mes));
    mz=m;
   }

  sumd=atof(row[2]);
  sumd1=sumd=iceb_u_okrug(sumd,0.01);
  snds=atof(row[3]);
  snds1=snds=iceb_u_okrug(snds,0.01);

  if(proc_dot != 0.)
   {
    sumd1=sumd1*(100.0-proc_dot)/100.;
    sumd1=iceb_u_okrug(sumd1,0.01);
    snds1=snds1*(100.0-proc_dot)/100.;
    snds1=iceb_u_okrug(snds1,0.01);
   }

  lnds=atoi(row[4]);

  if(korrekt == 1)
   {
    itogo_ou[0]+=sumd;
    if(lnds == 0)
     {
      itogo_ou[1]+=sumd-snds;
      itogo_ou[2]+=snds;
     }

    if(lnds == 4)
     {
      itogo_ou[3]+=sumd-snds;
      itogo_ou[4]+=snds;
     }

    if(lnds == 1)
     itogo_ou[5]+=sumd;

    if(lnds == 2)
     itogo_ou[6]+=sumd;

    if(lnds == 3)
     itogo_ou[8]+=sumd;

   }  
  sumd_pr+=sumd;
  sumnds_pr+=snds;
  
  metka_pods=atoi(row[7]);
  nomdok.new_plus(row[1]);

  naim_kontr.new_plus(row[6]);

  fprintf(ff,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*.*s|%-*s|%10.2f|",
  ++nomer_str,
  d,m,g,
  iceb_u_kolbait(8,nomdok.ravno()),iceb_u_kolbait(8,nomdok.ravno()),nomdok.ravno(),
  iceb_u_kolbait(5,row[13]),iceb_u_kolbait(5,row[13]),row[13],
  iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
  iceb_u_kolbait(14,row[5]),row[5],
  sumd1);
  
  if(ff_dot != NULL)
    fprintf(ff_dot,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*.*s|%-*s|%10.2f|",
    nomer_str,
    d,m,g,
    iceb_u_kolbait(8,nomdok.ravno()),iceb_u_kolbait(8,nomdok.ravno()),nomdok.ravno(),
    iceb_u_kolbait(5,row[13]),iceb_u_kolbait(5,row[13]),row[13],
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    iceb_u_kolbait(14,row[5]),row[5],
    sumd-sumd1);

  memset(stroka_xml,'\0',sizeof(stroka_xml));
  memset(stroka_xml_dot,'\0',sizeof(stroka_xml_dot));
  
  
  stroka_xml[0]=sumd1;
  stroka_xml_dot[0]=sumd-sumd1;

  if(lnds == 0) /*колонка 8,9*/
   {
    stroka_xml[1]=sumd-snds;
    stroka_xml[2]=snds;    
   }

  if(lnds == 4) /*колонка 10,11*/
   {
    stroka_xml[3]=sumd-snds;
    stroka_xml[4]=snds;    
   }

  if(lnds == 1) /*колонка 12*/
   {
    stroka_xml[5]=sumd;
   }

  if(lnds == 2) /*колонка 13*/
   {
    stroka_xml[6]=sumd;
   }


  if(lnds == 3) /*колонка 15*/
   {
    stroka_xml[8]=sumd;
   }

  for(int nom=0; nom < RAZMER_MAS; nom++)
   itogo_mes[nom]+=stroka_xml[nom];
  
  rasrnn_v_str8(nomdok.ravno(),naim_kontr.ravno(),lnds,sumd1,snds1,stroka_xml,row[13],ff);
  if(ff_dot != NULL)
    rasrnn_v_str8(nomdok.ravno(),naim_kontr.ravno(),lnds,sumd-sumd1,snds-snds1,stroka_xml_dot,row[13],ff_dot);

  podsis.new_plus("");

  if(metka_pods == 1)
   {
    class iceb_u_str imafil_xml("");
    podsis.new_plus(ICEB_MP_MATU);
    //Запись накладной в xml формате
    xml_nn_matw(&por_nom_dok,2,dd,md,gd,row[9],row[1],mn,gn,mk,period_type,row[13],&imafil_xml,wpredok);
   }

  if(metka_pods == 2)
   {
    class iceb_u_str imafil_xml("");
    podsis.new_plus(gettext("УCЛ"));
    //Запись накладной в xml формате
    xml_nn_uslw(&por_nom_dok,2,dd,md,gd,row[9],row[1],mn,gn,mk,period_type,row[13],&imafil_xml,wpredok);
   }
  if(metka_pods == 3)
   {
    class iceb_u_str imafil_xml("");
    podsis.new_plus(gettext("УОC"));
    //Запись накладной в xml формате
    xml_nn_uosw(&por_nom_dok,2,dd,md,gd,row[9],row[1],mn,gn,mk,period_type,row[13],&imafil_xml,wpredok);
   }
   

  fprintf(ff_prot,"%5d|%02d.%02d.%04d|%-*s|%11s|%9s|%d|%-5s|%-12s|%-*s|%02d.%02d.%04d|%-10s|%-*.*s|%s\n",
  nomer_str,d,m,g,
  iceb_u_kolbait(10,row[1]),
  row[1],
  row[2],row[3],lnds,row[10],row[5],
  iceb_u_kolbait(5,podsis.ravno()),
  podsis.ravno(),
  dd,md,gd,row[9],
  iceb_u_kolbait(24,naim_kontr.ravno()),
  iceb_u_kolbait(24,naim_kontr.ravno()),
  naim_kontr.ravno(),
  row[11]);


  if(korrekt == 1) /*Если в виде документа присутствует буква У - то это коректирующий документ*/
   {
  
    class iceb_u_str komentout(row[11]);
    if(komentout.getdlinna() <= 1)
      komentout.new_plus("Несвоєчасне отримання податкової накладної");
    if(dv == 0)
     {
      dv=d;
      mv=m;
      gv=g;
     }

    
    memset(stroka_xml_kor,'\0',sizeof(stroka_xml_kor));

    if(lnds == 0)
     {
      stroka_xml_kor[0]=sumd-snds;
      stroka_xml_kor[2]=snds;
     }
    if(lnds == 4)
     {
      stroka_xml_kor[1]=sumd-snds;
      stroka_xml_kor[3]=snds;
     }

    if(lnds == 1 || lnds == 2 || lnds == 3)
     stroka_xml_kor[4]=sumd;    

    for(int nom=0; nom < RAZMER_MAS_KOR; nom++)
      {
       itogo_rozkor[nom]+=stroka_xml_kor[nom];
      }         

    rasrnn_v_vvrozkor8(&nomer_str_rozkor,d,m,g,dv,mv,gv,nomdok.ravno(),naim_kontr.ravno(),row[5],stroka_xml,komentout.ravno(),lnds,ff_xml_rozkor);

    fprintf(ff_lst_rozkor,"%3d %-*.*s %*s %02d.%02d.%04d %-*s %-10s %02d.%02d.%04d %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    nomer_str_rozkor,
    iceb_u_kolbait(20,naim_kontr.ravno()),
    iceb_u_kolbait(20,naim_kontr.ravno()),
    naim_kontr.ravno(),
    iceb_u_kolbait(10,row[5]),row[5],
    dv,mv,gv,
    iceb_u_kolbait(10,nomdok.ravno()),nomdok.ravno(),
    "1",
    d,m,g,
    iceb_u_kolbait(10,komentout.ravno()),
    iceb_u_kolbait(10,komentout.ravno()),
    komentout.ravno(),
    stroka_xml_kor[0],
    stroka_xml_kor[1],
    stroka_xml_kor[2],
    stroka_xml_kor[3],
    stroka_xml_kor[4]);

   }

            
  rasrnn_v_str_xml8(++nomer_str_xml,d,m,g,nomdok.ravno(),naim_kontr.ravno(),row[5],stroka_xml,row[13],ff_xml);
  
   

  if(ff_xml_dot != NULL)
   {
    rasrnn_v_str_xml8(nomer_str_xml,d,m,g,nomdok.ravno(),naim_kontr.ravno(),row[5],stroka_xml_dot,row[13],ff_xml_dot);
   }  

  if(korrekt == 0)
   {
    if(dod5->innn2.find(row[5]) < 0)
     dod5->innn2.plus(row[5]);    

    if(mv != 0)   
     sprintf(strsql,"%s|%02d.%04d",row[5],mv,gv);
    else
     sprintf(strsql,"%s|%02d.%04d",row[5],md,gd);
     
    if((nomer_innn=dod5->innn_per2.find(strsql)) < 0)
     {
      dod5->innn_per2.plus(strsql);    
      dod5->naim_kontr2.plus(row[6]);
     }  

     dod5->suma_bez_nds2.plus(sumd-snds,nomer_innn);

    if(lnds == 4)
     {
      dod5->suma_nds2.plus(0.,nomer_innn);
      dod5->suma_nds2_7.plus(snds,nomer_innn);
     }
    else /*нужно учитывать что lnds может принимать разные значения а в массывы должно быть добавлено при любом значении lnds*/
     {
      dod5->suma_nds2.plus(snds,nomer_innn);
      dod5->suma_nds2_7.plus(0.,nomer_innn);
     }
    if(metka_pods == 3) /*Учёт основных средств*/
     {
      dod5->os_sr2[0]+=sumd-snds;
      dod5->os_sr2[1]+=snds;
     }
   }



 }
fprintf(ff_prot,"%*s:%10.2f %8.2f\n",iceb_u_kolbait(27,gettext("Итого")),gettext("Итого"),sumd_pr,sumnds_pr);

rasrnn_v_rm8(proc_dot,itogo_mes,itogo_o1,itogo_o2);

rpvnn_vi8(gettext("Всего за месяц"),itogo_o1,itogo_ou,ff);
if(ff_dot != NULL)
  rpvnn_vi8(gettext("Всего за месяц"),itogo_o2,itogo_ou,ff_dot);

for(int nom=0; nom < RAZMER_MAS; nom++)
 itogo_o[nom]+=itogo_mes[nom];

rasrnn_v_rm8(proc_dot,itogo_o,itogo_o1,itogo_o2);

if(mn != mk)
 {
  rpvnn_vi8(gettext("Общий итог"),itogo_o1,itogo_ou,ff);
  if(ff_dot != NULL)
    rpvnn_vi8(gettext("Общий итог"),itogo_o2,itogo_ou,ff_dot);
  
 }
rasrnn_v_xml_itog8(itogo_o1,itogo_ou,ff_xml);
if(ff_xml_dot != NULL)
  rasrnn_v_xml_itog8(itogo_o2,itogo_ou,ff_xml_dot);

if(nomer_str_rozkor > 0)
 {
  fprintf(ff_xml_rozkor,"   <R011G9>%.2f</R011G9>\n",itogo_rozkor[0]);//9
  fprintf(ff_xml_rozkor,"   <R011G10>%.2f</R011G10>\n",itogo_rozkor[1]);//10
  fprintf(ff_xml_rozkor,"   <R011G11>%.2f</R011G11>\n",itogo_rozkor[2]);//11
  fprintf(ff_xml_rozkor,"   <R011G12>%.2f</R011G12>\n",itogo_rozkor[3]);//12

  fprintf(ff_xml_rozkor,"   <R011G13>%.2f</R011G13>\n",0.);//13

  fprintf(ff_lst_rozkor,"%*s:%10.2f %10.2f %10.2f %10.2f %10.2f\n",iceb_u_kolbait(92,gettext("Итого")),gettext("Итого"),
  itogo_rozkor[0],
  itogo_rozkor[1],
  itogo_rozkor[2],
  itogo_rozkor[3],
  itogo_rozkor[4]);
 }

return(0);
}
/**********************************/
/*Запись строки в xml файл*/
/**************************/

void rasrnn_v_str_xml8(int nomer_str,
short d,short m,short g,
const char *nomer_dok,
const char *naim_kontr,
const char *inn,
double *suma,
const char *viddok,
FILE *ff_xml)
{

fprintf(ff_xml,"   <T1RXXXXG1 ROWNUM=\"%d\">%d</T1RXXXXG1>\n",nomer_str,nomer_str); //1
fprintf(ff_xml,"   <T1RXXXXG2D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG2D>\n",nomer_str,d,m,g); //2

if(nomer_dok[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG3S ROWNUM=\"%d\">%s</T1RXXXXG3S>\n",nomer_str,iceb_u_filtr_xml(nomer_dok)); //3
else
 fprintf(ff_xml,"   <T1RXXXXG3S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG3S>\n",nomer_str); //3


if(viddok[0] != '\0')
 {
  class iceb_u_str r1("");
  class iceb_u_str r2("");
  class iceb_u_str r3("");

  if(iceb_u_polen(viddok,&r1,1,',') != 0)
   r1.new_plus(viddok);
  else
   {
    iceb_u_polen(viddok,&r2,2,',');
    iceb_u_polen(viddok,&r3,3,',');
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
   fprintf(ff_xml,"   <T1RXXXXG41S ROWNUM=\"%d\">%s</T1RXXXXG41S>\n",nomer_str,r1.ravno()); //4
  if(r2.getdlinna() > 1)
   fprintf(ff_xml,"   <T1RXXXXG42S ROWNUM=\"%d\">%s</T1RXXXXG42S>\n",nomer_str,r2.ravno()); //4
  if(r3.getdlinna() > 1)
   fprintf(ff_xml,"   <T1RXXXXG43S ROWNUM=\"%d\">%s</T1RXXXXG43S>\n",nomer_str,r3.ravno()); //4
 }

if(naim_kontr[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG5S ROWNUM=\"%d\">%s</T1RXXXXG5S>\n",nomer_str,iceb_u_filtr_xml(naim_kontr)); //5
else
 fprintf(ff_xml,"   <T1RXXXXG5S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG5S>\n",nomer_str); //5

if(inn[0] == '\0' || isdigit(inn[0]) == 0) /*Значит символ не число*/
  fprintf(ff_xml,"   <T1RXXXXG6 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer_str); //6
else
  fprintf(ff_xml,"   <T1RXXXXG6 ROWNUM=\"%d\">%s</T1RXXXXG6>\n",nomer_str,inn); //6

fprintf(ff_xml,"   <T1RXXXXG7 ROWNUM=\"%d\">%.2f</T1RXXXXG7>\n",nomer_str,suma[0]); //7

fprintf(ff_xml,"   <T1RXXXXG8 ROWNUM=\"%d\">%.2f</T1RXXXXG8>\n",nomer_str,suma[1]); //8
fprintf(ff_xml,"   <T1RXXXXG9 ROWNUM=\"%d\">%.2f</T1RXXXXG9>\n",nomer_str,suma[2]); //9

fprintf(ff_xml,"   <T1RXXXXG110 ROWNUM=\"%d\">%.2f</T1RXXXXG110>\n",nomer_str,suma[3]); //10
fprintf(ff_xml,"   <T1RXXXXG111 ROWNUM=\"%d\">%.2f</T1RXXXXG111>\n",nomer_str,suma[4]); //11

fprintf(ff_xml,"   <T1RXXXXG10 ROWNUM=\"%d\">%.2f</T1RXXXXG10>\n",nomer_str,suma[5]); //12
fprintf(ff_xml,"   <T1RXXXXG113 ROWNUM=\"%d\">%.2f</T1RXXXXG113>\n",nomer_str,suma[6]); //13
fprintf(ff_xml,"   <T1RXXXXG114 ROWNUM=\"%d\">%.2f</T1RXXXXG114>\n",nomer_str,suma[7]); //14
fprintf(ff_xml,"   <T1RXXXXG12 ROWNUM=\"%d\">%.2f</T1RXXXXG12>\n",nomer_str,suma[8]); //15
fprintf(ff_xml,"   <T1RXXXXG13 ROWNUM=\"%d\">%.2f</T1RXXXXG13>\n",nomer_str,suma[9]); //16



}
/*****************************/
/*Вывод в xml файл расчёта корректировки*/
/*******************************/
void rasrnn_v_vvrozkor8(int *nomer_str,
short dnn,short mnn,short gnn, //Дата выписки налоговой накладной
short dpn,short mpn,short gpn, //Дата получения налоговой накладной
const char *nomer_dok,
const char *naim_kontr,
const char *inn,
double *stroka_xml,
const char *koment,
int lnds,
FILE *ff_xml_rozkor)
{
*nomer_str+=1;
 
fprintf(ff_xml_rozkor,"   <T11RXXXXG2S ROWNUM=\"%d\">%s</T11RXXXXG2S>\n",*nomer_str,iceb_u_filtr_xml(naim_kontr));//2
fprintf(ff_xml_rozkor,"   <T11RXXXXG3 ROWNUM=\"%d\">%s</T11RXXXXG3>\n",*nomer_str,inn);//3
fprintf(ff_xml_rozkor,"   <T11RXXXXG4D ROWNUM=\"%d\">%02d%02d%04d</T11RXXXXG4D>\n",*nomer_str,dpn,mpn,gpn);//4
fprintf(ff_xml_rozkor,"   <T11RXXXXG5S ROWNUM=\"%d\">%s</T11RXXXXG5S>\n",*nomer_str,iceb_u_filtr_xml(nomer_dok));//5
fprintf(ff_xml_rozkor,"   <T11RXXXXG6S ROWNUM=\"%d\">1</T11RXXXXG6S>\n",*nomer_str); /*6 номер расчёта корректировки*/
fprintf(ff_xml_rozkor,"   <T11RXXXXG7D ROWNUM=\"%d\">%02d%02d%04d</T11RXXXXG7D>\n",*nomer_str,dnn,mnn,gnn);//7
fprintf(ff_xml_rozkor,"   <T11RXXXXG8S ROWNUM=\"%d\">%s</T11RXXXXG8S>\n",*nomer_str,iceb_u_filtr_xml(koment));//8

fprintf(ff_xml_rozkor,"   <T11RXXXXG9 ROWNUM=\"%d\">%.2f</T11RXXXXG9>\n",*nomer_str,stroka_xml[0]);//9
fprintf(ff_xml_rozkor,"   <T11RXXXXG10 ROWNUM=\"%d\">%.2f</T11RXXXXG10>\n",*nomer_str,stroka_xml[1]);//10
fprintf(ff_xml_rozkor,"   <T11RXXXXG11 ROWNUM=\"%d\">%.2f</T11RXXXXG11>\n",*nomer_str,stroka_xml[2]);//11
fprintf(ff_xml_rozkor,"   <T11RXXXXG12 ROWNUM=\"%d\">%.2f</T11RXXXXG12>\n",*nomer_str,stroka_xml[3]);//12
fprintf(ff_xml_rozkor,"   <T11RXXXXG13 ROWNUM=\"%d\">%.2f</T11RXXXXG13>\n",*nomer_str,stroka_xml[4]);//13

}
/********************************/
/*Распечатать итог*/
/*****************************/
void rasrnn_v_xml_itog8(double *itog,double *itog_u,FILE *ff_xml)
{
fprintf(ff_xml,"   <R011G7>%.2f</R011G7>\n",itog[0]);
fprintf(ff_xml,"   <R011G8>%.2f</R011G8>\n",itog[1]);
fprintf(ff_xml,"   <R011G9>%.2f</R011G9>\n",itog[2]);
fprintf(ff_xml,"   <R011G110>%.2f</R011G110>\n",itog[3]);
fprintf(ff_xml,"   <R011G111>%.2f</R011G111>\n",itog[4]);
fprintf(ff_xml,"   <R011G10>%.2f</R011G10>\n",itog[5]);
fprintf(ff_xml,"   <R011G113>%.2f</R011G113>\n",itog[6]);
fprintf(ff_xml,"   <R011G114>%.2f</R011G114>\n",itog[7]);
fprintf(ff_xml,"   <R011G12>%.2f</R011G12>\n",itog[8]);
fprintf(ff_xml,"   <R011G13>%.2f</R011G13>\n",itog[9]);

fprintf(ff_xml,"   <R012G7>%.2f</R012G7>\n",itog_u[0]);
fprintf(ff_xml,"   <R012G8>%.2f</R012G8>\n",itog_u[1]);
fprintf(ff_xml,"   <R012G9>%.2f</R012G9>\n",itog_u[2]);
fprintf(ff_xml,"   <R012G110>%.2f</R012G110>\n",itog_u[3]);
fprintf(ff_xml,"   <R012G111>%.2f</R012G111>\n",itog_u[4]);
fprintf(ff_xml,"   <R012G10>%.2f</R012G10>\n",itog_u[5]);
fprintf(ff_xml,"   <R012G113>%.2f</R012G113>\n",itog_u[6]);
fprintf(ff_xml,"   <R012G114>%.2f</R012G114>\n",itog_u[7]);
fprintf(ff_xml,"   <R012G12>%.2f</R012G12>\n",itog_u[8]);
fprintf(ff_xml,"   <R012G13>%.2f</R012G13>\n",itog_u[9]);
}
/************************/
/*разбивка массива по проценту на два массива*/
/*******************************/
void rasrnn_v_rm8(double proc_dot,double *itogo_o,double *itogo_o1,double *itogo_o2)
{
memset(itogo_o1,'\0',sizeof(double)*RAZMER_MAS);
memset(itogo_o2,'\0',sizeof(double)*RAZMER_MAS);

for(int nom=0; nom < RAZMER_MAS; nom++)
 itogo_o1[nom]=itogo_o[nom];

if(proc_dot != 0.)
 for(int nom=0; nom < RAZMER_MAS; nom++)
  {   
   itogo_o1[nom]=itogo_o[nom]*(100.-proc_dot)/100.;
   itogo_o1[nom]=iceb_u_okrug(itogo_o1[nom],0.01);
   itogo_o2[nom]=itogo_o[nom]-itogo_o1[nom];
  }

}
/***********************/
/*вывод строки продолжения*/
/**************************/
void rasrnn_v_str8(const char *nomdok,const char *naim_kontr,int lnds,double sumd,double snds,
double *stroka_xml,
const char *viddok,
FILE *ff)
{

for(int nom=1; nom < 4; nom+=2)
 {
  if(stroka_xml[nom] != 0.)
   fprintf(ff,"%9.2f|",stroka_xml[nom]);
  else
   fprintf(ff,"%9s|","");

  if(stroka_xml[nom+1] != 0.) 
    fprintf(ff,"%8.2f|",stroka_xml[nom+1]);
  else
   {
    if(stroka_xml[nom] != 0.)
     fprintf(ff,"XXXXXXXX|");
    else
     fprintf(ff,"%8s|","");
   }     
 }

for(int nom=5; nom < RAZMER_MAS-1; nom++)
 {
  if(stroka_xml[nom] != 0.)
   fprintf(ff,"%9.2f|",stroka_xml[nom]);
  else  
   fprintf(ff,"%9s|","");
 }

if(stroka_xml[9] != 0.)
 fprintf(ff,"%10.2f|",stroka_xml[9]);
else  
 fprintf(ff,"%10s|","");
 

fprintf(ff,"\n");
if(iceb_u_strlen(naim_kontr) > 20 || iceb_u_strlen(nomdok) > 8 || iceb_u_strlen(viddok) > 5)
 {
  fprintf(ff,"|%5s|%10s|%-*.*s|%-*.*s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%8s|%9s|%9s|%9s|%9s|%10s|\n",
  "","",
  iceb_u_kolbait(8,iceb_u_adrsimv(8,nomdok)),iceb_u_kolbait(8,iceb_u_adrsimv(8,nomdok)),iceb_u_adrsimv(8,nomdok),
  iceb_u_kolbait(5,iceb_u_adrsimv(5,viddok)),iceb_u_kolbait(5,iceb_u_adrsimv(5,viddok)),iceb_u_adrsimv(5,viddok),
  iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),iceb_u_adrsimv(20,naim_kontr),
  "","","","","","","","","","","");
 }
for(int nom=40; nom < iceb_u_strlen(naim_kontr); nom+=20)
 {
  fprintf(ff,"|%5s|%10s|%8s|%5s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%8s|%9s|%9s|%9s|%9s|%10s|\n",
  "","","","",
  iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),iceb_u_adrsimv(nom,naim_kontr),
  "","","","","","","","","","","");
 }

}
/**************************/
void rpvnn_vi8s(char *vsego,double *itogo,FILE *ff)
{

if(itogo[1] != 0.)//8
 fprintf(ff,"%9.2f|",itogo[1]);
else
 fprintf(ff,"%9s|","");

if(itogo[2] != 0.)//9
 fprintf(ff,"%8.2f|",itogo[2]);
else
 fprintf(ff,"%8s|","");


if(itogo[3] != 0.)//10
 fprintf(ff,"%9.2f|",itogo[3]);
else
 fprintf(ff,"%9s|","");

if(itogo[4] != 0.)//11
 fprintf(ff,"%8.2f|",itogo[4]);
else
 fprintf(ff,"%8s|","");


if(itogo[5] != 0.)//12
 fprintf(ff,"%9.2f|",itogo[5]);
else
 fprintf(ff,"%9s|","");

if(itogo[6] != 0.)//13
 fprintf(ff,"%9.2f|",itogo[6]);
else
 fprintf(ff,"%9s|","");

if(itogo[7] != 0.)//14
 fprintf(ff,"%9.2f|",itogo[7]);
else
 fprintf(ff,"%9s|","");

if(itogo[8] != 0.)//15
 fprintf(ff,"%9.2f|",itogo[8]);
else
 fprintf(ff,"%9s|","");

if(itogo[9] != 0.)//16
 fprintf(ff,"%10.2f|",itogo[9]);
else
 fprintf(ff,"%10s|","");

fprintf(ff,"\n");
}
/***********************************************/

void rpvnn_vi8(char *vsego,double *itogo,double *itogo_u,FILE *ff)
{
class iceb_u_str str_pod;


str_pod.plus("-------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

fprintf(ff,"%s\n",str_pod.ravno());

fprintf(ff,"|%*s|%10.2f|",iceb_u_kolbait(67,vsego),vsego,itogo[0]);//7

rpvnn_vi8s(vsego,itogo,ff);

fprintf(ff,"%s\n",str_pod.ravno());


const char *repl={"З них включено до уточнюючих розрахунків за звітний період"};

fprintf(ff,"|%*s|%10.2f|",iceb_u_kolbait(67,repl),repl,itogo_u[0]);//7

rpvnn_vi8s(vsego,itogo_u,ff);

fprintf(ff,"%s\n",str_pod.ravno());

}
