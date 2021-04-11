/*$Id: impmatdok_r.c,v 1.32 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	16.11.2004	Белых А.И.	impmatdok_r.c
импорт документов из файла
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class impmatdok_r_data
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
  impmatdok_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   impmatdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class impmatdok_r_data *data);
gint impmatdok_r1(class impmatdok_r_data *data);
void  impmatdok_r_v_knopka(GtkWidget *widget,class impmatdok_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int impmatdok_r(const char *imaf,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
class impmatdok_r_data data;
int gor=0;
int ver=0;

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(impmatdok_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(impmatdok_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)impmatdok_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impmatdok_r_v_knopka(GtkWidget *widget,class impmatdok_r_data *data)
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

gboolean   impmatdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class impmatdok_r_data *data)
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

gint impmatdok_r1(class impmatdok_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[2048];
iceb_u_str repl;
struct stat work;


if(stat(data->imafz.ravno(),&work) != 0)
 {
  repl.new_plus(gettext("Нет такого файла !"));
  repl.ps_plus(data->imafz.ravno());
  iceb_menu_soob(&repl,data->window);
  return(FALSE);
 }



class iceb_u_str kodiopuc("");
int kodopuc=0; //Метка операции списания по учетной цене

iceb_poldan("Коды операций расхода по учетным ценам",&kodiopuc,"matnast.alx",data->window);
short metkaproh=0;
FILE *ff;

nazad:;


if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  return(FALSE);
 }


if(metkaproh == 0)
 sprintf(strsql,"%s\n",gettext("Проверяем записи в файле"));
if(metkaproh == 1)
 sprintf(strsql,"\n\n%s\n\n",gettext("Загружаем документы из файла"));

int metka_nnn=iceb_poldan_vkl("Нумерация налоговых накладных обновляется каждый месяц","matnast.alx",data->window);

iceb_printw(strsql,data->buffer,data->view);

float razmer=0;
class iceb_u_str nomdok("");
int koldok=0;
int koloh=0;
short dd,md,gd;
int tipz=0;
class iceb_u_str sklad=("");
class iceb_u_str strok("");
class iceb_u_str nomnalnak("");
SQL_str row1;
SQL_str row;
SQLCURSOR cur;
class iceb_u_str kontr("");
class iceb_u_str kodop("");
class iceb_u_str nomvstdok("");
short dpnn,mpnn,gpnn;
time_t tmm;
time(&tmm);
OPSHET shetv;
int vtara;
float nds;
class iceb_u_str nomzak;
int kodmat;
double kolih=0.;
double cena=0.;
class iceb_u_str eiz("");
int ndsvkl;
class iceb_u_str shetu("");
class iceb_u_str naimat("");
class iceb_u_str naim("");
int kodusl=0;
int kod_mat_pred=0;
class iceb_u_str naim_kontr("");
int nom_kod_kontr=1;
int metka_zapisi_kk=0; /*Метка записи кода контрагента*/
int lnds=0;
//#define KOLPOL 15
//class iceb_u_str rek_kon[KOLPOL];
class iceb_u_str shet_suma;
class iceb_u_str rek1("");
while(iceb_u_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());

  if(strok.ravno()[0] == '#')
   continue;
   

  iceb_printw(strok.ravno(),data->buffer,data->view);
  iceb_pbar(data->bar,work.st_size,razmer);    

  if(iceb_u_polen(strok.ravno(),&rek1,1,'|') != 0)
   {
    continue;
   }

  if(iceb_u_SRAV(rek1.ravno(),"KON",0) == 0) /*запись реквизитов контрагента*/
   {
    iceb_zkvsk(strok.ravno(),&nom_kod_kontr,NULL,data->window);
    continue;
   }


  if(iceb_u_SRAV(rek1.ravno(),"DOC",0) == 0)
   {
    if(koldok > 0 && metkaproh > 0 && nomdok.ravno()[0] != '\0')
     {
      podtdok1w(dd,md,gd,sklad.ravno_atoi(),nomdok.ravno(),tipz,dd,md,gd,1,data->window);
      podvdokw(dd,md,gd,nomdok.ravno(),sklad.ravno_atoi(),data->window);

      prosprw(1,sklad.ravno_atoi(),dd,md,gd,nomdok.ravno(),tipz,lnds,0,"",kodop.ravno(),&shet_suma,data->window); /*составляем список проводок которые должны быть сделаны*/
      avtpromu1(tipz,dd,md,gd,nomdok.ravno(),NULL,data->window); /*делаем проводки*/
      prosprw(0,sklad.ravno_atoi(),dd,md,gd,nomdok.ravno(),tipz,lnds,0,"",kodop.ravno(),&shet_suma,data->window); /*проверяем сделаны проводки все или нет*/
     }     
    metka_zapisi_kk=0;    
    lnds=0;
    koldok++;
    
    nomdok.new_plus("");
    dd=md=gd=0;
    tipz=0;

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),2,'|');

    if(strsql[0] == '+')
      tipz=1;
    if(strsql[0] == '-')
      tipz=2;

    if(tipz == 0)
     {
      repl.new_plus(gettext("Не определен вид документа (приход/расход) !"));
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    if(iceb_u_rsdat(&dd,&md,&gd,strsql,1) != 0)
     {
      repl.new_plus(gettext("Не верно введена дата!"));
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&sklad,4,'|');

    sprintf(strsql,"select kod from Sklad where kod=%d",sklad.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      repl.new_plus(gettext("Не найден код склада"));
      repl.plus(" ");
      repl.plus(sklad.ravno_atoi());
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&kontr,5,'|');
    if(iceb_u_polen(strok.ravno(),&naim_kontr,11,'|') != 0)
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
    if((metka_zapisi_kk=iceb_get_kkfname(naim_kontr.ravno(),kontr.ravno(),&kontr,&nom_kod_kontr,data->window)) < 0)
     {
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&nomnalnak,7,'|');

    if(tipz == 2 && nomnalnak.ravno()[0] != '\0')
     {
      if(metka_nnn == 0) /*месячная нумерация*/
        sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
        tipz,gd,md,gd,md,nomnalnak.ravno());
      else /*годовая*/
        sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
        tipz,gd,gd,nomnalnak.ravno());
      if(iceb_sql_readkey(strsql,&row1,&cur,data->window) >= 1)
       {
        sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak.ravno());
        repl.new_plus(strsql);
        sprintf(strsql,"%s %s %s",gettext("Материальный учет"),row1[0],row1[1]);
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

      if(iceb_sql_readkey(strsql,&row1,&cur,data->window) >= 1)
       {
        sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak.ravno());
        repl.new_plus(strsql);
        sprintf(strsql,"%s %s %s",gettext("Учёт услуг"),row1[0],row1[1]);
        repl.ps_plus(strsql);
        iceb_menu_soob(&repl,data->window);
        koloh++;
        continue;
       }
     }

    iceb_u_polen(strok.ravno(),&kodop,8,'|');
    if(tipz == 1)
      sprintf(strsql,"select kod from Prihod where kod='%s'",kodop.ravno());
    if(tipz == 2)
      sprintf(strsql,"select kod from Rashod where kod='%s'",kodop.ravno());

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      repl.new_plus(gettext("Не найден код операции"));
      repl.plus(" ");
      repl.plus(kodop.ravno());
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&nomvstdok,9,'|');

    dpnn=mpnn=gpnn=0;
    if(nomnalnak.ravno()[0] != '\0')
     {
      iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),10,'|');
      
      if(strsql[0] != '\0')
       {
        if(iceb_u_rsdat(&dpnn,&mpnn,&gpnn,strsql,1) != 0)
         {
          if(tipz == 1)
            repl.new_plus(gettext("Не верно введена дата получения налоговой накладной !"));
          if(tipz == 2)
            repl.new_plus(gettext("Не верно введена дата выдачи налоговой накладной !"));
          iceb_menu_soob(&repl,data->window);
          koloh++;
          continue;
         }
       }
      else
       {
        dpnn=dd; mpnn=md; gpnn=gd;
       }
     }

    
    //Номер документа определяем последним так как он является индикатором
    //наличия шапки документа
    if(iceb_u_polen(strok.ravno(),&nomdok,6,'|') != 0)
     {
      sprintf(strsql,"%s!",gettext("Не найдено поле с номером документа"));
      iceb_menu_soob(strsql,data->window);
      
      koloh++;
      continue;
     }
