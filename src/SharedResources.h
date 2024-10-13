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

// Menus
#define IDM_BASE                        2000
#define IDM_FILE_NEW                    (IDM_BASE + 1)
#define IDM_FILE_OPEN                   (IDM_BASE + 2)
#define IDM_FILE_SAVE                   (IDM_BASE + 3)
#define IDM_FILE_SAVEAS                 (IDM_BASE + 4)
#define IDM_FIELD_EDIT                  (IDM_BASE + 5)
#define IDM_FIELD_DELETE                (IDM_BASE + 6)
#define IDM_FIELD_PREPEND               (IDM_BASE + 7)
#define IDM_FIELD_APPEND                (IDM_BASE + 8)
#define IDM_FIELD_CREATECHILD           (IDM_BASE + 9)
#define IDM_FIELD_MOVEUP                (IDM_BASE + 10)
#define IDM_FIELD_MOVEDOWN              (IDM_BASE + 11)

// Controls (Main Window)
#define IDC_BASE                        2100

// Field Manager Dialog
#define IDD_FIELDMAN                    2200
#define IDC_FM_LBLCONTEXT               (IDD_FIELDMAN + 1)
#define IDC_FM_EDTCONTENT               (IDD_FIELDMAN + 2)
#define IDC_FM_BTNALTOK                 (IDD_FIELDMAN + 3)

#endif // _BOLOTA_SHARED_RESOURCES_H
