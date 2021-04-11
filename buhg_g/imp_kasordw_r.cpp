/*$Id: imp_kasordw_r.c,v 1.30 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	13.04.2007	Белых А.И.	imp_kasordw_r.c
импорт кассовых ордеров
*/
#include <sys/stat.h>
#include  <errno.h>
#include "buhg_g.h"

class imp_kasordw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  class iceb_u_str imafz;
  class iceb_u_spisok kodkontr_new; /*список контрагентов добавленных в справочник контрагентов*/        

  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  imp_kasordw_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_kasordw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_kasordw_r_data *data);
gint imp_kasordw_r1(class imp_kasordw_r_data *data);
void  imp_kasordw_r_v_knopka(GtkWidget *widget,class imp_kasordw_r_data *data);

int imp_kasord_zag(int metka,int razmer_fil,FILE *ff,class imp_kasordw_r_data*);

extern SQL_baza bd;
extern double okrcn;

int imp_kasordw_r(const char *imaf,
GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class imp_kasordw_r_data data;

data.imafz.new_plus(imaf);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт кассовых ордеров"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(imp_kasordw_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

GtkWidget *label=NULL;
label=gtk_label_new(gettext("Импорт кассовых ордеров"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(imp_kasordw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)imp_kasordw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_kasordw_r_v_knopka(GtkWidget *widget,class imp_kasordw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   imp_kasordw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_kasordw_r_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}

/******************************************/
/******************************************/

gint imp_kasordw_r1(class imp_kasordw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_clock sss(data->window);
struct stat work;

if(stat(data->imafz.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Не найден файл"),data->imafz.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

FILE *ff=NULL;

if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolih_oh=0;
if((kolih_oh=imp_kasord_zag(0,work.st_size,ff,data)) == 0)
 {
  rewind(ff);
  imp_kasord_zag(1,work.st_size,ff,data);
  unlink(data->imafz.ravno());
 }
else
 {
   sprintf(strsql,"%s %d !\n%s",gettext("Количество ошибок"),kolih_oh,
   gettext("Загрузка невозможна !"));
  iceb_menu_soob(strsql,data->window);
 }
fclose(ff);





data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);




data->voz=0;

return(FALSE);
}
/********************************************/
/*подтверждаем документ и делаем проводки*/
/******************************************/
int imp_kasordw_r_vp(short dd,short md,short gd,
const char *kassa,
int tipzap,
const char *nomdok,
const char *kodop,
const char *shetd,
GtkWidget *wpredok)
{
char strsql[512];
class SQLCURSOR cur;
SQL_str row;

/*подтверждаем документ*/
sprintf(strsql,"UPDATE Kasord1 set datp='%d-%d-%d' where kassa=%s and god=%d and tp=%d and nomd='%s' and datp='0000-00-00'",
gd,md,dd,kassa,gd,tipzap,nomdok);

iceb_sql_zapis(strsql,0,0,wpredok);

/*выполняем проводки*/

//Читаем счёт кассы в коде операции если он там есть
class iceb_u_str shetk("");

if(tipzap == 1)
 sprintf(strsql,"select shetkas,metkapr from Kasop1 where kod='%s'",kodop);
if(tipzap == 2)
 sprintf(strsql,"select shetkas,metkapr from Kasop2 where kod='%s'",kodop);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  shetk.new_plus(row[0]);
  if(row[1][0] == '0')
   return(1); /*проводок делать для этой операции не нужно*/
 }

//Читаем счёт кассы
if(shetk.ravno()[0] == '\0')
 {
  sprintf(strsql,"select shet from Kas where kod=%s",kassa);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найдена касса"),kassa);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
  else
   shetk.new_plus(row[0]);
  
 }
if(shetk.ravno()[0] == '\0')
 return(1);

avtprkasw(kassa,tipzap,nomdok,dd,md,gd,shetk.ravno(),shetd,kodop,wpredok); /*выполняем проводки*/

provpodkow(kassa,nomdok,dd,md,gd,tipzap,wpredok); /*проверяем подтверждение документа*/
provprkasw(kassa,tipzap,nomdok,dd,md,gd,kodop,wpredok); /*проверяем выполнение проводок*/

return(0);
}
/*************************/
/*Импорт*/
/**************************/

int imp_kasord_zag(int metka, //0-проверка на ошибки 1- загрузка
int razmer_fil,
FILE *ff,class imp_kasordw_r_data *data)
{
char strsql[1024];
if(metka == 0)
 sprintf(strsql,"%s\n",gettext("Проверка на наличие ошибок"));
if(metka == 1)
 sprintf(strsql,"\n\n%s\n",gettext("Загружаем документы из файла"));

iceb_printw(strsql,data->buffer,data->view);

class iceb_u_str stroka("");
class iceb_u_str kod_str("");
class iceb_u_str bros("");
int kol_oh=0; //количество ошибок
int tipzap=0;
class iceb_u_str tip("");
class iceb_u_str kassa("");
short dd,md,gd;
class iceb_u_str nomdok("");
class iceb_u_str shetd("");
class iceb_u_str shetk("");
class iceb_u_str kodop("");
class iceb_u_str osnov("");
class iceb_u_str dopol("");
class iceb_u_str fio("");
class iceb_u_str dokum("");
class iceb_u_str nomer_bl("");
float razmer=0;
SQL_str row;
SQLCURSOR cur;
struct OPSHET rek_shet;
class iceb_u_str kontr("");
double suma=0.;
class iceb_u_str fam("");
class iceb_u_str inn("");
int koldok_p=0;
int koldok_r=0;
double i_sumap=0.;
double i_sumar=0.;
int kodkontr=1;
while(iceb_u_fgets(&stroka,ff) == 0)
 {
  razmer+=strlen(stroka.ravno());

  iceb_printw(stroka.ravno(),data->buffer,data->view);

  iceb_pbar(data->bar,razmer_fil,razmer);

  if(iceb_u_polen(stroka.ravno(),&kod_str,1,'|') != 0)
   continue;  

  if(iceb_u_SRAV(kod_str.ravno(),"KON",0) == 0) /*запись реквизитов контрагента*/
   {
    iceb_zkvsk(stroka.ravno(),&kodkontr,NULL,data->window);
    continue;
   }
  
  if(iceb_u_SRAV("KDOK",kod_str.ravno(),0) == 0) //Строка с шапкой документа
   {
    if(nomdok.getdlinna() > 1) /*подтверждение документа и выполнение проводок*/
      imp_kasordw_r_vp(dd,md,gd,kassa.ravno(),tipzap,nomdok.ravno(),kodop.ravno(),shetd.ravno(),data->window);
  

    iceb_u_polen(stroka.ravno(),&tip,2,'|');
    
    if(tip.ravno()[0] == '+')
     tipzap=1;
    if(tip.ravno()[0] == '-')
     tipzap=2;
     
    iceb_u_polen(stroka.ravno(),&kassa,3,'|');
    iceb_u_polen(stroka.ravno(),&bros,4,'|');
    if(iceb_u_rsdat(&dd,&md,&gd,bros.ravno(),1) != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
      kol_oh++;
     }

    if(iceb_pbpds(md,gd,data->window) != 0) /*проверка блокировки*/
      kol_oh++;

    iceb_u_polen(stroka.ravno(),&nomdok,5,'|');
    iceb_u_polen(stroka.ravno(),&shetd,6,'|');
    iceb_u_polen(stroka.ravno(),&kodop,7,'|');
    iceb_u_polen(stroka.ravno(),&osnov,8,'|');
    iceb_u_polen(stroka.ravno(),&dopol,9,'|');
    iceb_u_polen(stroka.ravno(),&fio,10,'|');
    iceb_u_polen(stroka.ravno(),&dokum,11,'|');
    iceb_u_polen(stroka.ravno(),&nomer_bl,12,'|');

    if(metka == 0) //проверка на ошибки
     {
      //Проверяем код кассы
      sprintf(strsql,"select kod from Kas where kod=%s",kassa.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код кассы"),kassa.ravno());
        iceb_menu_soob(strsql,data->window);
        kol_oh++;
       }
      //проверяем код операции
      if(tip.ravno()[0] == '+')
        sprintf(strsql,"select kod from Kasop1 where kod='%s'",kodop.ravno());
      if(tip.ravno()[0] == '-')
        sprintf(strsql,"select kod from Kasop2 where kod='%s'",kodop.ravno());
        
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код операции"),kodop.ravno());
        iceb_menu_soob(strsql,data->window);
        kol_oh++;
       }

      //проверяем счёт
          
      if(iceb_prsh1(shetd.ravno(),&rek_shet,data->window) != 0)
        kol_oh++;

      //проверяем номер документа
      if(nomdok.getdlinna() > 1)
       {
        
        sprintf(strsql,"select nomd from Kasord where nomd='%s' and kassa=%s and tp=%d and god=%d",
        nomdok.ravno(),kassa.ravno(),tipzap,gd);
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
         {
          sprintf(strsql,gettext("Документ с номером %s уже есть в базе !"),nomdok.ravno());
          iceb_menu_soob(strsql,data->window);
          kol_oh++;
         }
           
       
       }
    
      /*Проверяем номер бланка кассового ордера*/
      if(nomer_bl.getdlinna() > 1)
       {
        sprintf(strsql,"select nomd from Kasord where nb='%s'",nomer_bl.ravno_filtr());
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
         {
          sprintf(strsql,gettext("Номер бланка %s уже имеет документ с номером %s!"),nomer_bl.ravno(),row[0]);
          iceb_menu_soob(strsql,data->window);
          kol_oh++;
         }
       }             
     }
      
    if(metka == 1) //запись в базу шапки документа
     {
      class iceb_lock_tables kkk("LOCK TABLE Kasord WRITE,icebuser READ");
      if(nomdok.getdlinna() <= 1)
          iceb_nomnak(gd,kassa.ravno(),&nomdok,tipzap,2,1,data->window);

      if(zaphkorw(0,kassa.ravno(),tipzap,dd,md,gd,nomdok.ravno(),shetd.ravno(),kodop.ravno(),osnov.ravno_filtr(),dopol.ravno_filtr(),fio.ravno_filtr(),
      dokum.ravno_filtr(),"","",0,0,0,nomer_bl.ravno(),"",data->window) != 0)
       {
        iceb_menu_soob("Ошибка записи шапки документа !",data->window);
        return(1);
       }
      if(tipzap == 1)
       koldok_p++;
      else
       koldok_r++;
     }
   }
  
  if(iceb_u_SRAV("KZAP",kod_str.ravno(),0) == 0) //Строка содержимым документа
   {
    iceb_u_polen(stroka.ravno(),&kontr,2,'|');
    iceb_u_polen(stroka.ravno(),&suma,3,'|');
    iceb_u_polen(stroka.ravno(),&inn,4,'|');
    iceb_u_polen(stroka.ravno(),&fam,5,'|');

    if(fam.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введeно наименование контрагента"),data->window);
      kol_oh++;
     }
    if(iceb_get_kkfname(fam.ravno(),kontr.ravno(),&kontr,&kodkontr,data->window) < 0)
     {
      kol_oh++;
      continue;
     }
#if 0
####################################333
    class iceb_u_str kod_kontr("");
    /*Проверяем индивидуальный налоговый номер*/
    if(inn.getdlinna() > 1)
     {         
      sprintf(strsql,"select kodkon from Kontragent where kod='%s'",inn.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
        kod_kontr.new_plus(row[0]);
     }
    if(kontr.getdlinna() <= 1)
     {
      /*Проверяем по фамилии*/
       
      sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",fam.ravno_filtr());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
        kod_kontr.new_plus(row[0]);
     }        

    if(kod_kontr.getdlinna() <= 1)
     {
      class iceb_lock_tables kkkk("LOCK TABLE Kontragent WRITE,icebuser READ");
      if(kontr.getdlinna() > 1) /*проверяем свободен ли код*/
       {
        sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kontr.ravno_filtr());
        if(iceb_sql_readkey(strsql,data->window) >= 1)
          kontr.new_plus(iceb_getnkontr(kodkontr,data->window));
       }
      else
          kontr.new_plus(iceb_getnkontr(kodkontr,data->window));

      sprintf(strsql,"insert into Kontragent (kodkon,naikon,kod,ktoz,vrem) values ('%s','%s','%s',%d,%ld)",
      kontr.ravno_filtr(),fam.ravno_filtr(),inn.ravno(),iceb_getuid(data->window),time(NULL));
      
      if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
       {
        kol_oh++;
        continue;
       }
      data->kodkontr_new.plus(kontr.ravno());
      kodkontr=kontr.ravno_atoi();
     }
    else
     kontr.new_plus(kod_kontr.ravno());
#################################3    
#endif
    if(metka == 1) //запись в базу строки документа
     {

      /*проверяем может контрагент грузится повторно*/
      sprintf(strsql,"select suma from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s' and kontr='%s'",
      kassa.ravno_atoi(),
      gd,
      tipzap,
      nomdok.ravno_filtr(),
      kontr.ravno_filtr());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        if(zapzkorw(kassa.ravno(),tipzap,dd,md,gd,nomdok.ravno(),kontr.ravno_filtr(),suma+atof(row[0]),kontr.ravno_filtr(),"",data->window) != 0)
         {
          iceb_menu_soob(gettext("Ошибка записи в кассовый ордер"),data->window);
          kol_oh++;
          continue;
         }
       }
      else      
       if(zapzkorw(kassa.ravno(),tipzap,dd,md,gd,nomdok.ravno(),kontr.ravno_filtr(),suma,"","",data->window) != 0)
        {
         iceb_menu_soob(gettext("Ошибка записи в кассовый ордер"),data->window);
         kol_oh++;
         continue;
        }

      if(tipzap == 1)
       i_sumap+=suma;
      else
       i_sumar+=suma;
#if 0
######################################################################33
      /*подтверждаем документ*/
      sprintf(strsql,"UPDATE Kasord1 set datp='%d-%d-%d' where \
kassa=%s and god=%d and tp=%d and nomd='%s' and datp='0000-00-00'",
      gd,md,dd,kassa.ravno(),gd,tipzap,nomdok.ravno());

      iceb_sql_zapis(strsql,0,0,data->window);
      
      /*выполняем проводки*/

      //Читаем счёт кассы в коде операции если он там есть
      shetk.new_plus("");

      if(tipzap == 1)
       sprintf(strsql,"select shetkas,metkapr from Kasop1 where kod='%s'",kodop.ravno());
      if(tipzap == 2)
       sprintf(strsql,"select shetkas,metkapr from Kasop2 where kod='%s'",kodop.ravno());

      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        shetk.new_plus(row[0]);
        if(row[1][0] == '0')
         continue; /*проводок делать для этой операции не нужно*/
       }

      //Читаем счёт кассы
      if(shetk.ravno()[0] == '\0')
       {
        sprintf(strsql,"select shet from Kas where kod=%s",kassa.ravno());
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найдена касса"),kassa.ravno());
          iceb_menu_soob(strsql,data->window);
          continue;
         }
        else
         shetk.new_plus(row[0]);
        
       }
      if(shetk.ravno()[0] == '\0')
       continue;
      
      avtprkasw(kassa.ravno(),tipzap,nomdok.ravno(),dd,md,gd,shetk.ravno(),shetd.ravno(),kodop.ravno(),data->window); /*выполняем проводки*/

      provpodkow(kassa.ravno(),nomdok.ravno(),dd,md,gd,tipzap,data->window); /*проверяем подтверждение документа*/
      provprkasw(kassa.ravno(),tipzap,nomdok.ravno(),dd,md,gd,kodop.ravno(),data->window); /*проверяем выполнение проводок*/
#################################################3
#endif
     }
   }




 }
