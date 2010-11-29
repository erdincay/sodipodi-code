#define __SP_PAPER_C__

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

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "intl.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#include "units.h"

/* fixme: use some fancy unit program */

/*
 * WARNING! Do not mess up with that - we use hardcoded numbers for base units!
 */

enum {
	DIMENSIONLESS,
	ABSOLUTE,
	DEVICE,
	USERSPACE
};

static const SPUnit sp_units[] = {
	/* Do not insert any elements before/between first 4 */
	{0, SP_UNIT_BASE_DIMENSIONLESS, SP_UNIT_NONE, 1.0f, N_("Unit"), "", N_("Units"), ""},
	{0, SP_UNIT_BASE_ABSOLUTE, SP_UNIT_POINT, 1.0f, N_("Point"), N_("pt"), N_("Points"), N_("Pt")},
	{0, SP_UNIT_BASE_DEVICE, SP_UNIT_PIXEL, 1.0f, N_("Pixel"), N_("px"), N_("Pixels"), N_("Px")},
	{0, SP_UNIT_BASE_USERSPACE, SP_UNIT_USERSPACE, 1.0f, N_("Userspace unit"), N_("User"), N_("Userspace units"), N_("User")},
	/* Volatiles do not have default, so there are none here */
	/* You can add new elements from this point forward */
	{0, SP_UNIT_BASE_DIMENSIONLESS, SP_UNIT_PERCENT, 0.01f, N_("Percent"), N_("%"), N_("Percents"), N_("%")},
	{0, SP_UNIT_BASE_ABSOLUTE, SP_UNIT_MM, (72.0f / 25.4f), N_("Millimeter"), N_("mm"), N_("Millimeters"), N_("mm")},
	{0, SP_UNIT_BASE_ABSOLUTE, SP_UNIT_CM, (72.0f / 2.54f), N_("Centimeter"), N_("cm"), N_("Centimeters"), N_("cm")},
	{0, SP_UNIT_BASE_ABSOLUTE, SP_UNIT_M, (72.0f / 0.0254f), N_("Meter"), N_("m"), N_("meters"), N_("m")},
	{0, SP_UNIT_BASE_ABSOLUTE, SP_UNIT_INCH, 72.0f, N_("Inch"), N_("in"), N_("Inches"), N_("in")},
	{0, SP_UNIT_BASE_VOLATILE, SP_UNIT_EM, 1.0, N_("Em square"), N_("em"), N_("Em squares"), N_("em")},
	{0, SP_UNIT_BASE_VOLATILE, SP_UNIT_EX, 1.0, N_("Ex square"), N_("ex"), N_("Ex squares"), N_("ex")},
};
#define NUM_UNITS (sizeof (sp_units) / sizeof (sp_units[0]))

const SPUnit *
sp_unit_get (unsigned int code)
{
	unsigned int i;
	for (i = 0; i < NUM_UNITS; i++) {
		if (sp_units[i].code == code) return &sp_units[i];
	}
	fprintf (stderr, "sp_unit_get: Illegal unit code %d", code);
	return NULL;
}

const SPUnit *
sp_unit_get_identity (unsigned int base)
{
	switch (base) {
	case SP_UNIT_BASE_DIMENSIONLESS:
		return &sp_units[DIMENSIONLESS];
		break;
	case SP_UNIT_BASE_ABSOLUTE:
		return &sp_units[ABSOLUTE];
		break;
	case SP_UNIT_BASE_DEVICE:
		return &sp_units[DEVICE];
		break;
	case SP_UNIT_BASE_USERSPACE:
		return &sp_units[USERSPACE];
		break;
	default:
		break;
	}
	fprintf (stderr, "sp_unit_get: Illegal unit base %d", base);
	return NULL;
}

const SPUnit *
sp_unit_get_default (void)
{
	return &sp_units[DIMENSIONLESS];
}

const SPUnit *
sp_unit_get_by_name (const unsigned char *name)
{
	unsigned int i;
	if (!name || !*name) return NULL;
	for (i = 0; i < NUM_UNITS; i++) {
		if (!strcasecmp (name, sp_units[i].name)) return &sp_units[i];
		if (!strcasecmp (name, sp_units[i].plural)) return &sp_units[i];
	}
	fprintf (stderr, "sp_unit_get: Illegal unit name %s", name);
	return NULL;
}

const SPUnit *
sp_unit_get_by_abbreviation (const unsigned char *abbreviation)
{
	unsigned int i;
	if (!abbreviation || !abbreviation) return NULL;
	for (i = 0; i < NUM_UNITS; i++) {
		if (!strcasecmp (abbreviation, sp_units[i].abbr)) return &sp_units[i];
		if (!strcasecmp (abbreviation, sp_units[i].abbr_plural)) return &sp_units[i];
	}
	fprintf (stderr, "sp_unit_get: Illegal unit abbreviation %s", abbreviation);
	return NULL;
}

unsigned int
sp_unit_get_units (const SPUnit ***units, unsigned int maxunits, const unsigned int *codes, unsigned int ncodes)
{
	unsigned int i, upos;
	upos = 0;
	for (i = 0; i < ncodes; i++) {
		unsigned int j;
		for (j = 0; j < NUM_UNITS; j++) {
			if (sp_units[j].code == codes[i]) {
				if (units && (upos < maxunits)) (*units)[upos] = &sp_units[j];
				upos += 1;
				break;
			}
		}
	}
	return upos;
}