//  printw("***nomdok=%s %d\n",nomdok,metkaproh);
 

    if(nomdok.getdlinna() > 1)
     {
      sprintf(strsql,"select nomd from Dokummat where tip=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and sklad=%d and nomd='%s'",
      tipz,gd,gd,sklad.ravno_atoi(),nomdok.ravno());
      if(iceb_sql_readkey(strsql,data->window) >= 1)
       {
        sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdok.ravno());
        iceb_menu_soob(strsql,data->window);
        koloh++;
        continue;
       }
     }

    //Запись в базу
    if(metkaproh != 0)
     {
      class iceb_lock_tables lk("LOCK TABLES Dokummat WRITE,Nalog READ,icebuser READ");
      if(nomdok.getdlinna() <= 1)
        iceb_nomnak(gd,sklad.ravno(),&nomdok,1,0,0,data->window);

      float pnds=iceb_pnds(dd,md,gd,data->window);
      if(lnds == 4)
       pnds=7.;
      zap_s_mudokw(tipz,dd,md,gd,sklad.ravno_atoi(),kontr.ravno(),nomdok.ravno(),nomnalnak.ravno(),kodop.ravno(),0,0,nomvstdok.ravno(),dpnn,mpnn,gpnn,0,"00",pnds,data->window);
     }        
   }

  //Строка записи в документе
  if(iceb_u_SRAV(rek1.ravno(),"ZP1",0) == 0)
   {
    iceb_u_polen(strok.ravno(),&kodmat,2,'|');
    iceb_u_polen(strok.ravno(),&naimat,11,'|');

    if(naimat.getdlinna() <= 1) /*наименования материалла нет*/
     {
      sprintf(strsql,"select kodm from Material where kodm=%d",kodmat);
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        repl.new_plus(gettext("Не найден код материалу"));
        repl.plus(" ");
        repl.plus(kodmat);
        repl.plus(" !");
        iceb_menu_soob(&repl,data->window);
        koloh++;
        continue;
       }
     }
    else
     {
      /*проверяем есть ли такое наименование в списке материаллов*/
      sprintf(strsql,"select kodm from Material where naimat='%s'",naimat.ravno_filtr());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        kodmat=atoi(row[0]);
       }
      else
       {
        int metka_nomkm=0;
        class iceb_lock_tables lk("LOCK TABLES Material WRITE,Uslugi READ,icebuser READ");
        if(kodmat != 0)
         {
          /*проверяем нет ли уже другого материала или услуги с этим кодом*/
          sprintf(strsql,"select kodm from Material where kodm=%d",kodmat);
          if(iceb_sql_readkey(strsql,data->window) >= 1)
           {
            kodmat=nomkmw(kod_mat_pred,data->window);
            metka_nomkm=1;
           }
          else
           {
            sprintf(strsql,"select kodus from Uslugi where kodus=%d",kodmat);
            if(iceb_sql_readkey(strsql,data->window) >= 1)
             {
              kodmat=nomkmw(kod_mat_pred,data->window);
              metka_nomkm=1;
             }
           }
         }
        else
         {
          kodmat=nomkmw(kod_mat_pred,data->window);
          metka_nomkm=1;
         }
        sprintf(strsql,"insert into Material (kodm,naimat,ktoz,vrem) values(%d,'%s',%d,%ld)",kodmat,naimat.ravno_filtr(),iceb_getuid(data->window),time(NULL));
        if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
         {
          if(metka_nomkm == 1)
           kod_mat_pred=kodmat;
         }
        else
         koloh++;
       }             
     }

    sprintf(strsql,"select kodm from Material where kodm=%d",kodmat);
    if(sql_readkey(&bd,strsql,&row1,&cur) != 1)
     {
      repl.new_plus(gettext("Не найден код материалу"));
      repl.plus(" ");
      repl.plus(kodmat);
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }
    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    kolih=atof(strsql);
    if(kolih == 0)
     {
      repl.new_plus(gettext("В записи нулевое количество материалла"));
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),4,'|');
    cena=atof(strsql);
    cena=iceb_u_okrug(cena,okrcn);

    iceb_u_polen(strok.ravno(),&eiz,5,'|');
    sprintf(strsql,"select kod from Edizmer where kod='%s'",eiz.ravno());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      repl.new_plus(gettext("Не найдена единица измерения"));
      repl.plus(" ");
      repl.plus(eiz.ravno());
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),6,'|');
    vtara=atoi(strsql);

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),7,'|');
    nds=atof(strsql);

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),8,'|');
    ndsvkl=atoi(strsql);


    iceb_u_polen(strok.ravno(),&shetu,9,'|'); //Для расходных документов не обязательный реквизит
    if(tipz == 1 || (tipz == 2  && shetu.ravno()[0] != '\0'))
      if(iceb_prsh1(shetu.ravno(),&shetv,data->window) != 0)
       {
        koloh++;
        continue;
       }

    iceb_u_polen(strok.ravno(),&nomzak,10,'|');
   
    if(metkaproh != 0 && nomdok.ravno()[0] != '\0')
     {
//      printw("%d.%d.%d %d %d %s %.10g %.10g %s %f %d %d %d\n",
//      dd,md,gd,sklad.ravno_atoi(),kodmat,nomdok.ravno(),kolih,cena,eiz.ravno(),nds,ndsvkl,vtara,tipz);      

      //Проверяем может такой код материалла уже есть в накладной
      if(tipz == 2)
       {
        kodopuc=0;
        if(iceb_u_proverka(kodiopuc.ravno(),kodop.ravno(),0,1) == 0)
           kodopuc=1;  //Списание по учетной цене

        sprintf(strsql,"select kodm from Dokummat1 where tipz=%d and \
  datd >= '%04d-01-01' and datd <= '%04d-12-31' and sklad=%d and nomd='%s' \
  and kodm=%d",tipz,gd,gd,sklad.ravno_atoi(),nomdok.ravno(),kodmat);
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
         {
          naimat.new_plus("");
          sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);
          if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
            naimat.new_plus(row[0]);

          repl.new_plus(gettext("Код материалла"));
          repl.plus(" ");
          repl.plus(kodmat);
          repl.plus(" ");
          repl.plus(naimat.ravno());
          
          repl.ps_plus(gettext("Такая запись в документе уже есть !"));
          repl.ps_plus(gettext("Документ"));
          repl.plus(" N");
          repl.plus(nomdok.ravno());
          iceb_menu_soob(&repl,data->window);
          continue;
         }
       }

      if(tipz == 1)      
       {
        zap_prihodw(dd,md,gd,nomdok.ravno(),sklad.ravno_atoi(),kodmat,kolih,cena,eiz.ravno(),shetu.ravno(),nds,vtara,ndsvkl,nomzak.ravno(),"",data->window);

       }

      if(tipz == 2)
       {
        priv_k_kartw(dd,md,gd,nomdok.ravno(),sklad.ravno_atoi(),kodmat,kolih,cena,shetu.ravno(),eiz.ravno(),nds,ndsvkl,vtara,nomzak.ravno(),kodopuc,"",data->window);
       }
     }
   }

  //Строка записи услуг
  if(iceb_u_SRAV(rek1.ravno(),"ZP2",0) == 0)
   {
    iceb_u_polen(strok.ravno(),&eiz,2,'|');
    sprintf(strsql,"select kod from Edizmer where kod='%s'",eiz.ravno());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      repl.new_plus(gettext("Не найдена единица измерения"));
      repl.plus(" ");
      repl.plus(eiz.ravno());
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }
    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    kolih=atof(strsql);

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),4,'|');
    cena=atof(strsql);
    cena=iceb_u_okrug(cena,okrcn);
    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),5,'|');
    nds=atof(strsql);

    iceb_u_polen(strok.ravno(),&naim,6,'|');

    if(metkaproh != 0 && nomdok.ravno()[0] != '\0')
     {
      sprintf(strsql,"insert into Dokummat3 (datd,sklad,nomd,ei,kolih,cena,nds,naimu,ktoi,vrem,ku)\
values ('%04d-%02d-%02d',%d,'%s','%s',%.10g,%.10g,%.10g,'%s',%d,%ld,%d)",
      gd,md,dd,sklad.ravno_atoi(),nomdok.ravno(),eiz.ravno(),kolih,cena,nds,naim.ravno(),
      iceb_getuid(data->window),tmm,kodusl);
//      printw("%s\n",strsql);
      if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
     }
   }

  //Строка записи дополнительной информации к документа
  if(iceb_u_SRAV(rek1.ravno(),"ZP3",0) == 0)
   {
    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),2,'|');
    int nomz=atoi(strsql);
    if(nomz == 0)
      continue;    
    iceb_u_polen(strok.ravno(),&naim,3,'|');

    if(nomz == 8)
     {
      sprintf(strsql,"select kod from Foroplat where kod='%s'",naim.ravno());
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        repl.new_plus(gettext("Не найдена форма оплаты"));
        repl.plus(" ");
        repl.plus(naim.ravno());
        repl.plus(" !");
        iceb_menu_soob(&repl,data->window);
        koloh++;
        continue;
       }

     }

    if(nomz == 11)
      iceb_u_polen(strok.ravno(),&lnds,3,'|');

    if(metkaproh != 0 && nomdok.ravno()[0] != '\0')
     {
      sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%s')",gd,sklad.ravno_atoi(),nomdok.ravno(),nomz,naim.ravno());
      if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
     }    
   }
 }
