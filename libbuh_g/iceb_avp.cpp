/*$Id: iceb_avp.c,v 1.18 2013/11/05 10:51:27 sasa Exp $*/
/*29.03.2017	14.04.2009	Белых А.И.	iceb_avp.c
Унивесальная подпрограмма выполнения проводок
*/
#include "iceb_libbuh.h"

void iceb_avp_vp(class iceb_avp *avp,const char *snast,class iceb_u_spisok *sp_prov,class iceb_u_double *sum_prov_dk,FILE *ff_prot,GtkWidget*);

extern SQL_baza bd;

void iceb_avp(class iceb_avp *avp,const char *imaf_nast,
class iceb_u_spisok *sp_prov,
class iceb_u_double *sum_prov_dk,
FILE *ff_prot,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_u_str kodstr("");
class iceb_u_str kodop("");
class iceb_u_str shetuh("");
class iceb_u_str shetsp("");
if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\n%s-\n\
%s=%.2f\n\
%s=%.2f\n\
%s=%.2f\n\
%s=%.2f\n\
%s=%.2f\n\
%s=%.2f\n\
%s=%d\n\
%s=%s\n\
%s=%s\n",
  __FUNCTION__,
  gettext("Cумма с НДС"),
  avp->suma_s_nds,
  gettext("Cумма без НДС"),
  avp->suma_bez_nds,
  gettext("Cумма НДС"),
  avp->suma_nds,
  gettext("Сумма учёта"),
  avp->suma_uheta,
  gettext("Сумма НДС введённая вручную"),
  avp->suma_nds_vv,
  gettext("Cумма услуг без НДС"),
  avp->suma_uslug_bez_nds,
  gettext("Метка НДС"),
  avp->metka_snds,
  gettext("Счёт учёта"),
  avp->shetuh.ravno(),
  gettext("Счёт списания"),
  avp->shetsp.ravno());
 }

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nast);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-%s %s\n",__FUNCTION__,gettext("Не найдены настройки"),imaf_nast);
  return;
 }

int metka_vp=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(strpbrk(row_alx[0],"|") == NULL) /*есть ли знак "-" в строке*/
   continue;

  if(metka_vp == 0)
   {
    if(iceb_u_polen(row_alx[0],&kodstr,1,'|') != 0)
     continue;

    if(iceb_u_SRAV("START",kodstr.ravno(),0) != 0)
      continue;

    iceb_u_polen(row_alx[0],&kodop,2,'|');
    iceb_u_polen(row_alx[0],&shetuh,3,'|');
    iceb_u_polen(row_alx[0],&shetsp,4,'|');

    if(iceb_u_proverka(kodop.ravno(),avp->kodop.ravno(),0,0) != 0)
     continue;

    if(iceb_u_proverka(shetuh.ravno(),avp->shetuh.ravno(),1,0) != 0)
     continue;

    if(avp->shetsp.getdlinna() > 1 && shetsp.getdlinna() > 1)
     if(iceb_u_proverka(shetsp.ravno(),avp->shetsp.ravno(),1,0) != 0)
      {
       if(ff_prot != NULL)
        fprintf(ff_prot,"%s-%s %s != %s\n",__FUNCTION__,gettext("Не подходит по счёту списания"),avp->shetsp.ravno(),shetsp.ravno());
       continue;
      }
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-%s",__FUNCTION__,row_alx[0]);
    metka_vp=1;
    continue;
   }
  
  if(iceb_u_SRAV("END",row_alx[0],1) == 0)
     break;
  if(iceb_u_SRAV("START",row_alx[0],1) == 0)
     break;

  if(ff_prot != NULL)
     fprintf(ff_prot,"%s-%s",__FUNCTION__,row_alx[0]);

  iceb_avp_vp(avp,row_alx[0],sp_prov,sum_prov_dk,ff_prot,wpredok);

 }
unlink(imaf_nast);

}

