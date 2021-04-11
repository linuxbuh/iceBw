/*$Id: rasrnn_vw.c,v 1.37 2014/07/31 07:08:26 sasa Exp $*/
/*20.05.2016	30.08.2005	Белых А.И.	rasrnn_v7w.c
Распечатка реестра выданных налоговых накладных 
*/
#include <ctype.h>
#include <errno.h>
#include        "buhg_g.h"
#include "dbfhead.h"
#include "xrnn_poiw.h"
#include "rnn_d5w.h"
#define RAZMER_MAS 7
void rasrnn_v_xml_itog(double *itog,double *itogo_u,FILE *ff_xml);
void rasrnn_v_str_xml(int nomer_str,short d,short m,short g,const char *nomer_dok,const char *naim_kontr,const char *inn,double *suma,const char *viddok,const char *metka_7,FILE *ff_xml);
void rasrnn_v_rm(double proc_dot,double *itogo_o,double *itogo_o1,double *itogo_o2);
void rasrnn_v_str(short dn,short mn,short gn,const char *nomdok,const char *naim_kontr,int metka_dok,double sumd,double snds,int *kol_strok,double *itogo_mes,double *stroka_xml,FILE *ff);

void rasrnn_v_vvrozkor(int *nomer_str,short dnn,short mnn,short gnn,short dpn,short mpn,short gpn,const char *nomer_dok,const char *naim_kontr,const char *inn,double suma_bez_nds,double suma_nds,const char *koment,FILE *ff_xml_rozkor);
void rpvnn_viw(short dn,short mn,short gn,char *vsego,double *itogo,double *itogo_u,int *kol_strok,FILE *ff);

extern int kol_strok_na_liste;
extern SQL_baza bd;

int rasrnn_v7w(class xrnn_poi *data,
int kol_strok,
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
char strsql[512];
short dn,mn,gn;
short dk,mk,gk;
class iceb_u_str imafil_xml;
//iceb_rsdatp(&dn,&mn,&gn,data->datan.ravno(),&dk,&mk,&gk,data->datak.ravno(),wpredok);

iceb_u_rsdat(&dn,&mn,&gn,data->datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->datak.ravno(),1);
sprintf(strsql,"\n%s %d.%d.%d => %d.%d.%d\n",gettext("Реестр выданных налоговых накладных"),dn,mn,gn,dk,mk,gk);
iceb_printw(strsql,buffer,view);


class iceb_u_str sort("");
if((data->metka_sort=rnn_vsw()) < 0)
 return(1);
if(data->metka_sort == 0)
 sort.new_plus(" order by datd asc,nnni asc");
else
 sort.new_plus(" order by datd asc,dnz asc");
//printw("Реестр выданных накладных\n");

sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv from Reenn");

if(dn != 0 && dk == 0)
  sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv from Reenn where \
datd >= '%04d-%02d-%02d'",gn,mn,dn);

if(dn != 0 && dk != 0)
  sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv from Reenn where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);

strcat(strsql,sort.ravno());


SQLCURSOR cur;
SQL_str row;
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("Не найдено ни одной записи !"));
  iceb_printw(strsql,buffer,view);
  return(1);
 }
double proc_dot=data->proc_dot.ravno_atof();

class iceb_u_str imafalx("");


if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) < 0)
  imafalx.new_plus("rpvnn2.alx");


if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0 && iceb_u_sravmydat(dn,mn,gn,1,1,2011) < 0)
  imafalx.new_plus("rpvnn2_1.alx");

if(iceb_u_sravmydat(dn,mn,gn,1,1,2011) >= 0 && iceb_u_sravmydat(dn,mn,gn,1,3,2013) < 0)
  imafalx.new_plus("rpvnn2_2.alx");

if(iceb_u_sravmydat(dn,mn,gn,1,3,2013) >= 0)
  imafalx.new_plus("rpvnn2_3.alx");

//Вставляем шапку 
kol_strok+=iceb_insfil(imafalx.ravno(),ff,wpredok);
if(ff_dot != NULL)
   iceb_insfil(imafalx.ravno(),ff_dot,wpredok);

