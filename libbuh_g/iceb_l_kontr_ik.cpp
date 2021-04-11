/*$Id: iceb_l_kontr_ik.c,v 1.5 2014/07/31 07:08:29 sasa Exp $*/
/*23.05.2016	21.06.2013	Белых А.И.	iceb_l_kontr_ik.c
импорт контрагентов
*/
#include        <sys/stat.h>
#include "iceb_libbuh.h"

class iceb_l_kontr_ik_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  class iceb_u_str imafz;

  int tipz;
  short dd,md,gd;
  class iceb_u_str kodop;
  class iceb_u_str nomdok;
  int podr;
  int kodol;
    
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  iceb_l_kontr_ik_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   iceb_l_kontr_ik_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontr_ik_data *data);
gint iceb_l_kontr_ik1(class iceb_l_kontr_ik_data *data);
void  iceb_l_kontr_ik_v_knopka(GtkWidget *widget,class iceb_l_kontr_ik_data *data);

extern SQL_baza bd;
extern iceb_u_str shrift_ravnohir;

int iceb_l_kontr_ik(const char *imaf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class iceb_l_kontr_ik_data data;

data.imafz.new_plus(imaf);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт контрагентов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_l_kontr_ik_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт контрагентов"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(iceb_l_kontr_ik_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)iceb_l_kontr_ik1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_l_kontr_ik_v_knopka(GtkWidget *widget,class iceb_l_kontr_ik_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_l_kontr_ik_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontr_ik_data *data)
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

gint iceb_l_kontr_ik1(class iceb_l_kontr_ik_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_u_str repl;
struct stat work;

if(stat(data->imafz.ravno(),&work) != 0)
 {
  repl.new_plus(gettext("Нет такого файла !"));
  repl.ps_plus(data->imafz.ravno());
  iceb_menu_soob(&repl,data->window);
  return(FALSE);
 }


class iceb_u_str strok("");

class iceb_fopen filin;
if(filin.start(data->imafz.ravno(),"r",data->window) != 0)
  return(FALSE);

class iceb_fopen filtmp;
char imafprom[64];
sprintf(imafprom,"prom%d.tmp",getpid());
if(filtmp.start(imafprom,"w",data->window) != 0)
  return(FALSE);

int	ktoi=iceb_getuid(data->window);
time_t  vrem;
time(&vrem);
#define KOLKOL 17
class iceb_u_str rek[KOLKOL];
int kolihoh=0;
int nom=0;
SQL_str row;
class SQLCURSOR cur;
int metkar=0;
int i=0;
int razmer=0;
int nomer_kontr=1;
while(iceb_u_fgets(&strok,filin.ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;
   
  i=strlen(strok.ravno());
  razmer+=i;

  iceb_printw(strok.ravno(),data->buffer,data->view);




  if(metkar == 1)
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    continue;
   }
  
  for(nom=0; nom < KOLKOL; nom++)
   if(iceb_u_polen(strok.ravno(),&rek[nom],nom+1,'|') != 0)
    {
     fprintf(filtmp.ff,"%s",strok.ravno());
     fprintf(filtmp.ff,"#%s %d\n",gettext("Не найдено поле"),nom+1);
     kolihoh++;
     break;     
    }

  if(nom < KOLKOL)
   continue;


  /*проверяем может такой контрагент уже есть*/
  sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",rek[1].ravno_filtr());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s! %s %s!\n",gettext("Контрагент с таким коротким наименованием уже есть"),gettext("Код"),row[0]);
    kolihoh++;
    continue;

   }


  if(iceb_u_SRAV(rek[0].ravno(),"авто",0) == 0 || rek[0].ravno()[0] == '\0')
   {
    rek[0].new_plus(iceb_getnkontr(nomer_kontr,data->window));
    nomer_kontr=rek[0].ravno_atoi();
   }
  else
   {
    /*Проверяем может код контрагента уже занят - проверка наименования сделана выше*/
    sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",rek[0].ravno_filtr());
    if(iceb_sql_readkey(strsql,data->window) > 0)
     {
      rek[0].new_plus(iceb_getnkontr(nomer_kontr,data->window));
      nomer_kontr=rek[0].ravno_atoi();

     }     
   }  

  sprintf(strsql,"insert into Kontragent (kodkon,naikon,naiban,adres,adresb,kod,mfo,nomsh,innn,nspnds,telef,grup,ktoz,vrem,na,regnom,pnaim,gk,en) \
values ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',\
%d,%ld,'%s','%s','%s','%s',%d)",
  rek[0].ravno_filtr(),
  rek[1].ravno_filtr(),
  rek[2].ravno_filtr(),
  rek[3].ravno_filtr(),
  rek[4].ravno_filtr(),
  rek[5].ravno_filtr(),
  rek[6].ravno_filtr(),
  rek[7].ravno_filtr(),
  rek[8].ravno_filtr(),
  rek[9].ravno_filtr(),
  rek[10].ravno_filtr(),
  rek[11].ravno_filtr(),
  ktoi,vrem,
  rek[12].ravno_filtr(),
  rek[13].ravno_filtr(),
  rek[14].ravno_filtr(),
  rek[15].ravno_filtr(),
  rek[16].ravno_atoi());

  if(sql_zap(&bd,strsql) != 0)
   {
    i=sql_nerror(&bd);
    if(i == ER_DUP_ENTRY) //Запись уже есть
     {
      fprintf(filtmp.ff,"#%s\n",gettext("Такая запись уже есть !"));
      fprintf(filtmp.ff,"%s",strok.ravno());
      kolihoh++;
      continue;
     }

    if(i == ER_DBACCESS_DENIED_ERROR) //Толко чтение
     {
      iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),data->window);
      fprintf(filtmp.ff,"%s",strok.ravno());
      metkar=1;
      continue;
     }
    fprintf(filtmp.ff,"%s",strok.ravno());
    iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
    kolihoh++;
   }
 }
filin.end();
filtmp.end();
unlink(data->imafz.ravno());

rename(imafprom,data->imafz.ravno());

if(kolihoh != 0)
 {
  sprintf(strsql,"%s %d",gettext("Количество не загруженых записей"),kolihoh);
  iceb_menu_soob(strsql,data->window);
 }


data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Импорт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);


repl.new_plus(gettext("Загрузка завершена"));

iceb_menu_soob(&repl,data->window);

data->voz=0;

return(FALSE);
}