iceb_pbar(data->bar,razmer_fil,razmer_fil); /*при чтении убираются обратные косые и символи возврата карретки*/

if(nomdok.getdlinna() > 1) /*подтверждение документа и выполнение проводок*/
  imp_kasordw_r_vp(dd,md,gd,kassa.ravno(),tipzap,nomdok.ravno(),kodop.ravno(),shetd.ravno(),data->window);

if(metka == 1)
 {
  if(data->kodkontr_new.kolih() > 0)
   {
    sprintf(strsql,"%s:\n",gettext("Список контрагентов добавленных в справочник"));
    iceb_printw(strsql,data->buffer,data->view);

    for(int nom=0; nom < data->kodkontr_new.kolih(); nom++)
     {
      sprintf(strsql,"%*s %s\n",iceb_u_kolbait(6,data->kodkontr_new.ravno(nom)),data->kodkontr_new.ravno(nom),iceb_get_pnk(data->kodkontr_new.ravno(nom),0,data->window));
      iceb_printw(strsql,data->buffer,data->view);
     }
   }
   
  class iceb_u_str repl;
  
  sprintf(strsql,"%s %d %s %.2f",
  gettext("Приходных документов"),
  koldok_p,
  gettext("на сумму"),
  i_sumap);
  
  repl.plus(strsql);
  
  sprintf(strsql,"%s %d %s %.2f",
  gettext("Расходных документов"),
  koldok_r,
  gettext("на сумму"),
  i_sumar);
  
  repl.ps_plus(strsql);

  sprintf(strsql,"%s:%d",gettext("Количество ошибок"),kol_oh);
  repl.ps_plus(strsql);

  iceb_menu_soob(&repl,data->window);  
 }

return(kol_oh);
}