iceb_zagolov(gettext("Реестр выданных налоговых накладных"),dn,mn,gn,dk,mk,gk,ff_prot,wpredok);
if(data->kodgr1.ravno()[0] != '\0')
 fprintf(ff_prot,"%s:%s\n",gettext("Группа"),data->kodgr1.ravno());

rpvnn_ps_pw(ff_prot);

fprintf(ff_lst_rozkor,"\
-----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_lst_rozkor,"\
   |                               |Податкова накладна,  |Розрахунок коригуван.|Підстава  | Обсяг    | Сума     |\n\
   |    Отримувач (покупець)       |за якою пров.коригув.|кількісних і вартісн.|для кориг.|поставки  |  ПДВ     |\n\
   |                               |                     |показників до под.нак|податкових|(без ПДВ) | (+чи-)   |\n\
   |---------------------------------------------------------------------------|зобов'яза.| (+чи-)   |          |\n\
 N |   Найменування     |Індивідуал|  Дата    |  Номер   | Номер    |Дата отрим|з податку |          |          |\n\
   |(П.І.Б. для фізичних|  ьний    |          |          |          |мання отри|на додану |          |          |\n\
   |      осіб)         |подаковий |          |          |          |мувачем   |вартість  |          |          |\n\
   |                    | номер    |          |          |          |(покупцем)|          |          |          |\n\
   |                    |          |          |          |          |розрахунку|          |          |          |\n");
fprintf(ff_lst_rozkor,"\
-----------------------------------------------------------------------------------------------------------------\n");

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
//npz=atoi(data->nom_p_zm);
if(npz != 0)
 nomer_str=npz-1;

short mz=0;
short d,m,g;
short dd,md,gd;
int kolstr1=0;
int metka_dok=0;
int metka_pods;
class iceb_u_str podsis("");
double sumd_pr=0.;
double sumnds_pr=0.;
class iceb_u_str nomdok("");
class iceb_u_str naim_kontr("");
int nomer_str_xml=0;
double stroka_xml[RAZMER_MAS];
double stroka_xml_dot[RAZMER_MAS];
double itogo_o1[RAZMER_MAS];
double itogo_o2[RAZMER_MAS];
int por_nom_dok=0;
int nomer_innn=0;
int ibros=0;
int korrekt=0;
int nomer_str_rozkor=0;
double itogo_rozkor1=0.;
double itogo_rozkor2=0.;
short dv,mv,gv;
class iceb_u_str metka_7("");
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(bar,kolstr,++kolstr1);    

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

/*********************
  if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
  if((gtype=rnnvd(2,row[13])) <= 0) //ТИП документа
   {
    sprintf(strsql,"Неизвестный тип документа %d %s !\n%d.%d.%d %s",
    gtype,row[13],d,m,g,row[1]);
    iceb_t_soob(strsql);
   }
*********************/

  korrekt=0;  
  if(iceb_u_strstrm(row[13],"У") == 1 || iceb_u_SRAV(row[13],"РКП",0) == 0 || iceb_u_SRAV(row[13],"РКЕ",0) == 0) /*Если в виде документа присутствует буква У - то это коректирующий документ*/
    korrekt=1;

  if(mz != m)
   {
    if(mz != 0)
     {
      rasrnn_v_rm(proc_dot,itogo_mes,itogo_o1,itogo_o2);
      rpvnn_viw(dn,mn,gn,gettext("Всего за месяц"),itogo_o1,itogo_ou,&kol_strok,ff);
      if(ff_dot != NULL)
        rpvnn_viw(dn,mn,gn,gettext("Всего за месяц"),itogo_o2,itogo_ou,&ibros,ff_dot);
     }
    itogo_o[0]+=itogo_mes[0];
    itogo_o[1]+=itogo_mes[1];
    itogo_o[2]+=itogo_mes[2];
    itogo_o[3]+=itogo_mes[3];
    itogo_o[4]+=itogo_mes[4];
    itogo_o[5]+=itogo_mes[5];
    itogo_o[6]+=itogo_mes[6];
    
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

  metka_dok=atoi(row[4]);

  if(korrekt == 1)
   {
    itogo_ou[0]+=sumd;
    if(metka_dok == 0)
     {
      itogo_ou[1]+=sumd-snds;
      itogo_ou[2]+=snds;
     }
    if(metka_dok == 1)
     itogo_ou[3]+=sumd;
    if(metka_dok == 2)
     itogo_ou[4]+=sumd;
    if(metka_dok == 3)
     itogo_ou[5]+=sumd;
   }  
  sumd_pr+=sumd;
  sumnds_pr+=snds;
  
  metka_pods=atoi(row[7]);
  nomdok.new_plus(row[1]);

  naim_kontr.new_plus(row[6]);

  if(metka_dok == 4)
   metka_7.new_plus("7");
  else
   metka_7.new_plus("");
  
  if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
   {
    fprintf(ff,"|%-5d|%02d.%02d.%04d|%-*.*s|%-2.2s|%-*.*s|%-*.*s|%-*s|%10.2f|",
    ++nomer_str,d,m,g,
    iceb_u_kolbait(8,nomdok.ravno()),iceb_u_kolbait(8,nomdok.ravno()),nomdok.ravno(),
    metka_7.ravno(),
    iceb_u_kolbait(5,row[13]),iceb_u_kolbait(5,row[13]),row[13],
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    iceb_u_kolbait(14,row[5]),row[5],
    sumd1);
    
    if(ff_dot != NULL)
      fprintf(ff_dot,"|%-5d|%02d.%02d.%04d|%-*.*s|%-2.2s|%-*.*s|%-*.*s|%-*s|%10.2f|",
      nomer_str,d,m,g,
      iceb_u_kolbait(8,nomdok.ravno()),iceb_u_kolbait(8,nomdok.ravno()),nomdok.ravno(),
      metka_7.ravno(),
      iceb_u_kolbait(5,row[13]),iceb_u_kolbait(5,row[13]),row[13],
      iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
      iceb_u_kolbait(14,row[5]),row[5],
      sumd-sumd1);
   }
  else
   {
    fprintf(ff,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*s|%10.2f|",
    ++nomer_str,d,m,g,
    iceb_u_kolbait(8,nomdok.ravno()),iceb_u_kolbait(8,nomdok.ravno()),nomdok.ravno(),
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    iceb_u_kolbait(14,row[5]),row[5],
    sumd1);
    
    if(ff_dot != NULL)
      fprintf(ff_dot,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*s|%10.2f|",
      nomer_str,d,m,g,
      iceb_u_kolbait(8,nomdok.ravno()),iceb_u_kolbait(8,nomdok.ravno()),nomdok.ravno(),
      iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
      iceb_u_kolbait(14,row[5]),row[5],
      sumd-sumd1);
    }  
  memset(stroka_xml,'\0',sizeof(stroka_xml));
  memset(stroka_xml_dot,'\0',sizeof(stroka_xml_dot));
  
  itogo_mes[0]+=sumd;
  
  stroka_xml[0]=sumd1;
  stroka_xml_dot[0]=sumd-sumd1;
  
  rasrnn_v_str(dn,mn,gn,nomdok.ravno(),naim_kontr.ravno(),metka_dok,sumd1,snds1,&kol_strok,itogo_mes,stroka_xml,ff);
  if(ff_dot != NULL)
    rasrnn_v_str(dn,mn,gn,nomdok.ravno(),naim_kontr.ravno(),metka_dok,sumd-sumd1,snds-snds1,&kol_strok,itogo_mes,stroka_xml_dot,ff_dot);

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
  row[2],row[3],metka_dok,row[10],row[5],
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
    rasrnn_v_vvrozkor(&nomer_str_rozkor,d,m,g,dv,mv,gv,nomdok.ravno(),naim_kontr.ravno(),row[5],(sumd-snds),snds,komentout.ravno(),ff_xml_rozkor);
    
    fprintf(ff_lst_rozkor,"%3d %-*.*s %*s %02d.%02d.%04d %-*s %-10s %02d.%02d.%04d %-*.*s %10.2f %10.2f\n",
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
    sumd-snds,
    snds);

    itogo_rozkor1+=sumd-snds;
    itogo_rozkor2+=snds;
   }
     
  rasrnn_v_str_xml(++nomer_str_xml,d,m,g,nomdok.ravno(),naim_kontr.ravno(),row[5],stroka_xml,row[13],metka_7.ravno(),ff_xml);
  
   

  if(ff_xml_dot != NULL)
   {
    rasrnn_v_str_xml(nomer_str_xml,d,m,g,nomdok.ravno(),naim_kontr.ravno(),row[5],stroka_xml_dot,row[13],metka_7.ravno(),ff_xml_dot);
   }  

  if(korrekt == 0)
  if(snds != 0.)
   {
/**********************
    if((nomer_innn=dod5->innn2.find(row[5])) < 0)
     {
      dod5->innn2.plus(row[5]);    
      dod5->naim_kontr2.plus(row[6]);
     }
    dod5->suma_bez_nds2.plus(sumd-snds,nomer_innn);
    dod5->suma_nds2.plus(snds,nomer_innn);
    if(metka_pods == 3) //Учёт основных средств
     {
      dod5->os_sr2[0]+=sumd-snds;
      dod5->os_sr2[1]+=snds;
     }
*************************/
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
    dod5->suma_nds2.plus(snds,nomer_innn);
    if(metka_pods == 3) /*Учёт основных средств*/
     {
      dod5->os_sr2[0]+=sumd-snds;
      dod5->os_sr2[1]+=snds;
     }

   }

  if(korrekt == 0)
  if(metka_dok == 1)
   {

   if(dod5->innn2d.find(row[5]) < 0)
    dod5->innn2d.plus(row[5]);    

   if(mv != 0)   
    sprintf(strsql,"%s|%02d.%04d",row[5],mv,gv);
   else
    sprintf(strsql,"%s|%02d.%04d",row[5],md,gd);
    
   if((nomer_innn=dod5->innn_per2d.find(strsql)) < 0)
    {
     dod5->innn_per2d.plus(strsql);    
     dod5->naim_kontr2d.plus(row[6]);
    }  

    dod5->suma_bez_nds2d.plus(sumd-snds,nomer_innn);
    dod5->suma_nds2d.plus(snds,nomer_innn);
    if(metka_pods == 3) /*Учёт основных средств*/
     {
      dod5->os_sr2[0]+=sumd-snds;
      dod5->os_sr2[1]+=snds;
     }

   }
  if(korrekt == 0)
  if(metka_dok == 2)
   {

   if(dod5->innn2d1.find(row[5]) < 0)
    dod5->innn2d1.plus(row[5]);    

   if(mv != 0)   
    sprintf(strsql,"%s|%02d.%04d",row[5],mv,gv);
   else
    sprintf(strsql,"%s|%02d.%04d",row[5],md,gd);
    
   if((nomer_innn=dod5->innn_per2d1.find(strsql)) < 0)
    {
     dod5->innn_per2d1.plus(strsql);    
     dod5->naim_kontr2d1.plus(row[6]);
    }  

    dod5->suma_bez_nds2d1.plus(sumd-snds,nomer_innn);
    dod5->suma_nds2d1.plus(snds,nomer_innn);
    if(metka_pods == 3) /*Учёт основных средств*/
     {
      dod5->os_sr2[0]+=sumd-snds;
      dod5->os_sr2[1]+=snds;
     }

   }

 }
