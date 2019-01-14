#!/bin/bash

# Default check for user{cert,key}.pem ;
if [ ! -f "$HOME/.globus/userkey.pem" -o ! -f "$HOME/.globus/usercert.pem" ]; then
    echo "Your Grid user certificate should be stored in $HOME/.globus/{usercert,userkey}.pem"
    exit 1
fi

# ALWAYS remove the group and other permissions
chmod g-rwx,o-rwx ${HOME}/.globus/userkey.pem ${HOME}/.globus/usercert.pem

# check if command arguments have jar files and build a list of them as classpath definition
ARGLIST="$@"
for jar in ${ARGLIST}; do
    if [[ ${jar} =~ .*\.jar ]]; then
        JARFILE="${BASH_REMATCH[0]}"
        [[ -z "${JARLIST}" ]] && JARLIST="${JARFILE}" || JARLIST="${JARLIST}:${JARFILE}";
        JARDIR="$(dirname ${JARFILE})"
        [[ ! ${JARLIST} =~ :${JARDIR} ]] && JARLIST="${JARLIST}:${JARDIR}" # use ":" when checking if path already added otherwise will match all paths of files

        ARGLIST=${ARGLIST//${JARFILE}/} # remove the found jar files from the argument list that will be than passed to java cmd
    fi
done

# if the command line have no jar files than find JALIEN_HOME and create CLASSPATH with all jars found there
if [[ -z "${JARLIST}" ]]; then
  if [[ -z "${JALIEN_HOME}" ]]; then
    ## find the location of jalien script
    SOURCE="${BASH_SOURCE[0]}"
    while [ -h "${SOURCE}" ]; do ## resolve $SOURCE until the file is no longer a symlink
      JALIEN_HOME="$( cd -P "$(dirname "${SOURCE}" )" && pwd )" ##"
      SOURCE="$(readlink "${SOURCE}")" ##"
      [[ "${SOURCE}" != /* ]] && SOURCE="${JALIEN_HOME}/${SOURCE}" ## if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
    done
    JALIEN_HOME="$(cd -P "$( dirname "${SOURCE}" )" && pwd)" ##"
    export JALIEN_HOME
  fi

  JAR_LIST_MAIN="";
  for mainjar in ${JALIEN_HOME}/*.jar; do JAR_LIST_MAIN="${JAR_LIST_MAIN}${mainjar}:" ; done;
  JAR_LIST_MAIN=$( echo "${JAR_LIST_MAIN}" | sed 's/.$//')

  JAR_LIST_LIB="";
  for libjar in ${JALIEN_HOME}/lib/*.jar; do JAR_LIST_LIB="${JAR_LIST_LIB}${libjar}:" ; done;
  JAR_LIST_LIB=$( echo "${JAR_LIST_LIB}" | sed 's/.$//')

  export CLASSPATH="${JAR_LIST_MAIN}:${JAR_LIST_LIB}"
fi

JALIEN_OPTS_DEFAULT="-server -Xms4G -Xmx4G -XX:+UseG1GC -XX:+DisableExplicitGC -XX:+UseCompressedOops -XX:+AggressiveOpts \
-XX:+OptimizeStringConcat -XX:MaxTrivialSize=1K -XX:CompileThreshold=20000 -Duserid=$(id -u) -Dcom.sun.jndi.ldap.connect.pool=false -Djava.io.tmpdir=/tmp"

[[ -n "${JARLIST}" ]] && JALIEN_OPTS_DEFAULT="${JALIEN_OPTS_DEFAULT} -cp ${JARLIST}"
RUNSH="exec java ${JALIEN_OPTS_DEFAULT} ${ARGLIST}"

# if not defined use default .j/config
JALIEN_CONF_DIR=${JALIEN_CONF_DIR:-${HOME}/.j/config}
if [ ! -d "${JALIEN_CONF_DIR}" ]; then # ups, setup was not run, so lets use the defaults from JALIEN_HOME
    JALIEN_CONF_DIR="${JALIEN_HOME}/config"
fi


ARG1="${1}"
if [ "${ARG1}" == "setup" ]; then
    #Copy the default logging and config files to user controllable versions - if present, keep the existing and copy the new ones with date postfix
    JCONFDIR="${JALIEN_HOME}/config/"
    USERCONFDIR="${HOME}/.j/config/"
    mkdir -p "${USERCONFDIR}"

    # if target is present KEEP the target - we don't want to lose user customisations
    [[ -e "${USERCONFDIR}/logging.properties" ]] && cp "${JCONFDIR}/logging.properties" "${USERCONFDIR}/logging.properties_$(date +%Y%m%d_%H%M%S)" || cp "${JCONFDIR}/logging.properties" "${USERCONFDIR}/"
    [[ -e "${USERCONFDIR}/config.properties" ]]  && cp "${JCONFDIR}/config.properties"  "${USERCONFDIR}/config.properties_$(date +%Y%m%d_%H%M%S)"  || cp "${JCONFDIR}/config.properties" "${USERCONFDIR}/"

    #mkdir -p ${HOME}/.j/trusts
    #wget -q "https://cafiles.cern.ch/cafiles/certificates/CERN%20Certification%20Authority.crt" -O ${HOME}/.j/trusts/CERN_CA.der
    #wget -q "https://cafiles.cern.ch/cafiles/certificates/CERN%20Root%20Certification%20Authority%202.crt" -O ${HOME}/.j/trusts/CERN_ROOT_CA2.der
    #wget -q "https://cafiles.cern.ch/cafiles/certificates/CERN%20Grid%20Certification%20Authority.crt" -O ${HOME}/.j/trusts/CERN_Grid_CA.der

    exit 0 # we done our job, let's exit

elif [ "${ARG1}" == "cleanup" ]; then
    # clean up the remaining JBox process
    JBOX_PID="$(ps -u $(id -u) -f | awk '/[a]lien.JBox/ {print $2}')"
    [[ -n "${JBOX_PID}" ]] && kill "${JBOX_PID}"
    exit 0 # we done our job, let's exit

elif [ "${ARG1}" == "login" ]; then
    RUNSH_OPTS="-DAliEnConfig=${JALIEN_CONF_DIR} alien.JBox"
    shift
    ARGLIST="$@"

elif [ "${ARG1}" == "JobAgent" ]; then
    RUNSH_OPTS="-DAliEnConfig=${JALIEN_CONF_DIR} -Djava.net.preferIPv4Stack=true alien.site.JobAgent"
    shift
    ARGLIST="$@"

elif [ "${ARG1}" == "ComputingElement" ]; then
    RUNSH_OPTS="-DAliEnConfig=${JALIEN_CONF_DIR} -Djava.net.preferIPv4Stack=true alien.site.ComputingElement"
    shift
    ARGLIST="$@"

elif [ "${ARG1}" == "TitanJobService" ]; then
    RUNSH_OPTS="-DAliEnConfig=${JALIEN_CONF_DIR} -Djava.net.preferIPv4Stack=true alien.site.TitanJobService"
    shift
    ARGLIST="$@"

elif [ "${ARG1}" == "ArchiveMemberDelete" ]; then
    RUNSH_OPTS="-DAliEnConfig=${JALIEN_CONF_DIR} -Djava.net.preferIPv4Stack=true alien.ArchiveMemberDelete"
    shift
    ARGLIST="$@"

elif [ "${ARG1}" == "run" ]; then
    # allows running custom commands from cli
    RUNSH_OPTS="-DAliEnConfig=${JALIEN_CONF_DIR}"
    shift
    ARGLIST="$@"

else
    RUNSH_OPTS="-DAliEnConfig=${JALIEN_CONF_DIR} alien.JSh"
    ARGLIST="$@"

fi

# RUNSH_OPTS="-Djavax.net.debug=ssl ${RUNSH_OPTS}"
# additional cli options can be added like:
# 1. JALIEN_OPTS="-Djavax.net.debug=ssl" jalien ComputingElement
# or
# 2. jalien ComputingElement -Djavax.net.debug=ssl

# prepend any JALIEN_OPTS additional options to java command to be run
[[ -n "${JALIEN_OPTS}" ]] && RUNSH_OPTS="${JALIEN_OPTS} ${RUNSH_OPTS}"
${RUNSH} "${ARGLIST}" "${RUNSH_OPTS}"

