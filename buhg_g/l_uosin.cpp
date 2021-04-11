/*$Id: l_uosin.c,v 1.23 2013/09/26 09:46:51 sasa Exp $*/
/*26.10.2020	20.12.2007	Белых А.И.	l_uosin.c
Просмотр списка инвентарных номеров
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "l_uosin.h"
enum
{
 FK2,
 FK3,
 FK4,
 FK5,
 FK10,
 KOL_F_KL
};

enum
{
 COL_INNOM,
 COL_NAIM,
 COL_OSNU,
 COL_OSBU,
 COL_NALIH,
 COL_ZAV_NOMER,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  l_uosin_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  class iceb_u_str name_window;
  
  int     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  

  class l_uosin_poi poi;
  class iceb_u_str zapros;

  class iceb_u_str innom_v; //Выбранный код 
  class iceb_u_str naimv; //Наименование выбранной записи
  int metka_rr;
  int metka_rp;
  //Конструктор
  l_uosin_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    metka_rp=0;
   }      
 };

gboolean   l_uosin_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosin_data *data);
void l_uosin_vibor(GtkTreeSelection *selection,class l_uosin_data *data);
void l_uosin_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class l_uosin_data *data);
void  l_uosin_knopka(GtkWidget *widget,class l_uosin_data *data);
void l_uosin_add_columns (GtkTreeView *treeview);
void l_uosin_create_list(class l_uosin_data *data);

void l_uosin_rs(class l_uosin_data *data);
int l_uosin_p(class l_uosin_poi *rek_poi,GtkWidget *wpredok);

extern SQL_baza	bd;
 
int l_uosin(int metka, //0-воод и корректировка 1-выбор
class iceb_u_str *innom,
class iceb_u_str *naimk,
GtkWidget *wpredok)
{
int gor=0;
int ver=0;

class l_uosin_data data;
char bros[512];
data.metka_rr=metka;
data.name_window.plus(__FUNCTION__);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr == 1)
 {
  if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
 }


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список инвентарных номеров"));

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uosin_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список инвентарных номеров"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки


sprintf(bros,"F2 %s",gettext("Просмотр"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_uosin_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Просмотр карточки инвентарного номера"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);


sprintf(bros,"F3 %s",gettext("Наличие"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_uosin_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Переключение режима просмотра (все,только в наличии,только списанные)"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_uosin_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_uosin_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_uosin_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_uosin_create_list(&data);
gtk_widget_show(data.window);

if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
if(data.metka_voz == 0)
 if(metka == 1)
  {
   innom->new_plus(data.innom_v.ravno());
   naimk->new_plus(data.naimv.ravno());
  }
return(data.metka_voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_uosin_knopka(GtkWidget *widget,class l_uosin_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("uosin__knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается
short d,m,g;
switch (knop)
 {
  case FK2:
      iceb_u_poltekdat(&d,&m,&g);
      uoskart2(data->innom_v.ravno_atoi(),d,m,g,data->window);    
      return;  

  case FK3:
    data->metka_rp++;
    if(data->metka_rp >= 3)
     data->metka_rp=0;
         
    l_uosin_create_list(data);
    return;
    
  case FK4:
    l_uosin_p(&data->poi,data->window);
    l_uosin_create_list(data);
    return;  

  case FK5:
    l_uosin_rs(data);
    return;  

    
  case FK10:
    if(data->metka_rr == 1)
      iceb_sizww(data->name_window.ravno(),data->window);
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosin_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosin_data *data)
{

switch(event->keyval)
 {
  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);


  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("l_uosin_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/***************************/
/*Проверка условий поиска*/
/***************************/

