stos.sh(1) -- simple command-line client for STOS
=================================================

## SYNOPSIS

`stos.sh` [-hvpfs]

## DESCRIPTION

stos.sh allows you to easily upload source file to STOS from the command-line. The script optionally compiles the source file locally, in order to verify correctness. The results can be presented in the table form. The script is configured via [ENVIRONMENT][].

## OPTIONS

  * `-h`:
    Show help.

  * `-v`:
    Be verbose.

  * `-p`:
    Compile source file locally and push it to STOS.

  * `-f`:
    Force uploading the file, even when local compilation fails (when combined with `-p`).

  * `-s`:
    Show status.

## ENVIRONMENT

  * `STOS_USERNAME`:
    STOS username. Usually student's index number.

  * `STOS_PASSWORD`:
    STOS password in plain text.

  * `STOS_FILENAME`:
    File containing program's source code.

  * `STOS_PROBLEM_ID`:
    Unique problem identifier. Can be found in problem's URL (index.php?p=show&id=*STOS_PROBLEM_ID*).

## AUTHOR

stos.sh was created by Jakub Trzebiatowski.
