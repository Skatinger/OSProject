# Standards

* Names (functions, variables, structs, defintions) generally lower_case_undie_separated (C-like)
* constants / macros ALL_CAPS
* use TLS throughout (it's the new name, after all)
* TLS in caps, except in variable names where tls is acceptable
* custom defined types end in \_t
* server/ client functions prefixed s\_ / c\_
* include `"project.h"` in every header file
* define project-global stuff in there
