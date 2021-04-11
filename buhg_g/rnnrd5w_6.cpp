/*$Id: rnnrd5.c,v 5.40 2014/07/31 07:09:17 sasa Exp $*/
/*26.08.2015	19.01.2015	Белых А.И.	rnnrd5w_6.c
Распечатка додатка 5
*/
#include <ctype.h>
#include <errno.h>
#include "buhg_g.h"
#include "rnn_d5w.h"

void rnnrd5_6_sap2(FILE *ff);
void rnnrd5_6_sap(FILE *ff_svod);
void rnn_rk5_6_kon1(double suma_bez_nds2,double suma_nds2,double suma_nds_7,double os_sr21,double os_sr22,double inh1,double inh2,double inh3,FILE *ff_svod,FILE*);
void rnnrd5_6_endras(class rnn_d5 *dod5,double,int,FILE *ff_svod,FILE *ff_xml,GtkWidget *wpredok);

extern class iceb_rnfxml_data rek_zag_nn;

int rnnrd5w_6(short dn,short mn,short gn,short dk,short mk,short gk,
char *imaf_xml,char *imaf_svod_naim,
char *imaf_xml_dot,char *imaf_svod_dot, /*Дополнительные отчёты*/
double proc_dot, /*Процент дотации*/
class rnn_d5 *dod5,
GtkWidget *wpredok)
{
int period_type;
FILE *ff_xml=NULL;
FILE *ff_xml_dot=NULL;


if(iceb_openxml(dn,mn,gn,dk,mk,gk,imaf_xml,iceb_getkoddok(dn,mn,gn,4),1,&period_type,&rek_zag_nn,&ff_xml,wpredok) != 0)
 return(1);

iceb_rnn_sap_xml(mn,gn,mk,iceb_getkoddok(dn,mn,gn,4),1,period_type,&rek_zag_nn,ff_xml);

fprintf(ff_xml," <DECLARBODY>\n");
fprintf(ff_xml,"   <HZ>1</HZ>\n"); /*1-звітний */
fprintf(ff_xml,"   <HZY>%d</HZY>\n",gk);
fprintf(ff_xml,"   <HZM>%d</HZM>\n",mk);
fprintf(ff_xml,"   <HZYP xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr.ravno()));
//fprintf(ff_xml,"   <HLOC>%s</HLOC>\n",iceb_u_filtr_xml(rek_zag_nn.adres));
fprintf(ff_xml,"   <HTIN>%s</HTIN>\n",rek_zag_nn.kod_edrpuo.ravno());
fprintf(ff_xml,"   <HDDGVSD xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <HNDGVSD xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn.ravno());
fprintf(ff_xml,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds.ravno());

if(proc_dot != 0.)
 {

  if(iceb_openxml(dn,mn,gn,dk,mk,gk,imaf_xml_dot,iceb_getkoddok(dn,mn,gn,5),1,&period_type,&rek_zag_nn,&ff_xml_dot,wpredok) != 0)
   return(1);

  iceb_rnn_sap_xml(mn,gn,mk,iceb_getkoddok(dn,mn,gn,5),1,period_type,&rek_zag_nn,ff_xml_dot);

  fprintf(ff_xml_dot," <DECLARBODY>\n");
  fprintf(ff_xml_dot,"   <HZ>1</HZ>\n"); /*1-звітний */
  fprintf(ff_xml_dot,"   <HZY>%d</HZY>\n",gk);
  fprintf(ff_xml_dot,"   <HZM>%d</HZM>\n",mk);
  fprintf(ff_xml_dot,"   <HZYP xsi:nil=\"true\"/>\n");
  fprintf(ff_xml_dot,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr.ravno()));
  //fprintf(ff_xml_dot,"   <HLOC>%s</HLOC>\n",iceb_u_filtr_xml(rek_zag_nn.adres));
  fprintf(ff_xml_dot,"   <HTINJ>%s</HTINJ>\n",rek_zag_nn.kod_edrpuo.ravno());
  fprintf(ff_xml_dot,"   <HDDGVSD xsi:nil=\"true\"/>\n");
  fprintf(ff_xml_dot,"   <HNDGVSD xsi:nil=\"true\"/>\n");
  fprintf(ff_xml_dot,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn.ravno());
  fprintf(ff_xml_dot,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds.ravno());
 
 }


FILE *ff_svod_dot=NULL;


if(proc_dot != 0.)
 {
  sprintf(imaf_svod_dot,"rnnsd%d.lst",getpid());

  if((ff_svod_dot=fopen(imaf_svod_dot,"w")) == NULL)
   {
    iceb_er_op_fil(imaf_svod_dot,"",errno,wpredok);
    return(1);
   }
 }

FILE *ff_svod_naim;

sprintf(imaf_svod_naim,"rnnsn%d.lst",getpid());

if((ff_svod_naim=fopen(imaf_svod_naim,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_svod_naim,"",errno,wpredok);
  return(1);
 }

iceb_zagolov("Додаток 5",dn,mn,gn,dk,mk,gk,ff_svod_naim,wpredok);



char kvrt[8];
char mes[8];
memset(kvrt,'\0',sizeof(kvrt));
memset(mes,'\0',sizeof(mes));
if(mn == mk && gn == gk)
  sprintf(mes,"%d",mk);
else
 {
  int kvt=0;
  if(mk <= 3)
   kvt=1;
  if( mk <= 6)
   kvt=2;
  if( mk <= 9)
   kvt=3;
  if( mk > 9)
   kvt=4;
   
  sprintf(kvrt,"%d",kvt);
 }
if(ff_svod_dot != NULL)
  iceb_zagolov("Додаток 2",dn,mn,gn,dk,mk,gk,ff_svod_dot,wpredok);

if(ff_svod_dot != NULL)
  rnnrd5_6_sap(ff_svod_dot);

double inh1=0.,inh2=0.,inh3=0.;

rnnrd5_6_sap(ff_svod_naim);

int nomer_str=0;
double suma_bez_nds=0.;
double suma_nds=0.;
double suma_nds_7=0.;
double i_suma_bez_nds=0.;
double i_suma_nds=0.;
double i_suma_nds_7=0.;
class iceb_u_str period("");
short m=0,g=0;
class iceb_u_str in2("");

for(int kkk=0; kkk < dod5->innn2.kolih(); kkk++)
 {
  for(int ii=0; ii < dod5->innn_per2.kolih(); ii++)
   {
    iceb_u_polen(dod5->innn_per2.ravno(ii),&in2,1,'|');  
    if(iceb_u_SRAV(dod5->innn2.ravno(kkk),in2.ravno(),0) != 0)
     continue;

    if(in2.ravno_atoi() == 0 
    || iceb_u_SRAV(in2.ravno(),"100000000000",0) == 0 
    || iceb_u_SRAV(in2.ravno(),"200000000000",0) == 0 
    || iceb_u_SRAV(in2.ravno(),"300000000000",0) == 0 
    || iceb_u_SRAV(in2.ravno(),"400000000000",0) == 0 )  /*Значит нет индивидуального налогового номера*/
     {
      inh1+=dod5->suma_bez_nds2.ravno(ii);
      inh2+=dod5->suma_nds2.ravno(ii);
      inh3+=dod5->suma_nds2_7.ravno(ii);
      continue;
     }

    if(dod5->suma_bez_nds2.ravno(ii) == 0. && dod5->suma_nds2.ravno(ii) == 0. && dod5->suma_nds2_7.ravno(ii) == 0.)
     continue;
    
    suma_bez_nds=dod5->suma_bez_nds2.ravno(ii);
    suma_nds=dod5->suma_nds2.ravno(ii);
    suma_nds_7=dod5->suma_nds2_7.ravno(ii);
    if(proc_dot != 0.)
     {
      suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
      suma_bez_nds=iceb_u_okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=iceb_u_okrug(suma_nds,0.01);

      suma_nds_7=suma_nds_7*(100.0-proc_dot)/100.;
      suma_nds_7=iceb_u_okrug(suma_nds_7,0.01);
      
     }
    
    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;
    i_suma_nds_7+=suma_nds_7;

    iceb_u_polen(dod5->innn_per2.ravno(ii),&period,2,'|');  
    iceb_u_rsdat1(&m,&g,period.ravno());
    
    nomer_str++;
    
    if(ff_svod_dot != NULL)
     {
      fprintf(ff_svod_dot,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|%10.2f|\n",
      nomer_str,
      iceb_u_kolbait(20,in2.ravno()),in2.ravno(),
      m,g,
      dod5->suma_bez_nds2.ravno(ii)-suma_bez_nds,
      dod5->suma_nds2.ravno(ii)-suma_nds,
      dod5->suma_nds2.ravno(ii)-suma_nds_7);
     }

    fprintf(ff_svod_naim,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|%10.2f|%s\n",
    nomer_str,
    iceb_u_kolbait(20,in2.ravno()),in2.ravno(),
    m,g,dod5->suma_bez_nds2.ravno(ii),dod5->suma_nds2.ravno(ii),dod5->suma_nds2_7.ravno(ii),dod5->naim_kontr2.ravno(ii));

    fprintf(ff_xml,"   <T1RXXXXG2 ROWNUM=\"%d\">%s</T1RXXXXG2>\n",nomer_str,in2.ravno());
    fprintf(ff_xml,"   <T1RXXXXG3A ROWNUM=\"%d\">%d</T1RXXXXG3A>\n",nomer_str,m);
    fprintf(ff_xml,"   <T1RXXXXG3B ROWNUM=\"%d\">%d</T1RXXXXG3B>\n",nomer_str,g);
    fprintf(ff_xml,"   <T1RXXXXG4 ROWNUM=\"%d\">%.2f</T1RXXXXG4>\n",nomer_str,suma_bez_nds);
    fprintf(ff_xml,"   <T1RXXXXG5 ROWNUM=\"%d\">%.2f</T1RXXXXG5>\n",nomer_str,suma_nds);
    fprintf(ff_xml,"   <T1RXXXXG6 ROWNUM=\"%d\">%.2f</T1RXXXXG6>\n",nomer_str,suma_nds_7);
    
    if(ff_xml_dot != NULL)
     {
      fprintf(ff_xml_dot,"   <T1RXXXXG2 ROWNUM=\"%d\">%s</T1RXXXXG2>\n",nomer_str,in2.ravno());
      fprintf(ff_xml_dot,"   <T1RXXXXG3A ROWNUM=\"%d\">%d</T1RXXXXG3A>\n",nomer_str,m);
      fprintf(ff_xml_dot,"   <T1RXXXXG3B ROWNUM=\"%d\">%d</T1RXXXXG3B>\n",nomer_str,g);
      fprintf(ff_xml_dot,"   <T1RXXXXG4 ROWNUM=\"%d\">%.2f</T1RXXXXG4>\n",nomer_str,dod5->suma_bez_nds2.ravno(ii)-suma_bez_nds);
      fprintf(ff_xml_dot,"   <T1RXXXXG5 ROWNUM=\"%d\">%.2f</T1RXXXXG5>\n",nomer_str,dod5->suma_nds2.ravno(ii)-suma_nds);
      fprintf(ff_xml_dot,"   <T1RXXXXG6 ROWNUM=\"%d\">%.2f</T1RXXXXG6>\n",nomer_str,dod5->suma_nds2_7.ravno(ii)-suma_nds_7);
     } 
   }
 }


double os1=dod5->os_sr2[0];
double os2=dod5->os_sr2[1];
double inhi1=inh1;
double inhi2=inh2;
double inhi3=inh3;
if(proc_dot != 0)
 {
  os1=os1*(100.0-proc_dot)/100.;
  os2=os2*(100.0-proc_dot)/100.;
  os1=iceb_u_okrug(os1,0.01);  
  os2=iceb_u_okrug(os2,0.01);  

  inhi1=inhi1*(100.0-proc_dot)/100.;
  inhi1=iceb_u_okrug(inhi1,0.01);
  inhi2=inhi2*(100.0-proc_dot)/100.;
  inhi2=iceb_u_okrug(inhi2,0.01);
  inhi3=inhi3*(100.0-proc_dot)/100.;
  inhi3=iceb_u_okrug(inhi3,0.01);
 }

rnn_rk5_6_kon1(i_suma_bez_nds+inh1,i_suma_nds+inh2,i_suma_nds_7,os1,os2,inhi1,inhi2,inhi3,ff_svod_naim,ff_xml);

if(ff_svod_dot != NULL)
  rnn_rk5_6_kon1(dod5->suma_bez_nds2.suma()-i_suma_bez_nds+inh1-inhi1,
  dod5->suma_nds2.suma()-i_suma_nds+inh2-inhi2,\
  dod5->suma_nds2_7.suma()-i_suma_nds_7+inh3-inhi3,\
  dod5->os_sr2[0]-os1,dod5->os_sr2[1]-os2,inh1-inhi1,inh2-inhi2,inh3-inhi3,ff_svod_dot,ff_xml_dot);



/****************************** податковый кредит***********************************/

rnnrd5_6_sap2(ff_svod_naim);

if(ff_svod_dot != NULL)
 rnnrd5_6_sap2(ff_svod_dot);
                  

class iceb_u_str in1("");
nomer_str=0;
i_suma_bez_nds=i_suma_nds=0.;

for(int kkk=0; kkk < dod5->innn1.kolih(); kkk++)
 {
  for(int ii=0; ii < dod5->innn_per1.kolih(); ii++)
   {
    iceb_u_polen(dod5->innn_per1.ravno(ii),&in1,1,'|');  
    if(iceb_u_SRAV(dod5->innn1.ravno(kkk),in1.ravno(),0) != 0)
     continue;
    iceb_u_polen(dod5->innn_per1.ravno(ii),&period,2,'|');  

    if(dod5->suma_bez_nds1.ravno(ii) == 0. && dod5->suma_nds1.ravno(ii) == 0. && dod5->suma_nds1_7.ravno(ii) == 0.)   
     continue;

    iceb_u_rsdat1(&m,&g,period.ravno());
    
    suma_bez_nds=dod5->suma_bez_nds1.ravno(ii);
    suma_nds=dod5->suma_nds1.ravno(ii);
    suma_nds_7=dod5->suma_nds1_7.ravno(ii);
   
    if(proc_dot != 0.)
     {
      suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
      suma_bez_nds=iceb_u_okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=iceb_u_okrug(suma_nds,0.01);

      suma_nds_7=suma_nds_7*(100.0-proc_dot)/100.;
      suma_nds_7=iceb_u_okrug(suma_nds_7,0.01);
      
     }
    
    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;
    i_suma_nds_7+=suma_nds_7;
    nomer_str++;    
    if(ff_svod_dot != NULL)
      fprintf(ff_svod_dot,"%5d|%-*s|%-17s|%10.2f|%10.2f|%10.2f|\n",
      nomer_str,
      iceb_u_kolbait(20,in1.ravno()),in1.ravno(),
      period.ravno(),dod5->suma_bez_nds1.ravno(ii)-suma_bez_nds,dod5->suma_nds1.ravno(ii)-suma_nds,dod5->suma_nds1_7.ravno(ii)-suma_nds_7);

    fprintf(ff_svod_naim,"%5d|%-*s|%-17s|%10.2f|%10.2f|%10.2f|%s\n",
    nomer_str,
    iceb_u_kolbait(20,in1.ravno()),in1.ravno(),
    period.ravno(),dod5->suma_bez_nds1.ravno(ii),
    dod5->suma_nds1.ravno(ii),
    dod5->suma_nds1_7.ravno(ii),
    dod5->naim_kontr1.ravno(ii));
    

    fprintf(ff_xml,"   <T2RXXXXG2 ROWNUM=\"%d\">%s</T2RXXXXG2>\n",nomer_str,in1.ravno());
    fprintf(ff_xml,"   <T2RXXXXG3A ROWNUM=\"%d\">%d</T2RXXXXG3A>\n",nomer_str,m);
    fprintf(ff_xml,"   <T2RXXXXG3B ROWNUM=\"%d\">%d</T2RXXXXG3B>\n",nomer_str,g);
    fprintf(ff_xml,"   <T2RXXXXG4 ROWNUM=\"%d\">%.2f</T2RXXXXG4>\n",nomer_str,suma_bez_nds);
    fprintf(ff_xml,"   <T2RXXXXG5 ROWNUM=\"%d\">%.2f</T2RXXXXG5>\n",nomer_str,suma_nds);
    fprintf(ff_xml,"   <T2RXXXXG6 ROWNUM=\"%d\">%.2f</T2RXXXXG6>\n",nomer_str,suma_nds_7);
    
    if(ff_xml_dot != NULL)
     {
      fprintf(ff_xml_dot,"   <T2RXXXXG2 ROWNUM=\"%d\">%s</T2RXXXXG2>\n",nomer_str,in1.ravno());
      fprintf(ff_xml_dot,"   <T2RXXXXG3A ROWNUM=\"%d\">%d</T2RXXXXG3A>\n",nomer_str,m);
      fprintf(ff_xml_dot,"   <T2RXXXXG3B ROWNUM=\"%d\">%d</T2RXXXXG3B>\n",nomer_str,g);
      fprintf(ff_xml_dot,"   <T2RXXXXG4 ROWNUM=\"%d\">%.2f</T2RXXXXG4>\n",nomer_str,dod5->suma_bez_nds1.ravno(ii)-suma_bez_nds);
      fprintf(ff_xml_dot,"   <T2RXXXXG5 ROWNUM=\"%d\">%.2f</T2RXXXXG5>\n",nomer_str,dod5->suma_nds1.ravno(ii)-suma_nds);
      fprintf(ff_xml_dot,"   <T2RXXXXG6 ROWNUM=\"%d\">%.2f</T2RXXXXG6>\n",nomer_str,dod5->suma_nds1_7.ravno(ii)-suma_nds_7);
     }    
   }
 }


fprintf(ff_svod_naim,"\
------------------------------------------------------------------------------\n");
fprintf(ff_svod_naim,"\
Разом                                       |%10.2f|%10.2f|%10.2f|\n",
dod5->suma_bez_nds1.suma(),dod5->suma_nds1.suma(),dod5->suma_nds1_7.suma());

rnnrd5_6_endras(dod5,proc_dot,0,ff_svod_naim,ff_xml,wpredok);

if(ff_svod_dot != NULL)
  rnnrd5_6_endras(dod5,proc_dot,1,ff_svod_dot,ff_xml_dot,wpredok);


return(0);

}
/**********************************/
/*шапка додатка 5 и 2*/
/**********************************/

void rnnrd5_6_sap(FILE *ff_svod)
{



fprintf(ff_svod,"Розділ І. Податкові зобов'язання\n");
fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");

fprintf(ff_svod,"\
  N  |Платник податку -   |Період складання |  Обсяги  |      Сумма ПДВ      |\n\
     |покупець (індивіду- |  податкових     | поставки |---------------------|\n\
     |альний податковий   |  накладних      |(без ПДВ) |Остновна  |ставка 7  |\n\
     | номер)             |                 |          | ставка   |          |\n");
fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");


}
/***************************************/
/*Первый итог раздела 1 */
/***************************************/
void rnn_rk5_6_kon1(double suma_bez_nds2,
double suma_nds2,
double suma_nds2_7,
double os_sr21,
double os_sr22,
double inh1,double inh2,double inh3,FILE *ff_svod,FILE *ff_xml)
{

if(inh1 == 0. && inh2 == 0.)
 {
  fprintf(ff_xml,"   <R001G4 xsi:nil=\"true\"/>\n");
  fprintf(ff_xml,"   <R001G5 xsi:nil=\"true\"/>\n");
  fprintf(ff_xml,"   <R001G6 xsi:nil=\"true\"/>\n");
 }
else
 {
  fprintf(ff_xml,"   <R001G4>%.2f</R001G4>\n",inh1);
  fprintf(ff_xml,"   <R001G5>%.2f</R001G5>\n",inh2);
  fprintf(ff_xml,"   <R001G6>%.2f</R001G6>\n",inh3);
 } 

fprintf(ff_xml,"   <R010G4>%.2f</R010G4>\n",suma_bez_nds2);
fprintf(ff_xml,"   <R010G5>%.2f</R010G5>\n",suma_nds2);
fprintf(ff_xml,"   <R010G5>%.2f</R010G5>\n",suma_nds2_7);

if(os_sr21 == 0.)
 fprintf(ff_xml,"   <R011G4 xsi:nil=\"true\"/>\n");
else
 fprintf(ff_xml,"   <R011G4>%.2f</R011G4>\n",os_sr21);

if(os_sr22 == 0.)
 fprintf(ff_xml,"   <R011G5 xsi:nil=\"true\"/>\n");
else
 fprintf(ff_xml,"   <R011G5>%.2f</R011G5>\n",os_sr22);


fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");
if(inh1 == 0. && inh2 == 0.)
  fprintf(ff_svod,"\
Інші                                   |               |          |          |\n");
else
  fprintf(ff_svod,"\
Інші                                   |%15.2f|%10.2f|%10.2f\n",inh1,inh2,inh3);

fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");

fprintf(ff_svod,"\
Усього за звітний період  (рядок 1 дек-|               |          |          |\n\
ларації),в тому числі:                 |%15.2f|%10.2f|%10.2f|\n",
suma_bez_nds2,suma_nds2,suma_nds2_7);


fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");

fprintf(ff_svod,"\
постачання необоротних активів         |%15.2f|%10.2f|          |\n",os_sr21,os_sr22);

fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");


}
/******************/
/*концовка*/
/******************/
void rnnrd5_6_endras(class rnn_d5 *dod5,
double proc_dot,
int metka_v,    /*0-основной 1-дополнительный*/
FILE *ff_svod,FILE *ff_xml,GtkWidget *wpredok)
{
double suma_bez_nds=dod5->suma_bez_nds1.suma();
double suma_nds=dod5->suma_nds1.suma();
double suma_nds_7=dod5->suma_nds1_7.suma();

double os1=dod5->os_sr1[0];
double os2=dod5->os_sr1[1];

if(proc_dot != 0.)
 {
  suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
  suma_bez_nds=iceb_u_okrug(suma_bez_nds,0.01);

  suma_nds=suma_nds*(100.0-proc_dot)/100.;
  suma_nds=iceb_u_okrug(suma_nds,0.01);

  suma_nds_7=suma_nds_7*(100.0-proc_dot)/100.;
  suma_nds_7=iceb_u_okrug(suma_nds_7,0.01);
  
  os1=os1*(100.0-proc_dot)/100.;
  os2=os2*(100.0-proc_dot)/100.;
  os1=iceb_u_okrug(os1,0.01);  
  os2=iceb_u_okrug(os2,0.01);  

  if(metka_v == 1)
   {
    suma_bez_nds=dod5->suma_bez_nds1.suma()-suma_bez_nds;
    suma_nds=dod5->suma_nds1.suma()-suma_nds;
    suma_nds_7=dod5->suma_nds1_7.suma()-suma_nds_7;
    os1=dod5->os_sr1[0]-os1;
    os2=dod5->os_sr1[1]-os2;
   } 
  
 }


fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");
fprintf(ff_svod,"\
Усього за місяць (квартал)(рядок 10.1 декла-|          |          |          |\n\
рації), в тому числі:                       |%10.2f|%10.2f|%10.2f|\n",
suma_bez_nds,suma_nds,suma_nds_7);


fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");
fprintf(ff_svod,"\
придбання (будівництво, спорудження)        |%10.2f|%10.2f|          |\n\
основних фондів                             |          |          |          |\n",os1,os2);

fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");
fprintf(ff_svod,"\
проведення розрахунків комісіонером/повірен-|          |          |          |\n\
ним за товари (послуги) на умовах передба-  |          |          |          |\n\
чених п.4.7 ст. 4 Закону                    |          |          |          |\n");

fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");





fprintf(ff_xml,"   <R020G4>%.2f</R020G4>\n",suma_bez_nds);
fprintf(ff_xml,"   <R020G5>%.2f</R020G5>\n",suma_nds);
fprintf(ff_xml,"   <R020G6>%.2f</R020G6>\n",suma_nds_7);

if(os1 == 0.)
  fprintf(ff_xml,"   <R021G4 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R021G4>%.2f</R021G4>\n",os1);

if(os2 == 0.)
  fprintf(ff_xml,"   <R021G5 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R021G5>%.2f</R021G5>\n",os2);

double i_suma_bez_nds=0.;
double i_suma_nds=0.;
double i_suma_nds_7=0.;
class iceb_u_str period("");
short m=0,g=0;
class iceb_u_str in1("");

fprintf(ff_svod,"\
Операції з придбання з ПДВ, які не надають права формування\n\
            податкового кредиту\n");


fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");


for(int kkk=0; kkk < dod5->innn1d.kolih(); kkk++)
 for(int nomer=0; nomer < dod5->innn_per1d.kolih(); nomer++)
  {
   iceb_u_polen(dod5->innn_per1d.ravno(nomer),&in1,1,'|');  
   if(iceb_u_SRAV(dod5->innn1d.ravno(kkk),in1.ravno(),0) != 0)
     continue;

   iceb_u_polen(dod5->innn_per1d.ravno(nomer),&period,2,'|');  
   iceb_u_rsdat1(&m,&g,period.ravno());

   suma_bez_nds=dod5->suma_bez_nds1d.ravno(nomer);
   suma_nds=dod5->suma_nds1d.ravno(nomer);
   suma_nds_7=dod5->suma_nds1d_7.ravno(nomer);

   if(proc_dot != 0.)
    {
     suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
     suma_bez_nds=iceb_u_okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=iceb_u_okrug(suma_nds,0.01);

      suma_nds_7=suma_nds_7*(100.0-proc_dot)/100.;
      suma_nds_7=iceb_u_okrug(suma_nds_7,0.01);
      if(metka_v == 1)
       {
        suma_bez_nds=dod5->suma_bez_nds1.suma()-suma_bez_nds;
        suma_nds=dod5->suma_nds1.suma()-suma_nds;
        suma_nds_7=dod5->suma_nds1_7.suma()-suma_nds_7;
       }
     }

    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;
    i_suma_nds_7+=suma_nds_7;

    fprintf(ff_svod,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|%10.2f|%s\n",
    nomer+1,
    iceb_u_kolbait(20,dod5->innn1d.ravno(kkk)),dod5->innn1d.ravno(kkk),
    m,g,suma_bez_nds,suma_nds,suma_nds_7,
    dod5->naim_kontr1d.ravno(kkk));

    if(dod5->innn1d.ravno(kkk)[0] == '\0' ||  isdigit(dod5->innn1d.ravno(kkk)[0]) == 0)  //Значит символ не число
     fprintf(ff_xml,"   <T3RXXXXG2 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer+1);
    else  
      fprintf(ff_xml,"   <T3RXXXXG2 ROWNUM=\"%d\">%s</T3RXXXXG2>\n",nomer+1,dod5->innn1d.ravno(kkk));

    fprintf(ff_xml,"   <T3RXXXXG3A ROWNUM=\"%d\">%d</T3RXXXXG3A>\n",nomer+1,m);
    fprintf(ff_xml,"   <T3RXXXXG3B ROWNUM=\"%d\">%d</T3RXXXXG3B>\n",nomer+1,g);
    fprintf(ff_xml,"   <T3RXXXXG4 ROWNUM=\"%d\">%.2f</T3RXXXXG4>\n",nomer+1,suma_bez_nds);
    fprintf(ff_xml,"   <T3RXXXXG5  ROWNUM=\"%d\">%.2f</T3RXXXXG5>\n",nomer+1,suma_nds);
    fprintf(ff_xml,"   <T3RXXXXG6  ROWNUM=\"%d\">%.2f</T3RXXXXG6>\n",nomer+1,suma_nds_7);

  }

if(dod5->innn1d.kolih() == 0)
 {
  fprintf(ff_svod,"\
     |                    |                 |          |          |          |\n");
 }
fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");

fprintf(ff_svod,"\
Усього за звітний період                    |%10.2f|%10.2f|%10.2f|\n",i_suma_bez_nds,i_suma_nds,suma_nds_7);
fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");




if(i_suma_bez_nds == 0.)
  fprintf(ff_xml,"   <R03G4 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R03G4>%.2f</R03G4>\n",i_suma_bez_nds);

if(i_suma_nds == 0.)
  fprintf(ff_xml,"   <R03G5 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R03G5>%.2f</R03G5>\n",i_suma_nds);

fprintf(ff_xml,"   <R03G6>%.2f</R03G6>\n",i_suma_nds_7);


fprintf(ff_svod,"Операції з придбання без податку на додану вартість\n");


fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");
for(int kkk=0; kkk < dod5->innn1d1.kolih(); kkk++)
 for(int nomer=0; nomer < dod5->innn_per1d1.kolih(); nomer++)
  {
   iceb_u_polen(dod5->innn_per1d1.ravno(nomer),&in1,1,'|');  
   if(iceb_u_SRAV(dod5->innn1d1.ravno(kkk),in1.ravno(),0) != 0)
     continue;

   iceb_u_polen(dod5->innn_per1d1.ravno(nomer),&period,2,'|');  
   iceb_u_rsdat1(&m,&g,period.ravno());

   suma_bez_nds=dod5->suma_bez_nds1d1.ravno(nomer);
   suma_nds=dod5->suma_nds1d1.ravno(nomer);

   if(proc_dot != 0.)
    {
     suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
     suma_bez_nds=iceb_u_okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=iceb_u_okrug(suma_nds,0.01);
      if(metka_v == 1)
       {
        suma_bez_nds=dod5->suma_bez_nds1.suma()-suma_bez_nds;
        suma_nds=dod5->suma_nds1.suma()-suma_nds;
       }
     }

    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;

    fprintf(ff_svod,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|%10s|%s\n",
    nomer+1,
    iceb_u_kolbait(20,dod5->innn1d1.ravno(kkk)),dod5->innn1d1.ravno(kkk),
    m,g,suma_bez_nds,suma_nds,
    "",
    dod5->naim_kontr1d1.ravno(kkk));

    if(dod5->innn1d1.ravno(kkk)[0] == '\0' ||  isdigit(dod5->innn1d1.ravno(kkk)[0]) == 0)  //Значит символ не число
     fprintf(ff_xml,"   <T4RXXXXG2 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer+1);
    else  
      fprintf(ff_xml,"   <T4RXXXXG2 ROWNUM=\"%d\">%s</T4RXXXXG2>\n",nomer+1,dod5->innn1d1.ravno(kkk));

    fprintf(ff_xml,"   <T4RXXXXG3A ROWNUM=\"%d\">%d</T4RXXXXG3A>\n",nomer+1,m);
    fprintf(ff_xml,"   <T4RXXXXG3B ROWNUM=\"%d\">%d</T4RXXXXG3B>\n",nomer+1,g);
    fprintf(ff_xml,"   <T4RXXXXG4 ROWNUM=\"%d\">%.2f</T4RXXXXG4>\n",nomer+1,suma_bez_nds);
    fprintf(ff_xml,"   <T4RXXXXG5 ROWNUM=\"%d\">%.2f</T4RXXXXG5>\n",nomer+1,suma_nds);

  }

if(dod5->innn1d1.kolih() == 0)
 {
  fprintf(ff_svod,"\
     |                    |                 |          |          |          |\n");
 }
fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");

fprintf(ff_svod,"\
Усього за звітний період                    |%10.2f|%10.2f|%10s|\n",i_suma_bez_nds,i_suma_nds,"");
fprintf(ff_svod,"\
------------------------------------------------------------------------------\n");




if(i_suma_bez_nds == 0.)
  fprintf(ff_xml,"   <R06G4 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R06G4>%.2f</R06G4>\n",i_suma_bez_nds);



class iceb_fioruk_rk rekruk;
class iceb_fioruk_rk rekbuh;

iceb_fioruk(1,&rekruk,wpredok);
iceb_fioruk(2,&rekbuh,wpredok);



fprintf(ff_xml,"   <HFILL>%02d%02d%04d</HFILL>\n",rek_zag_nn.dt,rek_zag_nn.mt,rek_zag_nn.gt);



fprintf(ff_xml,"   <HKBOS>%s</HKBOS>\n",rekruk.inn.ravno_filtr_xml());
fprintf(ff_xml,"   <HBOS>%s</HBOS>\n",rekruk.fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBUH>%s</HKBUH>\n",rekbuh.inn.ravno_filtr_xml());
fprintf(ff_xml,"   <HBUH>%s</HBUH>\n",rekbuh.fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HFO xsi:nil=\"true\"/>\n");

fprintf(ff_xml," </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");

iceb_podpis(ff_svod,wpredok);
fclose(ff_svod);

fclose(ff_xml);

}
/***********************/
/*шапка второго раздела*/
/***********************/
void rnnrd5_6_sap2(FILE *ff)
{
fprintf(ff,"\nРозділ ІІ. Податковий кредит\n");
fprintf(ff,"\
------------------------------------------------------------------------------\n");

fprintf(ff,"\
  N  |Платник податку -   |Період виписки   |  Обсяги  |      Сумма ПДВ      |\n\
     |покупець (індивіду- |податкових накла-| поставки |---------------------|\n\
     |альний податковий   |дних, інших доку-|(без ПДВ) |Основна   |ставка 7  |\n\
     | номер)             |ментів, що дають |          |ставка    |          |\n\
     |                    |право на податко-|          |          |          |\n\
     |                    |вий кредит       |          |          |          |\n");

/********
12345 12345678901234567890 12345678901234567 1234567890 1234567890
***********/
fprintf(ff,"\
------------------------------------------------------------------------------\n");
fprintf(ff,"\
Операції з придбання з ПДВ, які надають право формування податкового\n\
                  кредиту\n");
fprintf(ff,"\
------------------------------------------------------------------------------\n");
}