#! /bin/sh

header="$1"
test -z "$2" || cd "$2"

base_dir="$(dirname $0)"
top_dir="${base_dir}/.."

new=
if test -d "${top_dir}/.git"; then
    versionstr=`git --git-dir "${top_dir}/.git" describe --tags --match='[0-9][.0-9]*' 2>/dev/null`
    if test -n "$versionstr"; then
	save_IFS="$IFS" IFS=-; set $versionstr
	versionstr=$1
	commits=$2
	IFS=.; set $versionstr
	major=$1 minor=$2 micro=$3
	IFS="$save_IFS"
	version=`expr $major \* 1000000 + $minor \* 1000 + $micro`
	date=`git log --max-count=1 --format=%cd --date=short | tr -d -`
	new="#ifndef CHUPATEXT_VERSION_H
#define CHUPA_VERSION_STRING \"$versionstr\"
#define CHUPA_VERSION_CODE $version
#define CHUPA_VERSION_MAJOR $major
#define CHUPA_VERSION_MINOR $minor
#define CHUPA_VERSION_MICRO $micro
#define CHUPA_COMMITS ${commits:-0}
#define CHUPA_RELEASE_DATE $date

#ifdef __cplusplus
extern \"C\" {
#endif
const int chupa_version(void);
const char *chupa_version_string(void);
const int chupa_commits(void);
const int chupa_release_date(void);
const char *chupa_version_description(void);
#ifdef __cplusplus
}
#endif
#endif"
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
