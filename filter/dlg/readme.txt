This folder contains dialog controllsers used by AC3Filter dialog.
AC3Filter uses the following scheme to interact with user:


     Dialog <---> User
        ^          ^
        |          |
        v          |
   Controller -----+
        ^
        |
        v
   Application


Controller receives messages from dialog controls, validates it and updates
application state. And in reverse order: reads application state and updates
dialog controls. If parameter validation fails controller may inform the user.

Controller may work with a single dialog control, or with many controls at
once. It is convenient because controls may form interrelated groups.

One dialog may have multiple controllers. And one controller may consist of
multiple controllers.

Base class for all controllers is Controller and can be found at controls.h