/*********************/
/*Проверка счёта и кода контрагента*/
/*****************/
int iceb_avp_provsh(const char *shet,const char *kontr_v_dok,class iceb_u_str *shetv,class iceb_u_str *kontrv,FILE *ff_prot,
short metka_avk,
GtkWidget *wpredok)
{
class iceb_u_str kontrp("");
class iceb_u_str shetp("");


if(ff_prot != NULL)
 fprintf(ff_prot,"%s-%s=%s\n",__FUNCTION__,gettext("Проверка счёта и кода контрагента"),shet);

kontrv->new_plus("");
shetv->new_plus("");

if(iceb_u_polen(shet,&shetp,1,':') != 0)
 shetp.new_plus(shet);

if(iceb_u_polen(shet,&kontrp,2,':') == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-%s=%s\n",__FUNCTION__,gettext("Задан код контрагента"),kontrp.ravno());
 }

class OPSHET shet_op;

if(kontrp.ravno()[0] == '\0')
 kontrp.new_plus(kontr_v_dok);
if(ff_prot != NULL)
 fprintf(ff_prot,"%s-%s %s=%s %s=%s\n",__FUNCTION__,gettext("Проверяем"),gettext("Счёт"),shetp.ravno(),gettext("Контрагент"),kontrp.ravno());

if(iceb_provsh(&kontrp,shetp.ravno(),&shet_op,metka_avk,0,wpredok) != 0)
 {
  return(1);
 }

kontrv->new_plus(kontrp.ravno());
shetv->new_plus(shetp.ravno());
             
if(ff_prot != NULL)
 fprintf(ff_prot,"%s-%s=%s %s=%s\n",__FUNCTION__,gettext("Счёт"),shetv->ravno(),gettext("Контрагент"),kontrv->ravno());

return(0);
}
/******************************/
/*Разборка списка счетов*/
/**************************/

int iceb_avp_rsh(const char *shet,const char *kontr_v_dok,class iceb_u_str *shetv,class iceb_u_str *kontr,
class iceb_avp *avp,
FILE *ff_prot,
GtkWidget *wpredok)
{
kontr->new_plus("");
shetv->new_plus("");

int kolp=iceb_u_pole2(shet,',');

if(kolp == 0)
 {
  return(iceb_avp_provsh(shet,kontr_v_dok,shetv,kontr,ff_prot,avp->metka_avk,wpredok));
  
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"%s-%s=%s\n",__FUNCTION__,gettext("Составляем список счетов"),shet);
 
class iceb_u_str bros("");
class iceb_u_spisok spsh;
class iceb_u_spisok naimsh;
for(int ii=0; ii < kolp; ii++)
 {
  if(iceb_u_polen(shet,&bros,ii+1,',') != 0)
   break;
  if(bros.ravno()[0] == '\0')
   break;
  spsh.plus(bros.ravno());
  naimsh.plus("");
 }
class iceb_u_str shet_menu("");
class iceb_u_str kontr_menu("");
int nomer=0;

if((nomer=avp->vib_shets.find(shet)) >= 0)
 {

  if(iceb_u_polen(avp->vib_shet.ravno(nomer),&shet_menu,1,':') != 0)
   shet_menu.new_plus(avp->vib_shet.ravno(nomer));
  else
   iceb_u_polen(avp->vib_shet.ravno(nomer),&kontr_menu,2,':');

 }
else
 {

  char zagolov[512];
  class iceb_u_spisok naim_kol;
  class iceb_u_str naim_menu;

  sprintf(zagolov,"%s:%s %s",
  gettext("Счёт учёта"),
  avp->shetuh.ravno(),
  gettext("Выберите нужный счёт"));

  naim_kol.plus(gettext("Счёт"));
  naim_kol.plus(gettext("Наименование"));
  naim_menu.plus(gettext("Виберите нужный счёт"));

  int voz=0;
  if((voz=iceb_l_spisokm(&spsh,&naimsh,&naim_kol,&naim_menu,0,wpredok)) < 0)
   return(-1);

  avp->vib_shets.plus(shet);
  avp->vib_shet.plus(spsh.ravno(voz));


  if(iceb_u_polen(spsh.ravno(voz),&shet_menu,1,':') != 0)
   shet_menu.new_plus(spsh.ravno(voz));
  else
   iceb_u_polen(spsh.ravno(voz),&kontr_menu,2,':');
 }
 
if(kontr_menu.getdlinna() <= 1)
 kontr_menu.new_plus(kontr_v_dok);

return(iceb_avp_provsh(shet_menu.ravno(),kontr_menu.ravno(),shetv,kontr,ff_prot,avp->metka_avk,wpredok));

}
/****************************/
/*               */
/***************************/
void iceb_avp_vp(class iceb_avp *avp,const char *snast,
class iceb_u_spisok *sp_prov,
class iceb_u_double *sum_prov_dk,
FILE *ff_prot,
GtkWidget *wpredok)
{
class iceb_u_str shet("");
class iceb_u_str shetkor("");
class iceb_u_str sumaprov("");
class iceb_u_str koef_sumaprov("");
class iceb_u_str kekv("");
class iceb_u_str koment("");
class iceb_u_str kontr_shet("");
class iceb_u_str kontr_shetkor("");
class iceb_u_str nast_shet("");
class iceb_u_str nast_shetkor("");

if(ff_prot != NULL)
 fprintf(ff_prot,"%s-%s=%s\n",__FUNCTION__,gettext("Разбираем строку"),snast);
double deb=0.;
double kre=0.;

if(iceb_u_polen(snast,&nast_shet,1,'|') != 0)
 return;

if(iceb_u_SRAV(nast_shet.ravno(),"SHET_UCH",0) == 0) /*заменяем SHET_UCH на счёт учёта*/
 nast_shet.new_plus(avp->shetuh.ravno());

if(iceb_u_SRAV(nast_shet.ravno(),"SHET_SP",0) == 0) /*вместо SHET_SP используем счёт списания*/
 {
  if(avp->shetsp.getdlinna() <= 1)
   {
    fprintf(ff_prot,"%s-%s!\n",__FUNCTION__,gettext("Не введён счёт списания"));
    return;
   }
  nast_shet.new_plus(avp->shetsp.ravno());
 }
 
if(iceb_u_polen(snast,&nast_shetkor,2,'|') != 0)
 return;

if(iceb_u_SRAV(nast_shetkor.ravno(),"SHET_UCH",0) == 0) /*заменяем SHET_UCH на счёт учёта*/
 nast_shetkor.new_plus(avp->shetuh.ravno());

if(iceb_u_SRAV(nast_shetkor.ravno(),"SHET_SP",0) == 0) /*вместо SHET_SP используем счёт списания*/
 {
  if(avp->shetsp.getdlinna() <= 1)
   {
    fprintf(ff_prot,"%s-%s !\n",__FUNCTION__,gettext("Не введён счёт списания"));
    return;
   }
  nast_shetkor.new_plus(avp->shetsp.ravno());
 }

if(iceb_u_polen(snast,&sumaprov,3,'|') != 0)
 return;

iceb_u_polen(sumaprov.ravno(),&koef_sumaprov,2,',');


iceb_u_polen(snast,&kekv,4,'|');

iceb_u_polen(snast,&koment,5,'|');
if(iceb_u_strstrm(koment.ravno(),"NOMER_DOK") == 1) /*есть ли в строке*/
 {
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"%s-%s=%s\n",__FUNCTION__,gettext("Подставляем в комментарий номер документа"),avp->nomer_dok.ravno());
    if(avp->nomer_dok.getdlinna() <= 1)
     fprintf(ff_prot,"%s-%s!!!!\n",__FUNCTION__,gettext("В шапку документа не введено номер документа поставщика"));
   }
  iceb_u_zvstr(&koment,"NOMER_DOK",avp->nomer_dok.ravno());
 }
