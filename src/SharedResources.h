/**
 * SharedResources.h
 * Resource definitions that should be shared among projects.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_SHARED_RESOURCES_H
#define _BOLOTA_SHARED_RESOURCES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Application
#define IDS_APP_CLASS                   101
#define IDS_APP_TITLE                   102
#define IDI_BOLOTA                      103
#define IDI_SMALL                       104
#define IDM_MAIN                        105
#define IDR_ACCELERATORS                106
#ifndef UNDER_CE
#define IDC_STATIC                      -1
#endif // !UNDER_CE

// File icons.
#define IDI_FILE_95                     130
#define IDI_FILE_XP                     131
#define IDI_FILE_VISTA                  132

// Strings
#define IDS_DOC_DESC                    201
#define IDS_COPYRIGHT                   202
#define IDS_AUTHORWEBSITE               203
#define IDS_TBFILELABELS                204
#define IDS_TBTOPICSLABELS              205

// Menus
#define IDM_BASE                        2000
#define IDM_FILE_NEW                    (IDM_BASE + 1)
#define IDM_FILE_OPEN                   (IDM_BASE + 2)
#define IDM_FILE_RELOAD                 (IDM_BASE + 3)
#define IDM_FILE_SAVE                   (IDM_BASE + 4)
#define IDM_FILE_SAVEAS                 (IDM_BASE + 5)
#define IDM_FILE_PROPERTIES             (IDM_BASE + 6)
#define IDM_FILE_EXIT					(IDM_BASE + 7)
#define IDM_FIELD_EDIT                  (IDM_BASE + 8)
#define IDM_FIELD_DELETE                (IDM_BASE + 9)
#define IDM_FIELD_PREPEND               (IDM_BASE + 10)
#define IDM_FIELD_APPEND                (IDM_BASE + 11)
#define IDM_FIELD_CREATECHILD           (IDM_BASE + 12)
#define IDM_FIELD_MOVEUP                (IDM_BASE + 13)
#define IDM_FIELD_MOVEDOWN              (IDM_BASE + 14)
#define IDM_FIELD_INDENT                (IDM_BASE + 15)
#define IDM_FIELD_DEINDENT              (IDM_BASE + 16)
#define IDM_FIELD_APPENDPARENT          (IDM_BASE + 17)
#define IDM_VIEW_SHOWSUBLINES			(IDM_BASE + 18)
#define IDM_VIEW_SHOWROOTLINES			(IDM_BASE + 19)
#define IDM_VIEW_SHOWBUTTONS			(IDM_BASE + 20)
#define IDM_HELP_ASSOCIATE				(IDM_BASE + 21)
#define IDM_HELP_ABOUT					(IDM_BASE + 22)

// Controls (Main Window)
#define IDC_BASE                        2100
#ifndef IDC_CMDBAR
#define IDC_CMDBAR                      (IDC_BASE + 2)
#endif // !IDC_CMDBAR
#define IDC_TBRFILE                     (IDC_BASE + 3)
#define IDC_TBRTOPICS                   (IDC_BASE + 4)

// Field Manager Dialog
#define IDD_FIELDMAN                    2150
#define IDC_FM_LBLCONTEXT               (IDD_FIELDMAN + 1)
#define IDC_FM_EDTCONTENT               (IDD_FIELDMAN + 2)
#define IDC_FM_CMBTYPE                  (IDD_FIELDMAN + 3)
#define IDC_FM_CMBFIELDICON             (IDD_FIELDMAN + 4)
#define IDC_FM_DTPTIMESTAMP             (IDD_FIELDMAN + 5)
#define IDC_FM_BTNALTOK                 (IDD_FIELDMAN + 6)
#define IDC_FM_LBLICON                  (IDD_FIELDMAN + 7)

// Document Properties Dialog
#define IDD_DOCPROPS                    2200
#define IDC_DP_EDTTITLE                 (IDD_DOCPROPS + 1)
#define IDC_DP_EDTSUBTITLE              (IDD_DOCPROPS + 2)
#define IDC_DP_DTPTIMESTAMP             (IDD_DOCPROPS + 3)

// About Dialog
#define IDD_ABOUTBOX                    2950
#define IDC_AB_LBLAPPNAME               (IDD_ABOUTBOX + 1)
#define IDC_AB_LBLCOPYRIGHT             (IDD_ABOUTBOX + 2)
#define IDC_AB_BTNAUTHORWEB             (IDD_ABOUTBOX + 3)

// Icons and Bitmaps.
#define IDI_BASE                        3000
#define IDI_BLANK                       3000
#define IDB_TBCLASSIC                   3101

// Begin field icons.
#define IDI_FI_BULLET1                  2999
#define IDI_FI_BATTERY                  3001
#define IDI_FI_BOX                      3002
#define IDI_FI_CALENDAR                 3003
#define IDI_FI_CAMERA                   3004
#define IDI_FI_CHECK                    3005
#define IDI_FI_CLIPBOARD                3006
#define IDI_FI_CLOCK                    3007
#define IDI_FI_CPU                      3008
#define IDI_FI_FIND                     3009
#define IDI_FI_FOLDER                   3010
#define IDI_FI_GEAR                     3011
#define IDI_FI_HELP                     3012
#define IDI_FI_HISTORY                  3013
#define IDI_FI_LAPTOP                   3014
#define IDI_FI_LIGHT                    3015
#define IDI_FI_LOVE                     3016
#define IDI_FI_MEN                      3017
#define IDI_FI_MONEY                    3018
#define IDI_FI_MOVIE                    3019
#define IDI_FI_PLUS                     3020
#define IDI_FI_REDO                     3021
#define IDI_FI_REMOVE                   3022
#define IDI_FI_SIGNPOST                 3023
#define IDI_FI_SOUND                    3024
#define IDI_FI_STAR                     3025
#define IDI_FI_STOP                     3026
#define IDI_FI_TAGS                     3027
#define IDI_FI_TRASH                    3028
#define IDI_FI_UNDO                     3029
#define IDI_FI_WOMAN                    3030
#define IDI_FI_WRENCH                   3031
// End of field icons.

#endif // _BOLOTA_SHARED_RESOURCES_H
