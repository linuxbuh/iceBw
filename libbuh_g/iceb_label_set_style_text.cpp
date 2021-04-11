/*$Id:$*/
/*24.05.2016	24.05.2016	Белых А.И.	iceb_label_set_style_text.c
Вставить текст с нужным стилем

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

void iceb_label_set_style_text(GtkWidget *label,const char *style_text,const char *text)
{
if(text[0] == '\0')
 return;
if(style_text[0] == '\0')
 return;
if(text == NULL)
 return;
if(style_text == NULL)
 return;
 
class iceb_u_str text_out("<span ");

text_out.plus(style_text);

text_out.plus(">");

text_out.plus(text);

text_out.plus("</span>");

gtk_label_set_text(GTK_LABEL(label),text_out.ravno());
gtk_label_set_use_markup (GTK_LABEL (label), TRUE);/*Включает отработку разметки текста*/

}
