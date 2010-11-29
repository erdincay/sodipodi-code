#ifndef __FORWARD_H__
#define __FORWARD_H__

/*
 * Forward declarations of most used objects
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2001-2002 Lauris Kaplinski
 * Copyright (C) 2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

/* Generic containers */

typedef struct _Sodipodi Sodipodi;
typedef struct _SodipodiClass SodipodiClass;

/* Document */

typedef struct _SPDocument SPDocument;

/* Misc */

typedef struct _SPStyle SPStyle;

/* Objects */

typedef struct _SPObject SPObject;

typedef struct _SPItem SPItem;
typedef struct _SPItemClass SPItemClass;

typedef struct _SPGroup SPGroup;
typedef struct _SPGroupClass SPGroupClass;

typedef struct _SPDefs SPDefs;
typedef struct _SPDefsClass SPDefsClass;

typedef struct _SPRoot SPRoot;
typedef struct _SPRootClass SPRootClass;

typedef struct _SPHeader SPHeader;
typedef struct _SPHeaderClass SPHeaderClass;

typedef struct _SPNamedView SPNamedView;
typedef struct _SPNamedViewClass SPNamedViewClass;

typedef struct _SPObjectGroup SPObjectGroup;
typedef struct _SPObjectGroupClass SPObjectGroupClass;

typedef struct _SPPath SPPath;
typedef struct _SPPathClass SPPathClass;

typedef struct _SPShape SPShape;
typedef struct _SPShapeClass SPShapeClass;

typedef struct _SPEllipse SPEllipse;
typedef struct _SPEllipseClass SPEllipseClass;

typedef struct _SPCircle SPCircle;
typedef struct _SPCircleClass SPCircleClass;

typedef struct _SPArc SPArc;
typedef struct _SPArcClass SPArcClass;

typedef struct _SPChars SPChars;
typedef struct _SPCharsClass SPCharsClass;

typedef struct _SPText SPText;
typedef struct _SPTextClass SPTextClass;

typedef struct _SPTSpan SPTSpan;
typedef struct _SPTSpanClass SPTSpanClass;

typedef struct _SPString SPString;
typedef struct _SPStringClass SPStringClass;

typedef struct _SPPaintServer SPPaintServer;
typedef struct _SPPaintServerClass SPPaintServerClass;

typedef struct _SPStop SPStop;
typedef struct _SPStopClass SPStopClass;

typedef struct _SPGradient SPGradient;
typedef struct _SPGradientClass SPGradientClass;

typedef struct _SPLinearGradient SPLinearGradient;
typedef struct _SPLinearGradientClass SPLinearGradientClass;

typedef struct _SPRadialGradient SPRadialGradient;
typedef struct _SPRadialGradientClass SPRadialGradientClass;

typedef struct _SPPattern SPPattern;

typedef struct _SPMask SPMask;
typedef struct _SPMaskClass SPMaskClass;

typedef struct _SPClipPath SPClipPath;
typedef struct _SPClipPathClass SPClipPathClass;

typedef struct _SPAnchor SPAnchor;
typedef struct _SPAnchorClass SPAnchorClass;

/* Misc */

typedef struct _SPEvent SPEvent;

#endif
