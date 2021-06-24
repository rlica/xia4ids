
#----------Modules------------------
case "$0" in
    -sh|sh|*/sh)modules_shell=sh ;;
    -ksh|ksh|*/ksh)modules_shell=ksh ;;
    -zsh|zsh|*/zsh)modules_shell=zsh ;;
    -bash|bash|*/bash)modules_shell=bash ;;
esac
#module() { eval `/usr/Modules/$MODULE_VERSION/bin/modulecmd $modules_shell $*`; }
module() { eval `/usr/bin/modulecmd ${modules_shell} $*`; }
#module() { eval `/usr/bin/modulecmd bash $*`; }

#--------Default_Enviro_modules----
moduleStatus="$(which modulecmd)"
if [[ "${moduleStatus}" != "" ]] && [ $TERM != "dumb" ] ; then
#    export $MODULEPATH=${HOME}/programs/modulefiles:$MODULEPATH
    module use ${HOME}/programs/moduleFiles
    module load gsl/2.4 root/6.18.04 paass-scope Full_PAASS
fi


# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

# Uncomment the following line if you don't like systemctl's auto-paging feature:
# export SYSTEMD_PAGER=

# User specific aliases and functions
# Standard aliases are defined in /etc/profile.d/Utk_daq.sh
#These include rlwrap for damm, utkscanor, pacman. also the single_ldf_scan function 
#For root it includes the PLX load aliases and the reboot/poweroff confirmation aliases



### R. Lica (April 2021)
#Include IDS analysis programs in the path

export GSDIR=/home/pixie16/programs/GASPware/etc/
PATH=$PATH:/home/pixie16/programs/GASPware/bin/
PATH=$PATH:/home/pixie16/programs/xia4ids/bin/
PATH=$PATH:/home/pixie16/programs/xia4ids/scripts/
PATH=$PATH:/home/pixie16/programs/spec2root/

### Various aliases
alias firefox="firefox &"

# Control poll2 from outside the tmux session (assuming session name 'poll2' and poll2 is started in pane 1.0)
alias pixie_status="tmux send-keys -t poll2:1.0 \"status\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
alias pixie_start="tmux send-keys -t poll2:1.0 \"run\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"
alias pixie_stop="tmux send-keys -t poll2:1.0 \"stop\" Enter && sleep 1 && tmux capture-pane -pt poll2:1.0"

# Write the monitor output to a text file for sending it to Grafana
alias monitor_txt="monitor | tee monitor.txt"

# Function to create a new exp folder with required subfolders
new_exp_folder() {
	echo 'New folder $1 will be created' 
	mkdir /Data_Dir/2021/$1 
	mkdir /Data_Dir/2021/$1/RAW 
	mkdir /Data_Dir/2021/ONLINE
} 

########### EDIT BELOW


### Define current experiment path
##  Environment variable used by 'grafana_monitor'
export CURRENT_EXP=/Data_Dir/2021/IS622/tests