int  l_uosin_prov_row(SQL_str row,class l_uosin_data *data)
{
short		mm;
int		in;
int		poddz,kodotl;

if(data->poi.metka_poi == 0)
 return(0);
 
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
if(iceb_u_proverka(data->poi.naim.ravno(),row[2],4,0) != 0)
 return(1);

if(data->poi.zavod_iz.getdlinna() > 1)
 if(iceb_u_strstrm(row[3],data->poi.zavod_iz.ravno()) == 0)
  return(2);

if(data->poi.pasport.getdlinna() > 1)
 if(iceb_u_strstrm(row[4],data->poi.pasport.ravno()) == 0)
  return(3);

if(data->poi.model.getdlinna() > 1)
 if(iceb_u_strstrm(row[5],data->poi.model.ravno()) == 0)
  return(4);

if(data->poi.god_iz.getdlinna() > 1)
 if(iceb_u_strstrm(row[2],data->poi.god_iz.ravno()) == 0)
  return(5);

if(data->poi.zav_nomer.getdlinna() > 1)
 if(iceb_u_strstrm(row[6],data->poi.zav_nomer.ravno()) == 0)
  return(6);

if(data->poi.data_vvoda.getdlinna() > 1)
 if(iceb_u_sravmydat(data->poi.data_vvoda.ravno(),iceb_u_datzap(row[7])) != 0)
  return(7);

if(data->poi.shetu.getdlinna() > 1 || data->poi.hzt.getdlinna() > 1 || data->poi.hau.getdlinna() > 1 ||\
data->poi.hna.getdlinna() > 1 || data->poi.hnabu.getdlinna() > 1 || data->poi.popkf_nu.getdlinna() > 1  ||\
data->poi.popkf_bu.getdlinna() > 1 || data->poi.nom_znak.getdlinna() > 1)
 {
  in=atoi(row[0]);
  class poiinpdw_data rekin;
  if((mm=poiinpdw(in,mt,gt,&rekin,data->window)) != 0)
   {
    char strsql[100];
    sprintf(strsql,"Ошибка поиска переменных данных %d для %s !\n",mm,row[0]);
    iceb_menu_soob(strsql,data->window);
   }
  else
   {
    if(iceb_u_proverka(data->poi.shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
     return(8);

    if(iceb_u_proverka(data->poi.hzt.ravno(),rekin.hzt.ravno(),0,0) != 0)
     return(9);

    if(iceb_u_proverka(data->poi.hau.ravno(),rekin.hau.ravno(),0,0) != 0)
     return(10);

    if(iceb_u_proverka(data->poi.hna.ravno(),rekin.hna.ravno(),0,0) != 0)
     return(11);

    if(iceb_u_proverka(data->poi.hnabu.ravno(),rekin.hnaby.ravno(),0,0) != 0)
     return(11);

    if(data->poi.popkf_nu.getdlinna() > 1)
     if(fabs(data->poi.popkf_nu.ravno_atof()) - rekin.popkf > 0.00009)
      return(12);

    if(data->poi.popkf_bu.getdlinna() > 1)
     if(fabs(data->poi.popkf_bu.ravno_atof()) - rekin.popkfby > 0.00009)
      return(13);
    if(data->poi.nom_znak.getdlinna() > 1)
     if(iceb_u_strstrm(rekin.nomz.ravno(),data->poi.nom_znak.ravno()) == 0)
     return(14);

   }
 }

if(data->poi.podr.getdlinna() > 1 || data->poi.mat_ot.getdlinna() > 1)
 {
  in=atoi(row[0]);

  poiinw(in,dt,mt,gt,&poddz,&kodotl,data->window);
  
  if(iceb_u_proverka(data->poi.mat_ot.ravno(),kodotl,0,0) != 0)
   return(14);

  if(iceb_u_proverka(data->poi.podr.ravno(),poddz,0,0) != 0)
   return(15);
 } 
return(0);
}
/************************/
/*проверка показа записи*/
/************************/
int diros_prov(class iceb_u_str *metka_sp,class l_uosin_data *data,SQL_str row,short mt,short gt,int *podr,GtkWidget *wpredok)
{
if(data->metka_rr == 1)
 if(l_uosin_prov_row(row,data) != 0)
  return(1);

int kodotl=0;
int kod=0;

if((kod=poiinw(atoi(row[0]),1,mt,gt,podr,&kodotl,wpredok)) == 2)
 metka_sp->new_plus(gettext("Списан"));
else
 metka_sp->new_plus("");

if(data->metka_rp == 0)
  return(0);

if(data->metka_rp == 1 && kod == 2)
  return(1);

if(data->metka_rp == 2 && kod != 2)
  return(1);
  
return(0);
}

/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_uosin_create_list (class l_uosin_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
class iceb_u_str metka_sp("");
SQL_str row;
class iceb_u_str zagolov;
short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);
int podd=0;
class bsizw_data bal_st;

//printf("l_uosin_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_uosin_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_uosin_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_INT, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

sprintf(strsql,"select * from Uosin order by innom asc");

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

char osnu[64];
char osbu[64];

data->kolzap=0;

float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//   if(l_uosin_prov_row(row,data) != 0)
  if(diros_prov(&metka_sp,data,row,mt,gt,&podd,data->window) != 0)
    continue;

//  printf("%s-%s/%s\n",__FUNCTION__,iceb_u_vremzap(row[9]),row[9]);
  bal_st.clear();

  if(metka_sp.getdlinna() <= 1)
   bsizw(atoi(row[0]),podd,1,mt,gt,&bal_st,NULL,data->window);
  
  sprintf(osnu,"%.2f",bal_st.osnu);
  sprintf(osbu,"%.2f",bal_st.osbu);
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_INNOM,atoi(row[0]),
  COL_NAIM,row[2],
  COL_OSNU,osnu,
  COL_OSBU,osbu,
  COL_ZAV_NOMER,row[6],
  COL_NALIH,metka_sp.ravno(),
  COL_DATA_VREM,iceb_u_vremzap(row[9]),
  
  COL_KTO,iceb_kszap(row[8],data->window),

  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_uosin_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

if(data->poi.metka_poi == 1 )
 {

// printf("Формирование заголовка с реквизитами поиска.\n");
  class iceb_u_str strpoi;  

  strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->poi.naim.ravno(),gettext("Наименование"));
  iceb_str_poisk(&strpoi,data->poi.zavod_iz.ravno(),gettext("Завод изготовитель"));
  iceb_str_poisk(&strpoi,data->poi.pasport.ravno(),gettext("Паспорт"));
  iceb_str_poisk(&strpoi,data->poi.model.ravno(),gettext("Модель"));
  iceb_str_poisk(&strpoi,data->poi.god_iz.ravno(),gettext("Год изготовления"));
  iceb_str_poisk(&strpoi,data->poi.zav_nomer.ravno(),gettext("Заводской номер"));
  iceb_str_poisk(&strpoi,data->poi.data_vvoda.ravno(),gettext("Дата ввода в эксплуатацию"));
  iceb_str_poisk(&strpoi,data->poi.shetu.ravno(),gettext("Счёт учёта"));
  iceb_str_poisk(&strpoi,data->poi.hzt.ravno(),gettext("Шифр затрат аморт-отчислений"));
  iceb_str_poisk(&strpoi,data->poi.hau.ravno(),gettext("Шифр аналитического учёта"));
  iceb_str_poisk(&strpoi,data->poi.hna.ravno(),gettext("Группа налогового учёта"));
  iceb_str_poisk(&strpoi,data->poi.hnabu.ravno(),gettext("Группа бух.учёта"));
  iceb_str_poisk(&strpoi,data->poi.popkf_nu.ravno(),gettext("Поправочный коэффициент налогового учёта"));
  iceb_str_poisk(&strpoi,data->poi.popkf_bu.ravno(),gettext("Поправочный коэффициент бух. учёта"));
  iceb_str_poisk(&strpoi,data->poi.nom_znak.ravno(),gettext("Гос. номерной знак автомобиля"));
  iceb_str_poisk(&strpoi,data->poi.mat_ot.ravno(),gettext("Код материально-ответсвенного"));
  iceb_str_poisk(&strpoi,data->poi.podr.ravno(),gettext("Подразделение"));

  iceb_label_set_text_color(data->label_poisk,strpoi.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 {
  gtk_widget_hide(data->label_poisk); 
 }

zagolov.new_plus(gettext("Список инвентарных номеров"));

sprintf(strsql,". %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

if(data->metka_rp == 1)
  zagolov.plus(" ",gettext("Только в наличии"));

if(data->metka_rp == 2)
  zagolov.plus(" ",gettext("Только списанные"));

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void l_uosin_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;

//printf("l_uosin_add_columns\n");

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Инв-ный номер"),renderer,"text",COL_INNOM,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_INNOM);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Наименование"),renderer,"text",COL_NAIM,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NAIM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Ост. стоимость нал."),renderer,"text",COL_OSNU,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_OSNU);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Ост. стоимость бух."),renderer,"text",COL_OSBU,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_OSBU);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Наличие"),renderer,"text",COL_NALIH,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NALIH);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Заводской номер"),renderer,"text",COL_ZAV_NOMER,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_ZAV_NOMER);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer,"text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer,"text",COL_KTO,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KTO);
gtk_tree_view_append_column (treeview, column);

