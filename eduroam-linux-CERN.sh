#!/bin/bash


my_name=$0


function setup_environment {
  bf=""
  n=""
  ORGANISATION="CERN"
  URL="https://cern.service-now.com/service-portal/"
  SUPPORT="service-desk@cern.ch"
if [ ! -z "$DISPLAY" ] ; then
  if which zenity 1>/dev/null 2>&1 ; then
    ZENITY=`which zenity`
  elif which kdialog 1>/dev/null 2>&1 ; then
    KDIALOG=`which kdialog`
  else
    if tty > /dev/null 2>&1 ; then
      if  echo $TERM | grep -E -q "xterm|gnome-terminal|lxterminal"  ; then
        bf="[1m";
        n="[0m";
      fi
    else
      find_xterm
      if [ -n "$XT" ] ; then
        $XT -e $my_name
      fi
    fi
  fi
fi
}

function split_line {
echo $1 | awk  -F '\\\\n' 'END {  for(i=1; i <= NF; i++) print $i }'
}

function find_xterm {
terms="xterm aterm wterm lxterminal rxvt gnome-terminal konsole"
for t in $terms
do
  if which $t > /dev/null 2>&1 ; then
  XT=$t
  break
  fi
done
}


function ask {
     T="eduroam CAT"
#  if ! [ -z "$3" ] ; then
#     T="$T: $3"
#  fi
  if [ ! -z $KDIALOG ] ; then
     if $KDIALOG --yesno "${1}\n${2}?" --title "$T" ; then
       return 0
     else
       return 1
     fi
  fi
  if [ ! -z $ZENITY ] ; then
     if $ZENITY --question --text="${1}\n${2}?" --title="$T" ; then
       return 0
     else
       return 1
     fi
  fi

  yes=Y
  no=N
  yes1=`echo $yes | awk '{ print toupper($0) }'`
  no1=`echo $no | awk '{ print toupper($0) }'`
  if [ $3 == "0" ]; then
    def=$yes
  else
    def=$no
  fi

  echo "";
  while true
  do
  split_line "$1"
  read -p "${bf}$2 ${yes}/${no}? [${def}]:$n " answer
  if [ -z "$answer" ] ; then
    answer=${def}
  fi
  answer=`echo $answer | awk '{ print toupper($0) }'`
  case "$answer" in
    ${yes1})
       return 0
       ;;
    ${no1})
       return 1
       ;;
  esac
  done
}

function alert {
  if [ ! -z $KDIALOG ] ; then
     $KDIALOG --sorry "${1}"
     return
  fi
  if [ ! -z $ZENITY ] ; then
     $ZENITY --warning --text="$1"
     return
  fi
  echo "$1"

}

function show_info {
  if [ ! -z $KDIALOG ] ; then
     $KDIALOG --msgbox "${1}"
     return
  fi
  if [ ! -z $ZENITY ] ; then
     $ZENITY --info --width=500 --text="$1"
     return
  fi
  echo "$1"
}

function confirm_exit {
  if [ ! -z $KDIALOG ] ; then
     if $KDIALOG --yesno \"Really quit?\"  ; then
     exit 1
     fi
  fi
  if [ ! -z $ZENITY ] ; then
     if $ZENITY --question --text=\"Really quit?\" ; then
        exit 1
     fi
  fi
}



function prompt_nonempty_string {
  prompt=$2
  if [ ! -z $ZENITY ] ; then
    if [ $1 -eq 0 ] ; then
     H="--hide-text "
    fi
    if ! [ -z "$3" ] ; then
     D="--entry-text=$3"
    fi
  elif [ ! -z $KDIALOG ] ; then
    if [ $1 -eq 0 ] ; then
     H="--password"
    else
     H="--inputbox"
    fi
  fi


  out_s="";
  if [ ! -z $ZENITY ] ; then
    while [ ! "$out_s" ] ; do
      out_s=`$ZENITY --entry --width=300 $H $D --text "$prompt"`
      if [ $? -ne 0 ] ; then
        confirm_exit
      fi
    done
  elif [ ! -z $KDIALOG ] ; then
    while [ ! "$out_s" ] ; do
      out_s=`$KDIALOG $H "$prompt" "$3"`
      if [ $? -ne 0 ] ; then
        confirm_exit
      fi
    done  
  else
    while [ ! "$out_s" ] ; do
      read -p "${prompt}: " out_s
    done
  fi
  echo $out_s;
}

