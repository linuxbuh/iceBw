/*$Id: impmatdok_xml_r.c,v 1.32 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	28.01.2015	Белых А.И.	impmatdok_xml_r.c
импорт документов из файла в формате XML
*/
#include        <sys/stat.h>
#include "buhg_g.h"

class impmatdok_xml_r_data
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
  impmatdok_xml_r_data()
   {
    voz=1;
   }
 };
gboolean   impmatdok_xml_r_key_press(GtkWidget *widget,GdkEventKey *event,class impmatdok_xml_r_data *data);
gint impmatdok_xml_r1(class impmatdok_xml_r_data *data);
void  impmatdok_xml_r_v_knopka(GtkWidget *widget,class impmatdok_xml_r_data *data);
int impmatdok_xml_m(struct iceb_imp_xml_nn_r *rkv,GtkWidget *wpredok);

extern SQL_baza bd;
extern double okrcn;

int impmatdok_xml_r(const char *imaf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class impmatdok_xml_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт документов в формате XML"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(impmatdok_xml_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт документов в формате XML"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(impmatdok_xml_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)impmatdok_xml_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impmatdok_xml_r_v_knopka(GtkWidget *widget,class impmatdok_xml_r_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   impmatdok_xml_r_key_press(GtkWidget *widget,GdkEventKey *event,class impmatdok_xml_r_data *data)
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

gint impmatdok_xml_r1(class impmatdok_xml_r_data *data)
{
time_t vremn;
time(&vremn);
class iceb_u_str repl;
struct stat work;
class iceb_u_str stroka("");
class iceb_imp_xml_nn_r rk;
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kodmat=0;
int kodmat_start=1;
double kolih=0.;
double cena=0.;
class iceb_u_str eiz("");
int tipz=1;
class iceb_u_str shet_suma;

if(stat(data->imafz.ravno(),&work) != 0)
 {
  repl.new_plus(gettext("Нет такого файла !"));
  repl.ps_plus(data->imafz.ravno());
  iceb_menu_soob(&repl,data->window);
  goto kon;  
 }

rk.ima_tablic.new_plus("nalnakmu");
if(iceb_imp_xml_nn(data->imafz.ravno(),&rk,data->view,data->buffer,data->window) != 0)
 {
  goto kon;
 }

if(impmatdok_xml_m(&rk,data->window) != 0)
 goto kon;

/*загружаем записи в документ*/
sprintf(strsql,"select * from %s order by nz asc",rk.ima_tablic.ravno());
if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  goto kon;
 }
sprintf(strsql,"%s\n",gettext("Загружаем содержимое документа"));
iceb_printw(strsql,data->buffer,data->view);

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%2s %-*.*s %-*s %2s %10.10g %10.10g %10.10g %10.10g %10.10g %10.10g\n",
  row[0],
  iceb_u_kolbait(20,row[1]),
  iceb_u_kolbait(20,row[1]),
  row[1],
  iceb_u_kolbait(5,row[2]),
  row[2],
  row[3],atof(row[4]),atof(row[5]),atof(row[6]),atof(row[7]),atof(row[8]),atof(row[9]));

  iceb_printw(strsql,data->buffer,data->view);


  /*проверяем есть ли наименование материала в справочнике материалов*/
  sprintf(strsql,"select kodm from Material where naimat='%s'",sql_escape_string(row[1],0));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    kodmat=atoi(row1[0]);
   }
  else
   {
    class iceb_lock_tables lk("LOCK TABLES Material WRITE,Uslugi READ,icebuser READ");

    kodmat=nomkmw(kodmat_start,data->window);

    sprintf(strsql,"insert into Material (kodm,naimat,ktoz,vrem) values(%d,'%s',%d,%ld)",kodmat,sql_escape_string(row[1],0),iceb_getuid(data->window),time(NULL));
    if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
     {
      kodmat_start=kodmat;
     }
    else 
     continue;    
   }

  sprintf(strsql,"select kodm from Material where kodm=%d",kodmat);
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    repl.new_plus(gettext("Не найден код материалу"));
    repl.plus(" ");
    repl.plus(kodmat);
    repl.plus(" !");
    iceb_menu_soob(&repl,data->window);
    continue;
   }

  kolih=atof(row[4]);

  if(kolih == 0)
   {
    repl.new_plus(gettext("В записи нулевое количество материалла"));
    repl.plus(" !");
    iceb_menu_soob(&repl,data->window);
    continue;
   }

  eiz.new_plus(row[2]);
  cena=atof(row[5]);
  
  zap_prihodw(rk.datdok.ravno(),rk.nomdok.ravno(),rk.sklad.ravno_atoi(),kodmat,kolih,cena,eiz.ravno(),rk.shetu.ravno(),rk.pnds,0,rk.nds,"","",data->window);

 }

podtdok1w(rk.datdok.ravno(),rk.sklad.ravno_atoi(),rk.nomdok.ravno(),tipz,rk.datdok.ravno(),1,data->window);
podvdokw(rk.datdok.ravno(),rk.nomdok.ravno(),rk.sklad.ravno_atoi(),data->window);


prosprw(1,rk.sklad.ravno_atoi(),rk.datdok.ravno(),rk.nomdok.ravno(),tipz,rk.nds,0,"",rk.kod_op.ravno(),&shet_suma,data->window); /*составляем список проводок которые должны быть сделаны*/
avtpromu1(tipz,rk.datdok.ravno(),rk.nomdok.ravno(),NULL,data->window); /*делаем проводки*/
prosprw(0,rk.sklad.ravno_atoi(),rk.datdok.ravno(),rk.nomdok.ravno(),tipz,rk.nds,0,"",rk.kod_op.ravno(),&shet_suma,data->window); /*проверяем сделаны проводки все или нет*/

iceb_menu_soob(gettext("Загрузка завершена"),data->window);



kon:;


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

//iceb_printw_vr(vremn,data->buffer,data->view);


data->voz=0;

return(FALSE);
}