//printf("l_uosin_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void l_uosin_vibor(GtkTreeSelection *selection,class l_uosin_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gint kod;
gchar *naim;
gint  nomer;

gtk_tree_model_get(model,&iter,COL_INNOM,&kod,COL_NAIM,&naim,NUM_COLUMNS,&nomer,-1);

data->innom_v.new_plus(kod);
data->naimv.new_plus(naim);
data->snanomer=nomer;

g_free(naim);


}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void l_uosin_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uosin_data *data)
{

if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[FK2],"clicked");
  return;
 }

iceb_sizww(data->name_window.ravno(),data->window);

gtk_widget_destroy(data->window);

data->metka_voz=0;

}
/**********************/
/*Распечатка списка*/
/*********************/
void l_uosin_rs(class l_uosin_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

char            imaf[64];
SQL_str         row;
float		kolstr1=0.;
int		poddz,kodotl;
int		in;
char		strsql[512];
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

class SQLCURSOR cur;

int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }



class iceb_u_file fil;

sprintf(imaf,"uosp%d.lst",getpid());
if((fil.ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }


iceb_zagolov(gettext("Список инвентарных номеров"),0,0,0,0,0,0,fil.ff,data->window);

if(data->metka_rp == 1)
 fprintf(fil.ff,"%s\n",gettext("Только в наличии"));
if(data->metka_rp == 2)
 fprintf(fil.ff,"%s\n",gettext("Списанные"));

if(data->poi.naim.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Наименование"),data->poi.naim.ravno());
if(data->poi.zavod_iz.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Завод"),data->poi.zavod_iz.ravno());
if(data->poi.pasport.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Паспорт/чертеж N"),data->poi.pasport.ravno());


if(data->poi.model.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Модель/тип/марка"),data->poi.model.ravno());
if(data->poi.god_iz.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Год выпуска"),data->poi.god_iz.ravno());
if(data->poi.zav_nomer.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Заводской номер"),data->poi.zav_nomer.ravno());
if(data->poi.data_vvoda.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Дата ввода в эксплуатацию"),data->poi.data_vvoda.ravno());
if(data->poi.shetu.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Счёт учета"),data->poi.shetu.ravno());
if(data->poi.hzt.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",
 gettext("Шифр затрат амортизационных отчислений"),data->poi.hzt.ravno());
if(data->poi.hau.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Шифр аналитического учета"),data->poi.hau.ravno());
if(data->poi.hna.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Группа налогового учёта"),data->poi.hna.ravno());
if(data->poi.hnabu.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Группа бух.учёта"),data->poi.hnabu.ravno());
if(data->poi.popkf_nu.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Поправочний коэффициент налогового учёта"),data->poi.popkf_nu.ravno());
if(data->poi.popkf_bu.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Поправочний коэффициент бух.учёта"),data->poi.popkf_bu.ravno());
if(data->poi.nom_znak.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Гос. номерной знак"),data->poi.nom_znak.ravno());
if(data->poi.mat_ot.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Код ответственного лица"),data->poi.mat_ot.ravno());
if(data->poi.podr.getdlinna() > 1)
 fprintf(fil.ff,"%s: %s\n",gettext("Подразделение"),data->poi.podr.ravno());

fprintf(fil.ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(fil.ff,gettext("\
Инв. номер|         Наименование                   |Дата ввод.|Заводс.ном.|Гос. номер |Бал.стоим.|  Износ   |Бал.стоим.|  Износ   |Материально|\n\
          |                                        |в эксплуа.|           |           |Налог.учёт|Налог.учёт|Бухг. учёт|Бухг.учёт |отвец. лицо|\n"));
/*
1234567890 1234567890123456789012345678901234567890 1234567890 12345678901 12345678901 1234567890 1234567890 1234567890 1234567890 12345678901
*/
fprintf(fil.ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
double bal_stoim_bu=0.;
double iznos_bu=0.;
double bal_stoim_nu=0.;
double iznos_nu=0.;
double itogo[4];
memset(&itogo,'\0',sizeof(itogo));
short dve,mve,gve;
class iceb_u_str mat_otv("");
class iceb_u_str fio_mo("");
SQL_str row1;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  if(l_uosin_prov_row(row,data) != 0)
    continue;


  in=atoi(row[0]);

  bal_stoim_bu=0.;
  iznos_bu=0.;
  bal_stoim_nu=0.;
  iznos_nu=0.;



  if(poiinw(in,dt,mt,gt,&poddz,&kodotl,data->window) == 0)
   {
    class bsizw_data bal_st;
    bsizw(in,poddz,dt,mt,gt,&bal_st,NULL,data->window);

    bal_stoim_nu=bal_st.sbs+bal_st.bs;
    iznos_nu=bal_st.siz+bal_st.iz+bal_st.iz1;
   
    bal_stoim_bu=bal_st.sbsby+bal_st.bsby;
    iznos_bu=bal_st.sizby+bal_st.izby+bal_st.iz1by;

    itogo[0]+=bal_stoim_nu;
    itogo[1]+=iznos_nu;
     
    itogo[2]+=bal_stoim_bu;
    itogo[3]+=iznos_bu;
     
   } 
  class poiinpdw_data rekin; 
  poiinpdw(in,mt,gt,&rekin,data->window);
  
  iceb_u_rsdat(&dve,&mve,&gve,row[7],2);

  sprintf(strsql,"select naik from Uosol where kod=%d",kodotl);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   fio_mo.new_plus(row1[0]);
  else
   fio_mo.new_plus("");   
//  sprintf(mat_otv,"%d %s",kodotl,fio_mo);
  mat_otv.new_plus(kodotl);
  mat_otv.plus(" ",fio_mo.ravno());

  fprintf(fil.ff,"%-10d %-*s %02d.%02d.%d %-*.*s %-*.*s %10.2f %10.2f %10.2f %10.2f %-*.*s\n",
  in,
  iceb_u_kolbait(40,row[2]),row[2],
  dve,mve,gve,
  iceb_u_kolbait(11,row[6]),iceb_u_kolbait(11,row[6]),row[6],
  iceb_u_kolbait(11,rekin.nomz.ravno()),iceb_u_kolbait(11,rekin.nomz.ravno()),rekin.nomz.ravno(),
  bal_stoim_nu,iznos_nu,bal_stoim_bu,iznos_bu,
  iceb_u_kolbait(11,mat_otv.ravno()),iceb_u_kolbait(11,mat_otv.ravno()),mat_otv.ravno());

 }
fprintf(fil.ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(fil.ff,"%-10s %-40s %10s %-10s %*s %10.2f %10.2f %10.2f %10.2f\n",
" "," "," "," ",
iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),
itogo[0],itogo[1],itogo[2],itogo[3]);

iceb_podpis(fil.ff,data->window);


fil.close();


class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf);
naimf.plus(gettext("Список инвентарных номеров"));

iceb_ustpeh(imafil.ravno(0),3,data->window);

iceb_rabfil(&imafil,&naimf,data->window);
}
