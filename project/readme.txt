Project Folder
==============================================================================
All project-specific files should be stored in the /project folder to try to
keep the generic HW-level code and drivers seperate from the business logic
of your individual project.  This makes it easier to reuse your code in other
projects, and also update the drivers and HW-level code if newer version of
these common-files become available.

FOLDERS
==============================================================================
commands/       Code to implements specific commands for the
                command-line interface.  Requires CFG_INTERFACE
                to be enabled in projectconfig.h.  Generally,
                each command will be stored in a seperate file
                (ex.: "command/cmd_hello.c"), though this isn't
                a strict requirement and you may wish to store
                multiple related commands in one .c file, such
                as 'cmds_graphics.c' etc.
                
documentation/  Project-specific documentation is stored here

FILES
==============================================================================
cmd_tbl.h       Contains the master command list for the
                command-line interface if CFG_INTERFACE is
                enabled in projectconfig.h.  All commands
                must be present in this list to be properly
                handled by the CLI.
                
commands.c      Common helper functions for the command-line
                interface.

