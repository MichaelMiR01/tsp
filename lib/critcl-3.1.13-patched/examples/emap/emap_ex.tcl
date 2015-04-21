# emap_ex.tcl --
#
#	A template demonstrating the handling of emap conversions.
#
# Copyright (c) 2014 Andreas Kupries <andreas_kupries@users.sourceforge.net>

# # ## ### ##### ######## ############# #####################
## Requirements

package require Tcl          8.4
package require critcl       3.1.11
package require critcl::emap 1

# # ## ### ##### ######## ############# #####################
## Administrivia

critcl::license {Andreas Kupries} BSD

critcl::summary {Bitmap conversion}

critcl::description {
    This package implements nothing. It serves only as a
    demonstration and template on how to declare an emap
    converter and use it in cproc's or ccommand's.
}

critcl::subject demonstration {emap conversion} {encode emap} \
    {decode emap} {convert emap}

# # ## ### ##### ######## ############# #####################
## C code.

critcl::ccode {
    #define STATE_INIT 0
    #define STATE_MIX  1
    #define STATE_DONE 2
}

critcl::emap::def demo {
    init STATE_INIT
    mix  STATE_MIX
    done STATE_DONE
}
# Add -nocase as last argument for case-insensitive Tcl strings.

critcl::cproc encode {Tcl_Interp* ip Tcl_Obj* state} int {
    int scode;
    if (demo_encode (ip, state, &scode) != TCL_OK) {
	return -1;
    }
    return scode;
}

critcl::cproc decode {Tcl_Interp* ip int scode} object {
    Tcl_Obj* res = demo_decode (ip, scode);
    if (res) { Tcl_IncrRefCount (res); }
    return res;
}

# Encode hidden in the argtype.
critcl::cproc xencode {Tcl_Interp* ip demo state} int {
    return state;
}

# Decode hidden in the resultype
critcl::cproc xdecode {Tcl_Interp* ip int state} demo {
    return state;
}

# encode {exact filler} => 6
# decode 5              => {global filler}

# ### ### ### ######### ######### #########
## Ready
package provide emap_ex 1
