/*$Id: b_iBank_2_UAw.c,v 1.13 2013/08/13 06:09:31 sasa Exp $*/
/*23.05.2013	09.06.2006	Белых А.И.	b_iBank_2_UA.c

Экспорт платёжек для системы iBank 2 UA
Кодировка Сp1251
---------------------------------------------------------------
Наименование    Описание                        Тип        
поля
---------------------------------------------------------------
DATE_DOC        Дата документа                  date     10
NUM_DOC         Номер документа                 string   10
AMOUNT          Сумма документа                 decimal  11.2
CLN_NAME        Наименование плательщика        string   40
CLN_OKPO        ЕГРПОУ плательщика              string   10
CLN_ACCOUNT     Счёт плательщика                string   14
CLN_BANK_NAME   Наименование банка плательщика  string   45
CLN_BANK_MFO    МФО банка плательщика           string   6
RCPT_NAME       Наименование получателя         string   40
RCPT_OKPO       ЕГРПОУ получателя               string   10
RCPT_ACCOUNT    Счёт получателя                 string   14
RCPT_BANK_NAME  Наименование банка получателя   string   45
RCPT_BANK_MFO   МФО банка получателя            string   6
PAYMENT_DETAILS Назначение платежа              string   160
VALUE_DATE      Дата валютирования              date     10


*/

#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern struct REC rec;
extern SQL_baza bd;

int b_iBank_2_UAw(const char *tabl,GtkWidget *wpredok)
{
char		imaf1[64];
FILE		*ff1;
class iceb_u_str koment("");
char		strsql[1024];
short		d,m,g;
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select datd,nomd from %s where vidpl='1'",tabl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного документа для передачи в банк!"),wpredok);
  return(1);
 }
sprintf(imaf1,"plat.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return(1);
 }
fprintf(ff1,"Content-Type=doc/ua_payment\n\n");
short dv,mv,gv;


while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  if(readpdokw(tabl,g,row[1],wpredok) != 0)
     continue;

  /*Так как в системе предусморено только 40 символов для наименования организации то нужно брать короткое наименование*/
  class iceb_u_str naior(rec.naior.ravno());
  class iceb_u_str naior1(rec.naior1.ravno());
 
  if(iceb_u_strlen(rec.naior.ravno()) > 40)
   {
    naior.new_plus(iceb_get_pnk(rec.kodor.ravno(),0,wpredok));

    sprintf(strsql,"%s %s %s\n%s %s",
    gettext("Наименование контрагента"),
    rec.naior.ravno(),
    gettext("больше 40 символов"),
    gettext("заменили на короткое"),
    naior.ravno());

    iceb_menu_soob(strsql,wpredok);

   }
  if(iceb_u_strlen(rec.naior1.ravno()) > 40)
   {
    if(rec.kodor1.getdlinna() > 1) /*При выборе контрагента из файла код контрагента может быть не определён*/
     {
      naior1.new_plus(iceb_get_pnk(rec.kodor1.ravno(),0,wpredok));

      sprintf(strsql,"%s %s %s\n%s %s",
      gettext("Наименование контрагента"),
      rec.naior1.ravno(),
      gettext("больше 40 символов"),
      gettext("заменили на короткое"),
      naior1.ravno());

      iceb_menu_soob(strsql,wpredok);


     }
   }



  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);

  dv=rec.dv;
  mv=rec.mv;
  gv=rec.gv;
  if(dv == 0)
   {
    dv=d;
    mv=m;
    gv=g;
   }
  fprintf(ff1,"\
DATE_DOC=%02d.%02d.%04d\n\
NUM_DOC=%s\n\
AMOUNT=%.2f\n\
CLN_NAME=%.40s\n\
CLN_OKPO=%s\n\
CLN_ACCOUNT=%s\n\
CLN_BANK_NAME=%.*s\n\
CLN_BANK_MFO=%s\n\
RCPT_NAME=%.40s\n\
RCPT_OKPO=%s\n\
RCPT_ACCOUNT=%s\n\
RCPT_BANK_NAME=%.*s\n\
RCPT_BANK_MFO=%.6s\n\
PAYMENT_DETAILS=%.*s\n\
VALUE_DATE=%02d.%02d.%04d\n\n",
  d,m,g,
  row[1],
  rec.sumd,
  naior.ravno(),
  rec.kod.ravno(),
  rec.nsh.ravno(),
  iceb_u_kolbait(45,rec.naiban.ravno()),rec.naiban.ravno(),
  rec.mfo.ravno(),
  naior1.ravno(),
  rec.kod1.ravno(),
  rec.nsh1.ravno(),
  iceb_u_kolbait(45,rec.naiban1.ravno()),rec.naiban1.ravno(),
  rec.mfo1.ravno(),
  iceb_u_kolbait(160,koment.ravno()),koment.ravno(),
  dv,mv,gv);
  
  
 }

fclose(ff1);
iceb_perecod(2,imaf1,wpredok);
return(0);
}
