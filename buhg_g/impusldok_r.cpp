/*$Id: impusldok_r.c,v 1.22 2014/07/31 07:08:26 sasa Exp $*/
/*23.05.2016	28.11.2005	Белых А.И.	impusldok_r.c
импорт документов из файла
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class impusldok_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  iceb_u_str imafz;
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  impusldok_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   impusldok_r_key_press(GtkWidget *widget,GdkEventKey *event,class impusldok_r_data *data);
gint impusldok_r1(class impusldok_r_data *data);
void  impusldok_r_v_knopka(GtkWidget *widget,class impusldok_r_data *data);
int impusldokt1(int metkaproh,int *koldok,class impusldok_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int impusldok_r(const char *imaf,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_spisok repl_s;
class impusldok_r_data data;

data.imafz.new_plus(imaf);
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(impusldok_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт документов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(impusldok_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)impusldok_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impusldok_r_v_knopka(GtkWidget *widget,class impusldok_r_data *data)
{
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   impusldok_r_key_press(GtkWidget *widget,GdkEventKey *event,class impusldok_r_data *data)
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

gint impusldok_r1(class impusldok_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
struct stat work;

if(stat(data->imafz.ravno(),&work) != 0)
 {
  iceb_u_str repl;
  repl.new_plus(gettext("Нет такого файла !"));
  repl.ps_plus(data->imafz.ravno());
  iceb_menu_soob(&repl,data->window);
  return(FALSE);
 }

int koloh=0;
int koldok=0;
if((koloh=impusldokt1(0,&koldok,data)) == 0)
 impusldokt1(1,&koldok,data);
else
 {
  class iceb_u_str repl;
  
  sprintf(strsql,"%s %d !",gettext("Количество импортированных документов"),koldok);
  repl.plus(strsql);
  sprintf(strsql,"%s %d !",gettext("Количество ошибок"),koloh);
  repl.plus(strsql);
  iceb_menu_soob(&repl,data->window);
 } 


data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);


data->voz=0;

return(FALSE);
}
/****************************************/
/*Проверка-загрузка документов из файла*/
/****************************************/
int impusldokt1(int metkaproh,int *koldok,class impusldok_r_data *data)
{
FILE *ff;
char strsql[2048];
struct stat work;
time_t tmm;

stat(data->imafz.ravno(),&work);

if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  return(1);
 }


if(metkaproh == 0)
 sprintf(strsql,"%s\n",gettext("Проверяем записи в файле"));
if(metkaproh == 1)
 sprintf(strsql,"\n\n%s\n\n",gettext("Загружаем документы из файла"));

int metka_nnn=iceb_poldan_vkl("Нумерация налоговых накладных обновляется каждый месяц","matnast.alx",data->window);

iceb_printw(strsql,data->buffer,data->view);

SQL_str row;
SQLCURSOR cur;