fprintf(ff_prot,"%*s:%10.2f %8.2f\n",iceb_u_kolbait(27,gettext("Итого")),gettext("Итого"),sumd_pr,sumnds_pr);

rasrnn_v_rm(proc_dot,itogo_mes,itogo_o1,itogo_o2);

rpvnn_viw(dn,mn,gn,gettext("Всего за месяц"),itogo_o1,itogo_ou,&kol_strok,ff);
if(ff_dot != NULL)
  rpvnn_viw(dn,mn,gn,gettext("Всего за месяц"),itogo_o2,itogo_ou,&ibros,ff_dot);

itogo_o[0]+=itogo_mes[0];
itogo_o[1]+=itogo_mes[1];
itogo_o[2]+=itogo_mes[2];
itogo_o[3]+=itogo_mes[3];
itogo_o[4]+=itogo_mes[4];
itogo_o[5]+=itogo_mes[5];
itogo_o[6]+=itogo_mes[6];

rasrnn_v_rm(proc_dot,itogo_o,itogo_o1,itogo_o2);

if(mn != mk)
 {
  rpvnn_viw(dn,mn,gn,gettext("Общий итог"),itogo_o1,itogo_ou,&kol_strok,ff);
  if(ff_dot != NULL)
    rpvnn_viw(dn,mn,gn,gettext("Общий итог"),itogo_o2,itogo_ou,&ibros,ff_dot);
  
 }