if(iceb_u_strstrm(koment.ravno(),"KOD_KONTRAGENTA") == 1) /*есть ли в строке*/
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-%s=%s\n",__FUNCTION__,gettext("Подставляем в комментарий номер документа"),avp->kontr_v_dok.ravno());
  iceb_u_zvstr(&koment,"KOD_KONTRAGENTA",avp->kontr_v_dok.ravno());
 }

int znak=1;
int metka_is=0; /*Метка идентефикатора суммы*/
if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_S_NDS") == 1)
 {
  deb=avp->suma_s_nds;
  metka_is=1;
 }

if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_USLUG_BEZ_NDS") == 1)
 {
  deb=avp->suma_uslug_bez_nds;
  metka_is=1;
 }

if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_BEZ_NDS") == 1)
 {
  deb=avp->suma_bez_nds;
  metka_is=1;
 }

if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_IZNOSA") == 1)
 {
  deb=avp->suma_bez_nds;
  metka_is=1;
 }

if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_NDS") == 1)
 {
  if(avp->suma_nds_vv != 0.)
   {
    if(avp->metka_snds == 0)
     deb=avp->suma_nds_vv;
    avp->metka_snds=1;
   }
  else
   deb=avp->suma_nds;
  metka_is=1;
 }
if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_UHETA") == 1)
 {
  deb=avp->suma_uheta;
  metka_is=1;
 }

if(metka_is == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-%s! %s\n",__FUNCTION__,gettext("Неправильный идентификатор суммы"),sumaprov.ravno());
 }

if(strpbrk(sumaprov.ravno(),"-") != NULL) /*есть ли знак "-" в строке*/
  znak=-1;

if(deb != 0. && koef_sumaprov.ravno_atof() != 0.)
 {
  deb*=koef_sumaprov.ravno_atof();
  deb=iceb_u_okrug(deb,0.01);
 }

deb*=znak;

if(iceb_avp_rsh(nast_shet.ravno(),avp->kontr_v_dok.ravno(),&shet,&kontr_shet,avp,ff_prot,wpredok) != 0)
 return;
if(iceb_avp_rsh(nast_shetkor.ravno(),avp->kontr_v_dok.ravno(),&shetkor,&kontr_shetkor,avp,ff_prot,wpredok) != 0)
 return;


iceb_zapmpr(avp->gp,avp->mp,avp->dp,shet.ravno(),shetkor.ravno(),
kontr_shet.ravno(),kontr_shetkor.ravno(),deb,kre,koment.ravno(),2,kekv.ravno_atoi(),sp_prov,sum_prov_dk,ff_prot);

}
