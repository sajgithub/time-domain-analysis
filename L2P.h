/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  MAIN_PANEL                       1       /* callback function: OnMainPanel */
#define  MAIN_PANEL_GRAPH_2               2       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_GRAPH_HISTOGRAM       3       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_GRAPH                 4       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_NEXT_BUTTON           5       /* control type: command, callback function: OnNext */
#define  MAIN_PANEL_PREV_BUTTON           6       /* control type: command, callback function: OnPrev */
#define  MAIN_PANEL_ENVELOPE_BTT          7       /* control type: command, callback function: OnEnvelope */
#define  MAIN_PANEL_LOAD_BUTTON           8       /* control type: command, callback function: OnLoad */
#define  MAIN_PANEL_DECORATION            9       /* control type: deco, callback function: (none) */
#define  MAIN_PANEL_BUTTON_MEDIE          10      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BUTTON_KURTOSIS       11      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BUTTON_SKEWNESS       12      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BUTTON_MEDIANA        13      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BUTTON_DISPERSIE      14      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BUTTON_ZERO           15      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BUTTON_MAX            16      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BUTTON_MAXINDEX       17      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BUTTON_MININDEX       18      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BUTTON_MIN            19      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BTT_ALPHA             20      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_BTT_FILTRU            21      /* control type: binary, callback function: (none) */
#define  MAIN_PANEL_TEXTMSG               22      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnEnvelope(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoad(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMainPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNext(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrev(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif