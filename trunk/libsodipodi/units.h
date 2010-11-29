#ifndef __SP_UNIT_H__
#define __SP_UNIT_H__

/*
 * SPUnit
 *
 * Ported from libgnomeprint
 *
 * Authors:
 *   Dirk Luetjens <dirk@luedi.oche.de>
 *   Yves Arrouye <Yves.Arrouye@marin.fdn.fr>
 *   Lauris Kaplinski <lauris@ximian.com>
 *
 * Copyright 1999-2001 Ximian, Inc. and authors
 * Copyright 2002-2010 Lauris Kaplinski
 *
 */

#define SP_UNIT_VERSION_0 0

typedef struct _SPUnit SPUnit;
typedef struct _SPDistance SPDistance;

/*
 *
 * Unit bases define set of mutually unrelated measuring systems (numbers,
 * paper, screen and dimesionless user coordinates). Still, you can convert
 * between those, specifying scaling factors explicitly.
 *
 * Paper (i.e. output) coordinates are taken as absolute real world units.
 * It has some justification, because screen unit (pixel) size changes,
 * if you change screen resolution, while you cannot change output on paper
 * as easily (unless you have thermally contracting paper, of course).
 *
 * The base absolute unit is 1/72th of an inch (points) for now
 *
 * Please notice that this is different from SVG coordinate system because
 * we make distinction between userspace and absolute coordinates. So to
 * convert SVG distances, you have to treat userspace as absolute base.
 */

/* Unit bases */

enum {
	/* For percentages and like */
	SP_UNIT_BASE_DIMENSIONLESS = (1 << 0),
	/* Real world distances - i.e. mm, cm... */
	SP_UNIT_BASE_ABSOLUTE = (1 << 1),
	/* Semi-real device-dependent distances i.e. pixels */
	SP_UNIT_BASE_DEVICE = (1 << 2),
	/* Relative to coordinate system */
	SP_UNIT_BASE_USERSPACE = (1 << 3),
	/* em and ex */
	SP_UNIT_BASE_VOLATILE = (1 << 4)
};

#define SP_UNITS_BASES_ALL (SP_UNIT_DIMENSIONLESS | SP_UNIT_ABSOLUTE | SP_UNIT_DEVICE | SP_UNIT_USERSPACE | SP_UNIT_VOLATILE)

/* Well-known units */

enum {
	SP_UNIT_INVALID,
	SP_UNIT_UNKNOWN,
	SP_UNIT_NONE,
	SP_UNIT_PERCENT,
	SP_UNIT_MM,
	SP_UNIT_CM,
	SP_UNIT_M,
	SP_UNIT_INCH,
	SP_UNIT_POINT,
	SP_UNIT_PIXEL,
	SP_UNIT_USERSPACE,
	SP_UNIT_EM,
	SP_UNIT_EX,
	SP_UNIT_OTHER
};

/*
 * Notice, that for correct menus etc. you have to use
 * ngettext method family yourself. For that reason we
 * do not provide translations in unit names.
 * I also do not know, whether to allow user-created units,
 * because this would certainly confuse textdomain.
 */

struct _SPUnit {
	unsigned int version : 8;
	unsigned int base : 8;
	unsigned int code : 8;
	float unit2base;
	unsigned char *name;
	unsigned char *abbr;
	unsigned char *plural;
	unsigned char *abbr_plural;
};

struct _SPDistance {
	const SPUnit *unit;
	double distance;
};

/* Get unit */
const SPUnit *sp_unit_get (unsigned int code);
/* Get identity unit for given base */
const SPUnit *sp_unit_get_identity (unsigned int base);
/* Get universal identty unit - i.e. none */
const SPUnit *sp_unit_get_default (void);
/* The following two use untranslated names */
const SPUnit *sp_unit_get_by_name (const unsigned char *name);
const SPUnit *sp_unit_get_by_abbreviation (const unsigned char *abbreviation);

/* Get list of units */
unsigned int sp_unit_get_units (const SPUnit ***units, unsigned int maxunits, const unsigned int *codes, unsigned int ncodes);
/* Get list of all units for given bases */
unsigned int sp_unit_get_all_units (const SPUnit ***units, unsigned int maxunits, unsigned int bases);
/* Get list of units from semicolon separated list of abbreviations */
unsigned int sp_unit_get_units_from_list (const SPUnit ***units, unsigned int maxunits, const unsigned char *list);

/* Return TRUE if conversion is possible, FALSE if unit bases differ */
unsigned int sp_convert_distance (float *distance, const SPUnit *from, const SPUnit *to);
/* ctmscale is userspace->absolute, devicescale is device->absolute */
/* Generic conversion between volatile units would be useless anyways */
unsigned int sp_convert_distance_full (float *distance, const SPUnit *from, const SPUnit *to, float ctmscale, float devicescale);

/* Some more convenience */
/* Be careful to not mix bases */

float sp_distance_get_units (SPDistance *distance, const SPUnit *unit);
float sp_distance_get_points (SPDistance *distance);

float sp_points_get_units (float points, const SPUnit *unit);
float sp_units_get_points (float units, const SPUnit *unit);

#endif 
