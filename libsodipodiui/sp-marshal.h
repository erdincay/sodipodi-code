
#ifndef __sp_marshal_MARSHAL_H__
#define __sp_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* NONE:NONE (src\helper\sp-marshal.list:2) */
#define sp_marshal_VOID__VOID	g_cclosure_marshal_VOID__VOID
#define sp_marshal_NONE__NONE	sp_marshal_VOID__VOID

/* NONE:BOOLEAN (src\helper\sp-marshal.list:3) */
#define sp_marshal_VOID__BOOLEAN	g_cclosure_marshal_VOID__BOOLEAN
#define sp_marshal_NONE__BOOLEAN	sp_marshal_VOID__BOOLEAN

/* NONE:UINT (src\helper\sp-marshal.list:4) */
#define sp_marshal_VOID__UINT	g_cclosure_marshal_VOID__UINT
#define sp_marshal_NONE__UINT	sp_marshal_VOID__UINT

/* NONE:INT,INT (src\helper\sp-marshal.list:5) */
extern void sp_marshal_VOID__INT_INT (GClosure     *closure,
                                      GValue       *return_value,
                                      guint         n_param_values,
                                      const GValue *param_values,
                                      gpointer      invocation_hint,
                                      gpointer      marshal_data);
#define sp_marshal_NONE__INT_INT	sp_marshal_VOID__INT_INT

/* NONE:POINTER (src\helper\sp-marshal.list:6) */
#define sp_marshal_VOID__POINTER	g_cclosure_marshal_VOID__POINTER
#define sp_marshal_NONE__POINTER	sp_marshal_VOID__POINTER

/* NONE:POINTER,BOOLEAN (src\helper\sp-marshal.list:7) */
extern void sp_marshal_VOID__POINTER_BOOLEAN (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);
#define sp_marshal_NONE__POINTER_BOOLEAN	sp_marshal_VOID__POINTER_BOOLEAN

/* NONE:POINTER,UINT (src\helper\sp-marshal.list:8) */
extern void sp_marshal_VOID__POINTER_UINT (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data);
#define sp_marshal_NONE__POINTER_UINT	sp_marshal_VOID__POINTER_UINT

/* NONE:POINTER,DOUBLE (src\helper\sp-marshal.list:9) */
extern void sp_marshal_VOID__POINTER_DOUBLE (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);
#define sp_marshal_NONE__POINTER_DOUBLE	sp_marshal_VOID__POINTER_DOUBLE

/* NONE:DOUBLE (src\helper\sp-marshal.list:10) */
#define sp_marshal_VOID__DOUBLE	g_cclosure_marshal_VOID__DOUBLE
#define sp_marshal_NONE__DOUBLE	sp_marshal_VOID__DOUBLE

/* NONE:DOUBLE,DOUBLE (src\helper\sp-marshal.list:11) */
extern void sp_marshal_VOID__DOUBLE_DOUBLE (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);
#define sp_marshal_NONE__DOUBLE_DOUBLE	sp_marshal_VOID__DOUBLE_DOUBLE

/* NONE:OBJECT (src\helper\sp-marshal.list:12) */
#define sp_marshal_VOID__OBJECT	g_cclosure_marshal_VOID__OBJECT
#define sp_marshal_NONE__OBJECT	sp_marshal_VOID__OBJECT

/* NONE:OBJECT,ENUM,BOXED (src\helper\sp-marshal.list:13) */
extern void sp_marshal_VOID__OBJECT_ENUM_BOXED (GClosure     *closure,
                                                GValue       *return_value,
                                                guint         n_param_values,
                                                const GValue *param_values,
                                                gpointer      invocation_hint,
                                                gpointer      marshal_data);
#define sp_marshal_NONE__OBJECT_ENUM_BOXED	sp_marshal_VOID__OBJECT_ENUM_BOXED

/* NONE:OBJECT,OBJECT (src\helper\sp-marshal.list:14) */
extern void sp_marshal_VOID__OBJECT_OBJECT (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);
#define sp_marshal_NONE__OBJECT_OBJECT	sp_marshal_VOID__OBJECT_OBJECT

/* NONE:OBJECT,OBJECT,OBJECT (src\helper\sp-marshal.list:15) */
extern void sp_marshal_VOID__OBJECT_OBJECT_OBJECT (GClosure     *closure,
                                                   GValue       *return_value,
                                                   guint         n_param_values,
                                                   const GValue *param_values,
                                                   gpointer      invocation_hint,
                                                   gpointer      marshal_data);
#define sp_marshal_NONE__OBJECT_OBJECT_OBJECT	sp_marshal_VOID__OBJECT_OBJECT_OBJECT

/* NONE:BOXED (src\helper\sp-marshal.list:16) */
#define sp_marshal_VOID__BOXED	g_cclosure_marshal_VOID__BOXED
#define sp_marshal_NONE__BOXED	sp_marshal_VOID__BOXED

/* BOOLEAN:NONE (src\helper\sp-marshal.list:17) */
extern void sp_marshal_BOOLEAN__VOID (GClosure     *closure,
                                      GValue       *return_value,
                                      guint         n_param_values,
                                      const GValue *param_values,
                                      gpointer      invocation_hint,
                                      gpointer      marshal_data);
#define sp_marshal_BOOLEAN__NONE	sp_marshal_BOOLEAN__VOID

/* BOOLEAN:UINT (src\helper\sp-marshal.list:18) */
extern void sp_marshal_BOOLEAN__UINT (GClosure     *closure,
                                      GValue       *return_value,
                                      guint         n_param_values,
                                      const GValue *param_values,
                                      gpointer      invocation_hint,
                                      gpointer      marshal_data);

/* BOOLEAN:POINTER (src\helper\sp-marshal.list:19) */
extern void sp_marshal_BOOLEAN__POINTER (GClosure     *closure,
                                         GValue       *return_value,
                                         guint         n_param_values,
                                         const GValue *param_values,
                                         gpointer      invocation_hint,
                                         gpointer      marshal_data);

/* BOOLEAN:POINTER,UINT (src\helper\sp-marshal.list:20) */
extern void sp_marshal_BOOLEAN__POINTER_UINT (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);

/* BOOLEAN:POINTER,POINTER (src\helper\sp-marshal.list:21) */
extern void sp_marshal_BOOLEAN__POINTER_POINTER (GClosure     *closure,
                                                 GValue       *return_value,
                                                 guint         n_param_values,
                                                 const GValue *param_values,
                                                 gpointer      invocation_hint,
                                                 gpointer      marshal_data);

/* INT:POINTER,POINTER (src\helper\sp-marshal.list:22) */
extern void sp_marshal_INT__POINTER_POINTER (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);

/* DOUBLE:POINTER,UINT (src\helper\sp-marshal.list:23) */
extern void sp_marshal_DOUBLE__POINTER_UINT (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);

G_END_DECLS

#endif /* __sp_marshal_MARSHAL_H__ */