iceb_pbar(data->bar,work.st_size,work.st_size);    

fclose(ff);
if(koldok > 0 && metkaproh > 0 && nomdok.ravno()[0] != '\0')
 {
  podtdok1w(dd,md,gd,sklad.ravno_atoi(),nomdok.ravno(),tipz,dd,md,gd,1,data->window);
  podvdokw(dd,md,gd,nomdok.ravno(),sklad.ravno_atoi(),data->window);

  prosprw(1,sklad.ravno_atoi(),dd,md,gd,nomdok.ravno(),tipz,lnds,0,"",kodop.ravno(),&shet_suma,data->window); /*составляем список проводок которые должны быть сделаны*/
  avtpromu1(tipz,dd,md,gd,nomdok.ravno(),NULL,data->window); /*делаем проводки*/
  prosprw(0,sklad.ravno_atoi(),dd,md,gd,nomdok.ravno(),tipz,lnds,0,"",kodop.ravno(),&shet_suma,data->window); /*проверяем сделаны проводки все или нет*/
 }

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

if(koloh != 0)
 {



  repl.new_plus(gettext("Количество ошибок"));
  repl.plus(":");
  repl.plus(koloh);
  
  repl.ps_plus(gettext("Импорт документов осуществляется если нет ни одной ошибки"));
  iceb_menu_soob(&repl,data->window);
  return(FALSE);
 }

if(koldok == 0)
 {

  repl.new_plus(gettext("Не найдено ни одного документа !"));
  iceb_menu_soob(&repl,data->window);


  return(FALSE);
 }

if(metkaproh == 0)
 {
  metkaproh=1;
  goto nazad;
 }


repl.new_plus(gettext("Загрузка завершена"));
repl.ps_plus(gettext("Количество документов"));
repl.plus(":");
repl.plus(koldok);

iceb_menu_soob(&repl,data->window);

data->voz=0;

return(FALSE);
}
