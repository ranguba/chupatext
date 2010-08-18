#! /bin/sh

header="$1"
test -z "$2" || cd "$2"

new=
if test -d .git; then
    versionstr=`git describe --tags --match='[0-9][.0-9]*' 2>/dev/null`
    if test -n "$versionstr"; then
	save_IFS="$IFS" IFS=-; set $versionstr
	versionstr=$1
	commits=$2
	IFS=.; set $versionstr
	major=$1 minor=$2 micro=$3
	IFS="$save_IFS"
	version=`expr $major \* 1000000 + $minor \* 1000 + $micro`
	date=`git log --max-count=1 --format=%cd --date=short | tr -d -`
	new="\
#define CHUPA_VERSION_STRING \"$versionstr\"
#define CHUPA_VERSION_CODE $version
#define CHUPA_VERSION_MAJOR $major
#define CHUPA_VERSION_MINOR $minor
#define CHUPA_VERSION_MICRO $micro
#define CHUPA_COMMITS ${commits:-0}
#define CHUPA_RELEASE_DATE $date"
    fi
fi
unset old
if test -f "$header"; then
    old=`cat "$header"`
fi
if test -z "${old+exist}" -o \( -n "$versionstr$date" -a "$new" != "$old" \); then
    echo "updating $header"
    echo "$new" > "$header"
else
    echo "$header is up-to-date"
fi