function user_cred {
  PASSWORD="a"
  PASSWORD1="b"

  if ! USER_NAME=`prompt_nonempty_string 1 "enter your userid"` ; then
    exit 1
  fi

  while [ "$PASSWORD" != "$PASSWORD1" ]
  do
    if ! PASSWORD=`prompt_nonempty_string 0 "enter your password"` ; then
      exit 1
    fi
    if ! PASSWORD1=`prompt_nonempty_string 0 "repeat your password"` ; then
      exit 1
    fi
    if [ "$PASSWORD" != "$PASSWORD1" ] ; then
      alert "passwords do not match"
    fi
  done
}
setup_environment
show_info "This installer has been prepared for ${ORGANISATION}\n\nMore information and comments:\n\nEMAIL: ${SUPPORT}\nWWW: ${URL}\n\nInstaller created with software from the GEANT project."
         if ! ask "This installer will only work properly if you are a member of ${bf}CERN.${n}" "Continue" 1 ; then exit; fi
if [ -d $HOME/.eduroam ] ; then
   if ! ask "Directory $HOME/.eduroam exists; some of its files may be overwritten." "Continue" 1 ; then exit; fi
else
  mkdir $HOME/.eduroam
fi
# save certificates
echo "-----BEGIN CERTIFICATE-----
MIIGqTCCBJGgAwIBAgIQAojDcLlcbrhBX0qrEka4mzANBgkqhkiG9w0BAQ0FADBK
MQswCQYDVQQGEwJjaDENMAsGA1UEChMEQ0VSTjEsMCoGA1UEAxMjQ0VSTiBSb290
IENlcnRpZmljYXRpb24gQXV0aG9yaXR5IDIwHhcNMTMwMzE5MTI1NTM2WhcNMzMw
MzE5MTMwNTM0WjBKMQswCQYDVQQGEwJjaDENMAsGA1UEChMEQ0VSTjEsMCoGA1UE
AxMjQ0VSTiBSb290IENlcnRpZmljYXRpb24gQXV0aG9yaXR5IDIwggIiMA0GCSqG
SIb3DQEBAQUAA4ICDwAwggIKAoICAQDxqYPFW2qVVi3Rw1NKlEf7x70xF+6a8uE/
Tu4ZVQF/K2RXI95QLkYfKItZvy9Az3ib/VlUho5f8fBaqy4n70uwC7+qd3Aq1/xQ
ysykPCbBBAsOSQQpTlhrMD2V5Ya9zrirphOhutddiqV96zBCyMM+Gz5uYv9u+cm4
tg1EOmAMGh2UNxfTFNVmXKkk7eFTSC1+zgb28H6nd3xzV27sn9bfOfGh//ZPy5gm
Qx0Oh/tc6WMreWzRZBQm5SJiK0QOzPv09p5WmdY2WxZoqNTFBDACQO7ysFOktc74
fPVFX/lmt4jFNSZRIOvvaACI/qlEaAJTR4FHIY9uSMsV8DrtzhI1Ucyv3kqlQpbF
jDouq44IryA/np4s/124bW+x8+n/v+at/AxPjvHBLiGhB+J38Z6KcJogoDnGzIXR
S+YUr/vGz34jOmkRuDN5STuuAXzyCKFXaoAm0AwjTziIv3E0jxC1taw6FpKevnd1
CLsTLAEUiEjzStFkDhd/Hpipc57zmMFY8VYet2wVqSFjnt2REWOVbZlbCiMHmSeD
u5EuZLiU8xlkiaCfn4A5XZ6X0qprbgDviGJtwxzNvTg7Hn0ziW5/ELryfQXCwZJ+
FVne8Zu8sbgy/sDkX+pyFuyB4XgiM0eMNkoexIXJaRdlMWDIL5ysiIXQKjhynAv5
KLHbRjciVwIDAQABo4IBiTCCAYUwCwYDVR0PBAQDAgGGMA8GA1UdEwEB/wQFMAMB
Af8wHQYDVR0OBBYEFPp7+96bDaPyUrds7VsPC6KmpvgEMBAGCSsGAQQBgjcVAQQD
AgEAMIIBMgYDVR0gBIIBKTCCASUwggEhBgorBgEEAWAKBAEBMIIBETCBwgYIKwYB
BQUHAgIwgbUegbIAQwBFAFIATgAgAFIAbwBvAHQAIABDAGUAcgB0AGkAZgBpAGMA
YQB0AGkAbwBuACAAQQB1AHQAaABvAHIAaQB0AHkAIAAyACAAQwBlAHIAdABpAGYA
aQBjAGEAdABlACAAUABvAGwAaQBjAHkAIABhAG4AZAAgAEMAZQByAHQAaQBmAGkA
YwBhAHQAZQAgAFAAcgBhAGMAdABpAGMAZQAgAFMAdABhAHQAZQBtAGUAbgB0MEoG
CCsGAQUFBwIBFj5odHRwOi8vY2FmaWxlcy5jZXJuLmNoL2NhZmlsZXMvY3AtY3Bz
L2Nlcm4tcm9vdC1jYTItY3AtY3BzLnBkZjANBgkqhkiG9w0BAQ0FAAOCAgEAo0Px
l4CZ6C6bDH+b6jV5uUO0NIHtvLuVgQLMdKVHtQ2UaxeIrWwD+Kz1FyJCHTRXrCvE
OFOca9SEYK2XrbqZGvRKdDRsq+XYts6aCampXj5ahh6r4oQJ8U7aLVfziKTK13Gy
dYFoAUeUrlNklICt3v2wWBaa1tg2oSlU2g4iCg9kYpRnIW3VKSrVsdVk2lUa4EXs
nTEJ30OS7rqX3SdqZp8G+awtBEReh2XPhRgJ6w3xiScP/UdWYUam2LflCGX3RibB
/DZhgGHRRoE4/D0kQMP2XTz6cClbNklECTlp0qZIbiaf350HbcDEFzYRSSIi0emv
kRGcMgsi8yTTU87q8Cr4hETxAF3ZbSVNC0ZaTZ8RBbM9BXguhYzKkVBgG/cMpUjs
B6tY2HMZbAZ3TKQRb/bRyUigM9DniKWeXkeL/0Nsno+XbcpAqLjtVIRwCg6jTLUi
1NRsl3BP6C824dVaoI8Ry7m+o6O+mtocw4BMhHfTcoWCO8CWjT0ME67JzaAYa5eM
+OqoWtgbgweBlfO0/3GMnVGMAmI4FlhH2oWKWQgWdgr0Wgh9K05VcxSpJ87/zjhb
MQn/bEojWmp6eUppPaqNFcELvud41qoe6hLsOYQVUQ1sHi7n6ouhg4BAbwS2iyD2
uiA6FHTCeLreFGUzs5osPKiz3GE5D6V9she9xIQ=
-----END CERTIFICATE-----
" > $HOME/.eduroam/ca.pem
function run_python_script {
python << EOF > /dev/null 2>&1
#-*- coding: utf-8 -*-
import dbus
import re
import sys
import uuid
import os

class EduroamNMConfigTool:

    def connect_to_NM(self):
        #connect to DBus
        try:
            self.bus = dbus.SystemBus()
        except dbus.exceptions.DBusException:
            print "Can't connect to DBus"
            sys.exit(2)
        #main service name
        self.system_service_name = "org.freedesktop.NetworkManager"
        #check NM version
        nm_version = self.check_nm_version()
        if nm_version == "0.9":
            self.settings_service_name = self.system_service_name
            self.connection_interface_name = "org.freedesktop.NetworkManager.Settings.Connection"
            #settings proxy
            sysproxy = self.bus.get_object(self.settings_service_name, "/org/freedesktop/NetworkManager/Settings")
            #settings intrface
            self.settings = dbus.Interface(sysproxy, "org.freedesktop.NetworkManager.Settings")
        elif nm_version == "0.8":
            #self.settings_service_name = "org.freedesktop.NetworkManagerUserSettings"
            self.settings_service_name = "org.freedesktop.NetworkManager"
            self.connection_interface_name = "org.freedesktop.NetworkManagerSettings.Connection"
            #settings proxy
            sysproxy = self.bus.get_object(self.settings_service_name, "/org/freedesktop/NetworkManagerSettings")
            #settings intrface
            self.settings = dbus.Interface(sysproxy, "org.freedesktop.NetworkManagerSettings")
        else:
            print "This Network Manager version is not supported"
            sys.exit(2)

    def check_opts(self):
        self.cacert_file = '${HOME}/.eduroam/ca.pem'
        self.pfx_file = '${HOME}/.eduroam/user.p12'
        if not os.path.isfile(self.cacert_file):
            print "Certificate file not found, looks like a CAT error"
            sys.exit(2)

    def check_nm_version(self):
        try:
            proxy = self.bus.get_object(self.system_service_name, "/org/freedesktop/NetworkManager")
            props = dbus.Interface(proxy, "org.freedesktop.DBus.Properties")
            version = props.Get("org.freedesktop.NetworkManager", "Version")
        except dbus.exceptions.DBusException:
            version = "0.8"
        if re.match(r'^0\.9', version):
            return "0.9"
        if re.match(r'^0\.8', version):
            return "0.8"
        else:
            return "Unknown version"

    def byte_to_string(self, barray):
        return "".join([chr(x) for x in barray])


    def delete_existing_connections(self, ssid):
        "checks and deletes earlier connections"
        try:
            conns = self.settings.ListConnections()
        except dbus.exceptions.DBusException:
            print "DBus connection problem, a sudo might help"
            exit(3)
        for each in conns:
            con_proxy = self.bus.get_object(self.system_service_name, each)
            connection = dbus.Interface(con_proxy, "org.freedesktop.NetworkManager.Settings.Connection")
            try:
               connection_settings = connection.GetSettings()
               if connection_settings['connection']['type'] == '802-11-wireless':
                   conn_ssid = self.byte_to_string(connection_settings['802-11-wireless']['ssid'])
                   if conn_ssid == ssid:
                       connection.Delete()
            except dbus.exceptions.DBusException:
               pass

    def add_connection(self,ssid):
        s_con = dbus.Dictionary({
            'type': '802-11-wireless',
            'uuid': str(uuid.uuid4()),
            'permissions': ['user:$USER'],
            'id': ssid 
        })
        s_wifi = dbus.Dictionary({
            'ssid': dbus.ByteArray(ssid),
            'security': '802-11-wireless-security'
        })
        s_wsec = dbus.Dictionary({'key-mgmt': 'wpa-eap'})
        s_8021x = dbus.Dictionary({
            'eap': ['tls'],
            'identity': 'anonymous@cern.ch',
            'ca-cert': dbus.ByteArray("file://" + self.cacert_file + "\0"),
            'subject-match': 'cern.ch',
            'client-cert':  dbus.ByteArray("file://" + self.pfx_file + "\0"),
            'private-key':  dbus.ByteArray("file://" + self.pfx_file + "\0"),
            'private-key-password':  '$PASSWORD',
        })
        s_ip4 = dbus.Dictionary({'method': 'auto'})
        s_ip6 = dbus.Dictionary({'method': 'auto'})
        con = dbus.Dictionary({
            'connection': s_con,
            '802-11-wireless': s_wifi,
            '802-11-wireless-security': s_wsec,
            '802-1x': s_8021x,
            'ipv4': s_ip4,
            'ipv6': s_ip6
        })
        self.settings.AddConnection(con)

    def main(self):
        self.check_opts()
        ver = self.connect_to_NM()
        self.delete_existing_connections('eduroam')
        self.add_connection('eduroam')

if __name__ == "__main__":
    ENMCT = EduroamNMConfigTool()
    ENMCT.main()
EOF
}
function create_wpa_conf {
cat << EOFW >> $HOME/.eduroam/eduroam.conf

network={
  ssid="eduroam"
  key_mgmt=WPA-EAP
  eap=TLS
  ca_cert="${HOME}/.eduroam/ca.pem"
  identity="anonymous@cern.ch"
  subject_match="cern.ch"
  private_key="${HOME}/.eduroam/user.p12"
  private_key_passwd="${PASSWORD}"
}
EOFW
chmod 600 $HOME/.eduroam/eduroam.conf
}
function p12dialog {
  if [ ! -z $ZENITY ] ; then
    if ! cert=`$ZENITY --file-selection --file-filter="personal certificate file (p12 or pfx) | *.p12 *.P12 *.pfx *.PFX" --file-filter="All files | *" --title="personal certificate file (p12 or pfx)"` ; then
       exit
    fi
  elif [ ! -z $KDIALOG ] ; then
    if ! cert=`$KDIALOG --getopenfilename . "*.p12 *.P12 *.pfx *.PFX | personal certificate file (p12 or pfx)" --title "personal certificate file (p12 or pfx)"` ; then
       exit
    fi
  
  else
    cert=""
    fl_ct=`ls *.p12 *.P12 *.pfx *.PFX  2>/dev/null | wc -l`
    if [ "$fl_ct" = "1" ]; then
      cert=`ls *.p12 *.P12 *.pfx *.PFX 2>/dev/null `
    fi

    while true ; do
      prompt="personal certificate file (p12 or pfx)"
      read -p "${prompt} [$bf$cert${n}]" cert_f
      if [ "$cert" -a -z "$cert_f" ] ; then
         break
      else
        if [ -f "$cert_f" ] ; then
          cert=$cert_f
          break
        else
          echo "file not found"
          cert=""
        fi
      fi
    done
fi
    cp "$cert" $HOME/.eduroam/user.p12
    cert=$HOME/.eduroam/user.p12
    temp_cert=$HOME/.eduroam/temp.pem

    EMPTY_PASS="0"
    PASSWORD=""
    if openssl pkcs12 -in $cert -passin pass:"$PASSWORD" -noout 2>/dev/null; then
        USER_NAME="anonymous@cern.ch"
	EMPTY_PASS="1"
	PASSWORD="1"
    else	
    PASSWORD=""
    prompt="enter the password for the certificate file"
    fi

    while [ ! "$PASSWORD" ]
    do
      if ! PASSWORD=`prompt_nonempty_string 0 "enter the password for the certificate file"` ; then
        exit 1
      fi
      if openssl pkcs12 -in $cert -passin pass:"$PASSWORD" -noout 2>/dev/null; then
	USER_NAME="anonymous@cern.ch"
      else
        alert "incorrect password"
        PASSWORD=""
      fi
    done

    if [ "$EMPTY_PASS" -eq "1" ];
    then
	PASSWORD="lin3_pas4"
	openssl pkcs12 -in $cert -passin pass:"" -out $temp_cert -passout pass:"$PASSWORD"
	openssl pkcs12 -export -in $temp_cert -passin pass:"$PASSWORD" -out $cert -passout pass:"$PASSWORD"
	rm $temp_cert
    fi



}  
p12dialog
if run_python_script ; then
   show_info "Installation successful"
else
   show_info "Network Manager configuration failed, generating wpa_supplicant.conf"
if [ -f $HOME/.eduroam/eduroam.conf ] ; then
  if ! ask "File $HOME/.eduroam/eduroam.conf exists; it will be overwritten." "Continue" 1 ; then confirm_exit; fi
  rm $HOME/.eduroam/eduroam.conf
  fi
   create_wpa_conf
   show_info "Output written to $HOME/.eduroam/eduroam.conf"
fi
