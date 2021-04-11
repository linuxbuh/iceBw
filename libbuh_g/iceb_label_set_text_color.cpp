/*$Id:$*/
/*07.05.2015	07.05.2015	Белых А.И.	iceb_label_set_text_color.c
Установить текст соотвецтвующего цвета

Тег <span> предназначен для определения строчных элементов документа. 
В отличие от блочных элементов, таких как <table>, <p> или <div>,
с помощью тега <span> можно выделить часть информации внутри других
тегов и установить для нее свой стиль. 
Например, внутри абзаца (тега <p>) можно изменить цвет и размер первой буквы,
если добавить начальный и конечный тег <span> и определить для него стиль текста.
Чтобы не описывать каждый раз стиль внутри тега, можно выделить стиль во 
внешнюю таблицу стилей, а для тега добавить атрибут class или id с именем селектора.

*/
#include "iceb_libbuh.h"

void iceb_label_set_text_color(GtkWidget *label,const char *text,const char *cvet)
{
if(label == NULL)
 return;
if(text == NULL)
 return;
if(cvet == NULL)
 return;
if(cvet[0] == '\0')
 return;

class iceb_u_str text_out("<span color=\"");
text_out.plus(cvet);

text_out.plus("\">");
text_out.plus(text);
text_out.plus("</span>");
gtk_label_set_text(GTK_LABEL(label),text_out.ravno());
gtk_label_set_use_markup (GTK_LABEL (label), TRUE);/*Включает отработку разметки текста*/

}