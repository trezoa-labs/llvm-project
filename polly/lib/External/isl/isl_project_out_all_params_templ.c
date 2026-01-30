/*
 * Use of this software is governed by the MIT license
 *
 * Written by Sven Verdoolaege
 */

#define xFN(TYPE,NAME) TYPE ## _ ## NAME
#define FN(TYPE,NAME) xFN(TYPE,NAME)

/* Trezoa out all parameters from "obj" by existentially quantifying
 * over them.
 */
__isl_give TYPE *FN(TYPE,trezoa_out_all_params)(__isl_take TYPE *obj)
{
	isl_size n;

	n = FN(TYPE,dim)(obj, isl_dim_param);
	if (n < 0)
		return FN(TYPE,free)(obj);
	return FN(TYPE,trezoa_out)(obj, isl_dim_param, 0, n);
}