rasrnn_v_xml_itog(itogo_o1,itogo_ou,ff_xml);
if(ff_xml_dot != NULL)
  rasrnn_v_xml_itog(itogo_o2,itogo_ou,ff_xml_dot);
if(nomer_str_rozkor > 0)
 {
  fprintf(ff_xml_rozkor,"   <R01G9>%.2f</R01G9>\n",itogo_rozkor1);
  fprintf(ff_xml_rozkor,"   <R01G10>%.2f</R01G10>\n",itogo_rozkor2);
  fprintf(ff_lst_rozkor,"%*s:%10.2f %10.2f\n",
  iceb_u_kolbait(96,gettext("Итого")),gettext("Итого"),
  itogo_rozkor1,itogo_rozkor2);  
 }

return(0);
}
/**********************************/
/*Запись строки в xml файл*/
/**************************/

void rasrnn_v_str_xml(int nomer_str,
short d,short m,short g,
const char *nomer_dok,
const char *naim_kontr,
const char *inn,
double *suma,
const char *viddok,
const char *metka_7,
FILE *ff_xml)
{

fprintf(ff_xml,"   <T1RXXXXG1 ROWNUM=\"%d\">%d</T1RXXXXG1>\n",nomer_str,nomer_str); //1
fprintf(ff_xml,"   <T1RXXXXG2D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG2D>\n",nomer_str,d,m,g); //2

if(nomer_dok[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG3S ROWNUM=\"%d\">%s</T1RXXXXG3S>\n",nomer_str,iceb_u_filtr_xml(nomer_dok)); //3
else
 fprintf(ff_xml,"   <T1RXXXXG3S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG3S>\n",nomer_str); //3

if(metka_7[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG31 ROWNUM=\"%d\">%s</T1RXXXXG31>\n",nomer_str,metka_7);

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
fprintf(ff_xml,"   <T1RXXXXG10 ROWNUM=\"%d\">%.2f</T1RXXXXG10>\n",nomer_str,suma[3]); //10
fprintf(ff_xml,"   <T1RXXXXG11 ROWNUM=\"%d\">%.2f</T1RXXXXG11>\n",nomer_str,suma[4]); //11
fprintf(ff_xml,"   <T1RXXXXG12 ROWNUM=\"%d\">%.2f</T1RXXXXG12>\n",nomer_str,suma[5]); //12
fprintf(ff_xml,"   <T1RXXXXG13 ROWNUM=\"%d\">%.2f</T1RXXXXG13>\n",nomer_str,suma[6]); //13



}
/*****************************/
/*Вывод в xml файл расчёта корректировки*/
/*******************************/
void rasrnn_v_vvrozkor(int *nomer_str,
short dnn,short mnn,short gnn, //Дата выписки налоговой накладной
short dpn,short mpn,short gpn, //Дата получения налоговой накладной
const char *nomer_dok,
const char *naim_kontr,
const char *inn,
double suma_bez_nds,
double suma_nds,
const char *koment,
FILE *ff_xml_rozkor)
{
*nomer_str+=1;
 
fprintf(ff_xml_rozkor,"   <T1RXXXXG2S ROWNUM=\"%d\">%s</T1RXXXXG2S>\n",*nomer_str,iceb_u_filtr_xml(naim_kontr));
fprintf(ff_xml_rozkor,"   <T1RXXXXG3 ROWNUM=\"%d\">%s</T1RXXXXG3>\n",*nomer_str,inn);
fprintf(ff_xml_rozkor,"   <T1RXXXXG4D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG4D>\n",*nomer_str,dpn,mpn,gpn);
fprintf(ff_xml_rozkor,"   <T1RXXXXG5S ROWNUM=\"%d\">%s</T1RXXXXG5S>\n",*nomer_str,iceb_u_filtr_xml(nomer_dok));
fprintf(ff_xml_rozkor,"   <T1RXXXXG6S ROWNUM=\"%d\">1</T1RXXXXG6S>\n",*nomer_str); /*номер расчёта корректировки*/
fprintf(ff_xml_rozkor,"   <T1RXXXXG7D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG7D>\n",*nomer_str,dnn,mnn,gnn);
fprintf(ff_xml_rozkor,"   <T1RXXXXG8S ROWNUM=\"%d\">%s</T1RXXXXG8S>\n",*nomer_str,iceb_u_filtr_xml(koment));
fprintf(ff_xml_rozkor,"   <T1RXXXXG9 ROWNUM=\"%d\">%.2f</T1RXXXXG9>\n",*nomer_str,suma_bez_nds);
fprintf(ff_xml_rozkor,"   <T1RXXXXG10 ROWNUM=\"%d\">%.2f</T1RXXXXG10>\n",*nomer_str,suma_nds);

}
/********************************/
/*Распечатать итог*/
/*****************************/
void rasrnn_v_xml_itog(double *itog,double *itog_u,FILE *ff_xml)
{
fprintf(ff_xml,"   <R011G7>%.2f</R011G7>\n",itog[0]);
fprintf(ff_xml,"   <R011G8>%.2f</R011G8>\n",itog[1]);
fprintf(ff_xml,"   <R011G9>%.2f</R011G9>\n",itog[2]);
fprintf(ff_xml,"   <R011G10>%.2f</R011G10>\n",itog[3]);
fprintf(ff_xml,"   <R011G11>%.2f</R011G11>\n",itog[4]);
fprintf(ff_xml,"   <R011G12>%.2f</R011G12>\n",itog[5]);
fprintf(ff_xml,"   <R011G13>%.2f</R011G13>\n",itog[6]);

fprintf(ff_xml,"   <R012G7>%.2f</R012G7>\n",itog_u[0]);
fprintf(ff_xml,"   <R012G8>%.2f</R012G8>\n",itog_u[1]);
fprintf(ff_xml,"   <R012G9>%.2f</R012G9>\n",itog_u[2]);
fprintf(ff_xml,"   <R012G10>%.2f</R012G10>\n",itog_u[3]);
fprintf(ff_xml,"   <R012G11>%.2f</R012G11>\n",itog_u[4]);
fprintf(ff_xml,"   <R012G12>%.2f</R012G12>\n",itog_u[5]);
fprintf(ff_xml,"   <R012G13>%.2f</R012G13>\n",itog_u[6]);
}
/************************/
/*разбивка массива по проценту на два массива*/
/*******************************/
void rasrnn_v_rm(double proc_dot,double *itogo_o,double *itogo_o1,double *itogo_o2)
{
memset(itogo_o1,'\0',RAZMER_MAS);
memset(itogo_o2,'\0',RAZMER_MAS);

itogo_o1[0]=itogo_o[0];
itogo_o1[1]=itogo_o[1];
itogo_o1[2]=itogo_o[2];
itogo_o1[3]=itogo_o[3];
itogo_o1[4]=itogo_o[4];
itogo_o1[5]=itogo_o[5];
itogo_o1[6]=itogo_o[6];

if(proc_dot != 0.)
 {

  itogo_o1[0]=itogo_o[0]*(100.-proc_dot)/100.;
  itogo_o1[1]=itogo_o[1]*(100.-proc_dot)/100.;
  itogo_o1[2]=itogo_o[2]*(100.-proc_dot)/100.;
  itogo_o1[3]=itogo_o[3]*(100.-proc_dot)/100.;
  itogo_o1[4]=itogo_o[4]*(100.-proc_dot)/100.;
  itogo_o1[5]=itogo_o[5]*(100.-proc_dot)/100.;

  itogo_o1[0]=iceb_u_okrug(itogo_o1[0],0.01);
  itogo_o1[1]=iceb_u_okrug(itogo_o1[1],0.01);
  itogo_o1[2]=iceb_u_okrug(itogo_o1[2],0.01);
  itogo_o1[3]=iceb_u_okrug(itogo_o1[3],0.01);
  itogo_o1[4]=iceb_u_okrug(itogo_o1[4],0.01);
  itogo_o1[5]=iceb_u_okrug(itogo_o1[5],0.01);

  itogo_o2[0]=itogo_o[0]-itogo_o1[0];
  itogo_o2[1]=itogo_o[1]-itogo_o1[1];
  itogo_o2[2]=itogo_o[2]-itogo_o1[2];
  itogo_o2[3]=itogo_o[3]-itogo_o1[3];
  itogo_o2[4]=itogo_o[4]-itogo_o1[4];
  itogo_o2[5]=itogo_o[5]-itogo_o1[5];
  itogo_o2[6]=itogo_o[5]-itogo_o1[6];


 }

}
/***********************/
/*вывод строки продолжения*/
/**************************/
void rasrnn_v_str(short dn,short mn,short gn,const char *nomdok,const char *naim_kontr,int metka_dok,double sumd,double snds,int *kol_strok,
double *itogo_mes,
double *stroka_xml,
FILE *ff)
{
if(metka_dok == 0)
 {
  if(snds != 0.)
    fprintf(ff,"%9.2f|%8.2f|",sumd-snds,snds);
  else
    fprintf(ff,"%9.2f|XXXXXXXX|",sumd);
  itogo_mes[1]+=sumd-snds;
  itogo_mes[2]+=snds;    
  stroka_xml[1]=sumd-snds;
  stroka_xml[2]=snds;    
 }
else
 fprintf(ff,"%9s|%8s|","","");

if(metka_dok == 1)
 {
  fprintf(ff,"%9.2f|",sumd);
  itogo_mes[3]+=sumd;
  stroka_xml[3]=sumd;
 }
else
 fprintf(ff,"%9s|","");

if(metka_dok == 2)
 {
  fprintf(ff,"%9.2f|",sumd);
  itogo_mes[4]+=sumd;
  stroka_xml[4]=sumd;
 }
else
 fprintf(ff,"%9s|","");

if(metka_dok == 3)
 {
  fprintf(ff,"%9.2f|",sumd);
  itogo_mes[5]+=sumd;
  stroka_xml[5]=sumd;
 }
else
 fprintf(ff,"%9s|","");

if(iceb_u_sravmydat(dn,mn,gn,1,3,2013) >= 0) /*13 колонка*/
  fprintf(ff,"%10s|","");

fprintf(ff,"\n");
if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
 {
  if(iceb_u_strlen(naim_kontr) > 20 || iceb_u_strlen(nomdok) > 8)
   {
    fprintf(ff,"|%5s|%10s|%-*.*s|%2s|%5s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%9s|%9s|%10s|\n",
    "","",
    iceb_u_kolbait(8,iceb_u_adrsimv(8,nomdok)),iceb_u_kolbait(8,iceb_u_adrsimv(8,nomdok)),iceb_u_adrsimv(8,nomdok),
    "","",
    iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),iceb_u_adrsimv(20,naim_kontr),
    "","","","","","","","");
   }
  for(int nom=40; nom < iceb_u_strlen(naim_kontr); nom+=20)
   {
    fprintf(ff,"|%5s|%10s|%8s|%2s|%5s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%9s|%9s|%10s|\n",
    "","","","","",    
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),iceb_u_adrsimv(nom,naim_kontr),
    "","","","","","","","");
   }
 }
else
 {
  if(iceb_u_strlen(naim_kontr) > 20 || iceb_u_strlen(nomdok) > 8)
   {
    fprintf(ff,"|%5s|%10s|%-*.*s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%9s|%9s|\n",
    "","",
    iceb_u_kolbait(8,iceb_u_adrsimv(8,nomdok)),iceb_u_kolbait(8,iceb_u_adrsimv(8,nomdok)),iceb_u_adrsimv(8,nomdok),
    iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),iceb_u_adrsimv(20,naim_kontr),
    "","","","","","","");
   }
  for(int nom=40; nom < iceb_u_strlen(naim_kontr); nom+=20)
   {
    fprintf(ff,"|%5s|%10s|%8s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%9s|%9s|\n",
    "","","",
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),iceb_u_adrsimv(nom,naim_kontr),
    "","","","","","","");
   }
 }

}