unsigned int
sp_unit_get_all_units (const SPUnit ***units, unsigned int maxunits, unsigned int bases)
{
	unsigned int i, upos;
	upos = 0;
	for (i = 0; i < NUM_UNITS; i++) {
		if (bases & sp_units[i].base) {
			if (units && (upos < maxunits)) (*units)[upos] = &sp_units[i];
			upos += 1;
		}
	}
	return upos;
}

unsigned int
sp_unit_get_units_from_list (const SPUnit ***units, unsigned int maxunits, const unsigned char *list)
{
	char *c, *q;
	unsigned int upos, p;
	if (!list || !*list) return 0;
	q = c = strdup ((const char *) list);
	upos = 0;
	while (*q) {
		const SPUnit *unit;
		for (p = 0; q[p]; p++) {
			if (q[p] == ';') break;
		}
		if (q[p] == ';') {
			q[p] = 0;
			p += 1;
		}
		unit = sp_unit_get_by_abbreviation (q);
		if (unit) {
			if (units && (upos < maxunits)) (*units)[upos] = unit;
			upos += 1;
		}
		q = q + p;
	}
	free (c);
	return upos;
}

unsigned int
sp_convert_distance (float *distance, const SPUnit *from, const SPUnit *to)
{
	if (from->base != to->base) {
		if (from->base == SP_UNIT_BASE_DIMENSIONLESS) {
			/* 100% in meters is 1m */
			*distance = *distance * from->unit2base;
			return TRUE;
		}
		if (to->base == SP_UNIT_BASE_DIMENSIONLESS) {
			/* 1m in percent is 100% */
			*distance = *distance * to->unit2base;
			return TRUE;
		}
		return FALSE;
	}
	if (from == to) return TRUE;
	if (from->base == SP_UNIT_BASE_VOLATILE) return FALSE;

	*distance = *distance * from->unit2base / to->unit2base;

	return TRUE;
}

unsigned int
sp_convert_distance_full (float *distance, const SPUnit *from, const SPUnit *to, float ctmscale, float devicescale)
{
	if (from->base != to->base) {
		float absdist;
		if (from->base == SP_UNIT_BASE_DIMENSIONLESS) {
			/* 100% in meters is 1m */
			*distance = *distance * from->unit2base;
			return TRUE;
		}
		if (to->base == SP_UNIT_BASE_DIMENSIONLESS) {
			/* 1m in percent is 100% */
			*distance = *distance * to->unit2base;
			return TRUE;
		}
		if ((from->base == SP_UNIT_BASE_VOLATILE) || (to->base == SP_UNIT_BASE_VOLATILE)) return FALSE;
		switch (from->base) {
		case SP_UNIT_BASE_ABSOLUTE:
			absdist = *distance * from->unit2base;
			break;
		case SP_UNIT_BASE_DEVICE:
			if (devicescale) {
				absdist = *distance * from->unit2base * devicescale;
			} else {
				return FALSE;
			}
			break;
		case SP_UNIT_BASE_USERSPACE:
			if (ctmscale) {
				absdist = *distance * from->unit2base * ctmscale;
			} else {
				return FALSE;
			}
			break;
		default:
			fprintf (stderr, "sp_convert_distance_full: Illegal unit (base %d)", from->base);
			return FALSE;
		}

		switch (to->base) {
		case SP_UNIT_BASE_ABSOLUTE:
			*distance = absdist / to->unit2base;
			break;
		case SP_UNIT_BASE_DEVICE:
			if (devicescale) {
				*distance = absdist / (to->unit2base * devicescale);
			} else {
				return FALSE;
			}
			break;
		case SP_UNIT_BASE_USERSPACE:
			if (ctmscale) {
				*distance = absdist / (to->unit2base * ctmscale);
			} else {
				return FALSE;
			}
			break;
		default:
			fprintf (stderr, "sp_convert_distance_full: Illegal unit (base %d)", to->base);
			return FALSE;
		}
		return TRUE;
	}
	if (from == to) return TRUE;
	if (from->base == SP_UNIT_BASE_VOLATILE) return FALSE;

	*distance = *distance * from->unit2base / to->unit2base;

	return TRUE;
}

/* Some more convenience */
/* Be careful to not mix bases */

float
sp_distance_get_units (SPDistance *distance, const SPUnit *unit)
{
	float val;

	val = distance->distance;
	sp_convert_distance (&val, distance->unit, unit);

	return val;
}

float
sp_distance_get_points (SPDistance *distance)
{
	float val;

	val = distance->distance;
	sp_convert_distance (&val, distance->unit, &sp_units[ABSOLUTE]);

	return val;
}

float
sp_points_get_units (float points, const SPUnit *unit)
{
	sp_convert_distance (&points, &sp_units[ABSOLUTE], unit);

	return points;
}

float
sp_units_get_points (float units, const SPUnit *unit)
{
	sp_convert_distance (&units, unit, &sp_units[ABSOLUTE]);

	return units;
}