float razmer=0.;
class iceb_u_str nomdok("");
class iceb_u_str nomnalnak("");
class iceb_u_str strok("");
int koloh=0;
int tipz=0;
short dd,md,gd;
int kodpod=0;
class iceb_u_str kontragent("");
class iceb_u_str kodop("");
short lnds=0;
int kodus=0;
double kolih,cena;
class iceb_u_str shet("");
class iceb_u_str eiz("");
OPSHET rekshet;
class iceb_u_str dop_naim("");
time(&tmm);
class iceb_u_str naim_kontr("");
class iceb_u_str naim_uslug("");
int nom_kod_kontr=1;
int metka_zapisi_kk=0;
float pnds=0.;
int kod_usl_pred=1;
class iceb_u_str osnov("");
class iceb_u_str uslprod("");
class iceb_u_str herez("");
class iceb_u_str dover("");
class iceb_u_str datadover("");
class iceb_u_str shet_suma("");
class iceb_u_str rek1("");
while(iceb_u_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());

  if(strok.ravno()[0] == '#')
   continue;

  iceb_printw(strok.ravno(),data->buffer,data->view);
  iceb_pbar(data->bar,work.st_size,razmer);    

  if(iceb_u_polen(strok.ravno(),&rek1,1,'|') != 0)
   continue;

  if(iceb_u_SRAV(strok.ravno(),"DOCU|",1) == 0)
   {
    if(nomdok.getdlinna() > 1 && metkaproh > 0)
     {
      uslpoddokw(dd,md,gd,dd,md,gd,nomdok.ravno(),kodpod,tipz,1,data->window); /*подтверждаем документа*/
      podudokw(dd,md,gd,nomdok.ravno(),kodpod,tipz,data->window); /*проверяем подтверждение и ставим метку подтверждённого документа*/
      
      prosprusw(1,kodpod,dd,md,gd,nomdok.ravno(),tipz,lnds,kodop.ravno(),pnds,&shet_suma,data->window); /*составляем список проводок которые должны быть сделаны*/
      avtprousl1w(tipz,dd,md,gd,nomdok.ravno(),NULL,data->window); /*делаем проводки*/
      prosprusw(0,kodpod,dd,md,gd,nomdok.ravno(),tipz,lnds,kodop.ravno(),pnds,&shet_suma,data->window); /*проверяем сделаны ли проводки и ставим метку выполненных проводок*/
     }    
    metka_zapisi_kk=0;
    
    kodpod=lnds=dd=md=gd=0;
    tipz=0;
        
    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),2,'|');

    if(strsql[0] == '+')
      tipz=1;
    if(strsql[0] == '-')
      tipz=2;

    if(tipz == 0)
     {
      iceb_menu_soob(gettext("Не определен вид документа (приход/расход) !"),data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    if(iceb_u_rsdat(&dd,&md,&gd,strsql,1) != 0)
     {
      iceb_menu_soob(gettext("Не верно введена дата!"),data->window);
      koloh++;
      koloh++;
      continue;
     }

    pnds=iceb_pnds(dd,md,gd,data->window); /*получаем действующее значение НДС*/

    iceb_u_polen(strok.ravno(),&kodpod,4,'|');
    sprintf(strsql,"select kod from Uslpodr where kod=%d",kodpod);
    if(sql_readkey(&bd,strsql) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),kodpod);
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&kontragent,5,'|');
    if(iceb_u_polen(strok.ravno(),&naim_kontr,10,'|') != 0)
     {
      iceb_menu_soob(gettext("Не найдено поле наименование контрагента"),data->window);
      koloh++;
      continue;
     }

    if(naim_kontr.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введено поле наименование контрагента"),data->window);
      koloh++;
      continue;
     }
    /*определяем код контрагента в общем списке контрагентов*/
    if((metka_zapisi_kk=iceb_get_kkfname(naim_kontr.ravno(),kontragent.ravno(),&kontragent,&nom_kod_kontr,data->window)) < 0)
     {
      koloh++;
      continue;
     }
     
    iceb_u_polen(strok.ravno(),&nomnalnak,7,'|');

    if(tipz == 2 && nomnalnak.ravno()[0] != '\0')
     {
      if(metka_nnn == 0)
        sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
        tipz,gd,md,gd,md,nomnalnak.ravno());
      else
        sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
        tipz,gd,gd,nomnalnak.ravno());
      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        iceb_u_str repl;
        
        sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak.ravno());
        repl.plus(strsql);
        
        sprintf(strsql,"%s %s %s",gettext("Материальный учет"),row[0],row[1]);
        repl.ps_plus(strsql);

        iceb_menu_soob(&repl,data->window);
        koloh++;
        continue;
       }

      if(metka_nnn == 0)
        sprintf(strsql,"select datd,nomd from Usldokum where tp=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
        tipz,gd,md,gd,md,nomnalnak.ravno());
      else
        sprintf(strsql,"select datd,nomd from Usldokum where tp=%d and datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
        tipz,gd,gd,nomnalnak.ravno());
      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        iceb_u_str repl;
        
        sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak.ravno());
        repl.plus(strsql);

        sprintf(strsql,"%s %s %s",gettext("В учете услуг"),row[0],row[1]);
        repl.ps_plus(strsql);

        iceb_menu_soob(&repl,data->window);

        koloh++;
        continue;
       }
     }

    iceb_u_polen(strok.ravno(),&kodop,8,'|');
    if(tipz == 1)
      sprintf(strsql,"select kod from Usloper1 where kod='%s'",
      kodop.ravno());
    if(tipz == 2)
      sprintf(strsql,"select kod from Usloper2 where kod='%s'",
      kodop.ravno());

    if(sql_readkey(&bd,strsql) != 1)
     {
      iceb_u_str repl;
      repl.new_plus(gettext("Не найден код операции"));
      repl.plus(" ");
      repl.plus(kodop.ravno());
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&lnds,9,'|');
    iceb_u_polen(strok.ravno(),&osnov,11,'|');
    iceb_u_polen(strok.ravno(),&uslprod,12,'|');
    iceb_u_polen(strok.ravno(),&herez,13,'|');
    iceb_u_polen(strok.ravno(),&dover,14,'|');
    iceb_u_polen(strok.ravno(),&datadover,15,'|');

    //Номер документа определяем последним так как он является индикатором
    //наличия шапки документа
    iceb_u_polen(strok.ravno(),&nomdok,6,'|');
 
    sprintf(strsql,"select nomd from Usldokum where tp=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and podr=%d and nomd='%s'",
    tipz,gd,gd,kodpod,nomdok.ravno());
    if(sql_readkey(&bd,strsql) >= 1)
     {
      sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdok.ravno());
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    //Запись в базу
    if(metkaproh != 0 && nomdok.ravno()[0] != '\0')
     {
      sprintf(strsql,"insert into Usldokum \
(tp,datd,podr,kontr,nomd,nomnn,kodop,nds,ktoi,vrem,pn,osnov,uslpr,sherez,dover,datdov) \
values (%d,'%04d-%02d-%02d',%d,'%s','%s','%s','%s',%d,%d,%ld,%.2f,'%s','%s','%s','%s','%s')",
      tipz,gd,md,dd,kodpod,kontragent.ravno(),nomdok.ravno(),nomnalnak.ravno(),kodop.ravno(),lnds,iceb_getuid(data->window),tmm,pnds,
      osnov.ravno_filtr(),
      uslprod.ravno_filtr(),
      herez.ravno_filtr(),
      dover.ravno_filtr(),
      datadover.ravno_sqldata());

      if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
       *koldok+=1;
      else
       koloh+=1;
     }        
    
   }

  if(iceb_u_SRAV(rek1.ravno(),"KON",0) == 0) /*запись реквизитов контрагента*/
   {
    iceb_zkvsk(strok.ravno(),&nom_kod_kontr,NULL,data->window);
    continue;
   }

  if(iceb_u_SRAV(strok.ravno(),"ZPU|",1) == 0)
   {

    iceb_u_polen(strok.ravno(),&kodus,2,'|');
    iceb_u_polen(strok.ravno(),&naim_uslug,8,'|');
    if(naim_uslug.getdlinna() > 1)
     {
      /*проверяем есть ли такое наименование в списке материаллов*/
      sprintf(strsql,"select kodus from Uslugi where naius='%s'",naim_uslug.ravno_filtr());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        kodus=atoi(row[0]);
       }
      else
       {
        int metka_nomkm=0;
        class iceb_lock_tables lk("LOCK TABLES Material READ,Uslugi WRITE,icebuser READ");
        if(kodus != 0)
         {
          /*проверяем нет ли уже другого материала или услуги с этим кодом*/
          sprintf(strsql,"select kodus from Uslugi where kodus=%d",kodus);
          if(iceb_sql_readkey(strsql,data->window) == 1)
           {
            kodus=nomkmw(kod_usl_pred,data->window);
            metka_nomkm=1;
           }
          else
           {
            sprintf(strsql,"select kodm from Material where kodm=%d",kodus);
            if(iceb_sql_readkey(strsql,data->window) == 1)
             {
              kodus=nomkmw(kod_usl_pred,data->window);
              metka_nomkm=1;
             }
           }
         }
        else
         {
          kodus=nomkmw(kod_usl_pred,data->window);
          metka_nomkm=1;
         }
        sprintf(strsql,"insert into Uslugi (kodus,naius,ktoz,vrem) values(%d,'%s',%d,%ld)",kodus,naim_uslug.ravno_filtr(),iceb_getuid(data->window),time(NULL));

        if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
         {
          if(metka_nomkm == 1)        
           kod_usl_pred=kodus;
         }
        else
         koloh++;
       }             
     }    
    else
     {
      sprintf(strsql,"select kodus from Uslugi where kodus=%d",kodus);
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodus);
        iceb_menu_soob(strsql,data->window);
        koloh++;

        continue;
       }
     }
    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    kolih=atof(strsql);
    if(kolih == 0)
     {
      sprintf(strsql,"%s !",gettext("В записи нулевое количество"));
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),4,'|');
    cena=atof(strsql);
    cena=iceb_u_okrug(cena,okrcn);

    iceb_u_polen(strok.ravno(),&eiz,5,'|');
    sprintf(strsql,"select kod from Edizmer where kod='%s'",eiz.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      iceb_u_str repl;
      repl.new_plus(gettext("Не найдена единица измерения"));
      repl.plus(" ");
      repl.plus(eiz.ravno());
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&shet,6,'|'); 
    if(iceb_prsh1(shet.ravno(),&rekshet,data->window) != 0)
     {
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&dop_naim,7,'|');
    

    //Запись в базу
    if(metkaproh != 0 && nomdok.ravno()[0] != '\0')
      if(zapuvdokw(tipz,dd,md,gd,nomdok.ravno(),1,kodus,kolih,cena,eiz.ravno(),shet.ravno(),kodpod,"",dop_naim.ravno(),data->window) != 0)
       koloh++;

   }
   

 }
iceb_pbar(data->bar,work.st_size,work.st_size);

fclose(ff);

if(nomdok.getdlinna() > 1 && metkaproh > 0)
 {
  uslpoddokw(dd,md,gd,dd,md,gd,nomdok.ravno(),kodpod,tipz,1,data->window); /*подтверждаем документа*/
  podudokw(dd,md,gd,nomdok.ravno(),kodpod,tipz,data->window); /*проверяем подтверждение и ставим метку подтверждённого документа*/
  
  pnds=iceb_pnds(dd,md,gd,data->window); /*получаем действующее значение НДС*/

  prosprusw(1,kodpod,dd,md,gd,nomdok.ravno(),tipz,lnds,kodop.ravno(),pnds,&shet_suma,data->window); /*составляем список проводок которые должны быть сделаны*/
  avtprousl1w(tipz,dd,md,gd,nomdok.ravno(),NULL,data->window); /*делаем проводки*/
  prosprusw(0,kodpod,dd,md,gd,nomdok.ravno(),tipz,lnds,kodop.ravno(),pnds,&shet_suma,data->window); /*проверяем сделаны ли проводки и ставим метку выполненных проводок*/
 }    

return(koloh);
}
