#!/bin/bash

STOS_URL=https://kaims.pl/~kmocet/stos/index.php

while getopts "hvfps" opt; do
    case "$opt" in
    h)
        echo "usage: $0 [-hvfps]"
        exit 0
        ;;
    v)
        # Wypisywanie danych diagnostycznych
        VERBOSE=-v
        ;;
    f)
        # Wysyłaj plik, nawet kiedy lokalna kompilacja się nie powiodła
        FORCE=true
        ;;
    p)
        # Wyślij plik na STOS
        PUSH=true
        ;;
    s)
        # Wyświetl status zadania
        STATUS=true
        ;;
    esac
done

# Nazwa użytkownika (numer indeksu)
[ -z "$STOS_USERNAME" ] \
    && echo "Need to set STOS_USERNAME environment variable" && exit 1

# Hasło do STOSu
[ -z "$STOS_PASSWORD" ] \
     && echo "Need to set STOS_PASSWORD environment variable" && exit 1

# Plik źródłowy zawierający rozwiązanie
[ -z "$STOS_FILENAME" ] \
    && echo "Need to set STOS_FILENAME environment variable" && exit 1

# Identyfikator zadania
[ -z "$STOS_PROBLEM_ID" ] \
    && echo "Need to set STOS_PROBLEM_ID environment variable" && exit 1

# Logowanie do platformy STOS
curl $VERBOSE -s --cookie-jar /tmp/stos.cookie-jar.$$ --insecure \
     --data "login=$STOS_USERNAME&password=$STOS_PASSWORD" $STOS_URL?p=login || { exit 1; }

if [ -n "$PUSH" ]
then
    # Testowa kompilacja źródła
    if ! cc -o /dev/null $STOS_FILENAME && [ -z "$FORCE" ]
    then
        exit 1
    fi

    # Przesyłanie pliku źródłowego
    curl $VERBOSE -s --cookie /tmp/stos.cookie-jar.$$ --insecure \
        --form "fileupload=@$STOS_FILENAME;filename=afile1" --form code=$STOS_PROBLEM_ID --form context=84 $STOS_URL?p=put || { exit 1; }

    echo "**** Przesłano plik źródłowy ****"

    # Stos szybkim systemem jest
    [ -n "$STATUS" ] && sleep 5s
fi

if [ -n "$STATUS" ]
then
    # Wypisanie tabeli
    curl $VERBOSE -s --cookie /tmp/stos.cookie-jar.$$ --insecure \
        "$STOS_URL?p=status&probid=$STOS_PROBLEM_ID" | recode -f html..ascii | html2text | tail -n +7 || { exit 1; }
fi
