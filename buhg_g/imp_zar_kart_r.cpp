/*$Id: imp_zar_kart_r.c,v 1.19 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	24.10.2006 Белых А.И.  imp_zar_kart_r.c
импорт списка работников
только фио и идентификационный номер
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class imp_zar_kart_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  class iceb_u_str imafz;
        
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  imp_zar_kart_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_zar_kart_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_zar_kart_r_data *data);
gint imp_zar_kart_r1(class imp_zar_kart_r_data *data);
void  imp_zar_kart_r_v_knopka(GtkWidget *widget,class imp_zar_kart_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int imp_zar_kart_r(const char *imaf,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
class imp_zar_kart_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт карточек из файла"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(imp_zar_kart_r_key_press),&data);

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

label=gtk_label_new(gettext("Импорт карточек из файла"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(imp_zar_kart_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)imp_zar_kart_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_zar_kart_r_v_knopka(GtkWidget *widget,class imp_zar_kart_r_data *data)
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

gboolean   imp_zar_kart_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_zar_kart_r_data *data)
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

gint imp_zar_kart_r1(class imp_zar_kart_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
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



FILE *ff;

if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafprom[64];
FILE *ffprom;
sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  iceb_er_op_fil(imafprom,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_str tabnom("");
class iceb_u_str fam("");
class iceb_u_str imq("");
class iceb_u_str oth("");
class iceb_u_str inn("");
class iceb_u_str fio("");

int  kolkart=0;
float razmer=0.;
class iceb_u_str strok("");
SQL_str row;
class SQLCURSOR cur;

while(iceb_u_fgets(&strok,ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;
   
  razmer+=strlen(strok.ravno());

  iceb_printw(strok.ravno(),data->buffer,data->view);
 
  iceb_pbar(data->bar,work.st_size,razmer);    

  if(iceb_u_polen(strok.ravno(),&tabnom,1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(tabnom.ravno_atoi() == 0)
   {
    iceb_menu_soob(gettext("Табельный номер не может быть равен нолю !"),data->window);
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Проверяем на отсутсвие такого табельного номера в базе данных
  sprintf(strsql,"select fio from Kartb where tabn=%s",tabnom.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
   {
    class iceb_u_str repl;
    
    sprintf(strsql,gettext("Табельный номер %s уже введён !"),tabnom.ravno());
    repl.plus(strsql);
    repl.ps_plus(row[0]);
    iceb_menu_soob(&repl,data->window);
    fprintf(ffprom,"%s\n#%s\n",strok.ravno(),strsql);
    continue;
   } 

  if(iceb_u_polen(strok.ravno(),&fam,2,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(iceb_u_polen(strok.ravno(),&imq,3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(iceb_u_polen(strok.ravno(),&oth,4,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(iceb_u_polen(strok.ravno(),&inn,5,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Проверяем на отсутствие индивидуального налогового номера
  if(inn.ravno()[0] != 0)
   {
    sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",inn.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
     {
      class iceb_u_str repl;
      repl.plus(gettext("Такой идентификационный код уже введён !"));

      sprintf(strsql,"%s %s",row[0],row[1]);
      repl.ps_plus(strsql);

      fprintf(ffprom,"%s\n#%s:%s\n",strok.ravno(),
      gettext("Такой индентификационный код уже введён !"),
      strsql);
      continue;
     } 
   }

//  sprintf(fio,"%s %s %s",fam.ravno(),imq.ravno(),oth.ravno());
  fio.new_plus(fam.ravno());
  fio.plus(imq.ravno(),oth.ravno());
//  sqlfiltr(fio,sizeof(fio));
    
  sprintf(strsql,"insert into Kartb set tabn=%s, fio='%s', inn='%s'",tabnom.ravno(),fio.ravno_filtr(),inn.ravno());
  
  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
    continue;
  kolkart++;  
 }
iceb_pbar(data->bar,work.st_size,work.st_size);    

fclose(ff);
fclose(ffprom);
unlink(data->imafz.ravno());

rename(imafprom,data->imafz.ravno());


class iceb_u_str repl;

repl.new_plus(gettext("Загрузка завершена"));
repl.ps_plus(gettext("Количество загруженых карточек"));
repl.plus(":");
repl.plus(kolkart);

iceb_menu_soob(&repl,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);
}
