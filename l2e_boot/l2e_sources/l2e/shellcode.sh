#!/bin/ash

# Mr Robot & Hotel California Easter Eggs
alias astu=''
alias reboot='printf "We are all just prisoners here \nOf our own device \nWe are programmed to receive \nYou can check-out any time you like \nBut you can never leave!\n"'
shutdown() { printf "Hello, friend. \nHello, friend? That's lame. \nMaybe I should give you a name, but that's a slippery slope. \nYou're only in my head. We have to remember that. \nWelcome to the Hotel California!\n" ; }

#Amiga / AmigaDOS / TRIPOS Easter Eggs
alias DISKCOPY='cp'
alias FORMAT='mkfs'
alias INSTALL='echo You must be a Amiga Guru and meditate a lot for that ...'	
alias RELABEL='echo You miss that A 500 so much? ...'
alias INFO='du ; mount'
alias DIR='ls'
alias LIST='ls -l'
alias CD='cd'
alias MAKEDIR='mkdir'
alias ASSIGN='set'
alias COPY='cp'
alias DELETE='rm'
alias PROTECT='chmod +t'	
alias RENAME='mv'
alias TYPE='cat'
alias DATE='date'
alias ECHO='echo'
alias NEWCLI='ash'	
alias ENDCLI='exit'
alias SEARCH='grep'
alias SAY='echo Maybe in the next version? ...'

#Mr Robot & CCA & Doom Easter Eggs
alias xyzzy='cd / ; ln -s .fsociety fsociety; echo LOUDER!; export PS1="<CS30://> TEMPLE DOS #| " '
alias XYZZY='xyzzy ; cat /fsociety/shellcode.sh | rev'
alias plugh='xyzzy ; cd /fsociety ; ls -l ; cat readme.txt'
alias PLUGH='alias | sort'
alias plover='plugh ; ln -s fsociety00.dat fsociety00.wad & ln -s fsociety01.dat fsociety01.wad & ln -s fsociety02.dat fsociety02.wad'
alias doom='echo You need some MAGIC to make it happen ; /fsociety/astsu -iwad'
alias doomer='doom fsociety00.wad'
alias doomest='doom fsociety01.wad'
alias doomerest='doom fsociety02.wad'
alias magic='echo LOUDER!'
alias MAGIC='echo MAGIC WORDS: p****, p*****, x****, X****, a****, d***** ... The spells must be in order! Use a MIRROR to revERT!'

#BBS

alias Zzzzzz='echo The Joke is on us!'

#Credits / Help / Info / Menu 
alias credits=''
alias help=''
alias info=''
alias menu=''

#Display messages from l2e kernel module
alias call_trans_opt='echo Transcript from L2E Kernel Module: ; dmesg | grep -i T25 ; echo Note: We had killed the L2E kernel module user space process 15 seconds after boot up due to its buggy nature now...'

#kill l2e userspace to prevent l2e kernel module from working in background as it is buggy now
setsid ash -c "sleep 15; killall l2e" &>/dev/null

#Matrix Easter Egg
setsid ash -c "sleep 4; echo " 
setsid ash -c "sleep 5; echo Wake up, Neo..." 
setsid ash -c "sleep 9; echo The Matrix has you..." 
setsid ash -c "sleep 12; echo Follow the white llama." 
setsid ash -c "sleep 13; echo Knock, knock." 
setsid ash -c "sleep 14; echo Knock, knock, who?" 
setsid ash -c "sleep 15; echo Knock, knock, Neo." 

#alias cryptic
alias alias='alias | rev'






